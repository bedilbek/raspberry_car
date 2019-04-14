
#include <opencv2/opencv.hpp>


//Threshold a color frame in HSV space
cv::Mat thresh_frame_in_HSV(cv::Mat3b frame, cv::Scalar min_values, cv::Scalar max_values, bool verbous);

//Apply Sobel edge detection to an input frame, then threshold the result
cv::Mat thresh_frame_sobel(cv::Mat3b frame, int kernel_size);

//Apply histogram equalization to an input frame, threshold it and return the (binary) result.
cv::Mat get_binary_from_equalized_grayscale(cv::Mat3b frame);

//Convert an input frame to a binary image which highlight as most as possible the lane-lines.
//:param img : input color frame
cv::Mat binarize(cv::Mat img, bool verbous);