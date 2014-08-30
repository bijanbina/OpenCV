#ifndef TRACK_H
#define TRACK_H

#include <cv.h>
#include <highgui.h>
#include <unistd.h>

//Find the colored spot
int find_spot (cv::Mat frame, std::pair<double, double>& spot, int w);

//Find region around the given spot
cv::Scalar find_region( cv::Mat& frame, const std::pair<double, double>& spot, int w);

//Find center using fitLine and Canny
std::pair<double, double> find_center( cv::Mat& frame, cv::Point& center );

//Detect the center and produce results, given an input frame

#endif
