#include "highgui.h"
#include "cv.h"
IplImage* doPyrDown( IplImage* in, int filter = IPL_GAUSSIAN_5x5)
{
// Best to make sure input image is divisible by two.
//
    assert( in->width%2 == 0 && in->height%2 == 0 );
    IplImage* out = cvCreateImage(
                        cvSize( in->width/2, in->height/2 ),
                        in->depth,
                        in->nChannels
                    );
    cvPyrDown( in, out );
    return( out );
};

void example2_4( IplImage* image )
{
// Create some windows to show the input
// and output images in.
//
    cvNamedWindow( "Example4-out" , CV_WINDOW_AUTOSIZE );
// Create a window to show our input image
//
    cvShowImage( "Example4-in", image );
// Create an image to hold the smoothed output
//
    IplImage* out = cvCreateImage(
                        cvGetSize(image),
                        IPL_DEPTH_8U,
                        3
                    );
// Do the smoothing
//
    cvSmooth( image, out, CV_GAUSSIAN, 3, 3 ,0 ,0 );
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
    IplImage* img = cvLoadImage( argv[1] , CV_LOAD_IMAGE_COLOR );
    example2_4(img);
}
