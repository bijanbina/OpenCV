#include "calibratewindow.h"

CalibrateWindow::CalibrateWindow(QWidget *parent) :
    QDialog(parent)
{
    CreateLayout(parent);
    CreateMenu();

    connect(slider1,SIGNAL(valueChanged(int)), this, SLOT(slider1_change(int)));
    connect(slider2,SIGNAL(valueChanged(int)), this, SLOT(slider2_change(int)));
    connect(vslider1,SIGNAL(valueChanged(int)), this, SLOT(slider3_change(int)));
    connect(vslider2,SIGNAL(valueChanged(int)), this, SLOT(slider4_change(int)));
    connect(chk1,SIGNAL(stateChanged(int)), this, SLOT(chk1_change()));
    connect(chk2,SIGNAL(stateChanged(int)), this, SLOT(chk2_change()));
    connect(back_btn, SIGNAL(released()), this, SLOT(back_clicked()));
    connect(next_btn, SIGNAL(released()), this, SLOT(next_clicked()));
    connect(replace_btn, SIGNAL(released()), this, SLOT(replace_clicked()));

    connect(a_open, SIGNAL(triggered(bool)),this,SLOT(open_clicked()));
    connect(a_open_image, SIGNAL(triggered(bool)),this,SLOT(openimage_clicked()));
    connect(a_save, SIGNAL(triggered(bool)),this,SLOT(save_clicked()));
    connect(a_replace, SIGNAL(triggered(bool)),this,SLOT(replace_clicked()));
    connect(a_loop, SIGNAL(triggered(bool)),this,SLOT(state_change()));
    connect(a_width, SIGNAL(triggered(bool)),this,SLOT(width_clicked()));
    connect(a_equal, SIGNAL(triggered(bool)),this,SLOT(equal_clicked(bool)));
    connect(a_feature, SIGNAL(triggered(bool)),this,SLOT(feature_clicked(bool)));
    connect(a_corner, SIGNAL(triggered(bool)),this,SLOT(corner_clicked(bool)));
    connect(a_hough, SIGNAL(triggered(bool)),this,SLOT(hough_clicked(bool)));

    filename = filter_param.filename;
    isVideo = filter_param.isVideo;
    if (!isVideo)
    {
        imagesrc = cvLoadImage(filename.toLocal8Bit().data());
        image = cvLoadImage(filename.toLocal8Bit().data(),CV_LOAD_IMAGE_GRAYSCALE);

        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilate);

        option_menu->addAction(a_width);
        option_menu->addAction(a_loop);

        state_change();
    }
    else
    {
        capture = cvCreateFileCapture( filename.toLocal8Bit().data() );
        if (capture == NULL)
            return;
        imagesrc = cvQueryFrame( capture );
        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        chk2->setChecked(false);
        calibrate_state = TRM_STATE_FRAME;

        option_menu->addAction(a_width);

        vslider1->setValue(0);
        chk1->setChecked(false);
        slider1->setValue(0);
        state_change(1);
        slider1->setValue(filter_param.frame_num);
        slider2->setValue(filter_param.calibre_width);
    }

    setGeometry((qApp->desktop()->geometry().center() - rect().center()).x(),(qApp->desktop()->geometry().center() - rect().center()).y(),rect().width(),rect().height());
}

CalibrateWindow::~CalibrateWindow()
{

}


