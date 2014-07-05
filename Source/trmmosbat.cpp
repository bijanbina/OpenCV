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
    for( int i=0; i < points->total; i++ )
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
        for( int j=1; j < points->total; j++ )
        {
            p = (CvPoint*)cvGetSeqElem ( points, j );
            if ( dist_cv(middle,*p) < dist_cv(middle,*temp) )
            {
                temp = p;
                ID = j;
            }
        }
        if ( temp->y <= middle.y )
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
    if (center3.x < center4.x)
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
        if (dist_cv(top1,*p) < dist_cv(top1,top2))
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
        if (dist_cv(left1,*p) < dist_cv(left1,left2))
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
        if (dist_cv(down1,*p) < dist_cv(down1,down2))
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
    rect = (CvPoint *)malloc( 10 * sizeof (CvPoint));
    edge = (dist_cv(top2,center2) + dist_cv(right1,center2) + dist_cv(right2,center3) + dist_cv(down2,center3) + dist_cv(down1,center4) + dist_cv(left2,center4) + dist_cv(left1,center1))/7.0;
    //edge = dist_cv(right1,center2);
    rect[0] = cvPoint(0,0);
    rect[1] = cvPoint(0,0);
    rect[2] = cvPoint(0,0);
    rect[3] = cvPoint(0,0);
}

trmMosbat::trmMosbat()
{
    rect = (CvPoint *)malloc(4 * sizeof(CvPoint));
}

double trmMosbat::findAngle()
{
    double gradiant1 = findDerivative(top1,down1,center1,center4,true);
    gradiant1 += findDerivative(left1,right1,center1,center2);
    gradiant1 += findDerivative(left2,right2,center3,center4);
    gradiant1 += findDerivative(top2,down2,center2,center3,true);
    gradiant1 /= 4.0;
//    std::cout << findDerivative(top1,down1,center1,center4,true) << " \t " << findDerivative(left1,right1,center1,center2) <<
//                 " \t "<< findDerivative(left2,right2,center3,center4) << " \t " << findDerivative(top2,down2,center2,center3,true) << std::endl;

    return (90 + (atan(gradiant1) * 180 / PI));
}


double trmMosbat::dist_cv(CvPoint pt1, CvPoint pt2)
{
    return cv::sqrt((pt1.x - pt2.x) * (pt1.x - pt2.x)  + (pt1.y - pt2.y) *  (pt1.y - pt2.y));
}

CvPoint* trmMosbat::getRect()
{
    double angle = findAngle();

    double m = tan((angle + 45)/180.0 * PI);
    rect[0] = center1;
    double b = -rect[0].y - m*rect[0].x;
    double d = b + rect[0].y;
    double b1 = rect[0].x - m * d;
    double a1 = (m * m + 1);
    double c1 = ( rect[0].x * rect[0].x  + d * d - 3 * edge * edge);
    double x = (b1 - cv::sqrt(b1 * b1 - a1 * c1 ))/ a1;
    rect[0].x = x;rect[0].y = - (m * x + b);

    m = tan((angle - 45)/180.0 * PI);
    rect[1] = center2;
    b = -rect[1].y - m*rect[1].x;
    d = b + rect[1].y;
    b1 = rect[1].x - m * d;
    a1 = (m * m + 1);
    c1 = ( rect[1].x * rect[1].x  + d * d - 3 * edge * edge);
    x = (b1 + cv::sqrt(b1 * b1 - a1 * c1 ))/ a1;
    rect[1].x = x;rect[1].y = - (m * x + b);


    m = tan((angle + 45)/180.0 * PI);
    rect[2] = center3;
    b = -rect[2].y - m*rect[2].x;
    d = b + rect[2].y;
    b1 = rect[2].x - m * d;
    a1 = (m * m + 1);
    c1 = ( rect[2].x * rect[2].x  + d * d - 3 * edge * edge);
    x = (b1 + cv::sqrt(b1 * b1 - a1 * c1 ))/ a1;
    rect[2].x = x;rect[2].y = - (m * x + b);

    m = tan((angle - 45)/180.0 * PI);
    rect[3] = center4;
    b = -rect[3].y - m*rect[3].x;
    d = b + rect[3].y;
    b1 = rect[3].x - m * d;
    a1 = (m * m + 1);
    c1 = ( rect[3].x * rect[3].x  + d * d - 3 * edge * edge);
    x = (b1 - cv::sqrt(b1 * b1 - a1 * c1 ))/ a1;
    rect[3].x = x;rect[3].y = - (m * x + b);

    return rect;
}


