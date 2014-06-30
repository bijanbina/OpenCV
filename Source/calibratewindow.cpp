#include "calibratewindow.h"

CalibrateWindow::CalibrateWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateLayout();
    CreateMenu();
	QSignalMapper* signalMapper = new QSignalMapper (this) ;

    connect(slider1,SIGNAL(valueChanged(int)), this, SLOT(slider1_change(int)));
    connect(slider2,SIGNAL(valueChanged(int)), this, SLOT(slider2_change(int)));
    connect(vslider1,SIGNAL(valueChanged(int)), this, SLOT(slider3_change(int)));
    connect(vslider2,SIGNAL(valueChanged(int)), this, SLOT(slider4_change(int)));
    connect(chk1,SIGNAL(stateChanged(int)), this, SLOT(chk1_change(int)));
    connect(chk2,SIGNAL(stateChanged(int)), this, SLOT(chk2_change(int)));
    connect(open_btn, SIGNAL(released()), this, SLOT(open_clicked()));
    connect(save_btn, SIGNAL(released()), this, SLOT(save_clicked()));
    connect(calibrate_btn, SIGNAL(released()), this, SLOT(calibrate_clicked()));

    connect(a_open, SIGNAL(triggered(bool)),this,SLOT(open_clicked()));
    connect(a_save, SIGNAL(triggered(bool)),this,SLOT(save_clicked()));
    connect(a_corner, SIGNAL(triggered(bool)),this,SLOT(cornerdetect_clicked(bool)));
    connect(a_edge, SIGNAL(triggered(bool)),this,SLOT(edgedetect_clicked(bool)));
    connect(a_erode, SIGNAL(triggered(bool)),this,SLOT(erode_clicked(bool)));
    connect(a_dilute, SIGNAL(triggered(bool)),this,SLOT(dilute_clicked(bool)));
    connect(a_mix, SIGNAL(triggered(bool)),this,SLOT(mix_clicked(bool)));
    connect(a_loop, SIGNAL(triggered(bool)),this,SLOT(loop_clicked(bool)));
    connect(a_next, SIGNAL(triggered(bool)),this,SLOT(next_clicked()));

    openImage();
}

CalibrateWindow::~CalibrateWindow()
{
}

//Open image function call
void CalibrateWindow::openImage()
{
    imagesrc = cvLoadImage(file_name);
    image = cvLoadImage(file_name,CV_LOAD_IMAGE_GRAYSCALE);
	state_change();
}