void CalibrateWindow::state_change(int changed)
{
    if (imgout != NULL)
	{
        cvReleaseImage(&imgout);
	}
    if (image == NULL)
        return;
    if (changed)
	{
		chk1->setChecked(false);
		chk2->setChecked(false);
        treshold_1 = 0;
        treshold_3 = 0;
        slider1->setEnabled(true);
        slider2->setEnabled(true);
        slider1->setValue(0);
        slider2->setValue(0);
        vslider1->setValue(0);
        vslider2->setValue(0);
    }
    surface_height = floor((surface_width/image->width)*image->height);
    if ( calibrate_state == TRM_STATE_FRAME)
    {
        int frames = (int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT );
        if (frames - 100 < 0)
            return;
        //cvSetCaptureProperty(capture,CV_CAP_PROP_POS_MSEC,treshold_1);
        if (treshold_1 + 100 > frames )
            return;
        cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,treshold_1);
//        std::cout << cvGrabFrame( capture ) << "\t" << frames << std::endl;
        if (!cvGrabFrame( capture ))
            return;
        imagesrc = cvQueryFrame( capture );
        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setMaximum(frames - 100);
        slider2->setMinimum(200);
        slider2->setMaximum(2000);
        vslider1->setEnabled(false);
        vslider2->setEnabled(false);
    }
    else if (calibrate_state == TRM_STATE_EDGE)
    {
        if ( chk2->isChecked() )
        {
            if (a_equal->isChecked())
            {
                treshold_4 = treshold_3;
                vslider2->setEnabled(false);
            }
            else
            {
                vslider2->setEnabled(true);
                treshold_4 = vslider2->value();
            }
            imgout = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
            cvErode( image, imgout , NULL , treshold_3 );
            cvDilate( imgout, imgout , NULL , treshold_4 );
            imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        }
        if (treshold_1 == 0 && treshold_2 == 0 )
        {
            if ( chk2->isChecked() )
            {
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            }
            else
            {
                imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
            }
        }
        else
        {
            if ( chk2->isChecked() )
            {
                imgout = trmMosbat::doCanny( imgout, treshold_1 ,treshold_2, 3 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            }
            else
            {
                imgout = trmMosbat::doCanny( image, treshold_1 ,treshold_2, 3 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            }
        }
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        chk1->setText("Proportion 3");
        chk2->setText("Dilate+Erode");
		slider1->setMaximum(1000);
        slider2->setMaximum(1000);
        slider2->setMinimum(0);
        vslider1->setMaximum(30);
        vslider2->setMaximum(30);
        vslider1->setEnabled(true);
    }
    else if (calibrate_state == TRM_STATE_CORNER)
    {
        imgout = cvCloneImage(image);
        if (treshold_1 != 0 && treshold_1 < 15)
        {
            trmMosbat::bold_filter(imgout,treshold_1);
        }
        if (treshold_2 != 0 && treshold_2 < 15)
        {
            trmMosbat::narrowFilter(imgout,treshold_2);
        }
        if (treshold_4 != 0 && treshold_4 < 15)
        {
            imgout = trmMosbat::doCanny( imgout, treshold_4 ,treshold_4 *3, 3 );
        }
        if (a_feature->isChecked())
        {
            IplImage *imgclone = cvCloneImage(imgout);
            CvSeq* firstContour = NULL;
            CvMemStorage* cnt_storage = cvCreateMemStorage();
            CvMemStorage* poly_storage = cvCreateMemStorage();
            cvFindContours(imgclone,cnt_storage,&firstContour,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
            cvReleaseImage( &imgclone );
            if (chk2->isChecked())
            {
                cvZero( imgout );
            }
            CvSeq *dummy_seq = firstContour;
            CvSeq *poly = NULL;

            int i = 0;
            cv::Mat matout = imgout;
            while( dummy_seq != NULL )
            {
                poly = cvApproxPoly(dummy_seq,sizeof(CvContour),poly_storage, CV_POLY_APPROX_DP,treshold_3);
                if (poly->total == 12 || a_loop->isChecked())
                {
                    for(i = 0; i < poly->total; i++)
                    {
                        CvPoint *temp_point = (CvPoint*)cvGetSeqElem(poly,i);
                        cv::circle( matout, *temp_point, 10.0, 255, 3, 1 );
                    }
                }
                dummy_seq = dummy_seq->h_next;
            }
            cvClearMemStorage(poly_storage);
            cvReleaseMemStorage(&poly_storage);
            cvClearMemStorage(cnt_storage);
            cvReleaseMemStorage(&cnt_storage);
            slider2->setEnabled(false);
            vslider2->setEnabled(false);
        }
        else if (a_hough->isChecked() )
        {
            CvSeq* firstContour = NULL;
            CvMemStorage* hough_storage = cvCreateMemStorage();
            CvMemStorage* poly_storage = cvCreateMemStorage();
            firstContour = cvHoughLines2(imgout,hough_storage,CV_HOUGH_STANDARD,1,CV_PI/180,treshold_2);
            if (chk2->isChecked())
            {
                cvZero( imgout );
            }
            CvSeq *dummy_seq = firstContour;

            int i = 0;
            cv::Mat matout = imgout;
            for(i = 0; i < dummy_seq->total; i++)
            {
                float *lines = (float *)cvGetSeqElem(dummy_seq,i);
                float rho = lines[0], theta = lines[1];
                CvPoint pt1, pt2;
                double a = cos(theta), b = sin(theta);
                double x0 = a*rho, y0 = b*rho;
                pt1.x = cvRound(x0 + 1000*(-b));
                pt1.y = cvRound(y0 + 1000*(a));
                pt2.x = cvRound(x0 - 1000*(-b));
                pt2.y = cvRound(y0 - 1000*(a));
                cv::line( matout, pt1, pt2, cvScalar(255,255,255), 3, CV_AA);
            }
            cvClearMemStorage(poly_storage);
            cvReleaseMemStorage(&poly_storage);
            cvClearMemStorage(hough_storage);
            cvReleaseMemStorage(&hough_storage);
        }

        imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        chk1->setText("Bold Filter");
        chk2->setText("Zero Input");
        slider1->setMaximum(20);
        slider2->setMaximum(20);
        vslider1->setMaximum(100);
        vslider2->setMaximum(50);
        slider1->setEnabled(chk1->isChecked());
        vslider1->setEnabled(true);
        slider2->setEnabled(true);
        vslider2->setEnabled(true);
    }
    else if (calibrate_state == TRM_STATE_RESULT)
    {
        count = 0;
        if (filter_param.edge_1 == filter_param.edge_2 && filter_param.edge_2 == 0 )
        {
            imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_Indexed8).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        }
        else
        {
            IplImage *imgclone = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
            cvCvtColor( imagesrc, imgclone, CV_BGR2GRAY );
            if (filter_param.erode)
                cvErode( imgclone, imgclone , NULL , filter_param.erode );
            if (filter_param.dilate)
                cvDilate( imgclone, imgclone , NULL , filter_param.dilate );
            IplImage *buffer = imgclone;
            imgclone = trmMosbat::doCanny( imgclone, filter_param.edge_1 ,filter_param.edge_2, 3 );
            cvReleaseImage( &buffer );
            if (filter_param.bold)
                trmMosbat::bold_filter(imgclone,filter_param.bold);
            if (filter_param.narrow)
                trmMosbat::narrowFilter(imgclone,filter_param.narrow);
            if (filter_param.edge_corner)
            {
                buffer = imgclone;
                imgclone = trmMosbat::doCanny( imgclone, filter_param.edge_corner ,filter_param.edge_corner * 3, 3 );
                cvReleaseImage( &buffer );
            }
            CvSeq* firstContour = NULL;
            CvMemStorage* cnt_storage = cvCreateMemStorage();
            cvFindContours(imgclone,cnt_storage,&firstContour,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
            CvMemStorage* poly_storage = cvCreateMemStorage();
            CvSeq *dummy_seq = firstContour;
            CvSeq *poly = NULL;
            trmMosbat *plus;
            imgout = cvCloneImage(imagesrc);
            cv::RNG rng(1234);
            while( dummy_seq != NULL )
            {
                poly = cvApproxPoly(dummy_seq,sizeof(CvContour),poly_storage, CV_POLY_APPROX_DP,filter_param.corner_min);
                CvScalar color = cvScalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
                if (poly->total == 12)
                {
                    count++;
                    plus = create_from_point(poly,0);
                    if (plus != NULL)
                    {
                        cv::Mat mat_temp = imgout;
                        cv::line(mat_temp,(plus->getRect())[0],(plus->getRect())[1],color,2,16);
                        cv::line(mat_temp,(plus->getRect())[1],(plus->getRect())[2],color,2,16);
                        cv::line(mat_temp,(plus->getRect())[2],(plus->getRect())[3],color,2,16);
                        cv::line(mat_temp,(plus->getRect())[3],(plus->getRect())[0],color,2,16);
                        //cv::line(mat_temp,plus->center2,plus->center3,cvScalar(25,25,200),3);
                        //cv::rectangle(mat_temp,plus->getRegion(),color,2,16);
                        cv::circle( mat_temp, plus->middle, 4.0, cvScalar(200,50,0), 6, 1 );
                        //drawMark( mat_temp, plus->middle, cvScalar(0) );
                        delete plus;
                    }
                }
                dummy_seq = dummy_seq->h_next;
            }
            vslider1_label->setText(QString("value = %1").arg(count));
            imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_RGB888).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        }
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setEnabled(false);
        slider2->setEnabled(false);
        vslider1->setEnabled(false);
        vslider2->setEnabled(false);
    }
	else
	{
		;
	}
}
void CalibrateWindow::slider1_change(int value)
{
    treshold_1 = value;
    if (chk1->isChecked())
    {
        if (calibrate_state == TRM_STATE_EDGE)
		{
        	slider2->setValue(value/3);
        }
        else if (calibrate_state == TRM_STATE_RESULT)
		{
        	;
		}
    }
    else
    {
		;
    }
    if (chk2->isChecked())
    {
        if (calibrate_state == TRM_STATE_EDGE)
		{
        	;
        }
        else if (calibrate_state == TRM_STATE_RESULT)
		{
            ;
		}
    }
    else
    {
		;
    }
	state_change();

    slider1_label->setText(QString("value = %1").arg(value));
}

void CalibrateWindow::slider2_change(int value)
{
    treshold_2 = value;
	state_change();
    slider2_label->setText(QString("value = %1").arg(value));
}

void CalibrateWindow::slider3_change(int value)
{
    treshold_3 = value;
    vslider1_label->setText(QString("value = %1").arg(value));
    if (calibrate_state == TRM_STATE_EDGE && a_equal->isEnabled())
    {
       vslider2->setValue(value);
    }
    else
    {
        state_change();
    }
}

void CalibrateWindow::slider4_change(int value)
{
    treshold_4 = value;
    vslider2_label->setText(QString("value = %1").arg(value));
    state_change();
}

void CalibrateWindow::chk1_change()
{
    if (chk1->isChecked())
    {
        if (calibrate_state == TRM_STATE_EDGE)
        {
			slider2->setValue(slider1->value()/3);
            slider2->setEnabled(!chk1->isChecked());
        }
        else if (calibrate_state == TRM_STATE_CORNER)
        {
            ;
		}
    }
	else
	{
        if (calibrate_state == TRM_STATE_EDGE)
        {
            slider2->setEnabled(!chk1->isChecked());
        }
        else if (calibrate_state == TRM_STATE_CORNER)
		{
            ;
		}
    }
    state_change();
}

void CalibrateWindow::chk2_change()
{
    if (chk2->isChecked())
    {
        if (calibrate_state == TRM_STATE_EDGE)
        {
            ;
        }
        else if (calibrate_state == TRM_STATE_RESULT)
		{
            ;
		}
	}
	else
	{
        if (calibrate_state == TRM_STATE_EDGE)
        {
            ;
        }
        else if (calibrate_state == TRM_STATE_RESULT)
		{
            ;
		}
	}
    state_change();
}

void CalibrateWindow::equal_clicked(bool state)
{
    if (state)
    {
        state_change(0);
    }
    else
    {
        state_change(0);
    }
}

void CalibrateWindow::feature_clicked(bool state)
{
    if (state)
    {
        a_corner->setChecked(false);
        a_hough->setChecked(false);
        state_change(1);
    }
    else
    {
        a_feature->setChecked(true);
    }
}

void CalibrateWindow::corner_clicked(bool state)
{
    if (state)
    {
        a_feature->setChecked(false);
        a_hough->setChecked(false);
        state_change(1);
    }
    else
    {
        a_corner->setChecked(true);
    }
}

void CalibrateWindow::hough_clicked(bool state)
{
    if (state)
    {
        treshold_2 = 50;
        a_feature->setChecked(false);
        a_corner->setChecked(false);
        state_change(1);
    }
    else
    {
        a_hough->setChecked(true);
    }
}

void CalibrateWindow::save_clicked()
{
    filename = QFileDialog::getSaveFileName(this, "Save File", "","");
    if (!filename.isEmpty() && !imageView.isNull())
    {
        QPixmap::fromImage(imageView).save(filename.toLocal8Bit().data(),"PNG",100);
	}
}

void CalibrateWindow::back_clicked()
{
    if (calibrate_state == TRM_STATE_FRAME)
    {
        close();
    }
    else if (calibrate_state == TRM_STATE_EDGE)
    {
        if (isVideo)
        {
            calibrate_state = TRM_STATE_FRAME;
            state_change(1);
            slider1->setValue(filter_param.frame_num);
            slider2->setValue(filter_param.calibre_width);
            option_menu->removeAction(a_equal);
            option_menu->addAction(a_width);
        }
        else
        {
            close();
        }
    }
    else if (calibrate_state == TRM_STATE_CORNER)
    {
        if (image != NULL)
        {
            cvReleaseImage(&image);
        }
        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        calibrate_state = TRM_STATE_EDGE;
        state_change(1);
        chk2->setChecked(true);
        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilate);
        option_menu->addAction(a_equal);
        option_menu->removeAction(a_loop);
        algorithm_menu->setEnabled(false);
    }
    else if (calibrate_state == TRM_STATE_RESULT)
    {
        if (image != NULL)
        {
            cvReleaseImage(&image);
        }

        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        if (filter_param.erode)
            cvErode( image, image , NULL , filter_param.erode );
        if (filter_param.dilate)
            cvDilate( image, image , NULL , filter_param.dilate );
        IplImage *buffer = image;
        image = trmMosbat::doCanny( image, filter_param.edge_1 ,filter_param.edge_2, 3 );
        cvReleaseImage( &buffer );
        calibrate_state = TRM_STATE_CORNER;

        option_menu->addAction(a_loop);
        option_menu->setEnabled(true);

        state_change(1);
        slider1->setValue(filter_param.bold);
        vslider1->setValue(filter_param.corner_min);
        slider2->setValue(filter_param.narrow);
        vslider2->setValue(filter_param.edge_corner);
        chk1->setChecked(true);
        algorithm_menu->setEnabled(true);
        next_btn->setText("Next");
    }
}

