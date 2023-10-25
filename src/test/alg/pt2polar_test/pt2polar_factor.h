#ifndef __PT2POLAR_FACTOR_H__
#define __PT2POLAR_FACTOR_H__

#include "factor_graph/factor.h"
#include "factor_graph/variable.h"


class Pt2PolarVariable : public Variable
{
public:
    Pt2PolarVariable(const Eigen::Vector4d& x) : x_(x) {}

    Eigen::Vector3d rot() const { return x_.head<3>(); }

    Eigen::Matrix3d ext_R() const {
        return Eigen::Quaterniond(x_(0), x_(1), x_(2), 1).toRotationMatrix();
    }

    double scale() const { return x_(3); }

    virtual size_t Dim() const { return 4; }

    virtual void Plus(const Eigen::VectorXd &delta) { x_ += delta; }

    virtual void Print() const { std::cout << "x: " << x_.transpose() << std::endl; }
private:
    Eigen::Vector4d x_;
};

class Pt2PolarFactor : public Factor
{
public:
    Pt2PolarFactor(Pt2PolarVariable *v_a, const Eigen::Matrix4d& T, const double& baseline, const Eigen::Vector3d& p0, const Eigen::Vector3d& p1) 
        : T_(T), baseline_(baseline), p0_(p0), p1_(p1) { AddVariable(v_a);}

    virtual int Dim() const override { return 1;}

    virtual Eigen::VectorXd Error() const override
    {
        GRAPH_ASSERT(this->NumVariables() == 1);
        const Pt2PolarVariable *v_a = static_cast<Pt2PolarVariable *>(this->VariableAt(0));
        
        Eigen::Matrix4d T01 = Eigen::Matrix4d::Identity();
        T01 << T_(0,0), T_(0,1), T_(0,2), T_(0,3) * v_a->scale(),
               T_(1,0), T_(1,1), T_(1,2), T_(1,3) * v_a->scale(),
               T_(2,0), T_(2,1), T_(2,2), T_(2,3) * v_a->scale(),
               0, 0, 0, 1;
        
        Eigen::Matrix4d ext_T = Eigen::Matrix4d::Identity();
        ext_T.block<3, 3>(0, 0) = v_a->ext_R();
        ext_T(0, 3) = baseline_;

        T01 = T01 * ext_T;

        Eigen::Matrix3d E = hat(T01.block<3, 1>(0, 3)) * T01.block<3, 3>(0, 0);

        Eigen::Vector3d line = E * p0_;

        Eigen::VectorXd r(1);
        r << std::abs(p1_.dot(line) / line.norm());

        return r;
    }

    Eigen::Matrix3d hat(const Eigen::Vector3d& x) const
    {
        Eigen::Matrix3d x_hat;
        x_hat << 0, -x(2), x(1),
                 x(2), 0, -x(0),
                 -x(1), x(0), 0;
        return x_hat;
    }
private:
    Eigen::Matrix4d T_;
    double baseline_;
    Eigen::Vector3d p0_;
    Eigen::Vector3d p1_;
};








#endif // __PT2POLAR_FACTOR_H__