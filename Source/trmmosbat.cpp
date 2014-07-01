#include "trmmosbat.h"

trmMosbat::trmMosbat(CvSeq *points)
{
    int x_temp,y_temp;
    for( int i=0; i<points->total; i++ )
    {
        CvPoint *p = (CvPoint*)cvGetSeqElem ( points, i );
        std::cout << p->x << "\t" << p->y << std::endl;
    }
}