void CalibrateWindow::state_change(int changed)
{
    if (imgout != NULL)
	{
        cvReleaseImage(&imgout);
	}
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
    if (a_edge->isChecked())
	{
    	if (image != NULL)
    	{
        	if (treshold_1 == 0 && treshold_2 == 0 )
        	{
				if ( chk2_state )
				{
                    imgout = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
                    cvErode( image, imgout , NULL , treshold_3 );
                    cvDilate( imgout, imgout , NULL , treshold_4 );
                    imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
                    surface->setPixmap(QPixmap::fromImage(imageView));
				}
				else
				{
                    imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
            		surface->setPixmap(QPixmap::fromImage(imageView));
				}
        	}
        	else
        	{
				if ( chk2_state )
				{
            		IplImage* buffer = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
					cvErode( image, buffer , NULL , treshold_3 );
					cvDilate( buffer, buffer , NULL , treshold_4 );
                    imgout = doCanny( buffer, treshold_1 ,treshold_2, 3 );
                    imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            		surface->setPixmap(QPixmap::fromImage(imageView));
                    cvReleaseImage( &buffer );
				}
				else
                {
                    imgout = doCanny( image, treshold_1 ,treshold_2, 3 );
                    imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
                    surface->setPixmap(QPixmap::fromImage(imageView));
				}
        	}
			chk1->setText("Proportion 3");
			chk2->setText("Dilute & Erode");
    	}
		slider1->setMaximum(1000);
		slider2->setMaximum(1000);
	}
    else if (a_erode->isChecked())
	{
    	if (image != NULL)
    	{
        	if (treshold_1 == 0 && treshold_2 == 0 )
        	{
                imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
            	surface->setPixmap(QPixmap::fromImage(imageView));
        	}
        	else
        	{
                imgout = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
                cvErode( image, imgout , NULL , treshold_1 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
                surface->setPixmap(QPixmap::fromImage(imageView));
        	}
    	}
		chk1->setText("Null");
		chk2->setText("Null");
		slider1->setMaximum(30);
		slider2->setMaximum(30);
	}
    else if (a_dilute->isChecked())
	{
		if (image != NULL)
    	{
        	if (treshold_1 == 0 && treshold_2 == 0 )
        	{
                imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
            	surface->setPixmap(QPixmap::fromImage(imageView));
        	}
        	else
        	{
                imgout = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
                cvDilate( image, imgout , NULL , treshold_1 );
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
                surface->setPixmap(QPixmap::fromImage(imageView));
        	}
    	}
		chk1->setText("Null");
		chk2->setText("Null");
		slider1->setMaximum(30);
		slider2->setMaximum(30);
	}
    else if (a_mix->isChecked())
	{
		if (image != NULL)
    	{
        	if (treshold_1 == 0 && treshold_2 == 0 )
        	{
                imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
            	surface->setPixmap(QPixmap::fromImage(imageView));
        	}
        	else
        	{
                imgout = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
				if (chk1->isChecked())
				{
                    cvDilate( image, imgout , NULL , treshold_1 );
                    cvErode( imgout, imgout , NULL , treshold_2 );
				}
				else
				{
                    cvErode( image, imgout , NULL , treshold_1 );
                    cvDilate( imgout, imgout , NULL , treshold_2 );
				}
                imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
                surface->setPixmap(QPixmap::fromImage(imageView));
        	}
    	}
		chk1->setText("Reverse");
		chk2->setText("Dilute=Erode");
		slider1->setMaximum(30);
		slider2->setMaximum(30);
    }
    else if (a_corner->isChecked())
    {
        if (image != NULL)
        {
            if (treshold_1 == 0 && treshold_2 == 0 )
            {
                imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
                surface->setPixmap(QPixmap::fromImage(imageView));
            }
            else
            {
                calibrate_clicked();
            }
        }
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setMaximum(900);
        slider2->setMaximum(900);
        vslider1->setMaximum(300);
        vslider2->setEnabled(false);
    }
    else if (a_loop->isChecked())
    {
        if (image != NULL)
        {
            imgout = cvCloneImage(image);
            IplImage *imgclone = cvCloneImage(image);
            CvSeq* firstContour = NULL;
            CvMemStorage* cnt_storage = cvCreateMemStorage();
            cvFindContours(imgclone,cnt_storage,&firstContour,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
            CvMemStorage* poly_storage = cvCreateMemStorage();
            cvZero( imgout );
            for(volatile size_t i = 0; i < firstContour->total; i++)
            {
                CvSeq* poly = cvApproxPoly(cvGetSeqElem(firstContour,i),sizeof(CvContour),poly_storage, CV_POLY_APPROX_DP,treshold_3);
                cvDrawContours( imgout,poly, cvScalarAll(255), cvScalarAll(255),100);
            }
            imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView));
        }
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setMaximum(4);
        slider2->setMaximum(5);
        vslider1->setMaximum(100);
        vslider1->setEnabled(true);
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
    if (chk1_state)
    {
        if (a_edge->isChecked())
		{
        	slider2->setValue(value/3);
		}
        else if (a_erode->isChecked())
		{
        	;
		}
        else if (a_dilute->isChecked())
		{
			;
		}
        else if (a_mix->isChecked())
		{
        	;
		}
    }
    else
    {
		;
    }
    if (chk2_state)
    {
        if (a_edge->isChecked())
		{
        	;
		}
        else if (a_erode->isChecked())
		{
        	;
		}
        else if (a_dilute->isChecked())
		{
			;
		}
        else if (a_mix->isChecked())
		{
        	slider2->setValue(value);
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

void CalibrateWindow::chk1_change(int value)
{
    chk1_state = value;
    if (chk1_state)
    {
        if (a_edge->isChecked())
        {
			slider2->setValue(slider1->value()/3);
        	slider2->setEnabled(!chk1_state);
		}
        else if (a_erode->isChecked())
		{
			;
		}
        else if (a_dilute->isChecked())
		{
			;
		}
        else if (a_mix->isChecked())
		{
            state_change();
		}
    }
	else
	{
        if (a_edge->isChecked())
        {
        	slider2->setEnabled(!chk1_state);
		}
        else if (a_erode->isChecked())
		{
			;
		}
        else if (a_dilute->isChecked())
		{
			;
		}
        else if (a_mix->isChecked())
		{
			state_change();
		}
    }
}

void CalibrateWindow::chk2_change(int value)
{
    chk2_state = value;
    if (chk2_state)
    {
        if (a_edge->isChecked())
        {
			state_change();
		}
        else if (a_erode->isChecked())
		{
			;
		}
        else if (a_dilute->isChecked())
		{
			;
		}
        else if (a_mix->isChecked())
		{
			slider2->setValue(slider1->value());
        	slider2->setEnabled(!chk2_state);
		}
	}
	else
	{
        if (a_edge->isChecked())
        {
        	state_change();
		}
        else if (a_erode->isChecked())
		{
			;
		}
        else if (a_dilute->isChecked())
		{
			;
		}
        else if (a_mix->isChecked())
		{
			slider2->setEnabled(!chk2_state);	
		}
	}
}

void CalibrateWindow::save_clicked()
{
    file_name = QFileDialog::getSaveFileName(this, "Save File", "","").toLocal8Bit().data();
    if (strcmp(file_name,"") && !imageView.isNull())
    {
		QPixmap::fromImage(imageView).save(file_name,"PNG",100);
	}
}

void CalibrateWindow::calibrate_clicked()
{
    double quality_level = treshold_1/1000.0;
    double min_distance = treshold_3/10.0;
    int maxCorner = 100;
    double k = treshold_2/100.0;
    IplImage* input=cvCloneImage(image);
    find_corner(input,quality_level,min_distance,maxCorner,k);
}

void CalibrateWindow::next_clicked()
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
    file_name = QFileDialog::getOpenFileName(this, "Open File", "","Images (*.png *.jpg)").toLocal8Bit().data();
    if (strcmp(file_name,""))
    {
        openImage();
	}
}

void CalibrateWindow::cornerdetect_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_erode->setChecked(false);
        a_dilute->setChecked(false);
        a_mix->setChecked(false);
        state_change(1);
    }
    else
    {
        a_corner->setChecked(true);
    }
}

void CalibrateWindow::edgedetect_clicked(bool state)
{
    if (state)
    {
        a_corner->setChecked(false);
        a_erode->setChecked(false);
        a_dilute->setChecked(false);
        a_mix->setChecked(false);
        a_loop->setChecked(false);
        state_change(1);
    }
    else
    {
        a_edge->setChecked(true);
    }
}

void CalibrateWindow::erode_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_corner->setChecked(false);
        a_dilute->setChecked(false);
        a_mix->setChecked(false);
        a_loop->setChecked(false);
        state_change(1);
    }
    else
    {
        a_erode->setChecked(true);
    }
}

