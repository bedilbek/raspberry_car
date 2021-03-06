//
// Created by bedilbek on 4/14/19.
//

#include "bird_eye.h"

using namespace cv;

Mat bird_eye(cv::Mat img, cv::Mat forw_mat, cv::Mat backw_mat, int scale, int out, bool verbose)
{
    int width, height;

	float w_out = out / scale;
	float bhratio = 1;
	float thratio = 0.470;
	float wtratio = 0.3;
	float wbratio = 1 - wtratio;
	float ratio = 1.6;

    width = img.cols;
    height = img.rows;
	width += w_out;
    Point2f src[4];

	src[0] = Point2f((width - w_out) * wtratio, height * thratio);
	src[1] = Point2f((width - w_out) * wbratio, height * thratio);
	src[2] = Point2f(w_out / (-2), height * bhratio);
    src[3] = Point2f(width - w_out / 2, height * bhratio);


	
    Point2f dest[4];
	dest[0] = Point2f(0, 0);
	dest[1] = Point2f(width, 0);
	dest[2] = Point2f(0, width * ratio);
    dest[3] = Point2f(width, width * ratio);


    forw_mat = getPerspectiveTransform(src, dest);
    backw_mat = getPerspectiveTransform(dest, src);

    Mat warped_img;

    warpPerspective(img, warped_img, forw_mat, Size(width, width * ratio), INTER_LINEAR);

    if (verbose)
    {
//        f, axarray = plt.subplots(1, 2)
//        f.set_facecolor('white')
//        axarray[0].set_title('Before perspective transform')
//        axarray[0].imshow(img, cmap='gray')
//        for point in canny_src:
//            axarray[0].plot(*point, '.')
//        axarray[1].set_title('After perspective transform')
//        axarray[1].imshow(warped, cmap='gray')
//        for point in dst:
//            axarray[1].plot(*point, '.')
//        for axis in axarray:
//            axis.set_axis_off()
//        plt.show()

    }

    return warped_img;
}
