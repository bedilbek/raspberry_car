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
#include <csignal>
#include "datagram_socket_server.h"

using namespace std;
using namespace cv;
using namespace raspicam;

int thr = 0, str = 0;
int h = 0, s = 40, v = 102;
int hm = 5, sm = 255, vm = 255;
int center = 117;
int clip_limit = 2;
int tile_grid_size = 8;


int b = 30;
int c = 70;
double dis = 400;
bool ready_to_send = true;

int speed = 80; //atoi(argv[4]);
Sonar sonar;
IRLineDetector lineDetector;
IRWallDetector wallDetector;

Scalar min_th = { 18, 52, 102 };
Scalar max_th = { 255, 255, 255 };

VideoWriter video("sign_sample.avi", CV_FOURCC('M', 'J', 'P', 'G'), 24, cv::Size(640, 480), true);
Controller controller;
RaspiCam_Cv cam;

Mat frame_glob;
Mat frame_to_send;
Mat left_frame;
Mat right_frame;
Mat top_frame;
bool is_sending = false;
bool left_parking = true;
int current_sign = 6;
bool is_copying = false;
bool left_ir_line = false;
bool right_ir_line = false;

bool left_lane = true, right_lane = true;

void  signal_handler(int signum)
{

    cout << "Interrupt signal - " << signum << " received.\n";

    controller.stop();
	cam.release();
	video.release();
    // cleanup and close up stuff here
    // terminate program
    cout << "Everything is saved" << endl;
    exit(signum);
}

void update(int, void*)
{
	min_th = Scalar(h, s, v);
	max_th = Scalar(hm, sm, vm);
}

void* update_ir_line(void* par)
{
	while (true)
	{
		left_ir_line = lineDetector.left_detected();
		right_ir_line = lineDetector.right_detected();
	}
}

void* update_sonar(void* params)
{
	while (true)
	{
		dis = sonar.distance(500);
	}
}

void* resume_speed(void* params)
{
	delay(3000);
	speed = 80;
}
//
//void* sign_detection(void* params)
//{
//	cout << "Starting to create socket" << endl;
//	int sockfd, portno, n;
//	struct sockaddr_in serv_addr;
//	struct hostent *server;
//
//	char buffer[256];
//	portno = 8059;
//	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//	if (sockfd < 0)
//		cout << "ERROR opening socket" << endl;
//
//	cout << "getting host" << endl;
//	cout << "Host Got" << endl;
//
//	serv_addr.sin_family = AF_INET;
//	serv_addr.sin_port = htons(portno);
//	serv_addr.sin_addr.s_addr = INADDR_ANY;
//
//	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
//		cout << "ERROR connecting" << endl;
//
//	Point lp1 = Point(392, h);
//	Point lp2 = Point(542, 332);
//
//	Point rp1 = Point(76, s);
//	Point rp2 = Point(226, 312);
//
//	Point tp1 = Point(216, v);
//	Point tp2 = Point(366, 269);
//	cout << "Socket created" << endl;
//	socklen_t addr_len = sizeof(serv_addr);
//	while (1)
//	{
//		delay(5);
//		cout << frame.empty() << endl;
//		if (!frame.empty())
//		{
//			cout << "Starting" << endl;
//			char buffer[100];
//			int n, len;
//
//			frame.convertTo(frame_to_send, CV_8UC3);
//
//			is_sending = true;
//			
//			frame_to_send(Rect(lp1.x, lp1.y, 150, 250)).copyTo(left_frame);
//
//			
//			frame_to_send(Rect(rp1.x, rp1.y, 150, 250)).copyTo(right_frame);
//
//			
//			frame_to_send(Rect(tp1.x, tp1.y, 150, 250)).copyTo(top_frame);
//			
//
//			resize(left_frame, left_frame, Size(55, 100));
//			resize(right_frame, right_frame, Size(55, 100));
//			resize(top_frame, top_frame, Size(55, 100));
//			is_sending = false;
//			vector<unsigned char> left_vect;
//			vector<unsigned char> right_vect;
//			vector<unsigned char> top_vect;
//
//			left_vect.assign(left_frame.datastart, left_frame.dataend);
//			right_vect.assign(right_frame.datastart, right_frame.dataend);
//			top_vect.assign(top_frame.datastart, top_frame.dataend);
//
//			vector<unsigned char> data;
//			copy(left_vect.begin(), left_vect.end(), back_inserter(data));
//			copy(right_vect.begin(), right_vect.end(), back_inserter(data));
//			copy(top_vect.begin(), top_vect.end(), back_inserter(data));
//
//			cout << "Sending frame: " << data.size() << endl;
//			sendto(sockfd, data.data(), 10000, MSG_<< "L: " << (int)buffer[0] <<CONFIRM, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
//			sendto(sockfd, data.data(), data.size(), MSG_CONFIRM, (const struct sockaddr *) &serv_addr, sizeof(serv_addr));
//			cout << "Waiting to receive" << endl;
//			n = recvfrom(sockfd, buffer, 100, MSG_WAITALL, (struct sockaddr *) &serv_addr, &addr_len);
//			cout << "L: " << buffer[0] << " R: " << buffer[1] << " T: " << buffer[2] << endl;
//		}
//	}
//	close(sockfd);
//}