void CalibrateWindow::next_clicked()
{
    if (imgout != NULL)
    {
        cvReleaseImage(&image);
        image = imgout;
        imgout = NULL;
    }
    if (calibrate_state == TRM_STATE_FRAME)
    {
        filter_param.frame_num = treshold_1;
        if (chk2->isChecked())
        {
            filter_param.calibre_width = treshold_1;
        }
        calibrate_state = TRM_STATE_EDGE;
        state_change(1);
        chk1->setChecked(true);
        chk2->setChecked(true);
        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilate);

        option_menu->addAction(a_equal);
        option_menu->removeAction(a_width);
    }
    else if (calibrate_state == TRM_STATE_EDGE)
    {
        filter_param.edge_1 = treshold_1;
        filter_param.edge_2 = treshold_2;
        filter_param.erode = treshold_3;
        filter_param.dilate = treshold_4;
        treshold_1 = 0;
        treshold_2 = 0;
        calibrate_state = TRM_STATE_CORNER;
        state_change(1);
        vslider1->setValue(filter_param.corner_min);
        chk1->setChecked(true);
        algorithm_menu->setEnabled(true);
        slider1->setValue(filter_param.bold);
        slider2->setValue(filter_param.narrow);
        vslider2->setValue(filter_param.edge_corner);

        option_menu->removeAction(a_equal);
        option_menu->removeAction(a_width);
        option_menu->addAction(a_loop);

    }
    else if (calibrate_state == TRM_STATE_CORNER)
    {
        filter_param.bold = treshold_1;
        filter_param.narrow = treshold_2;
        filter_param.corner_min = treshold_3;
        filter_param.edge_corner = treshold_4;
        calibrate_state = TRM_STATE_RESULT;
        state_change(1);
        next_btn->setText("Finish");
        option_menu->removeAction(a_loop);
        option_menu->setEnabled(false);
        algorithm_menu->setEnabled(false);
    }
    else if (calibrate_state == TRM_STATE_RESULT)
    {
        trmMosbat::Saveparam(filter_param,"settings.json");
        close();
     }


}

