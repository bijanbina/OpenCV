#include "highgui.h"
#include "cv.h"
#include <stdio.h>

int g_slider_position = 0;
IplImage* image;


IplImage* doCanny( IplImage* in, double lowThresh, double highThresh, double aperture )
{
    if(in->nChannels != 1)
    {
        printf("Not supported\n");
        return(0); //Canny only handles gray scale images
    }
    IplImage* out = cvCreateImage( cvGetSize( in ) , IPL_DEPTH_8U, 1 );
    cvCanny( in, out, lowThresh, highThresh, aperture );
    return( out );
}

IplImage* doPyrDown( IplImage* in, int filter)
{
// Best to make sure input image is divisible by two.
//
    assert( in->width%2 == 0 && in->height%2 == 0 );
    IplImage* out = cvCreateImage(
                        cvSize( in->width/2, in->height/2 ),
                        in->depth,
                        in->nChannels
                    );
    cvPyrDown( in, out , CV_GAUSSIAN_5x5);
    return( out );
};

void onTrackbarSlide(int pos)
{
	double hey = pos;
	hey = hey / 3.0;
    IplImage* out = doCanny( image, hey,hey *3, 3 );
// Do the smoothing
//
// Show the smoothed image in the output window
//
    cvShowImage( "Example4-out", out );
// Be tidy
//
    cvReleaseImage( &out );
}

void example2_4( IplImage *input )
{
// Create some windows to show the input
// and output images in.
//
    cvNamedWindow( "Example4-out" , CV_WINDOW_AUTOSIZE );
// Create a window to show our input image
//
    cvShowImage( "Example4-in", input );
// Create an image to hold the smoothed output
//
cvCreateTrackbar( "Position", "Example4-in", &g_slider_position, 96,
            onTrackbarSlide
        );
    IplImage* out = doCanny( image, 1,3, 3 );
// Do the smoothing
//
// Show the smoothed image in the output window
//
    cvShowImage( "Example4-out", out );
// Be tidy
//
    cvReleaseImage( &out );
// Wait for the user to hit a key, then clean up the windows
//
    cvWaitKey( 0 );
    cvDestroyWindow( "Example4-in" );
    cvDestroyWindow( "Example4-out" );
}

int main( int argc, char** argv )
{
    image = cvLoadImage( argv[1] , CV_LOAD_IMAGE_GRAYSCALE );
    IplImage *in = cvLoadImage( argv[1] , CV_LOAD_IMAGE_COLOR );
    example2_4( in );
}
