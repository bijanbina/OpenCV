#ifndef CALIBRATEIIWINDOW_H
#define CALIBRATEIIWINDOW_H


#include <QMainWindow>
#include <opencv/highgui.h>
#include <QtWidgets>
#include <cv.h>
#include "trmmark.h"

#define TRM_STATE_FRAME_II     0
#define TRM_STATE_MAHYAR_II    1
#define TRM_STATE_RESULT_II    2

class CalibrateIIWindow : public QDialog {
    Q_OBJECT
public:
    explicit CalibrateIIWindow(QWidget *parent = 0);
    ~CalibrateIIWindow();
    trmParam start(int startFrame = 0);

protected:
    char        *FileOpName; //declare FileOpName as IplImage

    void resizeEvent ( QResizeEvent * event )
    {
        setGeometry((qApp->desktop()->geometry().center() - rect().center()).x(),(qApp->desktop()->geometry().center() - rect().center()).y(),rect().width(),rect().height());
    }

private slots:
    void slider1_change(int);
    void slider2_change(int);
    void open_clicked();
    void openimage_clicked();
    void save_clicked();
    void calibrate_clicked();
    void back_clicked();
    void state_change(int changed = 0);
    void next_clicked();
    void width_clicked();
    void equal_clicked(bool state);
    void mreversed_clicked(bool state);
    void mclose_clicked(bool state);
    void mopen_clicked(bool state);
    void mnormal_clicked(bool state);


private:
    void CreateLayout(QWidget *parent);
    void CreateMenu();
    void openImage();
    void find_corner(IplImage* in ,double quality_level ,double min_distance ,int MAX_CORNERS , double k) ;
    void drawMark(cv::Mat img , CvPoint pt, CvScalar) ;
    void MyFilledCircle( cv::Mat img, cv::Point center );

    QLabel       *surface;
    QMenuBar     *menu;
    QMenu        *file_menu;
    QMenu		 *morphology_menu;
    QMenu		 *option_menu;
    QMenu		 *help_menu;
    QAction		 *a_save;
    QAction		 *a_open;
    QAction		 *a_open_image;
    QAction		 *a_replace;
    QAction		 *a_about;
    QAction		 *a_equal;
    QAction		 *a_width;
    QAction		 *a_loop;
    QAction		 *a_mnormal;
    QAction		 *a_mreversed;
    QAction		 *a_mopen;
    QAction		 *a_mclose;
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
    QHBoxLayout  *surface_layout;
    QVBoxLayout  *surface2_layout;
    QHBoxLayout  *option_layout;
    QHBoxLayout  *chkbox_layout;
    QHBoxLayout  *button_layout;
    QPushButton  *back_btn;
    QPushButton  *next_btn;
    QPushButton  *calibrate_btn;
    QLabel       *color1_prev;
    QLabel       *color2_prev;
    QImage        color1_img;
    QImage        color2_img;
    QLineEdit    *color1_H;
    QLineEdit    *color1_S;
    QLineEdit    *color1_V;
    QLineEdit    *color2_H;
    QLineEdit    *color2_S;
    QLineEdit    *color2_V;
    double        treshold_1;
    double        treshold_2;
    int           framePosition;
    int           surface_height;
    int           count;
    int           calibrate_state;
    int           morphology_state;
    double        surface_width;
    QString       filename;
    bool          isVideo;
    trmParam      filter_param;
};


#endif // CALIBRATEIIWINDOW_H
