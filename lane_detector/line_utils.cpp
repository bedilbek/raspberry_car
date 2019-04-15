
#include "line_utils.h"
//push test
Line::Line(int buff_len)
{
	this->detected = false;
}

void Line::update_line(vector<double> new_fit_pixel, vector<double> new_fit_meter, bool detected, bool clear_buffer)
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

vector<double> Line::average_fit()
{
	vector<double> sum;
	Mat recent_fits = Mat(this->recent_fits_pixel.size(), this->recent_fits_pixel[0].size(), CV_64FC1);

	for (int i = 0; i < this->recent_fits_pixel.size(); i++)
		for (int j = 0; j < this->recent_fits_pixel[0].size(); j++)
			recent_fits.at<double>(i, j) = this->recent_fits_pixel[i][j];

	reduce(recent_fits, sum, 0, CV_REDUCE_AVG, CV_64FC1);
	return sum;
}

double Line::curvature()
{
	float y_eval = 0;
	vector<double> coeffs = this->average_fit();
	double result = pow((1 + pow((2 * coeffs[0] * y_eval + coeffs[2]), 2)), 1.5) / abs(2 * coeffs[0]);
	return result;
}

double Line::curvature_meter()
{
	float y_eval = 0;
	vector<double> coeffs;

	Mat recent_fits = Mat(this->recent_fits_meter.size(), this->recent_fits_meter[0].size(), CV_64FC1);

	for (int i = 0; i < this->recent_fits_meter.size(); i++)
		for (int j = 0; j < this->recent_fits_meter[0].size(); j++)
			recent_fits.at<double>(i, j) = this->recent_fits_meter[i][j];

	reduce(recent_fits, coeffs, 0, CV_REDUCE_AVG);
	double result = pow((1 + pow((2 * coeffs[0] * y_eval + coeffs[2]), 2)), 1.5) / abs(2 * coeffs[0]);
	return result;
}


double compute_offset_from_center(Line line_lt, Line line_rt, int frame_width)
{
	float ym_per_pix = 30 / 720;
	float xm_per_pix = 3.7 / 700;

	int time_window = 10;
	if (line_lt.detected && line_rt.detected)
	{
		double line_lt_bottom = 0, line_rt_bottom = 0;
		int count = 0;
		auto max_y = max_element(begin(line_lt.all_y), end(line_lt.all_y));
		for (int i = 0; i < line_lt.all_y.size(); i++)
		{
			if (line_lt.all_y[i] > max_y[0] * 0.95)
			{
				line_lt_bottom += line_lt.all_x[i];
				count++;
			}
		}
		line_lt_bottom /= count;
		count = 0;

		for (int i = 0; i < line_rt.all_y.size(); i++)
		{
			if (line_rt.all_y[i] > max_y[0] * 0.95)
			{
				line_rt_bottom += line_rt.all_x[i];
				count++;
			}
		}
		line_rt_bottom /= count;

		return xm_per_pix * abs((line_lt_bottom + (line_rt_bottom - line_lt_bottom) / 2) - frame_width / 2);
	}
	else
	{
		return -1;
	}
	return 0;
}

