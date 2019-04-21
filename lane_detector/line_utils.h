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

	Mat draw(Mat mask, Scalar color = {255, 0, 0}, int line_width = 50, bool average = false);
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

/**
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

/**
Get polynomial coefficients for lane-lines detected in an binary image.
This function starts from previously detected lane-lines to speed-up the search of lane-lines in the current frame.

:param birdeye_binary: input bird's eye view binary image
:param line_lt: left lane-line previously detected
:param line_rt: left lane-line previously detected
:param verbose: if True, display intermediate output
:return: updated lane lines and output image
*/
void get_fits_by_previous_fits(Mat birdeye_binary, Line line_lt, Line line_rt, bool verbose = false);

/**
Draw both the drivable lane area and the detected lane-lines onto the original (undistorted) frame.
:param img_undistorted: original undistorted color frame
:param Minv: (inverse) perspective transform matrix used to re-project on original frame
:param line_lt: left lane-line previously detected
:param line_rt: right lane-line previously detected
:param keep_state: if True, line state is maintained
:return: color blend
*/
Mat draw_back_onto_the_road(Mat img_undistorted, Mat Minv, Line line_lt, Line line_rt, bool keep_state);

/**
Return evenly spaced numbers over a specified interval.

Returns `num` evenly spaced samples, calculated over the
        interval [`start`, `stop`].

The endpoint of the interval can optionally be excluded.

Parameters
----------
start : double
        The starting value of the sequence.
stop : double
        The end value of the sequence, unless `endpoint` is set to False.
In that case, the sequence consists of all but the last of ``num + 1``
evenly spaced samples, so that `stop` is excluded.  Note that the step
size changes when `endpoint` is False.
num : int, optional
        Number of samples to generate. Default is 50. Must be non-negative.
endpoint : bool, optional
        If True, `stop` is the last sample. Otherwise, it is not included.
Default is True.

Returns
-------
samples : vector<double>
        There are `num` equally spaced samples in the closed interval
``[start, stop]`` or the half-open interval ``[start, stop)``
(depending on whether `endpoint` is True or False).
*/
vector<double> linspace(double start, double stop, int num, bool endpoint = true);


#endif