bool is_stop_detected(Mat frame)
{
	int scale = 4;
	Mat kernel = Mat(3, 3, CV_8UC1, Scalar(1));
	Mat img;
	resize(frame, img, Size(frame.cols / scale, frame.rows / scale));
	Mat3b bgr_inv = ~img;
	Mat3b hsv_inv;
	cvtColor(bgr_inv, hsv_inv, COLOR_BGR2HSV);
	Mat1b mask;
	inRange(hsv_inv, Scalar(90 - 15, 30, 230), Scalar(90 + 15, 255, 255), mask); // Cyan is 90
	erode(mask, mask, kernel);
	vector<Point> points;
	findNonZero(mask, points);
	if (points.size() > 60)
	{
		Point2f center;
		float radius;
		minEnclosingCircle(points, center, radius);
		cout << radius << endl;
		//imshow("c", mask);
		if (radius < (55 / scale) && radius >(30 / scale) && points.size() / (3.14*radius*radius) > 0.55)
			return true;
		waitKey(50);
	}
	return false;
}

Mat satur_image(Mat image, double alpha, int beta) 
{
	Mat new_image = Mat::zeros(image.size(), image.type());
	for (int y = 0; y < image.rows; y++) {
		for (int x = 0; x < image.cols; x++) {
			for (int c = 0; c < image.channels(); c++) {
				new_image.at<Vec3b>(y, x)[c] =
					saturate_cast<uchar>(alpha*image.at<Vec3b>(y, x)[c] + beta);
			}
		}
	}
	return new_image;
}

bool isRed(Mat frame) {
	Mat hsv;
	Mat m1, m2;
	Mat red_mask;
	Mat red_frame = satur_image(frame, 1.3, 40);
	int frame_width = red_frame.cols;
	int frame_height = red_frame.rows;
	cvtColor(red_frame, hsv, COLOR_BGR2HSV);

	//inRange(hsv, Scalar(hue_min, satur_min, value_min), Scalar(hue_max, satur_max, value_max), hsv);
	// red mask
	inRange(hsv, Scalar(0, 110, 60), Scalar(10, 255, 255), m1);
	inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), m2);
	red_mask = m1 + m2;

	//Cannying edge detection
	Canny(red_mask, red_mask, 10, 200, 3);

	vector<Vec3f> red_circles;

	GaussianBlur(red_mask, red_mask, Size(3, 3), 0);

	HoughCircles(red_mask, red_circles, CV_HOUGH_GRADIENT, 1, 5, 50, 30, 0, frame_height);

	unsigned long long int noOfcircles = red_circles.size();

	cout << noOfcircles << " circles detected at isStop() function." << endl;

	if (noOfcircles != 0) {
		cout << red_circles.at(0).val[0] << endl;

		return true;
	}
	else {
		return false;
	}
}

void *sign_updater(void*)
{
	datagram_socket_server *s = new datagram_socket_server(8073, "localhost", true, true);
	char message[1];
	while (1)
	{
		s->receive(message, 1);
		current_sign = (int)message[0];
		cout << "Sign: " << current_sign << endl;
	}

	while (true)
	{
		if (frame_glob.empty())
			continue;
		current_sign = is_stop_detected(frame_glob) ? 6 : 8;
		delay(30);
	}

}

void* frame_writer(void* param)
{
	while (true)
	{
		cam.grab();
		if (!is_copying)
		{
			cam.retrieve(frame_glob);
			if (frame_glob.empty())
				break;
			video.write(frame_glob);
		}
	}
}

bool obs_pos = false;

double minth = 0;
double maxth = 0;

bool sign_seen[] = { false, false, false, false, false, false };

void reset_signs(int index)
{
	if (!sign_seen[index])
	{
		for (int i = 0; i < 6; i++)
			sign_seen[i] == false;
	}
	else
	{
		sign_seen[index] = true;
	}
}