void CalibrateWindow::replace_clicked()
{
    if (imgout != NULL)
    {
        cvReleaseImage(&image);
        cvReleaseImage(&imagesrc);
        image = imgout;
        imagesrc = cvCreateImage( cvGetSize(image), 8, 3 );
        cvCvtColor( image, imagesrc, CV_GRAY2BGR );
        imgout = NULL;
    }
}

void CalibrateWindow::open_clicked()
{
    filename = QFileDialog::getOpenFileName(this, "Open File", "","Videos (*.mp4 *.avi *.mov)");
    if (!filename.isEmpty())
    {
        isVideo = true;
        capture = cvCreateFileCapture( filename.toLocal8Bit().data() );
        if (capture == NULL)
            return;
        imagesrc = cvQueryFrame( capture );
        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        int diff = surface_height - floor((surface_width/image->width)*image->height);
        chk2->setChecked(false);

        calibrate_state = TRM_STATE_FRAME;

        option_menu->addAction(a_width);

        vslider1->setValue(0);
        chk1->setChecked(false);
        slider1->setValue(0);
        state_change(1);
        setMinimumHeight(minimumHeight()-diff);
        resize(minimumSize());
    }

}

void CalibrateWindow::openimage_clicked()
{
    filename = QFileDialog::getOpenFileName(this, "Open File", "","Images (*.png *.jpg)");
    if (!filename.isEmpty())
    {
        isVideo = false;
        imagesrc = cvLoadImage(filename.toLocal8Bit().data());
        image = cvLoadImage(filename.toLocal8Bit().data(),CV_LOAD_IMAGE_GRAYSCALE);
        calibrate_state = TRM_STATE_EDGE;
        chk1->setChecked(false);
        slider1->setValue(0);

        option_menu->addAction(a_equal);

        int diff = surface_height - floor((surface_width/image->width)*image->height);
        state_change(1);
        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilate);
        chk2->setChecked(true);
        if ( filter_param.erode == filter_param.dilate && filter_param.dilate == 0 )
        {
            chk2->setChecked(false);
        }
        setMinimumHeight(minimumHeight()-diff);
        resize(minimumSize());
	}
}

