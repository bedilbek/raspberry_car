// LaneDetector.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <fstream>
#include "./controller.h"
#include "./lane_detector/binarization_utils.h"
#include "./lane_detector/bird_eye.h"
#include <raspicam/raspicam_cv.h>
#include "./PID.h"
#include "./obstacle_avoidance.h"
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>

using namespace std;
using namespace cv;
using namespace raspicam;

int thr = 0, str = 0;
int h = 0, s = 40, v = 102;
int hm = 5, sm = 255, vm = 255;
int center = 117;
int clip_limit = 2;
int tile_grid_size = 8;

int b = 50;
int c = 100;
double dis = 400;
bool ready_to_send = true;
Sonar sonar;
Scalar min_th = { 18, 52, 102 };
Scalar max_th = { 255, 255, 255 };

Mat frame;
Mat frame_to_send;
bool is_sending = false;

void update(int, void*)
{
	min_th = Scalar(h, s, v);
	max_th = Scalar(hm, sm, vm);
}

void* update_sonar(void* params)
{
	while (true)
	{
		dis = sonar.distance(500);
	}
}

void* sign_detection(void* params)
{
	cout << "Starting to create socket" << endl;
	int sockfd, portno, n;
	struct sockaddr_in serv_addr;
	struct hostent *server;

	char buffer[256];
	portno = 8059;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0)
		cout << "ERROR opening socket" << endl;

	cout << "getting host" << endl;
	cout << "Host Got" << endl;

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	serv_addr.sin_addr.s_addr = INADDR_ANY;

	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		cout << "ERROR connecting" << endl;

	Point lp1 = Point(392, 82);
	Point lp2 = Point(542, 332);

	Point rp1 = Point(76, 62);
	Point rp2 = Point(226, 312);

	Point tp1 = Point(216, 19);
	Point tp2 = Point(366, 269);
	cout << "Socket created" << endl;
	socklen_t addr_len = sizeof(serv_addr);
	while (1)
	{
		delay(5);
		//cout << frame.empty() << endl;
		if (!frame.empty())
		{
			cout << "Starting" << endl;
			char buffer[100];
			int n, len;

			frame.convertTo(frame_to_send, CV_8UC3);

			is_sending = true;
			Mat left;
			frame_to_send(Rect(lp1.x, lp1.y, 150, 250)).copyTo(left);

			Mat right;
			frame_to_send(Rect(rp1.x, rp1.y, 150, 250)).copyTo(right);

			Mat top;
			frame_to_send(Rect(tp1.x, tp1.y, 150, 250)).copyTo(top);
			is_sending = false;

			resize(left, left, Size(55, 100));
			resize(right, right, Size(55, 100));
			resize(top, top, Size(55, 100));

			vector<unsigned char> left_vect;
			vector<unsigned char> right_vect;
			vector<unsigned char> top_vect;

			left_vect.assign(left.datastart, left.dataend);
			right_vect.assign(right.datastart, right.dataend);
			top_vect.assign(top.datastart, top.dataend);

			vector<unsigned char> data;
			copy(left_vect.begin(), left_vect.end(), back_inserter(data));
			copy(right_vect.begin(), right_vect.end(), back_inserter(data));
			copy(top_vect.begin(), top_vect.end(), back_inserter(data));

			cout << "Sending frame: " << data.size() << endl;
			//sendto(sockfd, data.data(), 10000, MSG_CONFIRM, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
			sendto(sockfd, data.data(), data.size(), MSG_CONFIRM, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
			cout << "Waiting to receive" << endl;
			n = recvfrom(sockfd, buffer, 100, MSG_WAITALL, (struct sockaddr *) &serv_addr, &addr_len);
			cout << "Detected: " << (int)buffer[0] << endl;
		}
	}
	close(sockfd);
}

void error(const char *msg)
{
	perror(msg);
	exit(0);
}

double minth = 0;
double maxth = 0;

