#ifndef __ROOM_SEGMENT_H__
#define __ROOM_SEGMENT_H__

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ximgproc/fast_line_detector.hpp>
#include <iostream>
#include <vector>


class RoomSegment
{
public:
    struct Option
    {
        float m_thr;
        float b_thr;
        float min_dist_thr;
        float first_line_thr;
        int bin_thr;
        std::string img_path;
        std::string save_path;
    };

    RoomSegment(const Option& opt) : opt_(opt)
    {
        original_img_ = cv::imread(opt.img_path, 0);
        convert_to_bw(original_img_, opt.bin_thr);
    }

    void run()
    {
        cv::Mat img = convert_to_sqr(original_img_);

        std::vector<cv::Vec2f> mbs;
        std::vector<cv::Vec4f> detected_lines = line_segmentation(img, mbs);
        std::vector<cv::Vec4f> extended_lines = extend_lines(detected_lines, mbs);
        cv::Mat extended_lined_img = img;
        cv::ximgproc::createFastLineDetector(1)->drawSegments(extended_lined_img, extended_lines);

        cv::Mat lined_img;
        cv::cvtColor(extended_lined_img, lined_img, cv::COLOR_BGR2GRAY);

        cv::Mat rotated_img = rotate_img(lined_img, 90);
        detected_lines = line_segmentation(rotated_img, mbs);
        extended_lines = extend_lines(detected_lines, mbs);
        extended_lined_img = rotated_img;
        cv::ximgproc::createFastLineDetector(1)->drawSegments(extended_lined_img, extended_lines);
        cv::cvtColor(extended_lined_img, lined_img, cv::COLOR_BGR2GRAY);

        convert_to_bw(lined_img, 200);
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(2, 2));
        cv::erode(lined_img, lined_img, kernel, cv::Point(-1, -1), 1);
        cv::morphologyEx(lined_img, lined_img, cv::MORPH_OPEN, kernel, cv::Point(-1, -1), 5);

        rotated_img = rotate_img(lined_img, 270);
        cv::Mat components_img = connected_component(rotated_img);

        // 可视化
        cv::imshow("show", components_img);
        cv::waitKey(0);
    }
