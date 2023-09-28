/**
 * Copyright (C), 2023
 * @file 	 gmm.h
 * @author 	 peitianyu(https://github.com/peitianyu)
 * @date 	 2023-09-13
 * @brief 	 [简介] 混合高斯模型(无监督学习)
 * @note 	 [方法] EM算法, 启发式的迭代方法(最大似然估计)
 */

#ifndef __GMM_H__
#define __GMM_H__

#include <Eigen/Dense>
#include <Eigen/Core>
#include <Eigen/Cholesky>
#include <vector>

class GMM
{
public:
    struct Gaussian
    {
        Eigen::VectorXf mu;
        Eigen::MatrixXf cov;
        float weight;

        Gaussian(const Eigen::VectorXf& mu, const Eigen::MatrixXf& cov, const float& weight) 
            : mu(mu), cov(cov), weight(weight) {}
    };

    struct Param
    {
        uint n_components;  // 高斯分布的个数
        uint max_iter;      // 最大迭代次数
    };

    GMM(const Param& param) : param_(param) {}

    /**
     * @brief 	 [简介] 训练模型
     * @param 	 X [in], 样本数据
     */
    void train_model(Eigen::MatrixXf& X)
    {
        scale_data(X);
        init_model(X.cols(), param_.n_components);
        for(uint i = 0; i < param_.max_iter; ++i) 
            maximize_step(X, expectation_step(X));
    }

    std::vector<Gaussian> get_models() { return models_;}
    Eigen::MatrixXf predict(Eigen::MatrixXf& X) { return expectation_step(X);}
// private:
    /**
     * @brief 	 [简介] 初始化模型参数
     * @param 	 D [in], 样本维度
     * @param 	 K [in], 模型数
     */
    void init_model(const uint& D, const uint& K)
    {
        models_.clear();
        for(uint i = 0; i < K; ++i) {
            models_.emplace_back(Eigen::VectorXf::Random(D), 
                                Eigen::MatrixXf::Identity(D, D), 1.0 / K);
        }
    }

    /**
     * @brief 	 [简介] 计算多元高斯分布的概率密度函数(实际上起到启发函数的作用)
     * @param 	 x [in], 样本数据
     * @param 	 mu [in], 均值
     * @param 	 cov [in], 协方差矩阵
     * @return 	 [float] 多元高斯分布的概率密度函数
     */
    float multivariate_normal_pdf(const Eigen::VectorXf& x, const Eigen::VectorXf& mu, const Eigen::MatrixXf& cov) 
    {
        // p(x) = 1 / (2 * pi)^(D/2) * |cov|^(1/2) * exp(-1/2 * (x - mu).T * cov^-1 * (x - mu))
        uint D = x.size();
        float det = cov.determinant();                                          
        Eigen::MatrixXf inv = cov.inverse();                                   
        float exp = std::exp(-0.5 * (x - mu).transpose() * inv * (x - mu));    
        return 1.0 / std::sqrt(std::pow(2 * M_PI, D) * det) * exp;              
    }

    /**
     * @brief 	 [简介] 对数据进行归一化
     * @param 	 X [in/out], 将所有数据都缩放到 0 和 1 之间
     */
    void scale_data(Eigen::MatrixXf& X)
    {
        for(uint i = 0; i < X.cols(); ++i) {
            float max = X.col(i).maxCoeff();
            float min = X.col(i).minCoeff();
            X.col(i) = (X.col(i) - Eigen::VectorXf::Constant(X.rows(), min)) / (max - min);
        }
    }

    /**
     * @brief 	 [简介] E步, 计算每个样本属于每个模型的概率
     * @param 	 X [in], 样本数据
     * @return 	 [Eigen::MatrixXf] 每个样本属于每个模型的概率
     */
    Eigen::MatrixXf expectation_step(const Eigen::MatrixXf& X)
    {
        uint N = X.rows();
        uint K = param_.n_components;
        Eigen::MatrixXf gamma = Eigen::MatrixXf::Zero(N, K);
        
        // 计算每个样本属于每个模型的概率
        for(uint k = 0; k < K; ++k) 
        for(uint i = 0; i < N; ++i) 
            gamma(i, k) = models_[k].weight * multivariate_normal_pdf(X.row(i), models_[k].mu, models_[k].cov);

        // 归一化
        for(uint i = 0; i < N; ++i) 
            gamma.row(i) /= gamma.row(i).sum();
        
        // std::cout << "=======================================================" << std::endl;
        // std::cout << gamma << std::endl;
        return gamma;
    }

    /**
     * @brief 	 [简介] M步, 更新模型参数
     * @param 	 X [in], 样本数据
     * @param 	 gamma [in], 每个样本属于每个模型的概率
     */
    void maximize_step(const Eigen::MatrixXf& X, const Eigen::MatrixXf& gamma)
    {
        uint N = X.rows();
        uint K = param_.n_components;
        uint D = X.cols();
        
        models_.clear();
        for(uint j = 0; j < K; ++j) {
            float sum = gamma.col(j).sum();
            Eigen::VectorXf mu = Eigen::VectorXf::Zero(D);
            Eigen::MatrixXf cov = Eigen::MatrixXf::Zero(D, D);
            
            for(uint i = 0; i < N; ++i) 
                mu += gamma(i, j) * X.row(i); 
            mu /= sum;

            for(uint i = 0; i < N; ++i) {
                Eigen::VectorXf diff = X.row(i).transpose() - mu;
                cov += gamma(i, j) * diff * diff.transpose();
            }
            cov /= sum;
            models_.emplace_back(mu, cov, sum / N);
        }
    }
private:
    Param param_;
    std::vector<Gaussian> models_;
};

#endif // __GMM_H__