#include "core/tt_test.h"
#include "common/log.h"
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <Eigen/Dense>
#include <cassert>
#include <cmath>

class PinUpPicture{
public:
    PinUpPicture(const float& res, const uint width, const uint height)
    : m_resolution(res), m_size(width), m_data(Eigen::MatrixXf::Zero(width, height)) { assert(width == height); m_trj.push_back(0); }

    PinUpPicture(const float& res, const cv::Mat& gray) : PinUpPicture(res, gray.rows, gray.cols)
    {
        assert(gray.rows == gray.cols);
        for (int i = 0; i < gray.rows; i++) {
            for (int j = 0; j < gray.cols; j++){
                float prob = (255 - gray.at<uchar>(i, j)) / 255.0;
                m_data(i, j) = (prob < 0.5) ? 0 : std::log(prob / (1 - prob));
            }
        }
    }

    std::vector<uint> get_trj() const {
        return m_trj;
    }

    bool find_best_id(const PinUpPicture& other)
    {
        Eigen::MatrixXf diff_map = sub(other);
        Eigen::Vector2i xy = id2xy(m_trj.back());
        uint best_id = m_trj.back();
        uint best_cnt = 0;
        for(uint i = 0; i < m_resolution; ++i){
            uint cnt = brasenham(diff_map, xy(0), xy(1), id2xy(i)(0), id2xy(i)(1));
            if(cnt > best_cnt){
                best_cnt = cnt;
                best_id = i;
            }
        }

        std::cout << "best_id: " << best_id << std::endl;
        std::cout << "best_cnt: " << best_cnt << std::endl;

        if(best_id == m_trj.back() || best_cnt < 20){
            std::cout << "best_id == m_trj.back()" << std::endl;
            return false;
        }

        m_trj.push_back(best_id);
        brasenham(diff_map, xy(0), xy(1), id2xy(best_id)(0), id2xy(best_id)(1), 0.1);

        cv::Mat img(m_size, m_size, CV_8UC1);
        for (uint i = 0; i < m_size; i++) {
            for (uint j = 0; j < m_size; j++){
                float prob = 1 / (1 + std::exp(-m_data(i, j)));
                img.at<uchar>(i, j) = 255 * (1-prob);
            }
        }
        cv::imshow("img", img);
        cv::waitKey(1);

        return true;
    }
private:
    Eigen::MatrixXf sub(const PinUpPicture& other) 
    {
        return other.m_data - m_data;
    }

    Eigen::Vector2i id2xy(const uint id)
    {
        float theta = id * get_resolution();
        return Eigen::Vector2i(half_size() * (1+std::cos(theta)), half_size() * (1+std::sin(theta)));
    }

    constexpr float half_size() const {
        return m_size / 2;
    }

    constexpr float get_resolution() const {
        return 2 * M_PI / m_resolution;
    }

    uint brasenham(const Eigen::MatrixXf& diff_map, int x0, int y0, int x1, int y1, float log_odds = 0)
    {
        uint cnt = 0;
        int dx = std::abs( x1 - x0 );
        int dy = std::abs( y1 - y0 );
        bool inter_change = false;
        int e = -dx;// error
        int signX = x1 > x0 ? 1 : ( ( x1 < x0 ) ? -1 : 0 );
        int signY = y1 > y0 ? 1 : ( ( y1 < y0 ) ? -1 : 0 );
        if (dy > dx) {
            int temp = dx; dx = dy; dy = temp; inter_change = true;
        }

        int x = x0, y = y0;
        for (int i = 1; i <= dx; i++) { 
            add(x, y, log_odds);

            if(diff_map(x, y) > 0.2) cnt++;

            if (!inter_change) {x += signX;}
            else {y += signY;}
            e += 2 * dy;
            if (e >= 0) {
                if (!inter_change) {y += signY;}
                else {x += signX;}
                e -= 2 * dx;
            }
        }

        return cnt;
    }

    void add(int x, int y, float log_odds)
    {
        m_data(x, y) += log_odds;
    }
private:
    float m_resolution;
    uint m_size;
    Eigen::MatrixXf m_data;
    std::vector<uint> m_trj;
};


JUST_RUN_TEST(pin_up_picture, test)
TEST(pin_up_picture, test)
{
    cv::Mat img = cv::imread("../src/data/img.jpg");
    cv::Mat gray;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    resize(gray, gray, cv::Size(500, 500));
    LOG_TEST("gray: ", gray.size());

    PinUpPicture pup(100, gray);
    PinUpPicture blank(100, 500, 500);

    while(true){
        bool find = blank.find_best_id(pup);
        if(!find) break;
    }

    cv::waitKey(0);

    std::cout << "trj: " << blank.get_trj().size() << std::endl;
}