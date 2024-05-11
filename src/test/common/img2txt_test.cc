#include "core/tt_test.h"
#include "common/log.h"
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>


// JUST_RUN_TEST(img2txt, test)
TEST(img2txt, test)
{
    cv::Mat img = cv::imread("../src/data/download.jpg");
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    resize(gray, gray, cv::Size(40, 20));
    std::cout << "/*****************************************************" << std::endl;
    for (int i = 0; i < gray.rows; i++)
    {
        for (int j = 0; j < gray.cols; j++){
            uchar val = gray.at<uchar>(i, j);
            if(val < 5) std::cout << " ";
            else if(val < 100) std::cout << ".";
            else if(val < 150) std::cout << "*";
            else if(val < 200) std::cout << "#";
            else std::cout << "█";
            // else std::cout << "B";
        }
        std::cout << std::endl;
    }
    std::cout << "*******************************************************/" << std::endl;
}