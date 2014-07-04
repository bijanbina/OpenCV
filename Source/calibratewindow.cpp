#include "calibratewindow.h"

CalibrateWindow::CalibrateWindow(QWidget *parent) :
    QMainWindow(parent)
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
    connect(a_result, SIGNAL(triggered(bool)),this,SLOT(result_clicked(bool)));
    connect(a_edge, SIGNAL(triggered(bool)),this,SLOT(edgedetect_clicked(bool)));
    connect(a_loop, SIGNAL(triggered(bool)),this,SLOT(loop_clicked(bool)));
    connect(a_equal, SIGNAL(triggered(bool)),this,SLOT(equal_clicked(bool)));
    connect(a_frame, SIGNAL(triggered(bool)),this,SLOT(frame_clicked(bool)));
    connect(a_replace, SIGNAL(triggered(bool)),this,SLOT(replace_clicked()));

    imagesrc = cvLoadImage(file_name);
    image = cvLoadImage(file_name,CV_LOAD_IMAGE_GRAYSCALE);

    slider1->setValue(filter_param.edge_1);
    slider2->setValue(filter_param.edge_2);
    vslider1->setValue(filter_param.erode);
    vslider2->setValue(filter_param.dilute);
    state_change();
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
    surface_height = floor((calib_prev_size/image->width)*image->height);
    if (a_frame->isChecked())
    {
        int frames = (int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT );
        if (frames - 100 < 0)
            return;
        //cvSetCaptureProperty(capture,CV_CAP_PROP_POS_MSEC,treshold_1);
        if (treshold_1 + 100 > frames )
            return;
        cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,treshold_1);
        std::cout << cvGrabFrame( capture ) << "\t" << frames << std::endl;
        if (!cvGrabFrame( capture ))
            return;
        imagesrc = cvQueryFrame( capture );
        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(calib_prev_size,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setMaximum(frames - 100);
        slider2->setMaximum(2000);
        vslider1->setEnabled(false);
        vslider2->setEnabled(false);
    }
    if (a_edge->isChecked())
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
            surface->setPixmap(QPixmap::fromImage(imageView.scaled(calib_prev_size,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
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
                imgout = doCanny( imgout, treshold_1 ,treshold_2, 3 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
                cvReleaseImage( &buffer );
            }
            else
            {
                imgout = doCanny( image, treshold_1 ,treshold_2, 3 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            }
        }
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(calib_prev_size,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        chk1->setText("Proportion 3");
        chk2->setText("Dilute+Erode");
		slider1->setMaximum(1000);
        slider2->setMaximum(1000);
        vslider1->setMaximum(30);
        vslider2->setMaximum(30);
        vslider1->setEnabled(true);
    }
    else if (a_loop->isChecked())
    {
        imgout = cvCloneImage(image);
        if (chk1->isChecked())
        {
            bold_filter(imgout,treshold_1);
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
            if (poly->total == 12)
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
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(calib_prev_size,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
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
        if (filter_param.edge_1 == filter_param.edge_2 && filter_param.edge_2 == 0 )
        {
            imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_Indexed8).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView.scaled(calib_prev_size,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
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
            imgclone = doCanny( imgclone, filter_param.edge_1 ,filter_param.edge_2, 3 );
            cvReleaseImage( &buffer );
            if (filter_param.bold)
                bold_filter(imgclone,filter_param.bold);
            CvSeq* firstContour = NULL;
            CvMemStorage* cnt_storage = cvCreateMemStorage();
            cvFindContours(imgclone,cnt_storage,&firstContour,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
            CvMemStorage* poly_storage = cvCreateMemStorage();
            CvSeq *dummy_seq = firstContour;
            CvSeq *poly = NULL;
            trmMosbat *first_plus = new trmMosbat;
            trmMosbat *current_plus = first_plus;
            int i = 0;
            imgout = cvCloneImage(imagesrc);
            cv::RNG rng(1234);
            while( dummy_seq != NULL )
            {
                poly = cvApproxPoly(dummy_seq,sizeof(CvContour),poly_storage, CV_POLY_APPROX_DP,filter_param.corner_min);
                CvScalar color = cvScalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));
                if (poly->total == 12)
                {
                    i++;
                    current_plus->next = new trmMosbat(poly,0);
                    current_plus = current_plus->next;
                    cv::Mat mat_temp = imgout;
                    cv::line(mat_temp,(current_plus->getRect())[0],(current_plus->getRect())[1],color,2,16);
                    cv::line(mat_temp,(current_plus->getRect())[1],(current_plus->getRect())[2],color,2,16);
                    cv::line(mat_temp,(current_plus->getRect())[2],(current_plus->getRect())[3],color,2,16);
                    cv::line(mat_temp,(current_plus->getRect())[3],(current_plus->getRect())[0],color,2,16);
                    //cv::line(mat_temp,current_plus->center2,current_plus->center3,cvScalar(25,25,200),3);
                    cv::circle( mat_temp, current_plus->middle, 5.0, 0, 2, 1 );
                    //drawMark( mat_temp, current_plus->middle, cvScalar(0) );
                }
                dummy_seq = dummy_seq->h_next;
            }
            imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_RGB888).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView.scaled(calib_prev_size,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
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
    file_name = QFileDialog::getSaveFileName(this, "Save File", "","").toLocal8Bit().data();
    if (strcmp(file_name,"") && !imageView.isNull())
    {
        QPixmap::fromImage(imageView).save(file_name,"PNG",100);
	}
}

void CalibrateWindow::back_clicked()
{

    if (a_loop->isChecked())
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
        image = doCanny( image, filter_param.edge_1 ,filter_param.edge_2, 3 );
        cvReleaseImage( &buffer );
        a_loop->setChecked(true);
        a_edge->setChecked(false);
        a_result->setChecked(false);
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
        a_frame->setChecked(false);
        a_loop->setChecked(false);
        a_edge->setChecked(true);
        a_result->setChecked(false);
        state_change(1);
        vslider1->setValue(0);
        chk1->setChecked(false);
        slider1->setValue(0);
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
    }
    else if (a_result->isChecked())
    {
        /*Creating a json object*/
        // ---- create from scratch ----

        Json::Value json_main;
        Json::Value edge;
        edge["Treshold 1"] = filter_param.edge_1;
        edge["Treshold 2"] = filter_param.edge_2;
        json_main["Erode"] = filter_param.erode;
        json_main["Dilate"] = filter_param.dilute;
        json_main["Bold"] = filter_param.bold;
        json_main["Corner Minimum Distance"] = filter_param.corner_min;
        json_main["Edge Detection"] = edge;

        // write in a nice readible way
        Json::StyledWriter styledWriter;
        std::string str = styledWriter.write(json_main);
        std::vector<char> data(str.begin(), str.end());
        data.push_back('\0');
        QFile file;
        file.setFileName("settings.json");
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        file.write(data.data());
        file.close();

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
    file_name = QFileDialog::getOpenFileName(this, "Open File", "","Videos (*.mp4 *.avi)").toLocal8Bit().data();
    if (strcmp(file_name,""))
    {
        capture = cvCreateFileCapture( file_name );
        if (capture == NULL)
            return;
        imagesrc = cvQueryFrame( capture );
        image = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
        int diff = surface_height - floor((calib_prev_size/image->width)*image->height);
        cvCvtColor( imagesrc, image, CV_BGR2GRAY );
        a_frame->setEnabled(true);
        a_frame->setChecked(true);
        a_loop->setChecked(false);
        a_edge->setChecked(false);
        a_result->setChecked(false);
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
    file_name = QFileDialog::getOpenFileName(this, "Open File", "","Images (*.png *.jpg)").toLocal8Bit().data();
    if (strcmp(file_name,""))
    {
        imagesrc = cvLoadImage(file_name);
        image = cvLoadImage(file_name,CV_LOAD_IMAGE_GRAYSCALE);
        a_frame->setEnabled(false);
        a_frame->setChecked(false);
        a_loop->setChecked(false);
        a_edge->setChecked(true);
        a_result->setChecked(false);
        chk1->setChecked(false);
        slider1->setValue(0);
        int diff = surface_height - floor((calib_prev_size/image->width)*image->height);
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

void CalibrateWindow::frame_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_loop->setChecked(false);
        a_result->setChecked(false);
        state_change(1);
    }
    else
    {
        a_frame->setChecked(true);
    }
}

void CalibrateWindow::result_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_loop->setChecked(false);
        a_frame->setChecked(false);
        state_change(1);
    }
    else
    {
        a_result->setChecked(true);
    }
}

void CalibrateWindow::edgedetect_clicked(bool state)
{
    if (state)
    {
        a_result->setChecked(false);
        a_loop->setChecked(false);
        a_frame->setChecked(false);
        state_change(1);
    }
    else
    {
        a_edge->setChecked(true);
    }
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

void CalibrateWindow::loop_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_result->setChecked(false);
        a_frame->setChecked(false);
        state_change(1);
        vslider1->setValue(9);
    }
    else
    {
        a_loop->setChecked(true);
    }
}

void CalibrateWindow::CreateMenu()
{
    menu = new QMenuBar (this);
    setMenuBar(menu);

    file_menu = menu->addMenu("File");
    a_open = file_menu->addAction("Open");
    a_open_image = file_menu->addAction("Open Image");
    a_save = file_menu->addAction("Save");
    a_replace = file_menu->addAction("Replace");

    mode_menu = menu->addMenu("Mode");
    a_frame = mode_menu->addAction("Select Image");
    a_edge = mode_menu->addAction("Edge Detection");
    a_loop = mode_menu->addAction("Loop Detection");
    a_result = mode_menu->addAction("Result");

    option_menu = menu->addMenu("Option");
    a_equal = option_menu->addAction("Erode = Dilute");

    a_edge->setCheckable(true);
    a_loop->setCheckable(true);
    a_result->setCheckable(true);
    a_equal->setCheckable(true);
    a_frame->setCheckable(true);

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
    file_name = "/home/bijan/Pictures/IMG_20140519_090048.jpg";
    //default
    treshold_1 = 0;
    treshold_2 = 0;
    treshold_3 = 0;
    treshold_4 = 0;

    QFile json_file("settings.json");
    if(json_file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        Json::Value json_obj;
        Json::Reader reader;
        if (reader.parse(QString(json_file.readAll()).toUtf8().data(), json_obj))
        {
            filter_param.bold = json_obj.get("Bold",0).asInt();
            filter_param.erode = json_obj.get("Erode",0).asInt();
            filter_param.dilute = json_obj.get("Dilate",0).asInt();
            const Json::Value edge = json_obj["Edge Detection"];
            if (!edge.empty())
            {
                filter_param.edge_1 = edge.get("Treshold 1",0).asDouble();
                filter_param.edge_2 = edge.get("Treshold 2",0).asDouble();
            }
            filter_param.corner_min = json_obj.get("Corner Minimum Distance",0).asInt();
        }

    }
    else
    {
        filter_param.bold = 0;
        filter_param.erode = 0;
        filter_param.dilute = 0;
        filter_param.edge_1 = 0;
        filter_param.edge_2 = 0;
        filter_param.corner_min = 0;
    }

    if ( filter_param.erode == filter_param.dilute && filter_param.dilute == 0 )
    {
        chk2->setChecked(false);
    }

    //Window
    Main_Widget->setLayout(main_layout);
    setWindowTitle(trUtf8("Calibration"));
    setCentralWidget(Main_Widget);
    setAttribute(Qt::WA_DeleteOnClose);
    //setSizePolicy(QSizePolicy::Minimum);
    //setLayoutDirection(Qt::RightToLeft);
}

IplImage* CalibrateWindow::doCanny( IplImage* in, double lowThresh, double highThresh, double aperture )
{
    if(in->nChannels != 1)
    {
        printf("Not supported\n");
        exit(0); //Canny only handles gray scale images
    }
    IplImage *out = cvCreateImage( cvGetSize( in ) , IPL_DEPTH_8U, 1 );
    cvCanny( in, out, lowThresh, highThresh, aperture );
    return( out );
}

void CalibrateWindow::MyFilledCircle( cv::Mat img, cv::Point center )
{
 int thickness = 3;
 int lineType = 8;

 cv::circle( img, center, 10.0, 255, thickness, lineType );
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
    surface->setPixmap(QPixmap::fromImage(imageView.scaled(calib_prev_size,floor((calib_prev_size/imageView.width())*imageView.height()),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
}

void CalibrateWindow::bold_filter(IplImage *in,int kernel_size)
{
    cv::Mat grayFrame = cv::Mat(in);
    unsigned char imgdata[grayFrame.cols][grayFrame.rows];
    for (int y = 0 ; y < grayFrame.rows ; y++)
    {
        const unsigned char* ptr = (unsigned char*)(grayFrame.data + y * grayFrame.step);
        for (int x = 0 ; x < grayFrame.cols ; x++ )
        {
            imgdata[x][y] = *ptr;
            ptr++;
        }
    }
    for (int x = 0 ; x < grayFrame.cols - kernel_size ; x++)
    {
        for (int y = 0 ; y < grayFrame.rows - kernel_size ; y++ )
        {
            if (imgdata[x+kernel_size/2][y+kernel_size/2] == 255 )
            {
                cvRectangle(in,cvPoint(x,y),cvPoint(x+kernel_size,y+kernel_size),cvScalarAll(255));

            }
        }
    }
}

void CalibrateWindow::drawMark(cv::Mat img ,CvPoint pt,CvScalar color)
{
    int height = floor(img.rows / 100.0);
    cv::line(img,cvPoint(pt.x - height,pt.y),cvPoint(pt.x + height,pt.y),color,2);
    cv::line(img,cvPoint(pt.x,pt.y - height),cvPoint(pt.x,pt.y + height),color,2);
}