private:
    void convert_to_bw(cv::Mat& img, const float& thr)
    {
        for(int i = 0; i < img.rows; i++)
        for(int j = 0; j < img.cols; j++)
            img.at<uchar>(i, j) = img.at<uchar>(i, j) > thr ? 255 : 0;
    }

    cv::Mat convert_to_sqr(const cv::Mat& img) 
    {  
        int height = img.rows;  
        int width = img.cols;  
    
        int max_dim = std::max(height, width);  
        int top = (max_dim - height) / 2;  
        int bottom = max_dim - height - top;  
        int left = (max_dim - width) / 2;  
        int right = max_dim - width - left;   
    
        cv::Mat squared_image;  
        cv::copyMakeBorder(img, squared_image, top, bottom, left, right, cv::BORDER_CONSTANT, cv::Scalar(0));  
        return squared_image;
    }  

    cv::Mat circle_detection(cv::Mat& image) 
    {  
        std::vector<cv::Vec3f> circles;  
        cv::HoughCircles(image, circles, cv::HOUGH_GRADIENT, 1, image.rows/8, 50, 30, 50, 0);  
        
        for (size_t i = 0; i < circles.size(); i++) {  
            cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));  
            cv::circle(image, center, cvRound(circles[i][2]), cv::Scalar(0, 0, 0), 2);  
        }  
        return image;  
    }  

    std::vector<cv::Vec4f> line_segmentation(cv::Mat img, std::vector<cv::Vec2f>& mbs) 
    {
        mbs.clear();
        cv::Ptr<cv::ximgproc::FastLineDetector> fld = cv::ximgproc::createFastLineDetector(1);
        std::vector<cv::Vec4f> lines;
        fld->detect(img, lines);

        std::vector<cv::Vec4f> detected_lines;
        for (size_t i = 0; i < lines.size(); ++i) {
            cv::Vec4f line = lines[i];
            float x1 = line[0];
            float y1 = line[1];
            float x2 = line[2];
            float y2 = line[3];
            if ((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) < opt_.first_line_thr) continue;
            if (x2 - x1 == 0) continue;
            float m = (y2 - y1) / (x2 - x1);
            float b = y2 - m * x2;
            detected_lines.push_back({x1, y1, x2, y2});
            mbs.push_back({m, b});
        }

        return detected_lines;
    }

    std::vector<cv::Vec4f> extend_lines(const std::vector<cv::Vec4f>& lines, const std::vector<cv::Vec2f>& mbs) 
    {  
        std::vector<cv::Vec4f> temp_lines;  
        std::vector<cv::Vec4f> final_lines;  
  
        for (size_t i = 0; i < lines.size(); ++i) {  
            for (size_t j = i; j < lines.size(); ++j) {  
                if (i == j) continue; 
  
                cv::Vec4f l1 = lines[i];
                cv::Vec4f l2 = lines[j];  
                cv::Vec2f mb1 = mbs[i];
                cv::Vec2f mb2 = mbs[j];
                if (std::abs(mb1[0] - mb2[0]) < opt_.m_thr && std::abs(mb1[1] - mb2[1]) < opt_.b_thr) {  
                    temp_lines.push_back({l1[0], l1[1], l2[0], l2[1]});  
                    temp_lines.push_back({l1[0], l1[1], l2[2], l2[3]});  
                    temp_lines.push_back({l1[2], l1[3], l2[0], l2[1]});  
                    temp_lines.push_back({l1[2], l1[3], l2[2], l2[3]});  

                    static auto dist2 = [](const cv::Vec4f& a) {    
                        return (a[0]-a[2])*(a[0]-a[2]) + (a[1]-a[3])*(a[1]-a[3]);   
                    };
  
                    std::sort(temp_lines.begin(), temp_lines.end(), [](const cv::Vec4f& a, const cv::Vec4f& b) {   
                        return dist2(a) < dist2(b);
                    });  
  
                    cv::Vec4f line = temp_lines[0];
                    temp_lines.clear();


                    if (dist2(line) < opt_.min_dist_thr) {  
                        final_lines.push_back(line);  
                    }  
                }  
            }  
        }  
  
        return final_lines;  
    }  

    cv::Mat transfer_to_original(cv::Mat& final_img) 
    {  
        cv::Mat sqr_org_img = convert_to_sqr(original_img_);
  
        if (final_img.type() != sqr_org_img.type()) {  
            cv::Mat final_img_converted;  
            final_img.convertTo(final_img_converted, sqr_org_img.type());  
            final_img = final_img_converted;  
        }  
        cv::Mat output_img = cv::Mat::zeros(sqr_org_img.size(), sqr_org_img.type());  
          for (int y = 0; y < sqr_org_img.rows; ++y) {  
            for (int x = 0; x < sqr_org_img.cols; ++x) {  
                if (cv::sum(final_img.row(y).colRange(x, x+1)) != cv::Scalar(0, 0, 0)) {  
                    output_img.at<cv::Vec3b>(y, x) = final_img.at<cv::Vec3b>(y, x);  
                } else {  
                    uchar val = sqr_org_img.at<uchar>(y, x); // 假设sqr_org_img是灰度图像  
                    output_img.at<cv::Vec3b>(y, x) = cv::Vec3b(val, val, val);  
                }  
            }  
        }  
  
        return output_img;  
    }  

    cv::Mat connected_component(const cv::Mat& img) 
    {      
        cv::Mat labels(img.size(), CV_32S);  
        int numLabels = cv::connectedComponents(img, labels, 8, CV_32S);
    
        cv::Mat labeled_img(img.size(), CV_8UC3, cv::Scalar(0, 0, 0));
    
        std::vector<cv::Vec3b> colors(numLabels);  
        for (int label = 1; label < numLabels; ++label) {  
            colors[label] = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256);  
        }  
    
        for (int y = 0; y < img.rows; ++y) {  
            for (int x = 0; x < img.cols; ++x) {  
                int label = labels.at<int>(y, x);  
                if (label > 0) labeled_img.at<cv::Vec3b>(y, x) = colors[label];  
            }  
        }  
    
        return labeled_img;  
    }  

    cv::Mat rotate_img(const cv::Mat& img, double theta) 
    {  
        int rows = img.rows;  
        int cols = img.cols;  
    
        cv::Point2f center(cols / 2.0f, rows / 2.0f);  
        cv::Mat rotation_matrix = cv::getRotationMatrix2D(center, theta, 1.0);  

        cv::Mat dst;  
        cv::warpAffine(img, dst, rotation_matrix, cv::Size(cols, rows));  
    
        return dst;  
    } 
private:
    Option opt_;
    cv::Mat original_img_;
};


#endif // __ROOM_SEGMENT_H__