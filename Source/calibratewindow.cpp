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
    connect(a_corner, SIGNAL(triggered(bool)),this,SLOT(state_change()));
    connect(a_width, SIGNAL(triggered(bool)),this,SLOT(width_clicked()));

    filename = filter_param.filename;
    isVideo = filter_param.isVideo;
    if (!isVideo)
    {
        imagesrc = cvLoadImage(filename.toLocal8Bit().data());
        image = cvLoadImage(filename.toLocal8Bit().data(),CV_LOAD_IMAGE_GRAYSCALE);

        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilute);

        option_menu->addAction(a_width);
        option_menu->addAction(a_corner);

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
        a_frame->setEnabled(true);
        a_frame->setChecked(true);
        a_loop->setChecked(false);
        a_edge->setChecked(false);
        a_result->setChecked(false);

        option_menu->addAction(a_width);

        vslider1->setValue(0);
        chk1->setChecked(false);
        slider1->setValue(0);
        state_change(1);
        slider1->setValue(filter_param.frame_num);
        slider2->setValue(filter_param.calibre_width);
    }
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
        treshold_2 = 0;
        treshold_3 = 0;
        slider1->setEnabled(true);
        slider2->setEnabled(true);
        slider1->setValue(0);
        slider2->setValue(0);
        vslider1->setValue(0);
        vslider2->setValue(0);
    }
    surface_height = floor((surface_width/image->width)*image->height);
    if (a_frame->isChecked())
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
    else if (a_edge->isChecked())
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
                IplImage* buffer = imgout;
                imgout = trmMosbat::doCanny( imgout, treshold_1 ,treshold_2, 3 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
                cvReleaseImage( &buffer );
            }
            else
            {
                imgout = trmMosbat::doCanny( image, treshold_1 ,treshold_2, 3 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            }
        }
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        chk1->setText("Proportion 3");
        chk2->setText("Dilute+Erode");
		slider1->setMaximum(1000);
        slider2->setMaximum(1000);
        slider2->setMinimum(0);
        vslider1->setMaximum(30);
        vslider2->setMaximum(30);
        vslider1->setEnabled(true);
    }
    else if (a_loop->isChecked())
    {
        imgout = cvCloneImage(image);
        if (chk1->isChecked())
        {
            trmMosbat::bold_filter(imgout,treshold_1);
        }
        IplImage *imgclone = cvCloneImage(imgout);
        CvSeq* firstContour = NULL;
        CvMemStorage* cnt_storage = cvCreateMemStorage();
        cvFindContours(imgclone,cnt_storage,&firstContour,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
        CvMemStorage* poly_storage = cvCreateMemStorage();
        if (chk2->isChecked())
        {
            cvZero( imgout );
        }
        CvSeq *dummy_seq = firstContour;
        CvSeq *poly = NULL;

        int i = 0;
        while( dummy_seq != NULL )
        {
            poly = cvApproxPoly(dummy_seq,sizeof(CvContour),poly_storage, CV_POLY_APPROX_DP,treshold_3);
            if (poly->total == 12 || a_corner->isChecked())
            {
                for(i = 0; i < poly->total; i++)
                {
                    CvPoint *temp_point = (CvPoint*)cvGetSeqElem(poly,i);
                    cv::Mat mat_temp = imgout;
                    cv::circle( mat_temp, *temp_point, 10.0, 255, 3, 1 );
                }
            }
            dummy_seq = dummy_seq->h_next;
        }

        imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        chk1->setText("Bold Filter");
        chk2->setText("Zero Input");
        slider1->setMaximum(20);
        slider2->setMaximum(5);
        vslider1->setMaximum(100);
        vslider2->setMaximum(50);
        slider1->setEnabled(chk1->isChecked());
        slider2->setEnabled(false);
        vslider1->setEnabled(true);
        vslider2->setEnabled(false);
    }
    else if (a_result->isChecked())
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
            if (filter_param.dilute)
                cvDilate( imgclone, imgclone , NULL , filter_param.dilute );
            IplImage *buffer = imgclone;
            imgclone = trmMosbat::doCanny( imgclone, filter_param.edge_1 ,filter_param.edge_2, 3 );
            cvReleaseImage( &buffer );
            if (filter_param.bold)
                trmMosbat::bold_filter(imgclone,filter_param.bold);
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
        if (a_edge->isChecked())
		{
        	slider2->setValue(value/3);
        }
        else if (a_result->isChecked())
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
        if (a_edge->isChecked())
		{
        	;
        }
        else if (a_result->isChecked())
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
	state_change();
    vslider1_label->setText(QString("value = %1").arg(value));
}

void CalibrateWindow::slider4_change(int value)
{
    treshold_4 = value;
	state_change();
    vslider2_label->setText(QString("value = %1").arg(value));
}

