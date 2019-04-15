#include <deque>
#include <vector>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

class Line
{
public:
	bool detected;

	vector<double> last_fit_pixel;
	vector<double> last_fit_meter;
	double radius_of_curvature;

	vector<int> all_x;
	vector<int> all_y;

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


	vector<double> left_last = line_lt.last_fit_pixel;
	vector<double> right_last = line_rt.last_fit_pixel;

	vector<Point> bird_eye_nonzero;
	findNonZero(birdeye_binary, bird_eye_nonzero);

	int margin = 100;
	vector<bool> left_lane_indexes, right_lane_indexes;

	for(int i=0; i< bird_eye_nonzero.size(); i++)
	{
		left_lane_indexes.push_back(
				(bird_eye_nonzero[i].x > (left_last[0] * pow(bird_eye_nonzero[i].y, 2) + left_last[1] * bird_eye_nonzero[i].y + left_last[2] - margin))
				&
				(bird_eye_nonzero[i].x < (left_last[0] * pow(bird_eye_nonzero[i].y, 2) + left_last[1] * bird_eye_nonzero[i].y + left_last[2] + margin))
				);

		right_lane_indexes.push_back(
				(bird_eye_nonzero[i].x > (right_last[0] * pow(bird_eye_nonzero[i].y, 2) + right_last[1] * bird_eye_nonzero[i].y + right_last[2] - margin))
				&
				(bird_eye_nonzero[i].x < (right_last[0] * pow(bird_eye_nonzero[i].y, 2) + right_last[1] * bird_eye_nonzero[i].y + right_last[2] + margin))
				);
	}

	line_lt.all_y.clear();
	line_lt.all_x.clear();
	for (int j = 0; j < left_lane_indexes.size(); ++j) {
		if (left_lane_indexes[j]) {
			line_lt.all_x.push_back(bird_eye_nonzero[j].x);
			line_lt.all_y.push_back(bird_eye_nonzero[j].y);
		}
	}

	line_rt.all_y.clear();
	line_rt.all_x.clear();
	for (int j = 0; j < right_lane_indexes.size(); ++j) {
		if (right_lane_indexes[j]) {
			line_rt.all_x.push_back(bird_eye_nonzero[j].x);
			line_rt.all_y.push_back(bird_eye_nonzero[j].y);
		}
	}

	bool detected = true;
	vector<double> left_fit_pixel, left_fit_meter, right_fit_pixel, right_fit_meter;

	if (!line_lt.all_y.empty() or !line_lt.all_x.empty())
	{
		left_fit_pixel = line_lt.last_fit_pixel;
		left_fit_meter = line_lt.last_fit_meter;
		detected = false;
	} else
	{

//		left_fit_pixel = polyFit(line_lt.all_y, line_lt.all_x, 2)
//		left_fit_meter = polyFit(line_lt.all_y * ym_per_pix, line_lt.all_x * xm_per_pix, 2)
	}

	if (!line_rt.all_y.empty() or !line_rt.all_x.empty())
	{
		right_fit_pixel = line_rt.last_fit_pixel;
		right_fit_meter = line_rt.last_fit_meter;
		detected = false;
	} else
	{
//		right_fit_pixel = polyFit(line_rt.all_y, line_rt.all_x, 2)
//		right_fit_meter = polyFit(line_rt.all_y * ym_per_pix, line_rt.all_x * xm_per_pix, 2)
	}



}


