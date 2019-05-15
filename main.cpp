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
#include "controller.h"
#include <thread>
#include <stdlib.h>
#include <fstream>
#include <csignal>
#include "datagram_socket_server.h"
#include <uuid/uuid.h>
#include <getopt.h>
#include <string>

using namespace std;
using namespace raspicam;
using namespace cv;

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
    return uuid;
}
void video_writer()
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

	std::thread v(video_writer);


	controller.init_dc_motor();
	cout << "ready to receive" << endl;
	while (1)
	{
		s->receive(message, 1024);
		speed = (int)message[0] == 0 ? ((int)message[1]) * -1 : (int) message[1];
		steering = (int)message[2] == 0 ? ((int)message[3]) : (int) message[3] * -1;
		started = (int) message[4] == 1;
		controller.turn(steering, speed);
		if (debugging)
            cout <<"::::" << message << "::::" << "speed: " << speed << " steering: " << steering <<  endl;
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