void CalibrateWindow::chk1_change()
{
    if (chk1->isChecked())
    {
        if (a_edge->isChecked())
        {
			slider2->setValue(slider1->value()/3);
            slider2->setEnabled(!chk1->isChecked());
        }
        else if (a_loop->isChecked())
        {
            ;
		}
    }
	else
	{
        if (a_edge->isChecked())
        {
            slider2->setEnabled(!chk1->isChecked());
        }
        else if (a_loop->isChecked())
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
        if (a_edge->isChecked())
        {
            ;
        }
        else if (a_result->isChecked())
		{
            ;
		}
	}
	else
	{
        if (a_edge->isChecked())
        {
            ;
        }
        else if (a_result->isChecked())
		{
            ;
		}
	}
    state_change();
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
    if (a_frame->isChecked())
    {
        close();
    }
    else if (a_edge->isChecked())
    {
        if (a_frame->isEnabled())
        {
            a_edge->setChecked(false);
            a_loop->setChecked(false);
            a_result->setChecked(false);
            a_frame->setChecked(true);
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
    else if (a_loop->isChecked())
    {
        if (image != NULL)
        {
            cvReleaseImage(&image);
        }
        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        a_loop->setChecked(false);
        a_edge->setChecked(true);
        a_result->setChecked(false);
        state_change(1);
        chk2->setChecked(true);
        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilute);
        option_menu->addAction(a_equal);
        option_menu->removeAction(a_corner);
    }
    else if (a_result->isChecked())
    {
        if (image != NULL)
        {
            cvReleaseImage(&image);
        }

        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        if (filter_param.erode)
            cvErode( image, image , NULL , filter_param.erode );
        if (filter_param.dilute)
            cvDilate( image, image , NULL , filter_param.dilute );
        IplImage *buffer = image;
        image = trmMosbat::doCanny( image, filter_param.edge_1 ,filter_param.edge_2, 3 );
        cvReleaseImage( &buffer );
        a_loop->setChecked(true);
        a_edge->setChecked(false);
        a_result->setChecked(false);

        option_menu->addAction(a_corner);
        option_menu->setEnabled(true);

        state_change(1);
        slider1->setValue(filter_param.bold);
        vslider1->setValue(filter_param.corner_min);
        chk1->setChecked(true);
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
    if (a_frame->isChecked())
    {
        filter_param.frame_num = treshold_1;
        if (chk2->isChecked())
        {
            filter_param.calibre_width = treshold_1;
        }
        a_frame->setChecked(false);
        a_loop->setChecked(false);
        a_edge->setChecked(true);
        a_result->setChecked(false);
        state_change(1);
        chk1->setChecked(true);
        chk2->setChecked(true);
        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilute);

        option_menu->addAction(a_equal);
        option_menu->removeAction(a_width);
    }
    else if (a_edge->isChecked())
    {
        filter_param.edge_1 = treshold_1;
        filter_param.edge_2 = treshold_2;
        filter_param.erode = treshold_3;
        filter_param.dilute = treshold_4;
        a_loop->setChecked(true);
        a_frame->setChecked(false);
        a_edge->setChecked(false);
        a_result->setChecked(false);
        state_change(1);
        vslider1->setValue(filter_param.corner_min);
        chk1->setChecked(true);
        slider1->setValue(filter_param.bold);

        option_menu->removeAction(a_equal);
        option_menu->removeAction(a_width);
        option_menu->addAction(a_corner);
    }
    else if (a_loop->isChecked())
    {
        filter_param.bold = treshold_1;
        filter_param.corner_min = treshold_3;
        a_frame->setChecked(false);
        a_loop->setChecked(false);
        a_edge->setChecked(false);
        a_result->setChecked(true);
        state_change(1);
        next_btn->setText("Finish");
        option_menu->removeAction(a_corner);
        option_menu->setEnabled(false);
    }
    else if (a_result->isChecked())
    {
        /*Creating a json object*/
        // ---- create from scratch ----

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
        a_frame->setEnabled(true);
        a_frame->setChecked(true);
        a_loop->setChecked(false);
        a_edge->setChecked(false);
        a_result->setChecked(false);

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
        a_frame->setEnabled(false);
        a_frame->setChecked(false);
        a_loop->setChecked(false);
        a_edge->setChecked(true);
        a_result->setChecked(false);
        chk1->setChecked(false);
        slider1->setValue(0);

        option_menu->addAction(a_equal);

        int diff = surface_height - floor((surface_width/image->width)*image->height);
        state_change(1);
        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);
        vslider1->setValue(filter_param.erode);
        vslider2->setValue(filter_param.dilute);
        chk2->setChecked(true);
        if ( filter_param.erode == filter_param.dilute && filter_param.dilute == 0 )
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

    a_frame = new QAction("Select Image",NULL);
    a_edge = new QAction("Edge Detection",NULL);
    a_loop = new QAction("Loop Detection",NULL);
    a_result = new QAction("Result",NULL);

    option_menu = menu->addMenu("Option");
    a_equal = new QAction("Erode = Dilute",NULL);
    a_width = new QAction("Set Width",NULL);
    a_corner = new QAction("Show All Corner",NULL);

    a_edge->setCheckable(true);
    a_loop->setCheckable(true);
    a_result->setCheckable(true);
    a_equal->setCheckable(true);
    a_frame->setCheckable(true);
    a_corner->setCheckable(true);

    a_edge->setChecked(true);
    a_equal->setChecked(true);

    a_frame->setEnabled(false);

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


    if ( filter_param.erode == filter_param.dilute && filter_param.dilute == 0 )
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
