#include "calibrateiiwindow.h"

CalibrateIIWindow::CalibrateIIWindow(QWidget *parent) :
    QDialog(parent)
{
    CreateLayout(parent);
    CreateMenu();

    connect(slider1,SIGNAL(valueChanged(int)), this, SLOT(slider1_change(int)));
    connect(slider2,SIGNAL(valueChanged(int)), this, SLOT(slider2_change(int)));
    connect(back_btn, SIGNAL(released()), this, SLOT(back_clicked()));
    connect(next_btn, SIGNAL(released()), this, SLOT(next_clicked()));
    connect(calibrate_btn, SIGNAL(released()), this, SLOT(calibrate_clicked()));

    connect(a_open, SIGNAL(triggered(bool)),this,SLOT(open_clicked()));
    connect(a_open_image, SIGNAL(triggered(bool)),this,SLOT(openimage_clicked()));
    connect(a_save, SIGNAL(triggered(bool)),this,SLOT(save_clicked()));
    connect(a_replace, SIGNAL(triggered(bool)),this,SLOT(calibrate_clicked()));
    connect(a_loop, SIGNAL(triggered(bool)),this,SLOT(state_change()));
    connect(a_width, SIGNAL(triggered(bool)),this,SLOT(width_clicked()));
    connect(a_equal, SIGNAL(triggered(bool)),this,SLOT(equal_clicked(bool)));
    connect(a_mclose, SIGNAL(triggered(bool)),this,SLOT(mclose_clicked(bool)));
    connect(a_mreversed, SIGNAL(triggered(bool)),this,SLOT(mreversed_clicked(bool)));
    connect(a_mopen, SIGNAL(triggered(bool)),this,SLOT(mopen_clicked(bool)));
    connect(a_mnormal, SIGNAL(triggered(bool)),this,SLOT(mnormal_clicked(bool)));

    connect(color1_H, SIGNAL(textChanged(QString)), this, SLOT(state_change()));
    connect(color1_S, SIGNAL(textChanged(QString)), this, SLOT(state_change()));
    connect(color1_V, SIGNAL(textChanged(QString)), this, SLOT(state_change()));
    connect(color2_H, SIGNAL(textChanged(QString)), this, SLOT(state_change()));
    connect(color2_S, SIGNAL(textChanged(QString)), this, SLOT(state_change()));
    connect(color2_V, SIGNAL(textChanged(QString)), this, SLOT(state_change()));

    filename = filter_param.filename;
    isVideo = filter_param.isVideo;
    if (!isVideo)
    {
        imagesrc = cvLoadImage(filename.toLocal8Bit().data());
        image = cvLoadImage(filename.toLocal8Bit().data(),CV_LOAD_IMAGE_GRAYSCALE);

        slider1->setValue(filter_param.edge_1);
        slider2->setValue(filter_param.edge_2);

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
        calibrate_state = TRM_STATE_FRAME_II;

        option_menu->addAction(a_width);

        slider1->setValue(0);
        state_change(1);
        slider1->setValue(filter_param.frame_num);
        slider2->setValue(filter_param.calibre_width);
    }

    setGeometry((qApp->desktop()->geometry().center() - rect().center()).x(),(qApp->desktop()->geometry().center() - rect().center()).y(),rect().width(),rect().height());
}

CalibrateIIWindow::~CalibrateIIWindow()
{

}