void CalibrateWindow::width_clicked()
{
    filter_param.calibre_width = QInputDialog::getInt(this,"Diplay Width","Image Width (200,2000)",surface_width,200,2000);;
    int diff_x = surface_width - filter_param.calibre_width;
    surface_width = filter_param.calibre_width;
    int diff_y = surface_height - floor((surface_width/image->width)*image->height);
    surface_height = floor((surface_width/image->width)*image->height);
    state_change();
    setMinimumSize(minimumWidth() - diff_x,minimumHeight() - diff_y);
    resize(minimumSize());
}

void CalibrateWindow::CreateMenu()
{
    menu = new QMenuBar (this);
    //setMenuBar(menu);
    main_layout->setMenuBar(menu);

    file_menu = menu->addMenu("File");
    a_open = file_menu->addAction("Open");
    a_open_image = file_menu->addAction("Open Image");
    a_save = file_menu->addAction("Save");
    a_replace = file_menu->addAction("Replace");

    algorithm_menu = menu->addMenu("Algorithm");
    a_feature = algorithm_menu->addAction("Feature Detection");
    a_hough = algorithm_menu->addAction("Hough Transform");
    a_corner = algorithm_menu->addAction("Corner Detection");

    option_menu = menu->addMenu("Option");
    a_equal = new QAction("Erode = Dilate",NULL);
    a_width = new QAction("Set Width",NULL);
    a_loop = new QAction("Show All Corner",NULL);


    a_equal->setCheckable(true);
    a_loop->setCheckable(true);
    a_feature->setCheckable(true);
    a_hough->setCheckable(true);
    a_corner->setCheckable(true);

    calibrate_state = TRM_STATE_EDGE;
    a_equal->setChecked(true);
    a_feature->setChecked(true);
    algorithm_menu->setEnabled(false);

    help_menu = menu->addMenu("Help");
    a_about = help_menu->addAction("About");
}

