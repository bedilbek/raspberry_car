//
// Created by Shakhobiddin on 4/14/2019.
//
#include <opencv2/opencv.hpp>
#include <vector>
using namespace cv;

Mat mask_red(Mat frame){
    /*
     *  gets the Mat image
     *  returns b&w mask of the image where White is the detected Red
     */
    std::vector<Mat> vmat;
    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    Mat m1, m2;
    //red color min/max range
    inRange(hsv, Scalar(0, 110, 60), Scalar(10, 255, 255), m1);
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
    inRange(hsv, Scalar(44, 60, 60), Scalar(80, 255, 255), m1);
    //inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), m2);

    //m1 = m1 + m2;

//    Mat eroded;
//    erode(m1, eroded, Mat());

    return m1;
}

Mat mask_blue(Mat frame){
    /*
     *  gets the Mat image
     *  returns b&w mask of the image where White is Green
     * */

    Mat hsv;
    cvtColor(frame, hsv, COLOR_BGR2HSV);

    Mat m1, m2;
    //green color min/max range
    inRange(hsv, cv::Scalar(80, 65, 65, 0), cv::Scalar(140, 255, 255, 0), m2);


    m1 = m1 + m2;

    return m1;
}
