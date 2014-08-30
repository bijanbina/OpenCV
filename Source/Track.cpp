#include "Track.h"
#include "trmmark.h"

using namespace cv;
using namespace std;

pair<double, double> last_spot(0,0) ;

//Find the colored spot
int find_spot( Mat frame, pair<double, double>& spot, int w )
{
	int filter = 500;
	int fr, fc, c;
	int x, y, xb, yb;
	fr = frame.rows;
	fc = frame.cols;
	double sumX=0, sumY=0;
	int num=0;
	
	//Region
	x = (int) spot.first - w/2;
	y = (int) spot.second - w/2;
	if( x < 0 )
		x = 0;
	if( y < 0 )
		y = 0;
	if( x+w > fc )
		xb = frame.cols;
	else
		xb = x+w;
	if( y+w > fr )
		yb = frame.rows;
	else
		yb = y+w;
	
	//Search
	uchar* p;
	for( int i=y; i<yb; i++ )
	{
		p = frame.ptr<uchar>(i); 
		for (int j=x; j<xb; j++)
		{
			c = p[j];
			if (c > 0)
			{
				sumX += j;
				sumY += i;
				num++;
			}
		}
	}
	
	//Check if any points is detected
	if( !num )
		return -1;
	
	//Next level spot	
	sumX /= num;
	sumY /= num;
	spot.first = sumX;
	spot.second = sumY;
	if( num < 2*filter )
		return 0;
	else
		return find_spot( frame, spot, w /= 2 );
}

//Find region around the given spot
Scalar find_region( Mat& frame, const pair<double, double>& spot, int w)
{
	int xc=spot.first, yc=spot.second;
	int x=xc, y=yc;
	int counter=0, step=10;
	bool change_dir = FALSE;
	x = xc - w/2;
	y = yc - w/2;
	if (x < 0)
		x = 0;
	if (y < 0)
		y = 0;
	if (x+w > frame.cols)
		w = frame.cols - x;
	if (y+w > frame.rows)
		w = frame.rows - y;
	return Scalar( x, y, w );
}

//Find center using fitLine and Canny
pair<double, double> find_center( Mat& frame, Point& center )
{
	Mat cv_frame = frame.clone(), gray_frame, canny_frame;
    vector< vector<Point> > contours;
    //vector< vector<Point2f> > contours;
	vector<Point2f> line1, line2;
	Vec4f l1, l2;
	//Threshold to black and white
	cvtColor( frame, gray_frame, COLOR_BGR2GRAY);
	threshold( gray_frame, canny_frame, 150, 255, 0 );
	//Canny
	Canny( canny_frame, canny_frame, 100, 200, 3);
	//Mask center
	circle( canny_frame, center, frame.cols/3, Scalar( 0, 0, 0 ), CV_FILLED );
	//Search for Y points

    findContours( canny_frame, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);

	for( int i=0; i<contours.size(); i++ )
	{
		if( contours[i].size() < 20 )
			contours.erase( contours.begin() + i );
    }
	if( contours.size() != 4 )
		return pair<double, double> (-1, -1);

    line1.reserve( contours[0].size() + contours[3].size() ); // preallocate memory

    for( int i=0; i<contours[0].size(); i++ )
        line1.push_back( cvPointTo32f(contours[0][i]));

    for( int i=0; i<contours[3].size(); i++ )
        line1.push_back( cvPointTo32f(contours[3][i]));
//    line1.insert( line1.end(), contours[0].begin(), contours[0].end() );
//    line1.insert( line1.end(), contours[3].begin(), contours[3].end() );
	
    line2.reserve( contours[1].size() + contours[2].size() ); // preallocate memory
    for( int i=0; i<contours[0].size(); i++ )
        line2.push_back( cvPointTo32f(contours[1][i]));

    for( int i=0; i<contours[2].size(); i++ )
        line2.push_back( cvPointTo32f(contours[2][i]));


//    line2.insert( line2.end(), contours[1].begin(), contours[1].end() );
//    line2.insert( line2.end(), contours[2].begin(), contours[2].end() );

	fitLine( line1, l1, CV_DIST_L2, 0, 0.01, 0.01 );
	fitLine( line2, l2, CV_DIST_L2, 0, 0.01, 0.01 );

	double m1, m2, b1, b2, x, y;
	if( !l1[0] )
		return pair<double, double> ( l1[2], l2[3] );
	assert( l1[0] && l2[0] );
	m1 = l1[1]/l1[0];
	m2 = l2[1]/l2[0];
	b1 = l1[3]-m1*l1[2];
	b2 = l2[3]-m2*l2[2];
	x = (b1-b2)/(m2-m1);
	y = m1*x + b1;
	return pair<double, double> ( x, y );
}

