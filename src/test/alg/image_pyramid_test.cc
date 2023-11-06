#include "core/tt_test.h"
#include "common/log.h"
#include "common/image_pyramid.h"


// JUST_RUN_TEST(image_pyramid, test)
TEST(image_pyramid, test)
{
    LOG_TEST("------------------------------image_pyramid--------------------------");

    cv::Mat img0 = cv::imread("../src/data/l0.png", cv::IMREAD_GRAYSCALE);

    cv::Mat K = (cv::Mat_<double>(3, 3) << 718.856, 0, 607.1928,
                                            0, 718.856, 185.2157,
                                            0, 0, 1);

    ImagePyramid img_pyramid_0(K, img0);
    LOG_TEST("img0: ", img_pyramid_0.img().size());
    ImagePyramid img_pyramid_1(img_pyramid_0);
    LOG_TEST("img1: ", img_pyramid_1.img().size());
    ImagePyramid img_pyramid_2(img_pyramid_1);
    LOG_TEST("img2: ", img_pyramid_2.img().size());


    cv::imshow("img0", img_pyramid_0.img());
    cv::imshow("img1", img_pyramid_1.img());
    cv::imshow("img2", img_pyramid_2.img());

    cv::waitKey(0);
}