int main(int argc, char* argv[])
{
	wiringPiSetup();

	int roi = 20;
	bool playback = true;
	Mat mask;
	Mat fr, bc;
	int indxl = -1, indxr = -1;
	int pindxl = -1, pindxr = -1;
	vector<int> reduced;
	int seq = -1;
	int threshold = 50;
	int interval = 3;
	int pthreshold = 3;
	sonar.init(24, 25);


	IRLineDetector lineDetector;
	IRWallDetector wallDetector;
	
	pthread_t sonar_thread;
	int sonar_thread_id = pthread_create(&sonar_thread, NULL, update_sonar, NULL);

	//pthread_t socket_thread;
	//int socket_thread_id = pthread_create(&socket_thread, NULL, sign_detection, NULL);

	lineDetector.init();
	wallDetector.init();

	//namedWindow("img", WINDOW_GUI_NORMAL);
	//namedWindow("org", WINDOW_GUI_NORMAL);
	//namedWindow("Controls", WINDOW_GUI_NORMAL);
	//createTrackbar("H", "Controls", &h, 255, update);
	//createTrackbar("S", "Controls", &s, 255, update);
	//createTrackbar("V", "Controls", &v, 255, update);
	//createTrackbar("Hm", "Controls", &hm, 255, update);
	//createTrackbar("Sm", "Controls", &sm, 255, update);
	//createTrackbar("Vm", "Controls", &vm, 255, update);
	//createTrackbar("Clip", "Controls", &clip_limit, 100);
	//createTrackbar("Tile", "Controls", &tile_grid_size, 100);
	//createTrackbar("B", "Controls", &b, 300);
	//createTrackbar("C", "Controls", &c, 300);
	
	RaspiCam_Cv cam;
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	cam.set(CV_CAP_PROP_CONTRAST, 50);
	cam.set(CV_CAP_PROP_SATURATION, 50);
	cam.set(CV_CAP_PROP_FPS, 24);
	cout << "Opening" << endl;
	cam.open();
	cout << "Opened" << endl;

	int offset = 0;
	int prev_offset = 0;
	int direction = 0;
	int frame_count = 0;
	

	// Set PID gains
	double k_p = 1.5; //atof(argv[1]);
	double k_i = 0.0; //atof(argv[2]);
	double k_d = 1.8; //atof(argv[3]);
	int speed = 60; //atoi(argv[4]);
	int lane_width = 165;
	int current_lane = 0;
	int prev_lane = 0;
	//createTrackbar("LW", "Controls", &lane_width, 300);

	// Initialize PID controller
	PID pid;
	pid.Init(k_p, k_i, k_d);

	Controller controller;
	controller.init_dc_motor();
	bool prev_obs = false;
	int obs_count = 0;
	bool current_obs = false;

	while (1)
	{
		//ready_to_send = false;
		if (playback)
		{
			cam.grab();
			cam.retrieve(frame);
			if (frame.empty())
				break;
		}
		//ready_to_send = true;
		
		frame_count++;
		Mat colorMask = frame;

		resize(colorMask, colorMask, Size(colorMask.cols / 3, colorMask.rows / 3));
		//Bird eye transformation
		colorMask = bird_eye(colorMask, fr, bc, 3, 50);
		
		GaussianBlur(colorMask, colorMask, Size(3, 3), tile_grid_size);
		Canny(colorMask, colorMask, b, c);
		//imshow("edge", colorMask);

		colorMask = colorMask(Range(colorMask.rows * 0.8, colorMask.rows - 20), Range::all());
		floodFill(colorMask, Point(colorMask.cols / 2 + prev_offset, colorMask.rows - 5), 150);
		inRange(colorMask, { 149 }, { 151 }, colorMask);

		int p_offset = 5;
		Point pl1 = Point(0, 0);
		Point pl2 = Point(0, 0);

		for (int j = 0, c = 0; j < colorMask.cols; j++)
		{
			if (colorMask.at<unsigned char>(colorMask.rows - 2, j) > 0)
			{
				pl1 = Point(j, colorMask.rows - 2);
				break;
			}
		}

		for (int j = 0, c = 0; j < colorMask.cols; j++)
		{
			if (colorMask.at<unsigned char>(p_offset, j) > 0)
			{
				pl2 = Point(j, p_offset);
				break;
			}
		}

		Point pr1 = Point(0, 0);
		Point pr2 = Point(0, 0);
		Mat flipped;

		colorMask.copyTo(flipped);
		cv::flip(flipped, flipped, 1);

		for (int j = 0, c = 0; j < flipped.cols; j++)
		{
			if (flipped.at<unsigned char>(flipped.rows - 2, j) > 0)
			{
				pr1 = Point(j, flipped.rows - 2);
				break;
			}
		}

		for (int j = 0, c = 0; j < flipped.cols; j++)
		{
			if (flipped.at<unsigned char>(p_offset, j) > 0)
			{
				pr2 = Point(j, p_offset);
				break;
			}
		}

		double angl = 1.57;
		double angr = 1.57;
		bool left_lane = true, right_lane = true;

		if ((pl1.x - pl2.x) != 0)
			angl = atan(((double)(pl2.y - pl1.y) / (double)(pl2.x - pl1.x)));
		else if (pl1.x < 2)
			left_lane = false;

		if ((pr1.x - pr2.x) != 0)
			angr = atan(((double)(pr2.y - pr1.y) / (double)(pr2.x - pr1.x)));
		else if (pr1.x < 2)
			right_lane = false;

		if (left_lane && !right_lane && abs(angl) < 1.52)
			direction = 1;
		else if (!left_lane && right_lane && abs(angr) < 1.52)
			direction = -1;
		else if (left_lane && right_lane && abs(angr) < 1.52 && abs(angl) < 1.52)
		{
			if (angl > 0 && angr < 0)
				direction = -1;
			else if (angl < 0 && angr > 0)
				direction = 1;
			else
				direction = 0;
		}
		else
			direction = 0;

		//if (playback)
		//	cout << direction << " - " << left_lane << " & " << right_lane << endl;

		prev_offset = 80 * direction;

		int nonzero = 0;
		if (direction == 0)
		{
			if (left_lane || right_lane)
			{
				for (nonzero = 0; nonzero < colorMask.cols; nonzero++)
					if (colorMask.at<unsigned char>(colorMask.rows - 2, nonzero) > 0)
						break;
				offset = lane_width / 2 + nonzero - colorMask.cols / 2;
			}
			else
			{
				offset = 0;
				//avoid_obstacle();
			}
		}
		else if (direction == 1)
		{
			if ((pl1.x - pl2.x) != 0)
				angr = atan(abs((double)(pl2.y - pl1.y) / (double)(pl2.x - pl1.x)));
			offset = colorMask.cols / 2 - pl1.x - lane_width / 2 * sin(angr);
			offset *= -1;
		}
		else
		{
			if ((pr1.x - pr2.x) != 0)
				angl = atan(abs((double)(pr2.y - pr1.y) / (double)(pr2.x - pr1.x)));
			offset = colorMask.cols / 2 - pr1.x - lane_width / 2 * sin(angl);
		}

		int turning_delay = 400;

		//if (dis > 30 && !left_lane && !right_lane)
		//{
		//	cout << "> 30 " << endl;
		//	controller.turn(100, 100);
		//	delay(100);
		//	controller.stop();
		//}

		if (dis < 30 && (wallDetector.left_detected() || wallDetector.right_detected()) && obs_count < 1)
		{
			cout << "Sonar: " << dis << endl;
			cout << "Woman" << endl;
			obs_count++;
			while ((wallDetector.left_detected() || wallDetector.right_detected()))
			{
				controller.stop();
			}
		}
		else if (wallDetector.left_detected() && wallDetector.right_detected() && !left_lane && !right_lane && obs_count >= 1)
		{
			cout << "Obstacle Mode" << endl;
			controller.stop();
			avoid_obstacle(controller, sonar, lineDetector, wallDetector);
		}
		else
		{
			pid.UpdateError(offset);
			controller.turn(pid.TotalError(), speed);
		}

		//line(original, Point(original.cols / 2 + offset, original.rows - 5), Point(original.cols / 2, original.rows - 5), 150, 5);
		//line(colorMask, Point(colorMask.cols / 2 + offset, colorMask.rows - 5), Point(colorMask.cols / 2, colorMask.rows - 5), 150, 5);

		if (waitKey(5) == 'p')
			playback = !playback;
	}
	std::cout << "finished" << endl;
	return 0;
}