int main(int argc, char* argv[])
{
	wiringPiSetup();

	signal(SIGINT, signal_handler);

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
	lineDetector.init();
	wallDetector.init();

	cam.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	cam.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//cam.set(CV_CAP_PROP_FORMAT, CV_8UC1);
	//cam.set(CV_CAP_PROP_CONTRAST, 50);
	//cam.set(CV_CAP_PROP_SATURATION, 50);
	cam.set(CV_CAP_PROP_FPS, 24);
	cout << "Opening" << endl;
	cam.open();
	cout << "Opened" << endl;
	

	pthread_t sonar_thread;
	int sonar_thread_id = pthread_create(&sonar_thread, NULL, update_sonar, NULL);

	//pthread_t socket_thread;
	//int socket_thread_id = pthread_create(&socket_thread, NULL, sign_detection, NULL);

	pthread_t sign_thread;
	int sign_thread_id = pthread_create(&sign_thread, NULL, sign_updater, NULL);

	pthread_t ir_line_thread;
	pthread_create(&ir_line_thread, NULL, update_ir_line, NULL);

	
	
	//pthread_t video_thread;
	//pthread_create(&video_thread, NULL, frame_writer, NULL);


	//namedWindow("img", WINDOW_GUI_NORMAL);
	//namedWindow("org", WINDOW_GUI_NORMAL);
	//namedWindow("Controls");
	//namedWindow("left", WINDOW_GUI_NORMAL);
	//namedWindow("center", WINDOW_GUI_NORMAL);
	//namedWindow("right", WINDOW_GUI_NORMAL);
	//createTrackbar("H", "Controls", &h, 230, update);
	//createTrackbar("S", "Controls", &s, 230, update);
	//createTrackbar("V", "Controls", &v, 230, update);
	//createTrackbar("Hm", "Controls", &hm, 255, update);
	//createTrackbar("Sm", "Controls", &sm, 255, update);
	//createTrackbar("Vm", "Controls", &vm, 255, update);
	//createTrackbar("Clip", "Controls", &clip_limit, 100);
	//createTrackbar("Tile", "Controls", &tile_grid_size, 100);
	//createTrackbar("B", "Controls", &b, 300);
	//createTrackbar("C", "Controls", &c, 300);


	int offset = 0;
	int prev_offset = 0;
	int direction = 0;
	int frame_count = 0;
	

	// Set PID gains
	double k_p = atof(argv[1]);
	double k_i = atof(argv[2]);
	double k_d = atof(argv[3]);
	
	int lane_width = 165;
	int current_lane = 0;
	int prev_lane = 0;
	//createTrackbar("LW", "Controls", &lane_width, 300);

	// Initialize PID controller
	PID pid;
	pid.Init(k_p, k_i, k_d);

	
	controller.init_dc_motor();
	bool prev_obs = false;
	int obs_count = 0;
	bool current_obs = false;
	int prev_sign = 0;

	while (1)
	{
		//cout << "prev: " << prev_sign << " current: " << current_sign << endl;
		if (current_sign == 1 && (prev_sign ^ current_sign)) //Stop
		{
			prev_sign = current_sign;
			cout << "Stop" << endl;
			controller.stop();
			delay(5000);
		}
		else if (current_sign == 2 && (prev_sign ^ current_sign)) //Pedestrian
		{
			prev_sign = current_sign;
			cout << "Ped" << endl;
			speed *= 0.7;
			pthread_t speed_thread;
			int speed_thread_id = pthread_create(&speed_thread, NULL, resume_speed, NULL);
		}
		else if (current_sign == 3 && (prev_sign ^ current_sign)) //Left
		{
			prev_sign = current_sign;
			cout << "Left" << endl;
			left_parking = true;
		}
		else if (current_sign == 4 && (prev_sign ^ current_sign)) //Right
		{
			prev_sign = current_sign;
			cout << "RIght" << endl;
			left_parking = false;
		}
		else if (current_sign == 5 && (prev_sign ^ current_sign)) //Parking
		{
			prev_sign = current_sign;
			cout << "Park" << endl;
		}
		else if (current_sign == 6) //Red
		{
			prev_sign = current_sign;
			controller.stop();
		}
		else if ((current_sign == 8 && (prev_sign ^ current_sign)))
		{
			prev_sign = current_sign;
			cout << "Obstacle Mode" << endl;
			controller.stop();
			delay(500);

			avoid_obstacle(controller, sonar, lineDetector, wallDetector);
			break;
		}
		else if (dis < 15 && (wallDetector.left_detected() || wallDetector.right_detected()) && obs_count < 1)
		{
			cout << "Sonar: " << dis << endl;
			cout << "Woman" << endl;
			obs_count++;
			while (wallDetector.left_detected() || wallDetector.right_detected() || dis < 15)
			{
				controller.stop();
			}
			delay(500);
		}
		else
		{
			int start = millis();
			if (left_ir_line)
			{
				//cout << "moving right" << endl;
				controller.turn(-90, 90);
				delay(100);
				controller.stop();
				direction = 1;
			}
			if (right_ir_line)
			{
				//cout << "moving left" << endl;
				controller.turn(90, 90);
				delay(100);
				controller.stop();
				direction = -1;
			}
			
			////////
			Mat frame;
			//is_copying = true;
			//frame_glob.copyTo(frame);
			//is_copying = false;
			//if (frame.empty())
			//	continue;
			cam.grab();
			cam.retrieve(frame);
			if (frame.empty())
				break;
			frame_glob = frame;
			
			Mat colorMask = frame;
			resize(colorMask, colorMask, Size(colorMask.cols / 3, colorMask.rows / 3));
			
			cvtColor(colorMask, colorMask, COLOR_BGR2GRAY);
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
			left_lane = true;
			right_lane = true;
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
			//imshow("d", colorMask);

			pid.UpdateError(offset);
			controller.turn(pid.TotalError(), speed);
			//cout << millis() - start << endl;
			if (waitKey(5) == 'p')
				playback = !playback;
		}
	}
	std::cout << "finished" << endl;
	return 0;
}