// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "binarization_utils.h"
#include "bird_eye.h"
#include "line_utils.h"
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include "globals.h"
#include <iostream>
#include <vector>
#include <raspicam/raspicam_cv.h>
#include "../controller.h"
#include "../ir_line_detector.h"

using namespace std;
using namespace cv;
using namespace raspicam;

int h = 0, s = 0, v = 0;
int hm = 0, sm = 0, vm = 0;
int th1 = 1, th2 = 1;

Scalar yellow_th_min = { 160, 40, 180 };
Scalar yellow_th_max = { 255, 255, 255 };


int main()
{
    if (wiringPiSetup() == -1)
        return 0;

	Controller controller;
	controller.init_dc_motor();
	IRLineDetector irLineDetector;


	RaspiCam_Cv video;
    video.open();

	if (!video.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}
	//short kdata[] = {};

	int roi = 20;
	Point seed;
	bool playback = true;
	Mat frame, mask, edge, first, highlight, medium, highligh;
	Mat r1, f, b;
	Mat prev, res;
	int indxl = -1, indxr = -1;
	int pindxl = -1, pindxr = -1;
	vector<int> reduced;
	int seq = 10;
	int threshold = 50;
	bool left = true, right = true;
	int interval = 3;
	int cframe = 0;
	int pthreshold = 3;
	Mat kernel = Mat(5, 5, CV_8UC1, Scalar(1));
	Point p1, p2;

	while (1)
	{
		if (playback)
		{
			// Capture frame-by-frame
			video.grab();
			video.retrieve(frame);
			resize(frame, frame, Size(384, 216));
//            imshow("frame", frame);
			// If the frame is empty, break immediately
			if (frame.empty())
				break;
		}


		if(irLineDetector.left_detected())
        {
		    controller.turn(100, 100);
		    delay(100);
        } else if (irLineDetector.right_detected())
        {
            controller.turn(-100, 100);
            delay(100);
        }
		else {
            Mat colorMask = Mat(frame.rows, frame.cols, CV_8UC1, Scalar(0));
            colorMask = thresh_frame_in_LAB(frame, yellow_th_min, yellow_th_max, false);

            colorMask = bird_eye(colorMask, f, b, false);
            mask = colorMask(Range(colorMask.rows - roi, colorMask.rows), Range::all());
            reduce(mask, reduced, 0, CV_REDUCE_SUM, CV_32SC1);

            for (int i = 0, j = reduced.size() - 1;
                 i < (reduced.size() / 2)
                 && j > (reduced.size() / 2); i++, j--) {
                if ((i < pindxl + threshold && i > pindxl - threshold) || pindxl < 0) {
                    if (reduced[i] > 0 && indxl < 0 && left) {
                        indxl = i;
                    }
                }

                /// Canny detector
//		Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

                if ((j < pindxr + threshold && j > pindxr - threshold) || pindxr < 0) {
                    if (reduced[j] > 0 && indxr < 0 && right) {
                        indxr = j;
                    }
                }
            }

            for (int i = colorMask.rows - 1; i > colorMask.rows - threshold; i--) {
                for (int j = indxl - threshold / 2, count = 0; j < indxl + threshold / 2 && j < colorMask.cols; j++) {
                    if (j < 0) {
                        j = 0;
                    }

                    if (colorMask.at<unsigned char>(i, j) > 0) {
                        count++;
                    } else {
                        count = 0;
                    }

                    if (count > seq && colorMask.at<unsigned char>(i, j) != 150) {
                        floodFill(colorMask, Point(j, i), Scalar(150));
                        break;
                    }
                }
            }

            for (int i = colorMask.rows - 1; i > colorMask.rows - threshold; i--) {
                for (int j = indxr - threshold / 2, count = 0; j < indxr + threshold / 2 && j < colorMask.cols; j++) {
                    if (j < 0) {
                        j = 0;
                    }

                    if (colorMask.at<unsigned char>(i, j) > 0) {
                        count++;
                    } else {
                        count = 0;
                    }

                    if (count > seq && colorMask.at<unsigned char>(i, j) != 150) {
                        floodFill(colorMask, Point(j, i), Scalar(150));
                        break;
                    }
                }
            }

            inRange(colorMask, Scalar(149), Scalar(151), colorMask);
            erode(colorMask, colorMask, kernel);
            colorMask = colorMask(Range(colorMask.rows - 400, colorMask.rows), Range::all());
            Canny(colorMask, colorMask, 80, 200);

//		imshow("img", colorMask);

            pindxl = indxl;
            pindxr = indxr;
            indxl = -1;
            indxr = -1;

            for (int i = 0; i < colorMask.cols; i++) {
                if (colorMask.at<unsigned char>(colorMask.rows - 80, i) > 0) {
                    p1 = Point(i, colorMask.rows - 80);
                    break;
                }
            }

            for (int i = 0; i < colorMask.cols; i++) {
                if (colorMask.at<unsigned char>(colorMask.rows - 50, i) > 0) {
                    p2 = Point(i, colorMask.rows - 50);
                    break;
                }
            }

            double steering;
            steering = ((p2.y - p1.y) != 0) ? atan(((double) (p1.x - p2.x)) / ((double) (p2.y - p1.y))) * 300.0 / 3.14
                                            : 0;
            steering *= -1;

            if (steering > 0) {
                controller.turn(100.0 - steering, 100);
            } else {
                steering = -steering;
                controller.turn(100, 100.0 - steering);
            }
        }
	}

	// When everything done, release the video capture object
	video.release();

	// Closes all the frames
	destroyAllWindows();

	return 0;
}