
#ifndef POLY_FIT
#define POLY_FIT
#include <opencv2/opencv.hpp>
#include <vector>

std::vector<double> poly_fit(std::vector<float> x, std::vector<float> y, int order = 2);

#endif