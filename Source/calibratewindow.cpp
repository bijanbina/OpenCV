#include "calibratewindow.h"

CalibrateWindow::CalibrateWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateLayout();
    CreateMenu();

    connect(slider1,SIGNAL(valueChanged(int)), this, SLOT(slider1_change(int)));
    connect(slider2,SIGNAL(valueChanged(int)), this, SLOT(slider2_change(int)));
    connect(vslider1,SIGNAL(valueChanged(int)), this, SLOT(slider3_change(int)));
    connect(vslider2,SIGNAL(valueChanged(int)), this, SLOT(slider4_change(int)));
    connect(chk1,SIGNAL(stateChanged(int)), this, SLOT(chk1_change()));
    connect(chk2,SIGNAL(stateChanged(int)), this, SLOT(chk2_change()));
    connect(back_btn, SIGNAL(released()), this, SLOT(open_clicked()));
    connect(next_btn, SIGNAL(released()), this, SLOT(next_clicked()));
    connect(replace_btn, SIGNAL(released()), this, SLOT(replace_clicked()));

    connect(a_open, SIGNAL(triggered(bool)),this,SLOT(open_clicked()));
    connect(a_save, SIGNAL(triggered(bool)),this,SLOT(save_clicked()));
    connect(a_corner, SIGNAL(triggered(bool)),this,SLOT(cornerdetect_clicked(bool)));
    connect(a_edge, SIGNAL(triggered(bool)),this,SLOT(edgedetect_clicked(bool)));
    connect(a_loop, SIGNAL(triggered(bool)),this,SLOT(loop_clicked(bool)));
    connect(a_bijoo, SIGNAL(triggered(bool)),this,SLOT(bijoo_clicked(bool)));
    connect(a_equal, SIGNAL(triggered(bool)),this,SLOT(equal_clicked(bool)));
    connect(a_replace, SIGNAL(triggered(bool)),this,SLOT(replace_clicked()));

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
                surface->setPixmap(QPixmap::fromImage(imageView));
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
            surface->setPixmap(QPixmap::fromImage(imageView));
            chk1->setText("Proportion 3");
            chk2->setText("Dilute+Erode");
    	}
		slider1->setMaximum(1000);
        slider2->setMaximum(1000);
        vslider1->setMaximum(30);
        vslider2->setMaximum(30);
        vslider1->setEnabled(true);
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
                double quality_level = treshold_1/1000.0;
                double min_distance = treshold_3/10.0;
                int maxCorner = 100;
                double k = treshold_2/100.0;
                IplImage* input=cvCloneImage(image);
                find_corner(input,quality_level,min_distance,maxCorner,k);
            }
        }
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setMaximum(900);
        slider2->setMaximum(900);
        vslider1->setMaximum(1000);
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
            CvSeq* dummy_seq = firstContour;
            CvSeq  *poly = NULL;

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
//            while( i < treshold_4 )
//            {
//                dummy_seq = dummy_seq->h_next;
//                i++;
//            }
//            poly = cvApproxPoly(dummy_seq,sizeof(CvContour),poly_storage, CV_POLY_APPROX_DP,treshold_3);
//            //cvDrawContours( imgout,poly, cvScalarAll(255), cvScalarAll(255),100);

            imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView));
        }
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setMaximum(4);
        slider2->setMaximum(5);
        vslider1->setMaximum(100);
        vslider2->setMaximum(50);
        vslider1->setEnabled(true);
        vslider2->setEnabled(true);
    }
    else if (a_bijoo->isChecked())
    {
        if (image != NULL)
        {
            imgout = cvCloneImage(image);
            bijoo_filter(imgout,treshold_3);
            imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
            surface->setPixmap(QPixmap::fromImage(imageView));
        }
        chk1->setText("NULL");
        chk2->setText("NULL");
        slider1->setMaximum(10);
        slider2->setMaximum(10);
        vslider1->setMaximum(20);
        vslider2->setMaximum(50);
        vslider1->setEnabled(true);
        vslider2->setEnabled(true);
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
        else if (a_corner->isChecked())
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
        else if (a_corner->isChecked())
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

void CalibrateWindow::chk1_change()
{
    if (chk1->isChecked())
    {
        if (a_edge->isChecked())
        {
			slider2->setValue(slider1->value()/3);
            slider2->setEnabled(!chk1->isChecked());
        }
        else if (a_corner->isChecked())
		{
            state_change();
		}
    }
	else
	{
        if (a_edge->isChecked())
        {
            slider2->setEnabled(!chk1->isChecked());
        }
        else if (a_corner->isChecked())
		{
			state_change();
		}
    }
}

void CalibrateWindow::chk2_change()
{
    if (chk2->isChecked())
    {
        if (a_edge->isChecked())
        {
			state_change();
        }
        else if (a_corner->isChecked())
		{
			slider2->setValue(slider1->value());
            slider2->setEnabled(!chk2->isChecked());
		}
	}
	else
	{
        if (a_edge->isChecked())
        {
        	state_change();
        }
        else if (a_corner->isChecked())
		{
            slider2->setEnabled(!chk2->isChecked());
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

void CalibrateWindow::next_clicked()
{

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
        a_loop->setChecked(false);
        a_bijoo->setChecked(false);
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
        a_loop->setChecked(false);
        a_bijoo->setChecked(false);
        state_change(1);
    }
    else
    {
        a_edge->setChecked(true);
    }
}

void CalibrateWindow::bijoo_clicked(bool state)
{
    if (state)
    {
        a_corner->setChecked(false);
        a_loop->setChecked(false);
        a_edge->setChecked(false);
        state_change(1);
    }
    else
    {
        a_bijoo->setChecked(true);
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
        a_corner->setChecked(false);
        a_bijoo->setChecked(false);
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
    a_save = file_menu->addAction("Save");
    a_replace = file_menu->addAction("Replace");

    mode_menu = menu->addMenu("Mode");
    a_edge = mode_menu->addAction("Edge Detection");
    a_corner = mode_menu->addAction("Corner Detection");
    a_loop = mode_menu->addAction("Loop Detection");
    a_bijoo =  mode_menu->addAction("Bijoo Filter");

    option_menu = menu->addMenu("Option");
    a_equal = option_menu->addAction("Erode = Dilute");

    a_edge->setCheckable(true);
    a_loop->setCheckable(true);
    a_corner->setCheckable(true);
    a_bijoo->setCheckable(true);
    a_equal->setCheckable(true);

    a_edge->setChecked(true);
    a_equal->setChecked(true);

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
    file_name = "/home/bijan/2.png";
	treshold_1 = 0;
	treshold_2 = 0;
    treshold_3 = 0;
    treshold_4 = 0;
    //Window
    Main_Widget->setLayout(main_layout);
    setWindowTitle(trUtf8("Calibration"));
    setCentralWidget(Main_Widget);
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
    surface->setPixmap(QPixmap::fromImage(imageView));
}

void CalibrateWindow::bijoo_filter(IplImage *in,int kernel_size)
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