array<Line, 2> get_fits_by_sliding_windows(Mat birdeye_binary, int n_windows, bool verbose)
{
    float ym_per_pix = 30 / 720;
    float xm_per_pix = 3.7 / 700;
    Line line_lt, line_rt;
	int height = birdeye_binary.rows;
	int width = birdeye_binary.cols;
	Mat bottom_half = birdeye_binary(Range(0, height / 2), Range::all());
	vector<int> histogram;
	reduce(bottom_half, histogram, 0, CV_REDUCE_SUM, CV_32SC1);

	int midpoint = histogram.size() / 2;
	double lmin_val, lmax_val;

	int window_height = height / n_windows;

	// Identify all nonzero pixels in the image
	vector<Point> nonzero;
	findNonZero(birdeye_binary, nonzero);

	// Current positions to be updated for each window
	int leftx_current = distance(histogram.begin(), max_element(histogram.begin(), histogram.begin() + midpoint));
	int rightx_current = distance(histogram.begin(), max_element(histogram.begin() + midpoint, histogram.end()));

	int margin = 100;
	int minpix = 50;

	vector<int> left_lane_inds;
	vector<int> right_lane_inds;

	int win_y_low;
	int win_y_high;
	int win_xleft_low;
	int win_xleft_high;
	int win_xright_low;
	int win_xright_high;
	int sum = 0;
	vector<int> good_left_inds;
	vector<int> good_right_inds;
	int count = 0;

	// Step through the windows one by one
	for (int window = 0; window < n_windows; window++)
	{
		win_y_low = height - (window + 1) * window_height;
		win_y_high = height - window * window_height;
		win_xleft_low = leftx_current - margin;
		win_xleft_high = leftx_current + margin;
		win_xright_low = rightx_current - margin;
		win_xright_high = rightx_current + margin;

        // Identify the nonzero pixels in x and y within the window
        for (int i = 0; i < nonzero.size(); i++)
		{
			if ((nonzero[i].y >= win_y_low) && (nonzero[i].y < win_y_high) && (nonzero[i].x >= win_xleft_low)
				&& (nonzero[i].x < win_xleft_high))
			{
				good_left_inds.push_back(i);
				left_lane_inds.push_back(i);
			}

			if ((nonzero[i].y >= win_y_low) && (nonzero[i].y < win_y_high) && (nonzero[i].x >= win_xright_low)
				&& (nonzero[i].x < win_xright_high))
			{
				good_right_inds.push_back(i);
				right_lane_inds.push_back(i);
			}
		}

		sum = 0;
		if (good_left_inds.size() > minpix)
		{
			for (int i = 0; i < good_left_inds.size(); i++)
				sum += nonzero[good_left_inds[i]].x;
			leftx_current = sum / good_left_inds.size();
		}

		sum = 0;
		if (good_right_inds.size() > minpix)
		{
			for (int i = 0; i < good_right_inds.size(); i++)
				sum += nonzero[good_right_inds[i]].x;
			rightx_current = sum / good_right_inds.size();
		}

		good_left_inds.clear();
		good_right_inds.clear();
	}

	for (int i = 0; i < left_lane_inds.size(); i++)
	{
		line_lt.all_x.push_back(nonzero[left_lane_inds[i]].x);
		line_lt.all_y.push_back(nonzero[left_lane_inds[i]].y);
	}

	for (int i = 0; i < right_lane_inds.size(); i++)
	{
		line_rt.all_x.push_back(nonzero[right_lane_inds[i]].x);
		line_rt.all_y.push_back(nonzero[right_lane_inds[i]].y);
	}

	bool detected = true;
	vector<double> left_fit_pixel, right_fit_pixel;
	vector<double> left_fit_meter, right_fit_meter;
	if (line_lt.all_x.empty() || line_lt.all_y.empty())
	{
		left_fit_pixel = line_lt.last_fit_pixel;
		left_fit_meter = line_lt.last_fit_meter;
		detected = false;
	}
	else
	{
		left_fit_pixel = poly_fit(line_lt.all_x, line_lt.all_y, 2);
		vector<float> all_x_pix, all_y_pix;
		for (int i = 0; i < line_lt.all_x.size(); i++)
		{
			all_x_pix.push_back(0);
			all_y_pix.push_back(0);
		}
		transform(line_lt.all_x.begin(), line_lt.all_x.end(), all_x_pix.begin(), [xm_per_pix](float x) { return x * xm_per_pix; });
		transform(line_lt.all_y.begin(), line_lt.all_y.end(), all_y_pix.begin(), [ym_per_pix](float y) { return y * ym_per_pix; });
		left_fit_meter = poly_fit(all_x_pix, all_y_pix, 2);
	}

	if (line_rt.all_x.empty() || line_rt.all_y.empty())
	{
		right_fit_pixel = line_rt.last_fit_pixel;
		right_fit_meter = line_rt.last_fit_meter;
		detected = false;
	}
	else
	{
		right_fit_pixel = poly_fit(line_rt.all_x, line_rt.all_y, 2);
		vector<float> all_x_pix, all_y_pix;
		for (int i = 0; i < line_rt.all_x.size(); i++)
		{
			all_x_pix.push_back(0);
			all_y_pix.push_back(0);
		}
		transform(line_rt.all_x.begin(), line_rt.all_x.end(), all_x_pix.begin(), [xm_per_pix](float x) { return x * xm_per_pix; });
		transform(line_rt.all_y.begin(), line_rt.all_y.end(), all_y_pix.begin(), [ym_per_pix](float y) { return y * ym_per_pix; });
		right_fit_meter = poly_fit(all_x_pix, all_y_pix, 2);
	}

	line_lt.update_line(left_fit_pixel, left_fit_meter, detected);
	line_rt.update_line(right_fit_pixel, right_fit_meter, detected);
	return { line_lt, line_rt };
}


