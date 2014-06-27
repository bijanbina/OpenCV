#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "calibratewindow.h"
#include <opencv/highgui.h>
#include <QtWidgets>
#include <cv.h>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    char        *FileOpName; //declare FileOpName as IplImage
    IplImage    *imagerd;    //declare imagerd as IplImage

private slots:
    void open_clicked();
    void calibrate_clicked();
    void analysis_clicked();
    void save_clicked();
    void state_change();

private:
    void CreateLayout();
    void openImage();
    IplImage* doCanny( IplImage* in, double lowThresh, double highThresh, double aperture );
    IplImage* doPyrDown( IplImage* in, int filter);

    QLabel       	*surface;
	QCheckBox    	*chk1;
	QCheckBox    	*chk2;
    IplImage     	*image;
	QMenu			*menu;
    QImage       	 imageView;
    QVBoxLayout  	*main_layout;
    QHBoxLayout  	*slider1_layout;
    QLabel       	*radial_label;
    QLabel       	*freq_label;
    QLabel       	*peiod_label;
    QLabel       	*amp_label;
    QWidget      	*Main_Widget;
    QHBoxLayout  	*surface_layout;
    QVBoxLayout  	*surface2_layout;
    QHBoxLayout  	*option_layout;
    QHBoxLayout  	*data_layout;
    QHBoxLayout  	*plot_layout;
    QHBoxLayout  	*progress_layout;
    QHBoxLayout  	*button_layout;
    QPushButton  	*open_btn;
    QPushButton  	*calibrate_btn;
    QPushButton  	*analysis_btn;
    QPushButton  	*save_btn;
    CalibrateWindow *calibrate_window;
	QProgressBar	*progress;
    double        	treshold_1;
    double        	treshold_2;
    double        	erode_count;
    double        	dilate_count;
    char		 	*file_name;
};


#endif // MAINWINDOW_H
