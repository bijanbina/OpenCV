#ifndef CALIBRATEWINDOW_H
#define CALIBRATEWINDOW_H

#include <QMainWindow>
#include <opencv/highgui.h>
#include <QtWidgets>
#include <cv.h>

class CalibrateWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit CalibrateWindow(QWidget *parent = 0);
    ~CalibrateWindow();

protected:
    char        *FileOpName; //declare FileOpName as IplImage
    IplImage    *imagerd;    //declare imagerd as IplImage

private slots:
    void slider1_change(int);
    void slider2_change(int);
    void slider3_change(int);
    void slider4_change(int);
    void chk1_change(int);
    void chk2_change(int);
    void open_clicked();
    void save_clicked();
    void calibrate_clicked();
	void state_change(int is_radio = 0);


private:
    void CreateLayout();
    void CreateMenu();
    void openImage();
    void find_corner(IplImage* in ,double quality_level ,double min_distance ,int MAX_CORNERS , double k) ;
    void MyFilledCircle( cv::Mat img, cv::Point center );
    IplImage* doCanny( IplImage* in, double lowThresh, double highThresh, double aperture );
    IplImage* doPyrDown( IplImage* in, int filter);

    QLabel       *surface;
	QCheckBox    *chk1;
	QCheckBox    *chk2;
    QMenuBar     *menu;
    QMenu        *file_menu;
    QMenu		 *mode_menu;
    QMenu		 *help_menu;
    QAction		 *a_open;
    QAction		 *a_save;
    QAction		 *a_edgeDetection;
    QAction		 *a_dilute;
    QAction		 *a_erode;
    QAction		 *a_mix;
    QAction		 *a_corner;
    QAction		 *a_about;
    IplImage     *image;
    QImage        imageView;
    QVBoxLayout  *main_layout;
    QHBoxLayout  *slider1_layout;
    QLabel       *slider1_label;
    QWidget      *Main_Widget;
    QSlider      *slider1;
    QSlider      *slider2;
    QHBoxLayout  *slider2_layout;
    QLabel       *slider2_label;
    QSlider      *vslider1;
    QVBoxLayout  *vslider1_layout;
    QLabel       *vslider1_label;
    QSlider      *vslider2;
    QVBoxLayout  *vslider2_layout;
    QLabel       *vslider2_label;
    QHBoxLayout  *surface_layout;
    QVBoxLayout  *surface2_layout;
    QHBoxLayout  *option_layout;
    QHBoxLayout  *radio2_layout;
    QHBoxLayout  *chkbox_layout;
    QHBoxLayout  *radio_layout;
	QGroupBox    *radio1_groupbox;
	QRadioButton *radio_1;
	QRadioButton *radio_2;
	QRadioButton *radio_3;
	QRadioButton *radio_4;
	QRadioButton *radioa1;
	QRadioButton *radioa2;
	QRadioButton *radioa3;
	QRadioButton *radioa4;
    QHBoxLayout  *button_layout;
    QPushButton  *open_btn;
    QPushButton  *save_btn;
    QPushButton  *calibrate_btn;
    double        treshold_1;
    double        treshold_2;
    double        treshold_3;
    double        treshold_4;
    char		 *file_name;
	int           chk1_state;
	int           chk2_state;
};


#endif // MAINWINDOW_H