void CalibrateWindow::dilute_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_erode->setChecked(false);
        a_corner->setChecked(false);
        a_mix->setChecked(false);
        a_loop->setChecked(false);
        state_change(1);
    }
    else
    {
        a_dilute->setChecked(true);
    }
}

void CalibrateWindow::mix_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_erode->setChecked(false);
        a_corner->setChecked(false);
        a_dilute->setChecked(false);
        a_loop->setChecked(false);
        state_change(1);
    }
    else
    {
        a_mix->setChecked(true);
    }
}

void CalibrateWindow::loop_clicked(bool state)
{
    if (state)
    {
        a_edge->setChecked(false);
        a_erode->setChecked(false);
        a_corner->setChecked(false);
        a_dilute->setChecked(false);
        a_mix->setChecked(false);
        state_change(1);
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
    a_save = file_menu->addAction("Save");
    a_next = file_menu->addAction("Next");

    mode_menu = menu->addMenu("Mode");
    a_edge = mode_menu->addAction("Edge Detection");
    a_dilute = mode_menu->addAction("Dilute");
    a_erode = mode_menu->addAction("Erode");
    a_mix = mode_menu->addAction("Erode + Dilute");
    a_corner = mode_menu->addAction("Corner Detection");
    a_loop = mode_menu->addAction("Loop Detection");

    a_edge->setCheckable(true);
    a_dilute->setCheckable(true);
    a_erode->setCheckable(true);
    a_mix->setCheckable(true);
    a_loop->setCheckable(true);
    a_corner->setCheckable(true);

    a_edge->setChecked(true);

    help_menu = menu->addMenu("Help");
    a_about = help_menu->addAction("About");
}

void CalibrateWindow::CreateLayout()
{
	//Default
    imgout = NULL;
    surface = new QLabel();
    main_layout = new QVBoxLayout;
    Main_Widget = new QWidget;
	chk1 = new QCheckBox("prop 3");
	chk2 = new QCheckBox("option 2");
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
    open_btn = new QPushButton("Open");
    calibrate_btn = new QPushButton("Calibrate");
    save_btn = new QPushButton("Save");
	button_layout->addWidget(open_btn);
    button_layout->addWidget(calibrate_btn);
	button_layout->addWidget(save_btn);
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
    file_name = "/home/bijan/2.png";
    chk1_state = 0;
    chk2_state = 0;
	treshold_1 = 0;
	treshold_2 = 0;
    //Window
    Main_Widget->setLayout(main_layout);
    setWindowTitle(trUtf8("Calibration"));
    setCentralWidget(Main_Widget);
    //setLayoutDirection(Qt::RightToLeft);
}

void CalibrateWindow::trLoop(CvMat *in, CvMat *out)
{
    ;
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

IplImage* CalibrateWindow::doPyrDown( IplImage* in, int filter)
{
    // Best to make sure input image is divisible by two.
    //
    assert( in->width%2 == 0 && in->height%2 == 0 );
    IplImage *out = cvCreateImage(
                cvSize( in->width/2, in->height/2 ),
                in->depth,
                in->nChannels
                );
    cvPyrDown( in, out , CV_GAUSSIAN_5x5);
    return( out );
};

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
    int ct_type = grayFrame.type();
    cv::goodFeaturesToTrack(grayFrame, corners, MAX_CORNERS, quality_level,  min_distance ,cv::noArray() ,10,false,k);
    for (int i = 0 ; i < MAX_CORNERS ; i++)
    {
        MyFilledCircle(grayFrame,corners[i]);
    }
    IplImage out = grayFrame;
    imageView = QImage((const unsigned char*)(out.imageData), out.width,out.height,QImage::Format_Indexed8).rgbSwapped();
    surface->setPixmap(QPixmap::fromImage(imageView));
}
