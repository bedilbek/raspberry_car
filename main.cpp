#include <cstdio>
#include <iostream>
#include <raspicam/raspicam_cv.h>
#include <wiringPi.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <ctime>
#include <stdio.h>
#include <softPwm.h>
#include <zconf.h>
#include "libSonar.h"
#include "controller.h"
#include "ir_wall_detector.h"
#include "ir_line_detector.h"
#include <thread>
#include <stdlib.h>
#include <fstream>
#include <csignal>
#include "datagram_socket_server.h"
#include <uuid/uuid.h>
#include <getopt.h>
#include <string>
#include <pthread.h>

using namespace std;
using namespace raspicam;
using namespace cv;

int sign_counter = -1;

Controller controller;
RaspiCam_Cv raspiCam_cv;
VideoWriter videoWriter;
int frame_counter = 0;
int speed, steering;
ofstream fout;

// MAIN PARAMATERS
enum MODE  {
	ALGORITHM_MODE,
	AI_MODE,
	JOYSTICK_MODE
};
int mode = JOYSTICK_MODE;
int udp_socket_port = 8059;
string udp_socket_host = "localhost";
int video_frame_rate = 10;
int video_frame_width = 640;
int video_frame_height = 480;
string model_path;
bool debugging = false;
bool logging = true;
bool recording = false;
bool img_show = false;
bool started = false;

void usage()
{
	cout <<
	     "Usage: [--mode MODE] [options]\n"
	     "Mode:\n"
	     "-m MODE, --mode MODE                    Set the mode for the car (default: 3)\n"
	     "   0 - ALGORITHM\n"
	     "   1 - AI\n"
	     "   2 - JOYSTICK\n"
	     "Options:\n"
	     "-p PORT, --port PORT                    PORT number for UDP socket server initialization for joystick (default: 8059)\n"
	     "-s HOST, --host HOST                    HOST string for UDP socker server initialization for joystick (default: localhost)\n"
	     "-d, --debug                             Prints lots of debugging information (default: false)\n"
	     "-i, --img                               Shows image being captured (default: false)\n"
	     "-r, --record                            Enable video recording (default: false)\n"
	     "-f FRATE, --frame_rate FRATE            Set video capture frame rate (default: 10)\n"
	     "-w WIDTH, --frame_width WIDTH           Set video capture frame width (default: 640)\n"
	     "-g HEIGHT, --frame_height HEIGHT        Set video capture frame width (default: 480)\n"
	     "-a FILE, --model FILE                   Read model for AI prediction [mode=2]\n"
	     "-l, --no_log                            Disable joystick controller logging information (default: true) [mode=3]\n"
	     "-h, --help                              Show help info\n";
}

void  signal_handler(int signum)
{

    cout << "Interrupt signal - " << signum << " received.\n";

    controller.stop();
    raspiCam_cv.release();
    videoWriter.release();
    fout.close();
    // cleanup and close up stuff here
    // terminate program
    cout << "Everything is saved" << endl;
    exit(signum);
}

void *sign_updater(void*)
{
    datagram_socket_server *s = new datagram_socket_server(8073, "localhost", true, true);
    char message[1];
    while (1)
    {
        s->receive(message, 1);
        sign_counter = (int) message[0];
    }
}

char* generate_name()
{
    uuid_t  uuid4;
    uuid_generate(uuid4);
    char *uuid = (char *) malloc(41);
    uuid_unparse(uuid4, uuid);
    uuid[36] = '.';
    uuid[37] = 'a';
    uuid[38] = 'v';
    uuid[39] = 'i';
    uuid[40] = '\0';
    return uuid;
}

