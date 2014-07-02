#include "trmmosbat.h"

trmMosbat::trmMosbat(CvSeq *points,double previous)
{
    int ID = 0;
    middle.x = 0;
    middle.y = 0;
    center1.x = 9999;
    center1.y = 9999;
    center2.x = 9999;
    center2.y = 9999;
    center3.x = 9999;
    center3.y = 9999;
    center4.x = 9999;
    center4.y = 9999;

    //find centers
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        middle.x += p->x;
        middle.y +=+ p->y;
    }
    middle.x /= points->total;
    middle.y /= points->total;
    for( int i=0; i < 4; i++ )
    {
        ID = 0;
        CvPoint *temp = (CvPoint*)cvGetSeqElem ( points, 0 );
        CvPoint *p;
        for( int j=1; j<points->total; j++ )
        {
            p = (CvPoint*)cvGetSeqElem ( points, j );
            int a = ((middle.x - p->x) * (middle.x - p->x)  + (middle.y - p->y) *  (middle.y - p->y));
            int b = ((middle.x - temp->x) * (middle.x - temp->x)  + (middle.y - temp->y) *  (middle.y - temp->y));
            if ( a < b)
            {
                temp = p;
                ID = j;
            }
        }
        if ( temp->y >= middle.y )
        {
            if (center1.x == 9999)
            {
                center1 = *temp;
            }
            else if (center2.x == 9999)
            {
                center2 = *temp;
            }
            else
            {
                center3 = *temp;
            }
        }
        else
        {
            if (center3.x == 9999)
            {
                center3 = *temp;
            }
            else
            {
                center4 = *temp;
            }
        }
        cvSeqRemove(points,ID);
    }

    if (center1.x > center2.x)
    {
        CvPoint temp = center2;
        center2 = center1;
        center1 = temp;
    }
    if (center3.x > center4.x)
    {
        CvPoint temp = center3;
        center3 = center4;
        center4 = temp;
    }

    top1 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    ID = 0;
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        if ((p->y) < (top1.y))
        {
            top1 =  *p;
            ID = i;
        }
    }
    cvSeqRemove(points,ID);
    top2 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    ID = 0;
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        if (((top1.x - p->x) * (top1.x - p->x)  + (top1.y - p->y) *  (top1.y - p->y)) < ((top1.x - top2.x) * (top1.x - top2.x)  + (top1.y - top2.y) *  (top1.y - top2.y)))
        {
            top2 =  *p;
            ID = i;
        }
    }

    if (top1.x > top2.x)
    {
        CvPoint temp = top2;
        top2 = top1;
        top1 = temp;
    }

    cvSeqRemove(points,ID);

    left1 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    ID = 0;
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        if ((p->x) < (left1.x))
        {
            left1 =  *p;
            ID = i;
        }
    }
    cvSeqRemove(points,ID);
    left2 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    ID = 0;
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        if (((left1.x - p->x) * (left1.x - p->x)  + (left1.y - p->y) *  (left1.y - p->y)) < ((left1.x - left2.x) * (left1.x - left2.x)  + (left1.y - left2.y) *  (left1.y - left2.y)))
        {
            left2 =  *p;
            ID = i;
        }
    }
    cvSeqRemove(points,ID);

    if (left1.y > left2.y)
    {
        CvPoint temp = left2;
        left2 = left1;
        left1 = temp;
    }

    down1 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    ID = 0;
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        if ((p->y) > (down1.y))
        {
            down1 =  *p;
            ID = i;
        }
    }
    cvSeqRemove(points,ID);
    down2 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    ID = 0;
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        if (((down1.x - p->x) * (down1.x - p->x)  + (down1.y - p->y) *  (down1.y - p->y)) < ((down1.x - down2.x) * (down1.x - down2.x)  + (down1.y - down2.y) *  (down1.y - down2.y)))
        {
            down2 =  *p;
            ID = i;
        }
    }
    cvSeqRemove(points,ID);

    if (down1.x > down2.x)
    {
        CvPoint temp = down2;
        down2 = down1;
        down1 = temp;
    }

    right1 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    right2 = *(CvPoint*)cvGetSeqElem ( points, 1 );

    if (right1.y > right2.y)
    {
        CvPoint temp = right2;
        right2 = right1;
        right1 = temp;
    }

    pr = previous;
}

trmMosbat::trmMosbat()
{
    rect = (CvPoint *)malloc(4 * sizeof(CvPoint));
}

double trmMosbat::findAngle()
{
    int gradiant1 = findDerivative(top1,down1,center1,center3);
    return (atan(gradiant1) * 180 / PI);
}

double trmMosbat::findDerivative(CvPoint pt1, CvPoint pt2, CvPoint pt3, CvPoint pt4)
{
    double a1 = 4;
    double a2 = pt1.x + pt2.x + pt3.x + pt4.x;
    double b1 = a2;
    double b2 = pt1.x * pt1.x + pt2.x * pt2.x + pt3.x * pt3.x + pt4.x * pt4.x;
    double c1 = pt1.y + pt2.y + pt3.y + pt4.y;
    double c2 = pt1.x * pt1.y + pt2.x * pt2.y + pt3.x * pt3.y + pt4.x * pt4.y;

    double mat_1 = (a1 * c2 - a2* c1);
    double mat_2 = (a1 * b2 - a2* b1);

    return mat_1 / mat_2;
}
