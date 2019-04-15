#ifndef LINE_UTILS
#define LINE_UTILS
#include <vector>
#include <opencv2/opencv.hpp>
#include "poly_fit.h"

using namespace cv;
using namespace std;

class Line
{
public:
	bool detected;

	vector<double> last_fit_pixel;
	vector<double> last_fit_meter;
	double radius_of_curvature;

	vector<float> all_x;
	vector<float> all_y;

	vector<vector<double>> recent_fits_pixel;
	vector<vector<double>> recent_fits_meter;

	Line(int buff_len = 10);

	/**
	Update Line with new fitted coefficients.
		:param new_fit_pixel: new polynomial coefficients (pixel)
		:param new_fit_meter: new polynomial coefficients (meter)
		:param detected: if the Line was detected or inferred
		:param clear_buffer: if True, reset state
		:return: None*/
	void update_line(vector<double> new_fit_pixel, vector<double> new_fit_meter, bool detected, bool clear_buffer = false);

	vector<double> average_fit();

	double curvature();

	double curvature_meter();

};

/**
	Get polynomial coefficients for lane-lines detected in an binary image.

	:param birdeye_binary: input bird's eye view binary image
	:param line_lt: left lane-line previously detected
	:param line_rt: left lane-line previously detected
	:param n_windows: number of sliding windows used to search for the lines
	:param verbose: if True, display intermediate output
	:return: updated lane lines and output image
*/
array<Line, 2> get_fits_by_sliding_windows(Mat birdeye_binary, int n_windows = 9, bool verbose = false);

/*
	Compute offset from center of the inferred lane.
	The offset from the lane center can be computed under the hypothesis that the camera is fixed
	and mounted in the midpoint of the car roof. In this case, we can approximate the car's deviation
	from the lane center as the distance between the center of the image and the midpoint at the bottom
	of the image of the two lane-lines detected.

	:param line_lt: detected left lane-line
	:param line_rt: detected right lane-line
	:param frame_width: width of the undistorted frame
	:return: inferred offset
*/
double compute_offset_from_center(Line line_lt, Line line_rt, int frame_width);

void get_fits_by_previous_fits(Mat birdeye_binary, Line line_lt, Line line_rt, bool verbose = false);

#endif