int is_left_detected(Mat frame)
{
    int scale = 2;
    Mat img;
    Mat kernel = Mat(3, 3, CV_8UC1, Scalar(1));
    resize(frame, img, Size(frame.cols / scale, frame.rows / scale));
    Mat3b bgr_inv = ~img;
    Mat3b hsv_inv;
    cvtColor(bgr_inv, hsv_inv, COLOR_BGR2HSV);
    Mat1b mask;
    inRange(hsv_inv, Scalar(0 - 15, 40, 200), Scalar(0 + 15, 255, 255), mask); // Cyan is 90
    imshow("Before", mask);
    waitKey(0);
    erode(mask, mask, kernel);

    vector<Point> points;
    findNonZero(mask, points);
    cout << "points size: " << points.size() << endl;
    if (points.size() > 40)
    {
        Point2f center;
        float radius;
        minEnclosingCircle(points, center, radius);
        cout << "R: " << radius << " A: " << points.size() / (3.14*radius*radius) << endl;


        if (radius < (55 / scale) && radius > (32 / scale) && points.size() / (3.14*radius*radius) > 0.5)
        {
            circle(mask, center, radius, 150);

            imshow("Ar", mask);
            waitKey();

            vector<int> sum;
            reduce(mask, sum, 0, REDUCE_SUM);
            double min, max;
            int min_idx, max_idx;
            minMaxIdx(sum, &min, &max, &min_idx, &max_idx);
            cout << min_idx << "  " << max_idx << endl;

            if (min_idx > max_idx)
            {
                //cout << "Right" << endl;
                return 1;
            }
            else
            {
                //cout << "Left" << endl;
                return -1;
            }
        }
    }

    return 0;
}

void * video_writer(void*)
{
	raspiCam_cv.set(CAP_PROP_FPS, video_frame_rate);
	raspiCam_cv.set(CAP_PROP_FRAME_HEIGHT, video_frame_height);
	raspiCam_cv.set(CAP_PROP_FRAME_WIDTH, video_frame_width);
	raspiCam_cv.setVideoStabilization(true);
    videoWriter = VideoWriter();

	if (recording)
	{
		raspiCam_cv.open();
		if (img_show)
			namedWindow("frame", WINDOW_GUI_NORMAL);
	}

	double frame_rate = raspiCam_cv.get(CAP_PROP_FPS);
    double frame_width = raspiCam_cv.get(CAP_PROP_FRAME_WIDTH);
    double frame_height = raspiCam_cv.get(CAP_PROP_FRAME_HEIGHT);

    char *uuid, *filesave;

    Mat img;

    while (true)
    {
        cout << "thread, started: " << started << endl;
        if (started) {
            if (debugging && recording)
                cout << "VideoCapture is being started for recording" << endl;
            if (debugging && logging)
                cout << "Joystick controller log is being started for writing" << endl;
            uuid = generate_name();
            filesave = (char *) malloc(41);
            if (recording)
                videoWriter.open(uuid, CV_FOURCC('M', 'J', 'P', 'G'), frame_rate, cv::Size(frame_width, frame_height), true);
            strcpy(filesave, uuid);
            filesave[37] = 't';
            filesave[38] = 'x';
            filesave[39] = 't';
            if (logging)
                fout.open(filesave);
        }
        raspiCam_cv.grab();
        raspiCam_cv.retrieve(img);
        if (img_show)
        {
            imshow("frame", img);
            waitKey(10);
        }

        while (started) {
            if (recording)
            {
                raspiCam_cv.grab();
                raspiCam_cv.retrieve(img);
                if (img_show)
                {
                    imshow("frame", img);
                    waitKey(1);
                }
                videoWriter.write(img);
            }
            frame_counter++;
            if (logging)
                fout << speed << "," << steering << endl;
        }

        if (recording and !started)
        {
            if (videoWriter.isOpened())
            {
                videoWriter.release();
                if (debugging)
                    cout << "VideoWriter released and video saved "<< uuid << endl;
            }
        }
        if (logging and !started)
        {
            if (fout.is_open())
            {
                fout.close();
                if (debugging)
                    cout << "Joystick Controller log file saved " << filesave << endl;
            }
        }
    }

}

