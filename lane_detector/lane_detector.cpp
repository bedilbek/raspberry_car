// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "binarization_utils.h"
#include "bird_eye.h"
#include "line_utils.h"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

using namespace std;
using namespace cv;

int main()
{
	VideoCapture cap("files/test.mp4");

	if (!cap.isOpened()) {
		cout << "Error opening video stream or file" << endl;
		return -1;
	}
	bool playback = true;
	while (1)
	{
		if (playback)
		{
			Mat frame;
			// Capture frame-by-frame
			cap >> frame;

			// If the frame is empty, break immediately
			if (frame.empty())
				break;

			Mat binarized = binarize(frame, true);
			Mat forward, backward;
			Mat birdeye = bird_eye(binarized, forward, backward, false);
			Mat birdeye_org = bird_eye(frame, forward, backward, false);

			//auto lines = get_fits_by_sliding_windows(birdeye);
			//double offset = compute_offset_from_center(lines[0], lines[1], frame.cols);

			resize(birdeye, birdeye, Size(birdeye.cols / 2, birdeye.rows / 2));
			//resize(birdeye_org, birdeye_org, Size(birdeye.cols / 2, birdeye.rows / 2));
			imshow("Bin", birdeye);
			imshow("Org", birdeye_org);


			// Display the resulting frame
			//imshow("Frame", frame);
		}
		// Press  ESC on keyboard to exit
		char c = (char)waitKey(25);
		if (c == 27)
			break;
		if (c == 'p')
			playback = !playback;
	}

	// When everything done, release the video capture object
	cap.release();

	// Closes all the frames
	destroyAllWindows();

	return 0;
}

// Modify the program such that: 
// 1. It takes raspicam input frames instead of imread
// 2. When red traffic signs are identified, then the motors should act accordingly (for that use the motor control functions such as goForward(), goRight(), stop(), goLeft() etc.)