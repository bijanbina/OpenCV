#include "highgui.h"
#include "cv.h"
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>

int g_slider_position = 0;
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

void Treshold_change(int pos)
{
    double hey = pos;
    hey = hey / 3.0;
    IplImage* out = doCanny( image, hey,hey *3, 3 );
// Do the smoothing
//
// Show the smoothed image in the output window
//
    cvShowImage( "CV Test", out );
// Be tidy
//
    cvReleaseImage( &out );
}

void Max_Treshold(int pos)
{
    double hey = pos;
    hey = hey / 3.0;
    IplImage* out = doCanny( image, hey,hey *3, 3 );
// Do the smoothing
//
// Show the smoothed image in the output window
//
    cvShowImage( "CV Test", out );
// Be tidy
//
    cvReleaseImage( &out );
}

void example2_4( IplImage *input )
{
// Create some windows to show the input
// and output images in.
//
    cvNamedWindow( "output" , CV_WINDOW_AUTOSIZE );
// Create a window to show our input image
//
    cvShowImage( "CV Test-input", input );
// Create an image to hold the smoothed output
//
    cvCreateTrackbar( "Position", "Example4-in", &g_slider_position, 96,
            Treshold_change
        );
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
    cvDestroyWindow( "output" );
    cvDestroyWindow( "Example4-out" );
}

int main( int argc, char** argv )
{
    char *file_name = argv[1];
    if (argv[1] == NULL)
    {
    	file_name = "/home/bijan/Pictures/nenit_by_kiko11-d5ka4zo (copy).jpg";
    }
    image = cvLoadImage( file_name , CV_LOAD_IMAGE_GRAYSCALE );
    IplImage *in = cvLoadImage( file_name , CV_LOAD_IMAGE_COLOR );
    example2_4( in );
}
