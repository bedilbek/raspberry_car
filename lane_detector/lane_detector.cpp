// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "binarization_utils.h"
#include "bird_eye.h"
#include "line_utils.h"
#include <opencv2/opencv.hpp>
#include "globals.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

Mat src_gray, detected_edges;
Mat frame;
Mat bird_forw, bird_back, img_bird, img_bin, blend_on_road, blend_output;

int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";
double offset = 0;
int playback_slider = 0;
VideoCapture video;

array<Line, 2> lines = {Line(time_window), Line(time_window)};
int processed_frames = 0;

void on_canny_threshold_change(int, void *)
{
	cout << lowThreshold << endl;
}

void on_playback_slidebar_change(int, void *)
{
	video.set(CAP_PROP_POS_FRAMES, playback_slider);
}

void process_pipeline(Mat frame, bool keep_state = true)
{
    img_bin = binarize(frame, false);

    img_bird = bird_eye(img_bin, bird_forw, bird_back, false);

    if (processed_frames > 0 && keep_state && lines[0].detected && lines[1].detected)
    	get_fits_by_previous_fits(img_bird, lines[0], lines[1], false);
    else
		lines = get_fits_by_sliding_windows(img_bird);

	offset = compute_offset_from_center(lines[0], lines[1], frame.cols);

	blend_on_road = draw_back_onto_the_road(frame, img_bin, lines[0], lines[1], keep_state);

	imshow("blend", blend_on_road);
	processed_frames += 1;

}

int main()
{
	namedWindow(window_name, WINDOW_NORMAL);
	video = VideoCapture("files/test.mp4");
	int number_of_frames = video.get(CAP_PROP_FRAME_COUNT);


	createTrackbar("Min Threshold", window_name, &lowThreshold, max_lowThreshold, on_canny_threshold_change);
	createTrackbar("PLAYBACK", window_name, &playback_slider, number_of_frames, on_playback_slidebar_change);

	bool playback = true;

	while (1)
	{
		if (playback)
		{
			// Capture frame-by-frame
			video >> frame;

			// If the frame is empty, break immediately
			if (frame.empty())
				break;

			setTrackbarPos("PLAYBACK", window_name, ++playback_slider);

//			Mat binarized = binarize(frame, true);
		}
//		cvtColor(frame, src_gray, COLOR_BGR2GRAY);

		/// Reduce noise with a kernel 3x3
//		blur( src_gray, detected_edges, Size(3,3) );

		/// Canny detector
//		Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

//		imshow(window_name, detected_edges);
		process_pipeline(frame, true);
//			Mat forward, backward;
//			Mat birdeye = bird_eye(binarized, forward, backward, false);
//			Mat birdeye_org = bird_eye(frame, forward, backward, false);

		//auto lines = get_fits_by_sliding_windows(birdeye);
		//double offset = compute_offset_from_center(lines[0], lines[1], frame.cols);

//			resize(birdeye, birdeye, Size(birdeye.cols / 2, birdeye.rows / 2));
		//resize(birdeye_org, birdeye_org, Size(birdeye.cols / 2, birdeye.rows / 2));
//		imshow(window_name, detected_edges);
		imshow("Org", frame);

		// Press  ESC on keyboard to exit
		char c = (char)waitKey(25);
		if (c == 27)
			break;
		if (c == 'p')
			playback = !playback;
	}

	// When everything done, release the video capture object
	video.release();

	// Closes all the frames
	destroyAllWindows();

	return 0;
}

// Modify the program such that: 
// 1. It takes raspicam input frames instead of imread
// 2. When red traffic signs are identified, then the motors should act accordingly (for that use the motor control functions such as goForward(), goRight(), stop(), goLeft() etc.)