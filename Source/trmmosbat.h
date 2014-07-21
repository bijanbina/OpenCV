#ifndef TRMMOSBAT_H
#define TRMMOSBAT_H

#include <QtWidgets>
#include <cv.h>
#include <highgui.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string.h>

#define PI 3.14159265
#define calib_prev_size 800.0

#define MORPH_STATE_NORMALL     0
#define MORPH_STATE_REVERSED    1
#define MORPH_STATE_OPEN        2
#define MORPH_STATE_CLOSE       3

struct trm_param
{
    double edge_1;
    double edge_2;
    int erode;
    int dilate;
    int bold;
    int narrow;
    int edge_corner;
    int corner_min;
    int frame_num;
    bool isVideo;
    int calibre_width;
    int morph_algorithm;
    QString filename;
};

typedef trm_param trmParam;

class trmMosbat
{
public:
    trmMosbat();
	double findAngle();
    double findDerivative(CvPoint pt1, CvPoint pt2, CvPoint pt3, CvPoint pt4, bool reverse = false);
    CvPoint *getRect(); //return rectangle which contain plus
    CvRect   getRegion(); //return rectangle which contain plus
    static void bold_filter(IplImage *in,int kernel_size);
    static void narrowFilter(IplImage *in,int kernel_size);
    static IplImage* doCanny( IplImage* in, double lowThresh, double highThresh, double aperture );
    double dist_cv(CvPoint pt1, CvPoint pt2);

    static trmParam Loadparam(char *filename);
    static void Saveparam(trmParam data, char *filename);

    CvPoint top1;
    CvPoint top2;
    CvPoint down1;
    CvPoint down2;
    CvPoint left1;
    CvPoint left2;
    CvPoint right1;
    CvPoint right2;
    CvPoint center1;
    CvPoint center2;
    CvPoint center3;
    CvPoint center4;
    CvPoint middle;

    CvPoint *rect;
    double   edge;
    double   pr;//previeos angle

};
trmMosbat *mosbatFromImage(IplImage *imagesrc,trmParam data);
trmMosbat *create_from_point(CvSeq *points,double previous);
#endif // TRMMOSBAT_H
