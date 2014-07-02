#include "trmmosbat.h"

trmMosbat::trmMosbat(CvSeq *points)
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

    if (center1.y < center2.y)
    {
        CvPoint temp = center2;
        center2 = center1;
        center1 = temp;
    }
    if (center3.y < center4.y)
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

    right1 = *(CvPoint*)cvGetSeqElem ( points, 0 );
    right2 = *(CvPoint*)cvGetSeqElem ( points, 1 );
}

trmMosbat::trmMosbat()
{
    ;
}
