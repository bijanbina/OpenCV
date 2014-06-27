#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateLayout();
    QSignalMapper* signalMapper = new QSignalMapper (this) ;

    connect(open_btn, SIGNAL(released()), this, SLOT(open_clicked()));
    connect(analysis_btn, SIGNAL(released()), this, SLOT(analysis_clicked()));
    connect(save_btn, SIGNAL(released()), this, SLOT(save_clicked()));
    connect(calibrate_btn, SIGNAL(released()), this, SLOT(calibrate_clicked()));

	connect (signalMapper, SIGNAL(mapped(int)), this, SLOT(state_change(int))) ;

    openImage();
}

MainWindow::~MainWindow()
{
    delete   surface;
    if (image != NULL)
    {
        cvReleaseImage(&image);
    }
    delete   main_layout;
    delete   Main_Widget;
}

//Open image function call
void MainWindow::openImage()
{
    imagerd = cvLoadImage(file_name);
    image = cvLoadImage(file_name,CV_LOAD_IMAGE_GRAYSCALE);
	state_change();
}

void MainWindow::state_change()
{
	IplImage* buffer = cvCreateImage(cvGetSize(image),image->depth,image->nChannels);
    cvErode( image, buffer , NULL , erode_count);
    cvDilate( buffer, buffer , NULL , dilate_count);
    IplImage* out = doCanny( buffer, treshold_1 ,treshold_2, 3 );
    imageView = QImage((const unsigned char*)(out->imageData), out->width,out->height,QImage::Format_Indexed8).rgbSwapped();
    surface->setPixmap(QPixmap::fromImage(imageView));
    cvReleaseImage( &buffer );
    cvReleaseImage( &out );
}

void MainWindow::save_clicked()
{
    file_name = QFileDialog::getSaveFileName(this, "Save File", "","").toLocal8Bit().data();
    if (strcmp(file_name,"") && !imageView.isNull())
    {
		QPixmap::fromImage(imageView).save(file_name,"PNG",100);
	}
}

void MainWindow::analysis_clicked()
{
    file_name = QFileDialog::getOpenFileName(this, "Open File", "","Images (*.png *.jpg)").toLocal8Bit().data();
    if (strcmp(file_name,""))
    {
		openImage();
	}
}

void MainWindow::calibrate_clicked()
{
	calibrate_window->show();
}

void MainWindow::open_clicked()
{
    file_name = QFileDialog::getOpenFileName(this, "Open File", "","Images (*.png *.jpg)").toLocal8Bit().data();
    if (strcmp(file_name,""))
    {
		openImage();
	}
}

void MainWindow::CreateLayout()
{
    surface = new QLabel();
	calibrate_window = new CalibrateWindow(this);
    main_layout = new QVBoxLayout;
    Main_Widget = new QWidget;
    menu = new QMenu (this);

    option_layout = new QHBoxLayout;
    data_layout = new QHBoxLayout;
    plot_layout = new QHBoxLayout;
    progress_layout = new QHBoxLayout;
    button_layout = new QHBoxLayout;
    surface_layout = new QHBoxLayout;
    //Button
	button_layout = new QHBoxLayout;
    open_btn = new QPushButton("Open");
    calibrate_btn = new QPushButton("Calibrate");
    analysis_btn = new QPushButton("Analysis");
    save_btn = new QPushButton("Save");
	button_layout->addWidget(open_btn);
	button_layout->addWidget(calibrate_btn);
	button_layout->addWidget(analysis_btn);
	button_layout->addWidget(save_btn);
    //Options
    surface2_layout = new QVBoxLayout;
    surface2_layout->addLayout(option_layout);

	progress = new QProgressBar;
	progress_layout->addWidget(progress);
    //Start making layout
    main_layout->addLayout(data_layout);
    main_layout->addLayout(progress_layout);
    main_layout->addLayout(surface2_layout);
    main_layout->addLayout(surface_layout);
    main_layout->addLayout(button_layout);
    //Side object
    file_name = "/home/bijan/Pictures/test.jpg";
    erode_count = 0;
    dilate_count = 0;
	treshold_1 = 0;
	treshold_2 = 0;
    //Window
    Main_Widget->setLayout(main_layout);
    setWindowTitle(trUtf8("Tremor"));
    setCentralWidget(Main_Widget);
    //setLayoutDirection(Qt::RightToLeft);
}


IplImage* MainWindow::doCanny( IplImage* in, double lowThresh, double highThresh, double aperture )
{
    if(in->nChannels != 1)
    {
        printf("Not supported\n");
        exit(0); //Canny only handles gray scale images
    }
    IplImage* out = cvCreateImage( cvGetSize( in ) , IPL_DEPTH_8U, 1 );
    cvCanny( in, out, lowThresh, highThresh, aperture );
    return( out );
}

IplImage* MainWindow::doPyrDown( IplImage* in, int filter)
{
    // Best to make sure input image is divisible by two.
    //
    assert( in->width%2 == 0 && in->height%2 == 0 );
    IplImage* out = cvCreateImage(
                cvSize( in->width/2, in->height/2 ),
                in->depth,
                in->nChannels
                );
    cvPyrDown( in, out , CV_GAUSSIAN_5x5);
    return( out );
};
