#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <opencv/highgui.h>
#include <QtWidgets>
#include <cv.h>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    char*     FileOpName; //declare FileOpName as IplImage
    IplImage*   imagerd; //declare imagerd as IplImage

private slots:
    void slider_change(int);

private:
    void CreateLayout();
    void openImage();
    IplImage* doCanny( IplImage* in, double lowThresh, double highThresh, double aperture );
    IplImage* doPyrDown( IplImage* in, int filter);

    QLabel       *surface;
    IplImage     *image;
    QImage        imageView;
    QVBoxLayout  *main_layout;
    QHBoxLayout  *slider1_layout;
    QLabel       *slider1_label;
    QWidget      *Main_Widget;
    QSlider      *slider1;
};


#endif // MAINWINDOW_H
