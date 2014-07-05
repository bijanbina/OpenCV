#ifndef TRMMOSBAT_H
#define TRMMOSBAT_H

#include <QtWidgets>
#include <cv.h>
#include <unistd.h>
#include <math.h>
#include <iostream>
#include <jsoncpp/json/json.h>
#include <string.h>

#define PI 3.14159265

struct trm_param
{
    double edge_1;
    double edge_2;
    int erode;
    int dilute;
    int bold;
    int corner_min;
    int frame_num;
    QString filename;
};

typedef trm_param trmParam;

class trmMosbat
{
public:
    trmMosbat(CvSeq *points,double previous);
    trmMosbat();
    double findAngle();
    double findDerivative(CvPoint pt1, CvPoint pt2, CvPoint pt3, CvPoint pt4, bool reverse = false);
    CvPoint *getRect(); //return rectangle which contain plus
    CvRect   getRegion(); //return rectangle which contain plus

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
    trmMosbat *next;

private:
    double dist_cv(CvPoint pt1, CvPoint pt2);

    CvPoint *rect;
    double   edge;
    double   pr;//previeos angle

};

#endif // TRMMOSBAT_H
