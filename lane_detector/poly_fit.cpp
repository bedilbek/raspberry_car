#include "poly_fit.h"

std::vector<double> poly_fit(std::vector<float> x, std::vector<float> y, int order)
{
	std::vector<float> sumx;
	std::vector<float> sumy;

	for (int i = 0; i < order * 2 + 1; i++)
	{
		sumx.push_back(0);
		sumy.push_back(0);
	}

	for (int i = 0; i < order * 2 + 1; i++)
	{
		for (int j = 0; j < x.size(); j++)
		{
			sumx[i] += pow(x[j], i);
			sumy[i] += pow(x[j], i) * y[j];
		}
	}

	cv::Mat mat = cv::Mat(order + 1, order + 1, CV_32FC1);

	for (int i = 0; i <= order; i++)
		for (int j = 0; j <= order; j++)
			mat.at<float>(i, j) = sumx[j + i];

	cv::Mat temp = cv::Mat(order + 1, order + 1, CV_32FC1);
	std::vector<double> coeffs;
	double main_determinant = cv::determinant(mat);

	for (int k = 0; k < order + 1; k++)
	{
		for (int i = 0; i < order + 1; i++)
			for (int j = 0; j < order + 1; j++)
				if (k == i)
					temp.at<float>(j, i) = sumy[j];
				else
					temp.at<float>(j, i) = mat.at<float>(j, i);

		coeffs.push_back(cv::determinant(temp) / main_determinant);
	}

	return coeffs;
}