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

	Line(int buff_len = 10)
	{
		this->detected = false;
	}


	/**
	Update Line with new fitted coefficients.
		:param new_fit_pixel: new polynomial coefficients (pixel)
		:param new_fit_meter: new polynomial coefficients (meter)
		:param detected: if the Line was detected or inferred
		:param clear_buffer: if True, reset state
		:return: None*/
	void update_line(vector<double> new_fit_pixel, vector<double> new_fit_meter, bool detected, bool clear_buffer = false)
	{

		this->detected = detected;

		if (clear_buffer)
		{
			this->recent_fits_meter.clear();
			this->recent_fits_pixel.clear();
		}

		this->last_fit_meter = new_fit_meter;
		this->last_fit_pixel = new_fit_pixel;

		this->recent_fits_meter.push_back(new_fit_meter);
		this->recent_fits_pixel.push_back(new_fit_pixel);
	}

	vector<double> average_fit()
	{
		vector<double> sum;
		reduce(this->recent_fits_pixel, sum, 0, CV_REDUCE_AVG);
		return sum;
	}


	double curvature()
	{
		float y_eval = 0;
		vector<double> coeffs = this->average_fit();
		double result = pow((1 + pow((2 * coeffs[0] * y_eval + coeffs[2]), 2)), 1.5) / abs(2 * coeffs[0]);
		return result;
	}

	double curvature_meter()
	{
		float y_eval = 0;
		vector<double> coeffs;
		reduce(this->recent_fits_meter, coeffs, 0, CV_REDUCE_AVG);
		double result = pow((1 + pow((2 * coeffs[0] * y_eval + coeffs[2]), 2)), 1.5) / abs(2 * coeffs[0]);
		return result;
	}
};

/*
	Get polynomial coefficients for lane-lines detected in an binary image.

	:param birdeye_binary: input bird's eye view binary image
	:param line_lt: left lane-line previously detected
	:param line_rt: left lane-line previously detected
	:param n_windows: number of sliding windows used to search for the lines
	:param verbose: if True, display intermediate output
	:return: updated lane lines and output image
*/
void get_fits_by_sliding_windows(Mat birdeye_binary, Line line_lt, Line line_rt, int n_windows = 9, bool verbose = false);

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
//double compute_offset_from_center(Line line_lt, Line line_rt, int frame_width);
#endif