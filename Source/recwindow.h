#ifndef RECWINDOW_H
#define RECWINDOW_H

#include <QMainWindow>
#include <opencv/highgui.h>
#include <QtWidgets>
#include <cv.h>
#include "trmmark.h"

#define TRM_STATE_FRAME     0
#define TRM_STATE_EDGE      1
#define TRM_STATE_CORNER    2
#define TRM_STATE_RESULT    3

void *capture_main(void *data);

class RecWindow : public QDialog {
    Q_OBJECT
public:
    explicit RecWindow(QWidget *parent = 0);
    ~RecWindow();

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
    void save_clicked();
    void rec_clicked();
    void exit_clicked();
    void analysis_clicked();

private:
    void        CreateLayout(QWidget *parent);
    void        CreateMenu();
    QStringList getDeviceName();

    QLabel       *surface;
    QCheckBox    *chk1;
    QCheckBox    *chk2;
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
    QPushButton  *rec_btn;
    QPushButton  *save_btn;
    QPushButton  *analysis_btn;
    QString       filename;
    double        treshold_1;
    double        treshold_2;
    double        treshold_3;
    double        treshold_4;
    int           framePosition;
    int           surface_height;
    int           count;
    int           calibrate_state;
    int           morphology_state;
    double        surface_width;
    bool          isVideo;
    trmParam      filter_param;
    pthread_t       thread_cam;
    int           camID;
};


#endif // RECWINDOW_H
