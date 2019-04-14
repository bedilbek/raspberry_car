#include <deque>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

class Line
{
public:
	bool detected;

	Mat last_fit_pixel;
	Mat last_fit_meter;
	float radius_of_curvature;

	int all_x;
	int all_y;

	vector<Mat> recent_fits_pixel;
	vector<Mat> recent_fits_meter;

	Line(int buff_len = 10)
	{
		this->detected = false;
		this->last_fit_meter = NULL;
		this->last_fit_pixel = NULL;
		this->all_x = NULL;
		this->all_y = NULL;
	}


	/**
	Update Line with new fitted coefficients.
        :param new_fit_pixel: new polynomial coefficients (pixel)
        :param new_fit_meter: new polynomial coefficients (meter)
        :param detected: if the Line was detected or inferred
        :param clear_buffer: if True, reset state
        :return: None*/
	void update_line(Mat new_fit_pixel, Mat new_fit_meter, bool detected, bool clear_buffer = false)
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

	Mat average_fit()
	{
		Mat sum;
		reduce(this->recent_fits_pixel, sum, 0, CV_REDUCE_AVG);
		return sum;
	}


	float curvature()
	{
		float y_eval = 0;
		Mat coeffs = this->average_fit();
		double result = pow((1 + pow((2 * coeffs.at<double>(0) * y_eval + coeffs.at<double>(2)), 2)), 1.5) / abs(2 * coeffs.at<double>(0));
		return result;
	}

	float curvature_meter()
	{
		float y_eval = 0;
		Mat coeffs;
		reduce(this->recent_fits_meter, coeffs, 0, CV_REDUCE_AVG);
		double result = pow((1 + pow((2 * coeffs.at<double>(0) * y_eval + coeffs.at<double>(2)), 2)), 1.5) / abs(2 * coeffs.at<double>(0));
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
void get_fits_by_sliding_windows(Mat birdeye_binary, Line line_lt, Line line_rt, int n_windows = 9, bool verbose = false)
{
	int height = birdeye_binary.cols;
	int width = birdeye_binary.rows;

	Mat histogram;
	reduce(birdeye_binary, histogram, 0, CV_REDUCE_SUM);
}


void get_fits_by_previous_fits(Mat birdeye_binary, Line line_lt, Line line_rt, bool verbose = false)
{
	int height = birdeye_binary.cols;
	int width = birdeye_binary.rows;


	Mat left_last = line_lt.last_fit_pixel;
	Mat right_last = line_rt.last_fit_pixel;

	vector<Point> bird_eye_nonzero;
	findNonZero(birdeye_binary, bird_eye_nonzero);

	vector<int> left_lane_indcs, right_lane_indcs;

//	for(auto it=bird_eye_nonzero.begin(); it !=bird_eye_nonzero.end(); it++)
//	{
//		left_lane_indcs.insert(it, it > (left_last.at<double>(0))
//	}
}


