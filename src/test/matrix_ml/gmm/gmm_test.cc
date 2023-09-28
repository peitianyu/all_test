#include "core/tt_test.h"
#include "matrix_ml/gmm/gmm.h"
#include "3dr_party/plot.h"
#include <iostream>
#include <fstream>
#include <chrono> 

static Eigen::MatrixXf load_data(const std::string& data_path);
static Eigen::MatrixXf GetEllipseData(const GMM::Gaussian& model);

// JUST_RUN_TEST(gmm, test)
TEST(gmm, test)
{
    std::string data_path = "../src/data/gmm.data";
    Eigen::MatrixXf X = load_data(data_path);

    GMM::Param param = {2, 50};
    GMM gmm(param);
    gmm.train_model(X);
    Eigen::MatrixXf Y = gmm.predict(X);

    /////////////////////////plot//////////////////////////
    {
        signalsmith::plot::Plot2D plot(100, 100);
        auto &X_plot = plot.line(0);
        auto &Y_plot = plot.line(1);
        for(uint i = 0; i < X.rows(); ++i) {
            (Y(i, 0) > Y(i, 1)) ? X_plot.marker(X(i, 0), X(i, 1)) : Y_plot.marker(X(i, 0), X(i, 1));
        }

        for(uint i = 0; i < gmm.get_models().size(); ++i) {
            Eigen::MatrixXf ellipse_data = GetEllipseData(gmm.get_models()[i]);
            auto &ellipse = plot.line(i+2);
            for(uint j = 0; j < ellipse_data.cols(); ++j) {
                ellipse.add(ellipse_data(0, j), ellipse_data(1, j));
            }
        }
        
        plot.write("gmm.svg");
    }
    
    /////////////////////////逐步训练plot///////////////////////// 
    {
        signalsmith::plot::Plot2D plot(100, 100);
        auto &legend = plot.legend(2, 1); // 标签放置到位置
        auto &X_plot = plot.line(0);
        auto &Y_plot = plot.line(1);

        gmm.scale_data(X);
        gmm.init_model(X.cols(), param.n_components);
        for(uint i = 0; i < param.max_iter; ++i) {
            gmm.maximize_step(X, gmm.expectation_step(X));
            
            Eigen::MatrixXf Y = gmm.predict(X);
            
            for(uint i = 0; i < X.rows(); ++i) {
                (Y(i, 0) > Y(i, 1)) ? X_plot.marker(X(i, 0), X(i, 1)) : Y_plot.marker(X(i, 0), X(i, 1));
            }
            X_plot.toFrame(i);
            Y_plot.toFrame(i);
        }
        legend.add(X_plot, "X");
        legend.add(Y_plot, "Y");
        plot.write("gmm_step.svg");
    }
}













static Eigen::MatrixXf load_data(const std::string& data_path)
{
    std::ifstream fin(data_path);
    if(!fin.is_open()) {
        std::cerr << "open file " << data_path << " failed" << std::endl;
        exit(-1);
    }

    std::vector<float> data;
    float x, y;
    while(fin >> x >> y) {
        data.push_back(x);
        data.push_back(y);
    }
    fin.close();

    Eigen::MatrixXf X(data.size() / 2, 2);
    for(uint i = 0; i < data.size(); i += 2) {
        X(i / 2, 0) = data[i];
        X(i / 2, 1) = data[i + 1];
    }
    return X;
}

static Eigen::MatrixXf GetEllipseData(const GMM::Gaussian& model)
{
    Eigen::SelfAdjointEigenSolver<Eigen::MatrixXf> eigen_solver(model.cov);
    Eigen::VectorXf eigen_values = eigen_solver.eigenvalues();
    Eigen::MatrixXf eigen_vectors = eigen_solver.eigenvectors();
    float angle = std::atan2(eigen_vectors(1, 0), eigen_vectors(0, 0));
    float width = std::sqrt(eigen_values(0));
    float height = std::sqrt(eigen_values(1));

    std::vector<float> x, y;
    for(float i = 0; i < 2 * M_PI; i += 0.1) {
        x.push_back(width * std::cos(i));
        y.push_back(height * std::sin(i));
    }
    Eigen::MatrixXf R(2, 2);
    R << std::cos(angle), -std::sin(angle), std::sin(angle), std::cos(angle);
    Eigen::MatrixXf data(2, x.size());
    for(uint i = 0; i < x.size(); ++i) {
        data.col(i) << x[i], y[i];
    }
    data = R * data;
    for(uint i = 0; i < x.size(); ++i) {
        x[i] = data(0, i) + model.mu(0);
        y[i] = data(1, i) + model.mu(1);
    }
    Eigen::MatrixXf ellipse_data(2, x.size());
    for(uint i = 0; i < x.size(); ++i) {
        ellipse_data.col(i) << x[i], y[i];
    }
    return ellipse_data;
}
