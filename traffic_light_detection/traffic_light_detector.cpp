////
//// Created by Shakhobiddin on 4/14/2019.
////
//
#include "utils.cpp"
#include <vector>
#include <iostream>
using namespace std;
using namespace cv;
/*
 *
 */
Mat green_frame;
vector<Vec3f> circles;
int min_r=1, max_r=1;


class TrafficLightDetector{
    public:
    TrafficLightDetector(int minRadius=0, int maxRadius=0, bool strictMode=false){
        configure(minRadius, maxRadius, strictMode);
    }

    void configure(int minRadius, int maxRadius, bool strictMode){
        this->minRadius = minRadius;
        this->maxRadius = maxRadius;
        this->strictMode = strictMode;
    }

    bool isGreenHere(Mat frame){
        /// Gets simple RGB image frame
        Mat green_frame = mask_green(frame);
        GaussianBlur(green_frame, green_frame, Size(3, 3), 0);

        HoughCircles(green_frame, this->circles, CV_HOUGH_GRADIENT,  1, 100, 50, 30, this->minRadius, this->maxRadius);
        int noOfcircles = circles.size();
        int circleX, circleY, circleRadius;

        if(noOfcircles != 0){
            if(!this->strictMode){
                return true;
            }
            else
            {
                circleX = this->circles[0][0];
                circleY = this->circles[0][1];
                circleRadius = this->circles[0][2];

                if(noOfcircles == 1){
                    /// TODO:
                    int sum = countNonZero(green_frame);
                    if((float)sum/(float)green_frame.size().area() >= 0.05){
                        return true;
                    }
                    else return false;
                }

            }


        }
        else return false;
    }

    bool isRedHere(Mat frame){
        /// Gets simple RGB image frame
        Mat red_frame = mask_red(frame);
        GaussianBlur(green_frame, red_frame, Size(3, 3), 0);

        HoughCircles(red_frame, this->circles, CV_HOUGH_GRADIENT,  1, 100, 50, 30, this->minRadius, this->maxRadius);
        unsigned long long int noOfcircles = circles.size();
        int circleX, circleY, circleRadius;

        if(noOfcircles != 0){
            if(!this->strictMode){
                return true;
            }
            else
            {
                if(noOfcircles == 1){
                    /// TODO:
                    int sum = countNonZero(green_frame);
                    if((float)sum/(float)green_frame.size().area() >= 0.05){
                        return true;
                    }
                    else return false;
                }

            }


        }
        else return false;
    }

    int minRadius;
    int maxRadius;
    bool strictMode;
    Mat current_frame;
    vector<Vec3f> circles;
};
//
//void onChange(int, void*){
//    circles.clear();
//    Mat result;
//    HoughCircles(green_frame, circles, CV_HOUGH_GRADIENT,  1, 100, 50, 30, min_r, max_r);
//    green_frame.copyTo(result);
//
//    vector<Vec3f>::const_iterator itc = circles.begin();
//
//    while(itc != circles.end()){
//        cout << "I am here " << circles.size();
//        circle(result,
//               Point((*itc)[0], (*itc)[1]),
//               (*itc)[2],
//               Scalar(128),
//               2);
//
//        cout << "  "<< circles.at(0).val[0];
//        itc++;
//    }
//    cout << "\n";
//    imshow("img", result);
//    char k = (char)waitKey(25);
//    if(k=='q') exit(0);
//}



int main() {
    Mat frame = imread("../files/red_light_0.jpg", IMREAD_COLOR);

    TrafficLightDetector tfd(1, 95);

    cout << tfd.isGreenHere(frame);

    waitKey(0);
    return 0;
}


//    frame = imread("../files/green_light_3.png", IMREAD_COLOR);
//    //Mat red_frame = mask_red(frame);
//    green_frame = mask_green(frame);
//    Mat src = frame;
//    Mat src_gray;
//    Mat opened_frame;
//
//    //bitwise_not(green_frame, green_frame);
//
//    namedWindow("img", WINDOW_AUTOSIZE);
//
//    GaussianBlur(green_frame, green_frame, Size(3, 3), 0);
//    //Canny(green_frame, green_frame, 0, 60);
//    resize(green_frame, green_frame, Size(1920/2, 1080/2));
//
//
//    createTrackbar("Min", "img", &min_r, 1000, onChange);
//    createTrackbar("Max", "img", &max_r, 1000, onChange);
//    onChange(1, 0);



//    imshow("opened green mask", opened_frame);
    /// Read the image
    //src = imread(argv[1], 1);

//    if (!src.data) { return -1; }
//
//    /// Convert it to gray
//    cvtColor(src, src_gray, CV_BGR2GRAY);
//
//    /// Reduce the noise so we avoid false circle detection
//    GaussianBlur(src_gray, src_gray, Size(9, 9), 2, 2);
//
//    vector<Vec3f> circles;
//
//    /// Apply the Hough Transform to find the circles
//    HoughCircles(src_gray, circles, CV_HOUGH_GRADIENT, 1, src_gray.rows / 8, 200, 100, 0, 0);
//
//    /// Draw the circles detected
//    for (size_t i = 0; i < circles.size(); i++) {
//        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
//        int radius = cvRound(circles[i][2]);
//        // circle center
//        circle(src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
//        // circle outline
//        circle(src, center, radius, Scalar(0, 0, 255), 3, 8, 0);
//    }

    /// Show your results
//    namedWindow("Hough Circle Transform Demo", CV_WINDOW_AUTOSIZE);
//    imshow("Hough Circle Transform Demo", src);
