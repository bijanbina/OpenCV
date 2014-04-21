#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateLayout();
    connect(slider1,SIGNAL(valueChanged(int)), this, SLOT(slider_change(int)));
    openImage();
}

MainWindow::~MainWindow()
{
}

//Open image function call
void MainWindow::openImage()
{
    //declare FileOpName as the choosen opened file name
    FileOpName = "/home/bijan/Pictures/faith.jpeg";

    imagerd = cvLoadImage(FileOpName);
    image = cvLoadImage(FileOpName,CV_LOAD_IMAGE_GRAYSCALE);
    imageView = QImage((const unsigned char*)(imagerd->imageData), imagerd->width,imagerd->height,QImage::Format_RGB888).rgbSwapped();
    surface->setPixmap(QPixmap::fromImage(imageView));

}
void MainWindow::slider_change(int value)
{
    double hey = value;
    if (value)
    {
        IplImage* out = doCanny( image, hey/3,hey, 3 );
        imageView = QImage((const unsigned char*)(out->imageData), out->width,out->height,QImage::Format_Indexed8).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView));
        cvReleaseImage( &out );
    }
    else
    {
        imageView = QImage((const unsigned char*)(imagerd->imageData), imagerd->width,imagerd->height,QImage::Format_RGB888).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView));
    }

    slider1_label->setText(QString("value = %1").arg(value));
}

void MainWindow::CreateLayout()
{
    surface = new QLabel();
    main_layout = new QVBoxLayout;
    Main_Widget = new QWidget;
    slider1_layout = new QHBoxLayout;
    slider1_label = new QLabel("value = 0");
    slider1 = new QSlider(Qt::Horizontal);
    slider1->setMaximum(1000);
    slider1_layout->addWidget(slider1_label);
    slider1_layout->addWidget(slider1);
    //Start making layout
    main_layout->addLayout(slider1_layout);
    main_layout->addWidget(surface);
    //Window
    Main_Widget->setLayout(main_layout);
    setWindowTitle(trUtf8("Foad"));
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
