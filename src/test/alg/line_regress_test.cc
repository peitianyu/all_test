#include "core/tt_test.h"
#include "common/log.h"

#include "factor_graph/linear_regression_optimize.h"

// 模拟: y = 2*x^2 + 1  => y = a*x^b + c
void random(std::vector<double> &x, std::vector<double> &y);

class LinearRegressionFactor1 : public LinearRegressionFactor
{
public:
    LinearRegressionFactor1(LinearRegressionVariable *v_a, const Eigen::VectorXd& measurement) 
        : LinearRegressionFactor(v_a, measurement) {}
    
    virtual Eigen::VectorXd Error() const override
    {
        GRAPH_ASSERT(this->NumVariables() == 1);
        const LinearRegressionVariable *v_a = static_cast<LinearRegressionVariable *>(this->VariableAt(0));
        Eigen::VectorXd r(1);
        r(0) = measurement_(1) - (v_a->x()(0) * pow(measurement_(0), v_a->x()(1)) + v_a->x()(2));
        return r;
    }
};


JUST_RUN_TEST(line_regress, test)
TEST(line_regress, test)
{
    std::vector<double> xs, ys;
    random(xs, ys);

    LinearRegressionOptimize<LinearRegressionFactor1> linear_regression_optimize;

    Eigen::MatrixXd measurements = Eigen::MatrixXd::Zero(xs.size(), 2);
    for(uint i = 0; i < xs.size(); ++i)
    {
        measurements(i, 0) = xs[i];
        measurements(i, 1) = ys[i];
    }
    linear_regression_optimize.Optimize(Eigen::Vector3d(2.0, 1.0, 1.0), measurements);

    linear_regression_optimize.Print();
}

void random(std::vector<double> &x, std::vector<double> &y)
{
    for (int i = 0; i < 5000; ++i)
    {
        double x_ = i / 5000.0;
        double y_ = 2 * x_ * x_ + 1 + 0.1 * (rand() / (RAND_MAX + 1.0) - 0.5);
        x.push_back(x_);
        y.push_back(y_);
    }
}