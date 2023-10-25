#include "core/tt_test.h"
#include "common/log.h"

#include "factor_graph/graph_optimize.h"

// 模拟: y = 2*x^2 + 1  => y = a*x^b + c
void random(std::vector<double> &x, std::vector<double> &y);

class VariableX : public Variable
{
public:
    VariableX(const Eigen::Vector3d& x) : x_(x) {}

    double a() const { return x_(0); }
    double b() const { return x_(1); }
    double c() const { return x_(2); }

    virtual size_t Dim() const { return 3; }

    virtual void Plus(const Eigen::VectorXd &delta) { x_ += delta; }

    virtual void Print() const { std::cout << "x: " << x_.transpose() << std::endl; }
private:
    Eigen::Vector3d x_;
};

class LineRegressFactor : public Factor
{
public:
    LineRegressFactor(VariableX *v_a, double x, double y) : x_(x), y_(y)
    {
        AddVariable(v_a);
    }

    virtual int Dim() const override { return 1;}

    virtual Eigen::VectorXd Error() const override
    {
        GRAPH_ASSERT(this->NumVariables() == 1);
        const VariableX *v_a = static_cast<VariableX *>(this->VariableAt(0));
        Eigen::VectorXd r(1);
        r(0) = y_ - (v_a->a() * pow(x_, v_a->b()) + v_a->c());
        return r;
    }

    virtual Eigen::VectorXd SubtractError(const Eigen::VectorXd &e1, const Eigen::VectorXd &e2) const override
    {
        return e1 - e2;
    }
private:
    double x_, y_;
};


// JUST_RUN_TEST(line_regress, test)
TEST(line_regress, test)
{
    std::vector<double> xs, ys;
    random(xs, ys);

    // 构建图
    FactorGraph graph;
    VariableX *v_x = new VariableX(Eigen::Vector3d(2.0, 1.0, 1.0));
    graph.AddVariable(v_x);
    for (uint i = 0; i < xs.size(); ++i)
    {
        LineRegressFactor *factor = new LineRegressFactor(v_x, xs[i], ys[i]);
        graph.AddFactor(factor);
    }

    // 优化
    GraphOptimize::Option option = GraphOptimize::Option();
    GraphOptimize graph_optimize = GraphOptimize(option);
    graph_optimize.OptimizeGN(&graph);

    // 打印结果
    v_x->Print();
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