//Parameters for detect: '***' indicates parameters that should be received from the user
/*
input_frame: frame extracted from video
colorf_frame: color filtered frame output for color spot display on interface
region_frame: region-cut frame output for region display on interface
***spot: (0 to frame-length, 0 to frame-width): indicates center point for ROI
***window: max ROI width (square-shape): 0 to frame-length
***circle_diameter: diameter of the circle used as the sign
***color: (Scalar lower_boundry, Scalar upper_boundry): color in HSV format, each parameter 0 to 255
***pixel_mm_ratio: pixels/mm in double format
Check if the output loc is not (-1,-1) then display:
	1-loc and its displacement from the initial loc: sqrt x^2+y^2
	2-colorf_frame and region_frame
*/

trmMark* markFromMahyar (IplImage* image, trm_param pars)
{
	//Bijan Inits

    if ( pars.window == -1 ) //<-----Bijoo add this piece of ...
    {
        pars.window = image->width;
    }

    int window = pars.window;
    Scalar color[2] = pars.color;

	//Initialize
    Mat input_frame( image );
    pair<double, double> loc;
    Mat cv_frame,colorf_frame,region_frame;
	Scalar boundry;
	Mat element = getStructuringElement( MORPH_RECT, Size( 10, 10 ), Point( 5, 5) );
		
    if( input_frame.empty() )
        return NULL;

	//Convert frame to HSV
	cvtColor( input_frame, cv_frame, CV_BGR2HSV );
		
	//THRESHOLDING AND FINDING COLOR SPOT
	inRange( cv_frame, color[0], color[1], colorf_frame );
	dilate( colorf_frame, colorf_frame, element );

    if( find_spot (colorf_frame, last_spot, window) < 0 )
        return NULL;

	//CONTOURS AND CENTER DETECTION
    boundry = find_region( colorf_frame, last_spot, (MAHYAR_MARK_LENGHT * 10 * 0.8) );
    region_frame = Mat( input_frame, Rect( boundry[0], boundry[1], boundry[2], boundry[2] ) );
    Point tempp = Point( last_spot.first-boundry[0], last_spot.second-boundry[1] );
    loc = find_center( region_frame, tempp );
	if( loc.first < 0 )
        return NULL;

	loc.first += boundry[0];
	loc.second += boundry[1];	
	//return loc;

	//Return Bijan Style
	trmMark *result = new trmMark();
    result->middle = cvPoint(loc.first, loc.second);
//    result->rect = (CvPoint*) malloc( 4*sizeof(CvPoint) );
    result->region = cvRect(boundry[0], boundry[1], boundry[2], boundry[2]);
//    result->rect[0] = cvPoint( spot.first-boundry[0], spot.second-boundry[1] );
//    result->rect[1] = cvPoint( spot.first-boundry[0]+boundry[2], spot.second-boundry[1] );
//    result->rect[2] = cvPoint( spot.first-boundry[0], spot.second-boundry[1]+boundry[2] );
//    result->rect[3] = cvPoint( spot.first-boundry[0]+boundry[2], spot.second-boundry[1]+boundry[2] );

	return result;
}