void CalibrateWindow::CreateLayout(QWidget *parent)
{
	//Default
    imgout = NULL;
    surface = new QLabel();
    main_layout = new QVBoxLayout;
    Main_Widget = new QWidget;
	chk1 = new QCheckBox("prop 3");
	chk2 = new QCheckBox("option 2");
    chk2->setChecked(true);

    slider1_layout = new QHBoxLayout;
    slider1_label = new QLabel("value = 0");
    slider1 = new QSlider(Qt::Horizontal);
    slider1->setMaximum(1000);
    slider1_layout->addWidget(slider1_label);
    slider1_layout->addWidget(slider1);
    slider1_layout->addWidget(chk1);
    
    slider2_layout = new QHBoxLayout;
    slider2_label = new QLabel("value = 0");
    slider2 = new QSlider(Qt::Horizontal);
    slider2->setMaximum(1000);
    slider2_layout->addWidget(slider2_label);
    slider2_layout->addWidget(slider2);
    slider2_layout->addWidget(chk2);

    vslider1_layout = new QVBoxLayout;
    vslider1_label = new QLabel("value = 0");
    vslider1 = new QSlider();
    vslider1->setMaximum(30);
    vslider1_layout->addWidget(vslider1);
    vslider1_layout->addWidget(vslider1_label);

    vslider2_layout = new QVBoxLayout;
    vslider2_label = new QLabel("value = 0");
    vslider2 = new QSlider();
    vslider2->setMaximum(30);
    vslider2_layout->addWidget(vslider2);
    vslider2_layout->addWidget(vslider2_label);

    surface_layout = new QHBoxLayout;
    surface_layout->addLayout(vslider1_layout);
    surface_layout->addLayout(vslider2_layout);
    surface_layout->addWidget(surface);

    //Button
	button_layout = new QHBoxLayout;
    back_btn = new QPushButton("Back");
    replace_btn = new QPushButton("Replace");
    next_btn = new QPushButton("Next");
    button_layout->addWidget(back_btn);
    button_layout->addWidget(replace_btn);
    button_layout->addWidget(next_btn);
    //Options
	surface2_layout = new QVBoxLayout;
	option_layout = new QHBoxLayout;
    chkbox_layout = new QHBoxLayout;
    surface2_layout->addLayout(option_layout);
    //Start making layout
    main_layout->addLayout(slider1_layout);
    main_layout->addLayout(slider2_layout);
    main_layout->addLayout(surface2_layout);
    main_layout->addLayout(surface_layout);
    main_layout->addLayout(button_layout);
    //Side object
    //file_name = "/home/bijan/Downloads/IMG_20140630_213804.jpg";
    filter_param = trmMosbat::Loadparam("settings.json");
    //default
    treshold_1 = 0;
    treshold_2 = 0;
    treshold_3 = 0;
    treshold_4 = 0;
    surface_width = filter_param.calibre_width;


    if ( filter_param.erode == filter_param.dilate && filter_param.dilate == 0 )
    {
        chk2->setChecked(false);
    }

    //Window
    setLayout(main_layout);
    setWindowTitle(trUtf8("Calibration"));
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window);
    //setSizePolicy(QSizePolicy::Minimum);
    //setLayoutDirection(Qt::RightToLeft);
}

