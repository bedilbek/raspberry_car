//
// Created by bedilbek on 4/14/19.
//

#include "bird_eye.h"

using namespace cv;

Mat bird_eye(cv::Mat img, cv::Mat forw_mat, cv::Mat backw_mat, bool verbose)
{
    int width, height;

    width = img.cols;
    height = img.rows;

    Point2f src[4];
    src[0] = Point2f(width, height-150);
    src[1] = Point2f(0, height-150);
    src[2] = Point2f(380, 350);
    src[3] = Point2f(800, 350);

    Point2f dest[4];
    dest[0] = Point2f(width, height);
    dest[1] = Point2f(0, height);
    dest[2] = Point2f(0, 0);
    dest[3] = Point2f(width, 0);

    forw_mat = getPerspectiveTransform(src, dest);
    backw_mat = getPerspectiveTransform(dest, src);

    Mat warped_img;

    warpPerspective(img, warped_img, forw_mat, Size(width, height), INTER_LINEAR);

    if (verbose)
    {
//        f, axarray = plt.subplots(1, 2)
//        f.set_facecolor('white')
//        axarray[0].set_title('Before perspective transform')
//        axarray[0].imshow(img, cmap='gray')
//        for point in src:
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
