#include "core/tt_test.h"
#include "common/log.h"
#include <ceres/ceres.h>


class CostFunctor
{
public:
    CostFunctor(double x, double y) : x_(x), y_(y) {}
    template<typename T>
    bool operator()(const T* const m, T* residual) const
    {
        residual[0] = T(y_) - ceres::exp(m[0] * T(x_) + m[1]);
        return true;
    }
private:
    const double x_;
    const double y_;
};


// JUST_RUN_TEST(ceres, base)
TEST(ceres, base)
{
    double m[2] = {0.0, 0.0};
    ceres::Problem problem;
    for (int i = 0; i < 100; ++i)
    {
        double x = 0.01 * i;
        double y = exp(3.0 * x + 0.5) + 0.01 * random() / RAND_MAX;
        ceres::CostFunction* cost_function = new ceres::AutoDiffCostFunction<CostFunctor, 1, 2>(new CostFunctor(x, y));
        problem.AddResidualBlock(cost_function, nullptr, m);
    }

    ceres::Solver::Options options;
    options.linear_solver_type = ceres::DENSE_QR;
    options.minimizer_progress_to_stdout = true;
    ceres::Solver::Summary summary;
    ceres::Solve(options, &problem, &summary);

    LOG_TEST("m: ", m[0], ", ", m[1]);
    LOG_TEST(summary.BriefReport());
}