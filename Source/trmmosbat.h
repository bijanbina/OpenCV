#ifndef TRMMOSBAT_H
#define TRMMOSBAT_H

#include <QtWidgets>
#include <cv.h>

class trmMosbat
{
public:
    trmMosbat(CvSeq *points);

    CvPoint top1;
    CvPoint top2;
    CvPoint below1;
    CvPoint below2;
    CvPoint left1;
    CvPoint left2;
    CvPoint right1;
    CvPoint right2;
    CvPoint center1;
    CvPoint center2;
    CvPoint center3;
    CvPoint center4;
};

#endif // TRMMOSBAT_H
