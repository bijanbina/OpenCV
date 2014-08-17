#include "recwindow.h"

void *capture_main(void *data)
{
    RecWindow *window = (RecWindow*)data;
//    IplImage *imagesrc;
//    int i = 0;
//    clock_t before = clock();
//    window->slider1->setMaximum(10000);
//    while (true)
//    {
//        imagesrc = cvQueryFrame( window->capture );
//        if (imagesrc == NULL)
//        {
//            window->imageView = QImage((const unsigned char*)(window->NA_image->imageData), window->NA_image->width,window->NA_image->height,QImage::Format_RGB888).rgbSwapped();
//            window->preview->setPixmap(QPixmap::fromImage(window->imageView.scaled(prev_size,
//                                       floor((prev_size/window->NA_image->width)*window->NA_image->height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));
//            return NULL;
//        }
//        window->imageView = QImage((const unsigned char*)(imagesrc->imageData), imagesrc->width,imagesrc->height,QImage::Format_RGB888).rgbSwapped();
//        window->preview->setPixmap(QPixmap::fromImage(window->imageView.scaled(prev_size,
//                                      floor((prev_size/imagesrc->width)*imagesrc->height),Qt::IgnoreAspectRatio,Qt::SmoothTransformation)));

//        window->slider1->setValue((clock() - before)/CLOCKS_PER_SEC);
//    }
    return NULL;
}

RecWindow::RecWindow(QWidget *parent) :
    QDialog(parent)
{
    CreateLayout(parent);
    CreateMenu();

    connect(slider1,SIGNAL(valueChanged(int)), this, SLOT(slider1_change(int)));
    connect(slider2,SIGNAL(valueChanged(int)), this, SLOT(slider2_change(int)));
    connect(rec_btn, SIGNAL(released()), this, SLOT(rec_clicked()));
    connect(save_btn, SIGNAL(released()), this, SLOT(save_clicked()));
    connect(analysis_btn, SIGNAL(released()), this, SLOT(analysis_clicked()));

    connect(a_open, SIGNAL(triggered(bool)),this,SLOT(open_clicked()));
    filename = filter_param.filename;

    setGeometry((qApp->desktop()->geometry().center() - rect().center()).x(),(qApp->desktop()->geometry().center() - rect().center()).y(),rect().width(),rect().height());
}

RecWindow::~RecWindow()
{

}

void RecWindow::open_clicked()
{
    QString DeviceName;
    QStringList cam_list = getDeviceName();
    camID = -1;
    if (cam_list.size() > 1)
    {
        DeviceName = QInputDialog::getItem(this,"Select Camera","Camera",cam_list);
        DeviceName = DeviceName[DeviceName.size()-1];
        camID = DeviceName.toInt();
    }
    else if (cam_list.size() == 1)
    {
        DeviceName = cam_list[0];
        DeviceName = DeviceName[DeviceName.size()-1];
        camID = DeviceName.toInt();
    }
    //else device id is unchanged
    capture = cvCreateCameraCapture(camID);
    if (capture == NULL)
    {
        return;
    }

    char command[50];
    sprintf(command,"v4l2-ctl -d %d -c exposure_auto=1",camID);
    system(command);
    sprintf(command,"v4l2-ctl -d %d -c exposure_absolute=1",camID);
    system(command);
}

void RecWindow::save_clicked()
{

}

void RecWindow::rec_clicked()
{
    pthread_cancel(thread_cam);
    pthread_create( &thread_cam, NULL, capture_main, (void*) this);
    return;
}

void RecWindow::exit_clicked()
{

}

void RecWindow::analysis_clicked()
{

}

void RecWindow::slider1_change(int value)
{
    treshold_1 = value;
    slider1_label->setText(QString("value = %1").arg(value));
}

void RecWindow::slider2_change(int value)
{
    treshold_2 = value;
    slider2_label->setText(QString("value = %1").arg(value));
}

void RecWindow::CreateMenu()
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

    calibrate_state = TRM_STATE_EDGE;
    a_equal->setChecked(true);
    a_mnormal->setChecked(true);
    morphology_menu->setEnabled(false);

    help_menu = menu->addMenu("Help");
    a_about = help_menu->addAction("About");
}

void RecWindow::CreateLayout(QWidget *parent)
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
    rec_btn = new QPushButton("Record");
    save_btn = new QPushButton("Save");
    analysis_btn = new QPushButton("Analysis");
    button_layout->addWidget(rec_btn);
    button_layout->addWidget(save_btn);
    button_layout->addWidget(analysis_btn);
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
    filter_param = trmMark::Loadparam("settings.json");
    //default
    treshold_1 = 0;
    treshold_2 = 0;
    treshold_3 = 0;
    treshold_4 = 0;
    morphology_state = MORPH_STATE_NORMALL;
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

//---------Base Code -----------

QStringList RecWindow::getDeviceName()
{
    QStringList return_data;
    QDir *dip = new QDir("/dev","video*",QDir::SortFlags(QDir::Name | QDir::IgnoreCase ), QDir::AllEntries | QDir::System);
    return_data = dip->entryList();
    return return_data;
}
