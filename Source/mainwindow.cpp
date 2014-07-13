#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateLayout();
	
	//Actions
    connect(a_open, SIGNAL(triggered(bool)),this,SLOT(open_clicked()));
    connect(a_save, SIGNAL(triggered(bool)),this,SLOT(save_clicked()));
    connect(a_exit, SIGNAL(triggered(bool)),this,SLOT(exit_clicked()));
    connect(slider1,SIGNAL(valueChanged(int)), this, SLOT(slider1_change(int)));
    connect(slider2,SIGNAL(valueChanged(int)), this, SLOT(slider2_change(int)));
    connect(a_calibrate, SIGNAL(triggered(bool)),this,SLOT(calibrate_clicked()));
	//Buttons
    connect(open_btn, SIGNAL(released()), this, SLOT(open_clicked()));
    connect(analysis_btn, SIGNAL(released()), this, SLOT(analysis_clicked()));
    connect(save_btn, SIGNAL(released()), this, SLOT(save_clicked()));
    connect(calibrate_btn, SIGNAL(released()), this, SLOT(calibrate_clicked()));

    loadVideo();
    slider1->setValue(filter_param.frame_num);
}

MainWindow::~MainWindow()
{
}

//Open image function call
void MainWindow::loadVideo()
{
    if (!filter_param.isVideo)
    {
        imagerd = cvLoadImage(filter_param.filename.toLocal8Bit().data());
        slider1->setValue(0);
        slider2->setValue(0);
    }
    else
    {
        capture = cvCreateFileCapture( filter_param.filename.toLocal8Bit().data() );
        if (capture == NULL)
            return;
        imagerd = cvQueryFrame( capture );
        slider2->setMaximum((int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT ));
        slider1->setMaximum((int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT ));
        slider1->setValue(filter_param.frame_num);
        slider2->setValue(slider1->maximum()-1);
        videoLoaded = true;
    }

    updatePrev();
}

void MainWindow::exit_clicked()
{
	close();
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
    int frameNumber = 0;
    frameNumber = filter_param.frame_num;
    trmData *head,*ptr;
    int size = 0;
    if (videoLoaded)
    {
        head = createTrmdata(capture,filter_param,treshold_1,treshold_2,progress,&size);
        std::cout << size << std::endl;
        QVector< double > X(size);
        QVector< double > Y(size);
        ptr = head->next;
        for (int i = 0 ; i < size ; i++)
        {
            if (ptr == NULL)
                break;
            X[i] = ptr->x;
            Y[i] = ptr->y;
            ptr = ptr->next;
        }
        xy_curve->setPen( Qt::blue, 4 ),
        xy_curve->setRenderHint( QwtPlotItem::RenderAntialiased, true );

        QwtSymbol *symbol = new QwtSymbol( QwtSymbol::Ellipse,
            QBrush( Qt::yellow ), QPen( Qt::red, 2 ), QSize( 8, 8 ) );
        xy_curve->setSymbol( symbol );
        xy_curve->setSamples(X,Y);
        xy_curve->attach(xy_plot);
        xy_plot->update();
        progress->setValue(0);
    }
}

void MainWindow::slider1_change(int value)
{
    treshold_1 = value;
    filter_param.frame_num = treshold_1;
    slider1_label->setText(QString("Start Frame = %1").arg(value));
    updatePrev();
}

void MainWindow::slider2_change(int value)
{
    treshold_2 = value;
    slider2_label->setText(QString("End Frame = %1").arg(value));
}