CvRect trmMosbat::trmMosbat::getRegion()
{
    getRect();
    CvRect return_data = cvRect(rect[0].x,rect[0].y,rect[0].x,rect[0].y);
    for( int i = 1; i < 4; i++ )
    {
        if ( rect[i].x < return_data.x )
            return_data.x = rect[i].x;
        if ( rect[i].y < return_data.y)
            return_data.y = rect[i].y;
        if ( rect[i].x > return_data.width )
            return_data.width = rect[i].x;
        if ( rect[i].y > return_data.height)
            return_data.height = rect[i].y;
    }

    return_data.width = return_data.width - return_data.x;
    return_data.height = return_data.height - return_data.y;

    return return_data;

}

double trmMosbat::findDerivative(CvPoint pt1, CvPoint pt2, CvPoint pt3, CvPoint pt4,bool reverse)
{
    long a1 = 4;
    long a2 = pt1.x + pt2.x + pt3.x + pt4.x;
    long b1 = a2;
    long b2 = pt1.x * pt1.x + pt2.x * pt2.x + pt3.x * pt3.x + pt4.x * pt4.x;
    long c1 = -(pt1.y + pt2.y + pt3.y + pt4.y);
    long c2 = -(pt1.x * pt1.y + pt2.x * pt2.y + pt3.x * pt3.y + pt4.x * pt4.y);

    if (reverse)
    {
        a1 = 4;
        a2 = -(pt1.y + pt2.y + pt3.y + pt4.y);
        b1 = a2;
        b2 = pt1.y * pt1.y + pt2.y * pt2.y + pt3.y * pt3.y + pt4.y * pt4.y;
        c1 = -(pt1.x + pt2.x + pt3.x + pt4.x);
        c2 = (pt1.x * pt1.y + pt2.x * pt2.y + pt3.x * pt3.y + pt4.x * pt4.y);
    }

    double mat_1 = (a1 * c2 - a2 * c1);
    double mat_2 = (a1 * b2 - a2 * b1);

    double slope = mat_1 / mat_2;

    return slope;
}

trmParam trmMosbat::Loadparam(char *filename)
{
    trmParam return_data;
    QFile json_file(filename);
    if(json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Json::Value json_obj;
        Json::Reader reader;
        if (reader.parse(QString(json_file.readAll()).toUtf8().data(), json_obj))
        {
            return_data.bold = json_obj.get("Bold",0).asInt();
            return_data.erode = json_obj.get("Erode",0).asInt();
            return_data.dilute = json_obj.get("Dilate",0).asInt();
            return_data.frame_num = json_obj.get("Start Frame Number",0).asInt();
            std::string buffer = json_obj.get("File Address","NULL").asString();
            int len = strlen(buffer.c_str());
            char *buffer2 = (char *)malloc(len);
            strncpy(buffer2, buffer.c_str(), len);
            return_data.filename = buffer2;

            const Json::Value edge = json_obj["Edge Detection"];
            if (!edge.empty())
            {
                return_data.edge_1 = edge.get("Treshold 1",0).asDouble();
                return_data.edge_2 = edge.get("Treshold 2",0).asDouble();
            }
            return_data.corner_min = json_obj.get("Corner Minimum Distance",0).asInt();
        }

    }
    else
    {
        return_data.bold = 0;
        return_data.erode = 0;
        return_data.dilute = 0;
        return_data.edge_1 = 0;
        return_data.edge_2 = 0;
        return_data.corner_min = 0;
    }
    return return_data;
}

void trmMosbat::Saveparam(trmParam data,char *filename)
{
    Json::Value json_main;
    Json::Value edge;
    edge["Treshold 1"] = data.edge_1;
    edge["Treshold 2"] = data.edge_2;
    json_main["Erode"] = data.erode;
    json_main["Dilate"] = data.dilute;
    json_main["Bold"] = data.bold;
    json_main["Corner Minimum Distance"] = data.corner_min;
    json_main["File Address"] = data.filename.toUtf8().data();
    json_main["Start Frame Number"] = data.frame_num;
    json_main["Edge Detection"] = edge;

    // write in a nice readible way
    Json::StyledWriter styledWriter;
    std::string str = styledWriter.write(json_main);
    std::vector<char> json_data(str.begin(), str.end());
    json_data.push_back('\0');
    QFile file;
    file.setFileName(filename);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    file.write(json_data.data());
    file.close();
}