void CalibrateWindow::MyFilledCircle( cv::Mat img, cv::Point center )
{
 int thickness = 3;
 int lineType = 8;

 cv::circle( img, center, 10.0, 255, thickness, lineType );
}

trmParam CalibrateWindow::start(int startFrame)
{
    if (startFrame && isVideo)
    {
        filter_param.frame_num = startFrame;
        slider1->setValue(filter_param.frame_num);
    }
    exec();
    return filter_param;
}

void CalibrateWindow::find_corner(IplImage* in ,double quality_level ,double min_distance ,int MAX_CORNERS , double k )
{
    cv::Mat grayFrame;
    std::vector<cv::Point2f> corners;
    grayFrame = cv::Mat(in);
    cv::goodFeaturesToTrack(grayFrame, corners, MAX_CORNERS, quality_level,  min_distance ,cv::noArray() ,10,false,k);
    for (int i = 0 ; i < MAX_CORNERS ; i++)
    {
        MyFilledCircle(grayFrame,corners[i]);
    }
    IplImage out = grayFrame;
    imageView = QImage((const unsigned char*)(out.imageData), out.width,out.height,QImage::Format_Indexed8).rgbSwapped();
    surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,floor((surface_width/imageView.width())*imageView.height()),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
}

void CalibrateWindow::drawMark(cv::Mat img ,CvPoint pt,CvScalar color)
{
    int height = floor(img.rows / 100.0);
    cv::line(img,cvPoint(pt.x - height,pt.y),cvPoint(pt.x + height,pt.y),color,2);
    cv::line(img,cvPoint(pt.x,pt.y - height),cvPoint(pt.x,pt.y + height),color,2);
}
