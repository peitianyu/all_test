#ifndef __HMM_H__
#define __HMM_H__

#include <Eigen/Core>
#include <cassert>

class HMM
{
public:
    struct Param
    {
        Eigen::MatrixXd A;  // transition matrix
        Eigen::MatrixXd B;  // emission matrix
        Eigen::VectorXd pi; // initial state distribution

        Param(const Eigen::MatrixXd& A, const Eigen::MatrixXd& B, const Eigen::VectorXd& pi)
            : A(A), B(B), pi(pi)
        {
            assert(A.rows() == A.cols());
            assert(A.rows() == B.rows());
            assert(A.rows() == pi.rows());
        }
    };

    HMM(const Param& param) : param_(param) {}

    double forward(const Eigen::VectorXi& obs, Eigen::MatrixXd& alpha, Eigen::VectorXd& c)
    {
        uint N = param_.A.rows();
        uint T = obs.size();

        alpha = Eigen::MatrixXd::Zero(N, T);
        c = Eigen::VectorXd::Zero(T);

        double logL = 0.0;
        for (uint t = 0; t < T; ++t) {
            assert(obs[t] < param_.B.cols());
            c[t] = 0.0;
            for (uint i = 0; i < N; ++i) {
                if (0 == t) alpha(i, 0) = param_.pi[i] * param_.B(i, obs[0]); 
                else for (uint j = 0; j < N; ++j) alpha(i, t) += alpha(j, t - 1) * param_.A(j, i) * param_.B(i, obs[t]); 
                
                c[t] += alpha(i, t);
            }

            if (c[t] == 0.0 || HUGE_VAL == 1.0 / c[t]) {
                c[t] = 1.0;
                logL = -HUGE_VAL;
            } else {
                c[t] = 1.0 / c[t];
                alpha.col(t) *= c[t];
                logL -= log2(c[t]);
            }
        }

        // std::cout << "c = " << c.transpose() << std::endl;

        return logL;
    }

    double forward_backward(const Eigen::VectorXi& obs, Eigen::MatrixXd& gamma)
    {
        uint N = param_.A.rows();
        uint T = obs.size();

        gamma = Eigen::MatrixXd::Zero(N, T);
        Eigen::VectorXd c;

        double logL = forward(obs, gamma, c);
        for (uint i = 0; i < N; ++i) gamma(i, T-1) = c[T-1];
        for (uint t = T-1; t > 0; --t) {
            for (uint i = 0; i < N; ++i) {
                gamma(i, t-1) = 0.0;
                for (uint j = 0; j < N; ++j) gamma(i, t-1) += param_.A(i, j) * param_.B(j, obs[t]) * gamma(j, t);
                gamma(i, t-1) *= c[t-1];
            }
        }

        return logL;
    }

    double viterbi(const Eigen::VectorXi& obs, Eigen::VectorXi& path)
    {
        uint N = param_.A.rows();
        uint T = obs.size();

        Eigen::MatrixXd delta(N, T);
        Eigen::MatrixXd delta_next(N, T);
        Eigen::MatrixXi psi(N, T);
        for (uint i = 0; i < N; ++i) {
            delta(i, 0) = log2(param_.pi[i]) + log2(param_.B(i, obs[0]));
            psi(i, 0) = 0;
        }

        double maxLL;
        uint argmax;
        for (uint t = 1; t < T; ++t) {
            for (uint i = 0; i < N; ++i) {
                maxLL = -HUGE_VAL;
                argmax = 0;

                for (uint j = 0; j < N; ++j) {
                    double LL = delta(j, t-1) + log2(param_.A(j, i));
                    if (LL > maxLL) {
                        maxLL = LL;
                        argmax = j;
                    }
                }

                delta_next(i, t) = maxLL + log2(param_.B(i, obs[t]));
                psi(i, t) = argmax;
            }

            Eigen::MatrixXd tmp = delta;
            delta = delta_next;
            delta_next = tmp;
        }

        maxLL = -HUGE_VAL;
        for (uint i = 0; i < N; ++i) {
            if (delta(i, T-1) > maxLL) {
                maxLL = delta(i, T-1);
                argmax = i;
            }
        }

        path = Eigen::VectorXi(T);
        for (uint t = T-1; t > 0; --t) {
            path[t] = argmax;
            argmax = psi(argmax, t);
        }

        path[0] = argmax;

        return maxLL;
    }

    void baum_welch(const Eigen::VectorXi& obs, const uint& iter_num)
    {
        for(uint i = 0; i < iter_num; ++i)
            m_step(obs, e_step(obs));
    }

    Param param() const { return param_; }
private:
    Eigen::MatrixXd e_step(const Eigen::VectorXi& obs)
    {
        // return Eigen::MatrixXd();

    }

    void m_step(const Eigen::VectorXi& obs, const Eigen::MatrixXd& gamma)
    {
        
    }
private:
    Param param_;
};


#endif // !__HMM_H__