void mode_joystick()
{
	datagram_socket_server *s = new datagram_socket_server(udp_socket_port, const_cast<char *>(udp_socket_host.c_str()), true, true);
	char message[1024];

	controller.init_dc_motor();
	cout << "ready to receive" << endl;
    while (1)
	{
        s->receive(message, 1024);
        speed = (int)message[0] == 0 ? ((int)message[1]) * -1 : (int) message[1];
        steering = (int)message[2] == 0 ? ((int)message[3]) : (int) message[3] * -1;
        started = (int) message[4] == 1;
        cout << "message: " << (int) message[4] << endl;
        cout << "started: " << started << endl;
        controller.turn(steering, speed);
		if (debugging)
            cout <<"::::" << message << "::::" << "speed: " << speed << " steering: " << steering << " started: " << started << endl;
		delay(20);
	}

}

void mode_ai()
{

}

void mode_algorithm()
{

}

int main(int argc, char **argv) {
    Mat img = imread("files/left1.jpg");
    is_left_detected(img);

    return 0;
	const char* const short_options = "m:p:s:drf:w:g:a:lh";
	const option long_options[] = {
			{"mode",            required_argument,  nullptr, 'm'},
			{"port",            required_argument,  nullptr, 'p'},
			{"host",            required_argument,  nullptr, 's'},
			{"debug",           no_argument,        nullptr, 'd'},
			{"img",             no_argument,        nullptr, 'i'},
			{"record",          no_argument,        nullptr, 'r'},
			{"frame_rate",      required_argument,  nullptr, 'f'},
			{"frame_width",     required_argument,  nullptr, 'w'},
			{"frame_height",    required_argument,  nullptr, 'g'},
			{"model",           required_argument,  nullptr, 'a'},
			{"no_log",          no_argument,        nullptr, 'l'},
			{"help",            no_argument,        nullptr, 'h'},
			{nullptr,           no_argument,        nullptr, 0}
	};

    pthread_t video_writer_thread;
    pthread_t sign_thread;
    int video_writer_thread_id = pthread_create(&video_writer_thread, NULL, sign_updater, NULL);
    int sign_thread_id = pthread_create(&sign_thread, NULL, video_writer, NULL);


	int opt;
	while ( ( opt = getopt_long(argc, argv, short_options, long_options, nullptr)) != -1)
	{
		switch (opt)
		{
			case 'm':
				mode = stoi(optarg);
				if (mode < 0 || mode > 2)
				{
					cerr << "There are only 3 modes available, look at the help option: ./main -h" << endl;
					return -1;
				}
				cout << "MODE is set to: " << mode << endl;
				break;

			case 'p':
				udp_socket_port = stoi(optarg);
				cout << "PORT is set to " << udp_socket_port << endl;
				break;

			case 's':
				udp_socket_host = optarg;
				cout << "HOST is set to: " << udp_socket_host << endl;
				break;

			case 'd':
				debugging = true;
				std::cout << "DEBUGGING enabled: " << endl;
				break;

			case 'i':
				img_show = true;
				std::cout << "IMG_SHOW enabled: " << endl;
				break;

			case 'r':
				recording = true;
				std::cout << "VIDEO RECORDING enabled: " << endl;
				break;

			case 'l':
				logging = false;
				std::cout << "JOYSTICK LOGGING disabled: " << endl;
				break;

			case 'f':
				video_frame_rate = stoi(optarg);
				cout << "VIDEO FRAME RATE is set to: " << video_frame_rate << endl;
				break;

			case 'w':
				video_frame_width = stoi(optarg);
				cout << "VIDEO FRAME WIDTH is set to: " << video_frame_width << endl;
				break;

			case 'g':
				video_frame_height = stoi(optarg);
				cout << "VIDEO FRAME HEIGHT is set to: " << video_frame_height << endl;
				break;

			case 'a':
				model_path = optarg;
				cout << "MODEL Path is set to: " << model_path << endl;
				break;

			case 'h': // -h or --help
			case '?': // Unrecognized option
			default:
				usage();
				exit(-1);
		}
	}

	//  setup signal handler for SIGINT
	signal(SIGINT, signal_handler);

	if (wiringPiSetup() == -1)
	{
		cerr << "wiringPi is not set up properly" << endl;
		return -1;
	}

	switch (mode)
	{
		case ALGORITHM_MODE:
			mode_algorithm();
			break;
		case AI_MODE:
			mode_ai();
			break;
		case JOYSTICK_MODE:
			mode_joystick();
			break;
	}

	return 0;
}

