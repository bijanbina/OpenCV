#ifndef CALIBRATEWINDOW_H
#define CALIBRATEWINDOW_H

#include <QMainWindow>
#include <opencv/highgui.h>
#include <QtWidgets>
#include <cv.h>
#include <trmmosbat.h>



class CalibrateWindow : public QDialog {
    Q_OBJECT
public:
    explicit CalibrateWindow(QWidget *parent = 0);
    ~CalibrateWindow();
    trmParam start();


    void resizeEvent ( QResizeEvent * event )
    {
        setGeometry((qApp->desktop()->geometry().center() - rect().center()).x(),(qApp->desktop()->geometry().center() - rect().center()).y(),rect().width(),rect().height());
    }

protected:
    char        *FileOpName; //declare FileOpName as IplImage

private slots:
    void slider1_change(int);
    void slider2_change(int);
    void slider3_change(int);
    void slider4_change(int);
    void chk1_change();
    void chk2_change();
    void open_clicked();
    void openimage_clicked();
    void save_clicked();
    void replace_clicked();
    void back_clicked();
    void state_change(int changed = 0);
    void result_clicked(bool);
    void edgedetect_clicked(bool);
    void loop_clicked(bool);
    void equal_clicked(bool);
    void next_clicked();
    void frame_clicked(bool);
    void width_clicked();


private:
    void CreateLayout(QWidget *parent);
    void CreateMenu();
    void openImage();
    void find_corner(IplImage* in ,double quality_level ,double min_distance ,int MAX_CORNERS , double k) ;
    void drawMark(cv::Mat img , CvPoint pt, CvScalar) ;
    void MyFilledCircle( cv::Mat img, cv::Point center );

    QLabel       *surface;
	QCheckBox    *chk1;
	QCheckBox    *chk2;
    QMenuBar     *menu;
    QMenu        *file_menu;
    QMenu		 *mode_menu;
    QMenu		 *option_menu;
    QMenu		 *help_menu;
    QAction		 *a_save;
    QAction		 *a_open;
    QAction		 *a_open_image;
    QAction		 *a_replace;
    QAction		 *a_edge;
    QAction		 *a_result;
    QAction		 *a_loop;
    QAction		 *a_about;
    QAction		 *a_equal;
    QAction		 *a_frame;
    QAction		 *a_width;
    IplImage     *image;
    IplImage     *imgout;
    IplImage     *imagesrc; 
    CvCapture    *capture;
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
    QHBoxLayout  *chkbox_layout;
    QHBoxLayout  *button_layout;
    QPushButton  *back_btn;
    QPushButton  *next_btn;
    QPushButton  *replace_btn;
    double        treshold_1;
    double        treshold_2;
    double        treshold_3;
    double        treshold_4;
    char         *file_name;
    int           framePosition;
    int           surface_height;
    int           count;
    double        surface_width;
    trmParam      filter_param;
};


#endif // MAINWINDOW_H
