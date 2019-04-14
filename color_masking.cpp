//
// Created by Shakhobiddin on 4/14/2019.
//
#include <opencv2/opencv.hpp>
using namespace cv;

Mat mask_red(Mat frame){
    /*
     *  gets the Mat image
     *  returns b&w mask of the image where White is the detected Red
     */

    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    Mat m1, m2;
    //red color min/max range
    inRange(hsv, Scalar(0, 120, 70), Scalar(10, 255, 255), m1);
    inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), m2);

    m1 = m1 + m2;

    return m1;
}

Mat mask_green(Mat frame){
    /*
     *  gets the Mat image
     *  returns b&w mask of the image where White is Green
     * */

    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    Mat m1, m2;
    //green color min/max range
    inRange(hsv, Scalar(65, 60, 60), Scalar(80, 255, 255), m1);
    inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), m2);

    m1 = m1 + m2;

    return m1;
}
