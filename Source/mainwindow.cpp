#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CreateLayout();
	
	//Actions
    connect(a_open, SIGNAL(triggered(bool)),this,SLOT(open_clicked()));
    connect(a_save, SIGNAL(triggered(bool)),this,SLOT(save_clicked()));
    connect(a_exit, SIGNAL(triggered(bool)),this,SLOT(exit_clicked()));
    connect(a_calibrate, SIGNAL(triggered(bool)),this,SLOT(calibrate_clicked()));
	//Buttons
    connect(open_btn, SIGNAL(released()), this, SLOT(open_clicked()));
    connect(analysis_btn, SIGNAL(released()), this, SLOT(analysis_clicked()));
    connect(save_btn, SIGNAL(released()), this, SLOT(save_clicked()));
    connect(calibrate_btn, SIGNAL(released()), this, SLOT(calibrate_clicked()));

    openImage();
}

MainWindow::~MainWindow()
{
}

//Open image function call
void MainWindow::openImage()
{
    imagerd = cvLoadImage(file_name);

    imageView = QImage((const unsigned char*)(imagerd->imageData), imagerd->width,imagerd->height,QImage::Format_RGB888).rgbSwapped();
    preview->setPixmap(QPixmap::fromImage(imageView.scaled(prev_size,floor((prev_size/imagerd->width)*imagerd->height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
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
    file_name = QFileDialog::getOpenFileName(this, "Open File", "","Images (*.png *.jpg)").toLocal8Bit().data();
    if (strcmp(file_name,""))
    {
		openImage();
	}
}

void MainWindow::updatePrev()
{
    IplImage *imagesrc = cvLoadImage(filter_param.filename.toUtf8().data());
    if (imagesrc == NULL)
    {
        return;
    }
    IplImage *imgclone = cvCreateImage( cvGetSize(imagesrc), 8, 1 );
    cvCvtColor( imagesrc, imgclone, CV_BGR2GRAY );
    if (filter_param.erode)
        cvErode( imgclone, imgclone , NULL , filter_param.erode );
    if (filter_param.dilute)
        cvDilate( imgclone, imgclone , NULL , filter_param.dilute );
    IplImage *buffer = imgclone;
    imgclone = CalibrateWindow::doCanny( imgclone, filter_param.edge_1 ,filter_param.edge_2, 3 );
    cvReleaseImage( &buffer );
    if (filter_param.bold)
        CalibrateWindow::bold_filter(imgclone,filter_param.bold);
    CvSeq* firstContour = NULL;
    CvMemStorage* cnt_storage = cvCreateMemStorage();
    cvFindContours(imgclone,cnt_storage,&firstContour,sizeof(CvContour),CV_RETR_TREE,CV_CHAIN_APPROX_SIMPLE);
    CvMemStorage* poly_storage = cvCreateMemStorage();
    CvSeq *dummy_seq = firstContour;
    CvSeq *poly = NULL;
    trmMosbat *first_plus = new trmMosbat;
    trmMosbat *current_plus = first_plus;
    IplImage *imgout = cvCloneImage(imagesrc);
    while( dummy_seq != NULL )
    {
        poly = cvApproxPoly(dummy_seq,sizeof(CvContour),poly_storage, CV_POLY_APPROX_DP,filter_param.corner_min);
        if (poly->total == 12)
        {
            current_plus->next = new trmMosbat(poly,0);
            current_plus = current_plus->next;
            //crop
            cvSetImageROI(imgout, current_plus->getRegion());
            IplImage *buffer = cvCreateImage(cvGetSize(imgout), imgout->depth, imgout->nChannels);
            cvCopy(imgout, buffer, NULL);
            cvResetImageROI(imgout);
            imgout = cvCloneImage(buffer);

            cvReleaseImage( &buffer );
            break;
        }
        dummy_seq = dummy_seq->h_next;
    }
    imageView = QImage((const unsigned char*)(imgout->imageData), imgout->width,imgout->height,QImage::Format_RGB888).rgbSwapped();
    preview->setPixmap(QPixmap::fromImage(imageView.scaled(prev_size,floor((prev_size/imgout->width)*imgout->height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
}

void MainWindow::calibrate_clicked()
{
    calibrate_window = new CalibrateWindow(this);
    filter_param = calibrate_window->start();
    updatePrev();
}

void MainWindow::open_clicked()
{
    file_name = QFileDialog::getOpenFileName(this, "Open File", "","Images (*.png *.jpg)").toLocal8Bit().data();
    if (strcmp(file_name,""))
    {
		openImage();
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
    main_layout->addLayout(surface_layout);
    main_layout->addLayout(progress_layout);
    main_layout->addLayout(button_layout);
    //Side object
    file_name = "../Resources/NA.jpg";
    filter_param = trmMosbat::Loadparam("settings.json");
    erode_count = 0;
    dilate_count = 0;
	treshold_1 = 0;
	treshold_2 = 0;
    //Window
    Main_Widget->setLayout(main_layout);
    setWindowTitle(trUtf8("Tremor"));
    setCentralWidget(Main_Widget);
//    move
    //setLayoutDirection(Qt::RightToLeft);
}