void get_fits_by_previous_fits(Mat birdeye_binary, Line line_lt, Line line_rt, bool verbose)
{
    float ym_per_pix = 30 / 720;
    float xm_per_pix = 3.7 / 700;

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

    //	 line_lt
	if (line_lt.all_y.empty() || line_lt.all_x.empty())
	{
		left_fit_pixel = line_lt.last_fit_pixel;
		left_fit_meter = line_lt.last_fit_meter;
		detected = false;
	} else
	{

		left_fit_pixel = poly_fit(line_lt.all_y, line_lt.all_x, 2);
        vector<float> all_x_pix, all_y_pix;
        for (int i = 0; i < line_lt.all_x.size(); i++)
        {
            all_x_pix.push_back(0);
            all_y_pix.push_back(0);
        }
        transform(line_lt.all_x.begin(), line_lt.all_x.end(), all_x_pix.begin(), [xm_per_pix](float x) { return x * xm_per_pix; });
        transform(line_lt.all_y.begin(), line_lt.all_y.end(), all_y_pix.begin(), [ym_per_pix](float y) { return y * ym_per_pix; });
        left_fit_meter = poly_fit(all_x_pix, all_y_pix, 2);
	}

	//	 line_rt
	if (line_rt.all_y.empty() || line_rt.all_x.empty())
	{
		right_fit_pixel = line_rt.last_fit_pixel;
		right_fit_meter = line_rt.last_fit_meter;
		detected = false;
	} else
	{
		right_fit_pixel = poly_fit(line_rt.all_y, line_rt.all_x, 2);
        vector<float> all_x_pix, all_y_pix;
        for (int i = 0; i < line_rt.all_x.size(); i++)
        {
            all_x_pix.push_back(0);
            all_y_pix.push_back(0);
        }
        transform(line_rt.all_x.begin(), line_rt.all_x.end(), all_x_pix.begin(), [xm_per_pix](float x) { return x * xm_per_pix; });
        transform(line_rt.all_y.begin(), line_rt.all_y.end(), all_y_pix.begin(), [ym_per_pix](float y) { return y * ym_per_pix; });
        right_fit_meter = poly_fit(all_x_pix, all_y_pix, 2);
	}

    line_lt.update_line(left_fit_pixel, left_fit_meter, detected);
	line_rt.update_line(right_fit_pixel, right_fit_meter, detected);

    // Plot x and v values
    // Create an image to draw on and an image to show the selection window
    // Color in left and right line pixels
    // Generate a polygon to illustrate the search window area
    // And recast the x and y points into usable format for cv2.fillPoly()
    // Draw the lane onto the warped blank image

    // if (verbose) { }
}