void MainWindow::updatePrev()
{
    IplImage *imagesrc;
    if (!filter_param.isVideo)
    {
        imagesrc = cvLoadImage(filter_param.filename.toLocal8Bit().data());
    }
    else
    {
        if (capture == NULL)
            return;
        cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,filter_param.frame_num);
        if (!cvGrabFrame( capture ))
            return;
        imagesrc = cvQueryFrame( capture );
    }
    if (imagesrc == NULL)
    {
        return;
    }

    trmMosbat *plus_obj = mosbatFromImage(imagesrc,filter_param) ;
    if (plus_obj != NULL)
    {
        cvSetImageROI(imagesrc, plus_obj->getRegion());
        IplImage *imgout  = cvCreateImage(cvGetSize(imagesrc), imagesrc->depth, imagesrc->nChannels);
        cvCopy(imagesrc, imgout, NULL);
        cvResetImageROI(imagesrc);
        imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_RGB888).rgbSwapped();
        preview->setPixmap(QPixmap::fromImage(imageView.scaled(prev_size,floor((prev_size/imgout->width)*imgout->height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        cvReleaseImage( &imgout );
        delete plus_obj;
        return;
    }
    else if (filter_param.bold)
    {
        int temp = filter_param.bold;
        filter_param.bold = 0;
        plus_obj = mosbatFromImage(imagesrc,filter_param) ;
    }
    if (plus_obj != NULL)
    {
        cvSetImageROI(imagesrc, plus_obj->getRegion());
        IplImage *imgout  = cvCreateImage(cvGetSize(imagesrc), imagesrc->depth, imagesrc->nChannels);
        cvCopy(imagesrc, imgout, NULL);
        cvResetImageROI(imagesrc);
        imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_RGB888).rgbSwapped();
        preview->setPixmap(QPixmap::fromImage(imageView.scaled(prev_size,floor((prev_size/imgout->width)*imgout->height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
        cvReleaseImage( &imgout );
        delete plus_obj;
        return;
    }
    imageView = QImage((const unsigned char*)(NA_image->imageData), NA_image->width,NA_image->height,QImage::Format_RGB888).rgbSwapped();
    preview->setPixmap(QPixmap::fromImage(imageView.scaled(prev_size,floor((prev_size/NA_image->width)*NA_image->height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
}

void MainWindow::calibrate_clicked()
{
    calibrate_window = new CalibrateWindow(this);
    filter_param = calibrate_window->start(filter_param.frame_num);
    updatePrev();
}

void MainWindow::open_clicked()
{
    filter_param.filename = QFileDialog::getOpenFileName(this, "Open File", "","Videos (*.mp4 *.avi *.mov)");
    if (!filter_param.filename.isEmpty())
    {
        filter_param.isVideo = true;
        capture = cvCreateFileCapture( filter_param.filename.toLocal8Bit().data() );
        if (capture == NULL)
            return;
        trmMosbat::Saveparam(filter_param,"settings.json");
        slider2->setMaximum((int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT ));
        slider1->setMaximum((int) cvGetCaptureProperty( capture, CV_CAP_PROP_FRAME_COUNT ));
        videoLoaded = true;
    }

}

void MainWindow::CreateMenu()
{
    menu = new QMenuBar (this);
	setMenuBar(menu);

	file_menu = menu->addMenu("File");
	a_open = file_menu->addAction("Open");
	a_save = file_menu->addAction("Save");
	a_exit = file_menu->addAction("Exit");

	option_menu = menu->addMenu("Option");
	a_calibrate = option_menu->addAction("Calibrate");

	help_menu = menu->addMenu("Help");
	a_about = help_menu->addAction("About");
}

void MainWindow::CreateLayout()
{
    main_layout = new QVBoxLayout;
    Main_Widget = new QWidget;

	CreateMenu();

    option_layout = new QHBoxLayout;
    data_layout = new QGridLayout;
    plot_layout = new QGridLayout;
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

    //
    slider1_layout = new QHBoxLayout;
    slider1_label = new QLabel("Start Frame = 0");
    slider1 = new QSlider(Qt::Horizontal);
    slider1_layout->addWidget(slider1_label);
    slider1_layout->addWidget(slider1);

    slider2_layout = new QHBoxLayout;
    slider2_label = new QLabel("End Frame = 0");
    slider2 = new QSlider(Qt::Horizontal);
    slider2_layout->addWidget(slider2_label);
    slider2_layout->addWidget(slider2);

    //Options
    surface2_layout = new QVBoxLayout;
    surface2_layout->addLayout(option_layout);

	progress = new QProgressBar;
	progress_layout->addWidget(progress);

	//Plot
    xy_plot = new QwtPlot(QwtText("XY Plot"));
    xt_plot = new QwtPlot(QwtText("XT Plot"));
    yt_plot = new QwtPlot(QwtText("YT Plot"));
    vt_plot = new QwtPlot(QwtText("VT Plot"));

    plot_layout->addWidget(xy_plot,0,0);
    plot_layout->addWidget(xt_plot,0,1);
    plot_layout->addWidget(yt_plot,1,0);
    plot_layout->addWidget(vt_plot,1,1);


    QFont font = QFont("Tahoma",10);
    QwtText title;
    title.setFont(font);

    //XY
    xy_grid = new QwtPlotGrid();
    xy_curve = new QwtPlotCurve(trUtf8("Signal"));
    xy_plot ->canvas()->setStyleSheet("border: 0px;");
    xy_plot->titleLabel()->setStyleSheet("font-size: 12px");
    xy_plot ->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
    title.setText("X (cm)");
    xy_plot->setAxisTitle(xy_plot->xBottom, title);
    title.setText("Y (cm)");
    xy_plot->setAxisTitle(xy_plot->yLeft, title);
    xy_zoomer = new QwtPlotZoomer(xy_plot->canvas());
    xy_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

    //Xt
    xt_grid = new QwtPlotGrid();
    xt_curve = new QwtPlotCurve(trUtf8("Signal"));
    xt_plot ->canvas()->setStyleSheet("border: 0px;");
    xt_plot->titleLabel()->setStyleSheet("font-size: 12px");
    xt_plot ->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
    title.setText("Time (s)");
    xt_plot->setAxisTitle(xt_plot->xBottom, title);
    title.setText("X (cm)");
    xt_plot->setAxisTitle(xt_plot->yLeft, title);
    xt_zoomer = new QwtPlotZoomer(xt_plot->canvas());
    xt_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

    //Yt
    yt_grid = new QwtPlotGrid();
    yt_curve = new QwtPlotCurve(trUtf8("Signal"));
    yt_plot ->canvas()->setStyleSheet("border: 0px;");
    yt_plot->titleLabel()->setStyleSheet("font-size: 12px");
    yt_plot ->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
    title.setText("Time (s)");
    yt_plot->setAxisTitle(yt_plot->xBottom, title);
    title.setText("Y (cm)");
    yt_plot->setAxisTitle(yt_plot->yLeft, title);
    yt_zoomer = new QwtPlotZoomer(yt_plot->canvas());
    yt_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

    //Vt
    vt_grid = new QwtPlotGrid();
    vt_curve = new QwtPlotCurve(trUtf8("Signal"));
    vt_plot ->canvas()->setStyleSheet("border: 0px;");
    vt_plot->titleLabel()->setStyleSheet("font-size: 12px");
    vt_plot ->insertLegend(new QwtLegend(), QwtPlot::RightLegend);
    title.setText("Time (s)");
    vt_plot->setAxisTitle(vt_plot->xBottom, title);
    title.setText("Speed (m/s)");
    vt_plot->setAxisTitle(vt_plot->yLeft, title);
    vt_zoomer = new QwtPlotZoomer(vt_plot->canvas());
    vt_zoomer->setMousePattern(QwtEventPattern::MouseSelect3,Qt::RightButton);

    //Data
    feature = new QGroupBox("Feature");
    radial_label = new QLabel("Radial : 0 cm");
    freq_label = new QLabel("Frequency : 0 Hz");
    peiod_label = new QLabel("Period : 0 ms");
    amp_label = new QLabel ("Amplitude : 0 cm");
    data_layout->addWidget(radial_label,0,0);
    data_layout->addWidget(freq_label,0,1);
    data_layout->addWidget(peiod_label,1,0);
    data_layout->addWidget(amp_label,1,1);
    feature->setLayout(data_layout);

    //Preview
    preview = new QLabel;
    preview_layout = new QHBoxLayout;
    preview_layout->addWidget(preview);
    preview_group = new QGroupBox("Preview");
    preview_group->setLayout(preview_layout);

    //
    surface2_layout->addWidget(feature);
    surface2_layout->addWidget(preview_group);

    surface_layout->addLayout(plot_layout);
    surface_layout->addLayout(surface2_layout);

    //Start making layout
    main_layout->addLayout(slider1_layout);
    main_layout->addLayout(slider2_layout);
    main_layout->addLayout(surface_layout);
    main_layout->addLayout(progress_layout);
    main_layout->addLayout(button_layout);
    //Side object
    NA_image = cvLoadImage("../Resources/NA.jpg");
    filter_param = trmMosbat::Loadparam("settings.json");
    videoLoaded = false;
    //Window
    Main_Widget->setLayout(main_layout);
    setWindowTitle(trUtf8("Tremor"));
    setCentralWidget(Main_Widget);
//    move
    //setLayoutDirection(Qt::RightToLeft);
}

trmData *createTrmdata(CvCapture *capture,trmParam param,int startFrame,int endFrame,QProgressBar *progress,int *size)
{
    trmData *head,*temp,*ptr;
    head = new trmData;
    head->next = NULL;
    ptr = head;
    *size = 0;
    progress->setMaximum(endFrame - startFrame);
    for  (int frameNumber = startFrame; frameNumber < endFrame ; frameNumber++)
    {
        IplImage *imagesrc;
        if (capture == NULL)
            return NULL;
        cvSetCaptureProperty(capture,CV_CAP_PROP_POS_FRAMES,frameNumber);
        if (!cvGrabFrame( capture ))
            return NULL;
        imagesrc = cvQueryFrame( capture );
        if (imagesrc == NULL)
        {
            return NULL;
        }
        trmMosbat *plus_obj = mosbatFromImage(imagesrc,param) ;
        if (plus_obj != NULL)
        {
            temp = new trmData;
            temp->x = plus_obj->middle.x;
            temp->y = plus_obj->middle.y;
            temp->next = NULL;
            ptr->next = temp;
            ptr = ptr->next;
            delete plus_obj;
            (*size)++;
        }
        else
        {
            CalibrateWindow *calibrate_window = new CalibrateWindow();
            calibrate_window->start(frameNumber);
        }
        frameNumber++;
        progress->setValue(frameNumber - startFrame);
    }
    return head;
}