void CalibrateIIWindow::state_change(int changed)
{
    if (imgout != NULL)
    {
        cvReleaseImage(&imgout);
    }
    if (image == NULL)
        return;
    if (changed)
    {
        treshold_1 = 0;
        slider1->setValue(0);
        slider2->setValue(0);
    }
    surface_height = floor((surface_width/image->width)*image->height);
    if ( calibrate_state == TRM_STATE_FRAME_II)
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
        image = cvCloneImage(imagesrc);
        imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        slider1->setMaximum(frames - 100);
        slider2->setMaximum(200);
    }
    else if (calibrate_state == TRM_STATE_MAHYAR_II)
    {
        color1_img.fill(QColor::fromHsv(color1_H->text().toInt(),color1_S->text().toInt(),color1_V->text().toInt()));
        color2_img.fill(QColor::fromHsv(color2_H->text().toInt(),color2_S->text().toInt(),color2_V->text().toInt()));
        color1_prev->setPixmap(QPixmap::fromImage(color1_img));
        color2_prev->setPixmap(QPixmap::fromImage(color2_img));

        //Initialize
        cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT, cvSize( 10, 10 ), cvPoint( 5, 5) );
        cv::Scalar color1 = cv::Scalar(color1_H->text().toInt(),color1_S->text().toInt(),color1_V->text().toInt());
        cv::Scalar color2 = cv::Scalar(color2_H->text().toInt(),color2_S->text().toInt(),color2_V->text().toInt());

        //Convert frame to HSV
        cv::Mat mat_image, hsv_frame, colorf_frame;
        mat_image = cvCloneImage(image);
        cv::cvtColor( mat_image, hsv_frame, CV_BGR2HSV );

        //THRESHOLDING AND FINDING COLOR SPOT
        cv::inRange( hsv_frame, color1, color2, colorf_frame );
        cv::dilate( colorf_frame, colorf_frame, element );

        IplImage cnvt = colorf_frame;
        imgout = cvCloneImage(&cnvt);

        imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_Indexed8).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    }
    else if (calibrate_state == TRM_STATE_RESULT_II)
    {
        trmParam temp_param = filter_param;
        temp_param.window = treshold_1;
        imgout = cvCloneImage(imagesrc);
        trmMark *plus = markFromMahyar(imgout,temp_param);

        imgout = cvCloneImage(imagesrc);

        cv::RNG rng(1234);
        CvScalar color = cvScalar(rng.uniform(0,255), rng.uniform(0, 255), rng.uniform(0, 255));

        if (plus != NULL)
        {
            cv::Mat mat_temp = imgout;
            cv::rectangle(mat_temp,plus->region,color,2,16);
            delete plus;
        }

        imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_RGB888).rgbSwapped();
        surface->setPixmap(QPixmap::fromImage(imageView.scaled(surface_width,surface_height,Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
    }
}
void CalibrateIIWindow::slider1_change(int value)
{
    if ( calibrate_state != TRM_STATE_MAHYAR_II)//We dont have any slide in TRM_STATE_MAHYAR_II
    {
        treshold_1 = value;
        state_change();
        slider1_label->setText(QString("value = %1").arg(value));
    }
}

void CalibrateIIWindow::slider2_change(int value)
{
    if ( calibrate_state != TRM_STATE_MAHYAR_II)//We dont have any slide in TRM_STATE_MAHYAR_II
    {
        treshold_2 = value;
        slider2_label->setText(QString("value = %1").arg(value));
    }
}

void CalibrateIIWindow::equal_clicked(bool state)
{
    state_change(0);
}

void CalibrateIIWindow::mclose_clicked(bool state)
{
    if (state)
    {
        a_mreversed->setChecked(false);
        a_mopen->setChecked(false);
        a_mnormal->setChecked(false);
        morphology_state = MORPH_STATE_CLOSE;
        state_change(0);
    }
    else
    {
        a_mclose->setChecked(true);
    }
}

void CalibrateIIWindow::mopen_clicked(bool state)
{
    if (state)
    {
        a_mclose->setChecked(false);
        a_mreversed->setChecked(false);
        a_mnormal->setChecked(false);
        morphology_state = MORPH_STATE_OPEN;
        state_change(0);
    }
    else
    {
        a_mopen->setChecked(true);
    }
}

void CalibrateIIWindow::mnormal_clicked(bool state)
{
    if (state)
    {
        a_mclose->setChecked(false);
        a_mreversed->setChecked(false);
        a_mopen->setChecked(false);
        morphology_state = MORPH_STATE_NORMALL;
        state_change(0);
    }
    else
    {
        a_mnormal->setChecked(true);
    }
}

void CalibrateIIWindow::mreversed_clicked(bool state)
{
    if (state)
    {
        a_mclose->setChecked(false);
        a_mopen->setChecked(false);
        a_mnormal->setChecked(false);
        morphology_state = MORPH_STATE_REVERSED;
        state_change(0);
    }
    else
    {
        a_mreversed->setChecked(true);
    }
}

void CalibrateIIWindow::save_clicked()
{
    filename = QFileDialog::getSaveFileName(this, "Save File", "","");
    if (!filename.isEmpty() && !imageView.isNull())
    {
        QPixmap::fromImage(imageView).save(filename.toLocal8Bit().data(),"PNG",100);
    }
}

void CalibrateIIWindow::back_clicked()
{
    if (calibrate_state == TRM_STATE_FRAME_II)
    {
        close();
    }
    else if (calibrate_state == TRM_STATE_MAHYAR_II)
    {
        if (isVideo)
        {
            calibrate_state = TRM_STATE_FRAME_II;
            state_change(1);
            slider1->setValue(filter_param.frame_num);
            slider2->setValue(filter_param.calibre_width);
            option_menu->removeAction(a_equal);
            option_menu->addAction(a_width);
            morphology_menu->setEnabled(false);
            next_btn->setText("Next");
        }
        else
        {
            close();
        }
    }
}

void CalibrateIIWindow::next_clicked()
{
    if (imgout != NULL)
    {
        //TODO: a critical bug lyes here
        //cvReleaseImage(&image);
        image = imgout;
        imgout = NULL;
    }
    if (calibrate_state == TRM_STATE_FRAME_II)
    {
        filter_param.frame_num = treshold_1;
        calibrate_state = TRM_STATE_MAHYAR_II;
        slider1_layout->removeWidget(slider1);
        slider1->hide();
        slider1_layout->addWidget(color1_H);
        slider1_layout->addWidget(color1_S);
        slider1_layout->addWidget(color1_V);
        slider1_layout->addWidget(color1_prev);
        slider1_label->setText("HSV 1:");

        slider2_layout->removeWidget(slider2);
        slider2->hide();
        slider2_layout->addWidget(color2_H);
        slider2_layout->addWidget(color2_S);
        slider2_layout->addWidget(color2_V);
        slider2_layout->addWidget(color2_prev);
        state_change(1);
        slider2_label->setText("HSV 2:");
    }
    else if (calibrate_state == TRM_STATE_MAHYAR_II)
    {
        filter_param.color[0] = cvScalar(color1_H->text().toInt(),color1_S->text().toInt(),color1_V->text().toInt());
        filter_param.color[1] = cvScalar(color2_H->text().toInt(),color2_S->text().toInt(),color2_V->text().toInt());

        slider1_layout->addWidget(slider1);
        calibrate_state = TRM_STATE_RESULT_II;
        slider1_layout->removeWidget(color1_H);
        slider1_layout->removeWidget(color1_S);
        slider1_layout->removeWidget(color1_V);
        slider1_layout->removeWidget(color1_prev);
        color1_H->hide();
        color1_S->hide();
        color1_V->hide();
        color1_prev->hide();
        slider1->show();
        slider1_label->setText("Window:");

        slider2_layout->addWidget(slider2);
        slider2->show();
        slider1_layout->removeWidget(color2_H);
        slider1_layout->removeWidget(color2_S);
        slider1_layout->removeWidget(color2_V);
        slider1_layout->removeWidget(color2_prev);
        color2_H->hide();
        color2_S->hide();
        color2_V->hide();
        color2_prev->hide();
        next_btn->setText("Finish");

        slider1->setMaximum(imagesrc->width);
        if ( filter_param.window == -1)
        {
            slider1->setValue(imagesrc->width);
        }
        else
        {
            slider1->setValue(filter_param.window);
        }
    }
    else if (calibrate_state == TRM_STATE_RESULT_II)
    {
        filter_param.window = treshold_1;
        trmMark::Saveparam(filter_param,SETTING_FILENAME);
        close();
    }


}


//use bijan algorithm to find out mm to pixel ratio
void CalibrateIIWindow::calibrate_clicked()
{
    if (image != NULL)
    {
        trmMark *mark = markFromImage(image,filter_param,NULL);
        if (image != NULL)
        {
            slider2->setValue(mark->edge);
        }
    }
}

void CalibrateIIWindow::open_clicked()
{

}

void CalibrateIIWindow::openimage_clicked()
{

}

void CalibrateIIWindow::width_clicked()
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

void CalibrateIIWindow::CreateMenu()
{
    menu = new QMenuBar (this);
    //setMenuBar(menu);
    main_layout->setMenuBar(menu);

    file_menu = menu->addMenu("File");
    a_open = file_menu->addAction("Open");
    a_open_image = file_menu->addAction("Open Image");
    a_save = file_menu->addAction("Save");
    a_replace = file_menu->addAction("Replace");

    morphology_menu = menu->addMenu("Morphology");
    a_mnormal = morphology_menu->addAction("Normal");
    a_mreversed = morphology_menu->addAction("Reversed");
    a_mopen = morphology_menu->addAction("Open");
    a_mclose = morphology_menu->addAction("Close");

    option_menu = menu->addMenu("Option");
    a_equal = new QAction("Erode = Dilate",NULL);
    a_width = new QAction("Set Width",NULL);
    a_loop = new QAction("Show All Corner",NULL);


    a_equal->setCheckable(true);
    a_loop->setCheckable(true);
    a_mclose->setCheckable(true);
    a_mopen->setCheckable(true);
    a_mreversed->setCheckable(true);
    a_mnormal->setCheckable(true);

    calibrate_state = TRM_STATE_MAHYAR_II;
    a_equal->setChecked(true);
    a_mnormal->setChecked(true);
    morphology_menu->setEnabled(false);

    help_menu = menu->addMenu("Help");
    a_about = help_menu->addAction("About");
}

void CalibrateIIWindow::CreateLayout(QWidget *parent)
{
    //Default
    imgout = NULL;
    surface = new QLabel();
    main_layout = new QVBoxLayout;
    Main_Widget = new QWidget;

    //Load Param
    filter_param = trmMark::Loadparam(SETTING_FILENAME);

    color1_H = new QLineEdit(QString("%1").arg( filter_param.color[0][0] ));
    color1_S = new QLineEdit(QString("%1").arg( filter_param.color[0][1] ));
    color1_V = new QLineEdit(QString("%1").arg( filter_param.color[0][2] ));
    color2_H = new QLineEdit(QString("%1").arg( filter_param.color[1][0] ));
    color2_S = new QLineEdit(QString("%1").arg( filter_param.color[1][1] ));
    color2_V = new QLineEdit(QString("%1").arg( filter_param.color[1][2] ));
    color1_prev = new QLabel;
    color2_prev = new QLabel;
    color1_img = QImage(50,50,QImage::Format_RGB888);
    color2_img = QImage(50,50,QImage::Format_RGB888);

    slider1_layout = new QHBoxLayout;
    slider1_label = new QLabel("value = 0");
    slider1 = new QSlider(Qt::Horizontal);
    slider1->setMaximum(1000);
    slider1_layout->addWidget(slider1_label);
    slider1_layout->addWidget(slider1);

    slider2_layout = new QHBoxLayout;
    slider2_label = new QLabel("value = 0");
    slider2 = new QSlider(Qt::Horizontal);
    slider2->setMaximum(1000);
    slider2_layout->addWidget(slider2_label);
    slider2_layout->addWidget(slider2);

    surface_layout = new QHBoxLayout;
    surface_layout->addWidget(surface);

    //Button
    button_layout = new QHBoxLayout;
    back_btn = new QPushButton("Back");
    calibrate_btn = new QPushButton("Calibrate");
    next_btn = new QPushButton("Next");
    button_layout->addWidget(back_btn);
    button_layout->addWidget(calibrate_btn);
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
    //default
    treshold_1 = 0;
    treshold_2 = 0;
    morphology_state = MORPH_STATE_NORMALL;
    surface_width = filter_param.calibre_width;

    //Window
    setLayout(main_layout);
    setWindowTitle(trUtf8("Calibration"));
    //setAttribute(Qt::WA_DeleteOnClose);
    setWindowFlags(Qt::Window);
    //setSizePolicy(QSizePolicy::Minimum);
    //setLayoutDirection(Qt::RightToLeft);
}

void CalibrateIIWindow::MyFilledCircle( cv::Mat img, cv::Point center )
{
    int thickness = 3;
    int lineType = 8;

    cv::circle( img, center, 10.0, 255, thickness, lineType );
}

trmParam CalibrateIIWindow::start(int startFrame)
{
    if (startFrame && isVideo)
    {
        filter_param.frame_num = startFrame;
        slider1->setValue(filter_param.frame_num);
    }
    exec();
    return filter_param;
}
