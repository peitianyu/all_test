#ifndef __BA_OPTIMIZE_H__
#define __BA_OPTIMIZE_H__


#include <Eigen/Core>
#include <Eigen/Dense>
#include <vector>

class BA_Optimize
{
public:
    struct Option
    {
        uint max_iters;
        double eps;
        bool use_point;

        Option(const uint& _max_iters = 100, const double& _eps = 1e-6, const bool& _use_point = true)
            : max_iters(_max_iters), eps(_eps), use_point(_use_point) {}
    };

    BA_Optimize(const Eigen::Matrix3d& camera_info, const Option& option = Option())
        : option_(option), camera_info_(camera_info) {}


    void optimize(const std::vector<Eigen::Vector2d>& p2ds, std::vector<Eigen::Vector3d>& p3ds, std::vector<Eigen::Matrix4d>& poses)
    {
        p2ds_ = p2ds;
        p3ds_ = p3ds;
        poses_ = poses;
        double err = 0, err_prev = 999999999.9;

        Eigen::MatrixXd J;
        Eigen::VectorXd r;
        Eigen::MatrixXd H;
        Eigen::MatrixXd g;

        for(uint iters = 0; iters < option_.max_iters; iters++)
        {
            optimize_func(J, r, err);
            if( ( fabs(err-err_prev) < option_.eps ) || ( err < option_.eps) )
                std::cout<<"Small reprojection error. Stop optimization."<<std::endl;

            H = J.transpose() * J;
            g = -J.transpose() * r;
            Eigen::VectorXd delta_x = H.ldlt().solve(g);

            if(delta_x.head(6).norm() < option_.eps) break;

            update_poses_and_points(delta_x);
            err_prev = err;
        }
    }
private:
    void optimize_func(Eigen::MatrixXd& J, Eigen::VectorXd& r, double& err)
    {
        double cx = camera_info_(0, 2), cy = camera_info_(1, 2);
        double fx = camera_info_(0, 0), fy = camera_info_(1, 1);
        // define Jacobians, and residuals
        int n_poses = poses_.size();
        int n_points = p3ds_.size();
        int n_pixels = p2ds_.size();
        if(option_.use_point)
            J = Eigen::MatrixXd::Zero(2 * n_pixels, 6 * n_poses + 3 * n_points);// add points
        else
            J = Eigen::MatrixXd::Zero(2 * n_pixels, 6 * n_poses);
        r = Eigen::VectorXd::Zero(2 * n_pixels, 1);
        Eigen::MatrixXd J_pose_i(Eigen::MatrixXd::Zero(2, 6));
        Eigen::MatrixXd J_point_j(Eigen::MatrixXd::Zero(2, 3));
        Eigen::VectorXd r_pixels_ij(Eigen::VectorXd::Zero(2, 1));
        Eigen::MatrixXd J_cam(Eigen::MatrixXd::Zero(2, 3));
        for(int i = 0; i < n_poses; i++)
        {
            for(int j = 0; j < n_points; j++)
            {
                Eigen::Matrix4d Tcw = poses_[i];
                Eigen::Vector3d point = p3ds_[j];
                Eigen::Vector2d pixel = p2ds_[i * n_points + j]; // assume all the points are observed by all the poses
                Eigen::Matrix3d Rcw = Tcw.block(0, 0, 3, 3);
                Eigen::Vector3d tcw = Tcw.block(0, 3, 3, 1);
                Eigen::Vector3d p = Rcw*point + tcw;
                double x = p(0), y = p(1), z =p(2), z_2 = z * z;
                double p_u = cx + fx * x / z, p_v = cy + fy * y / z;
                double du = pixel(0) - p_u, dv = pixel(1) - p_v;
                err += (du * du + dv * dv);
                r_pixels_ij<< du, dv;
                J_cam << -fx/z, 0, fx*x/z_2,
                        0, -fy/z, fy*y/z_2;
                J_point_j = J_cam * Rcw;
                J_pose_i.block(0, 3, 2, 3) = - J_cam * skew(p);
                J_pose_i.block(0, 0, 2, 3) = J_cam;
                J.block<2, 6>(2*(i*n_points + j), 6*i) = J_pose_i;
                r.segment(2*(i*n_points + j), 2) = r_pixels_ij;
                if(option_.use_point)
                    J.block<2,3>(2*(i*n_points + j), 6*n_poses + 3*j) = J_point_j;
            }
        }
        err /= n_pixels;
    }

    void update_poses_and_points(const Eigen::VectorXd& delta_x)
    {
        int n_poses = poses_.size();
        int n_points = p3ds_.size();
        for(int i = 0; i < n_poses; i++)
        {
            Eigen::VectorXd delta_i = delta_x.segment(6*i, 6);
            Eigen::Matrix4d T = poses_[i];
            poses_[i] = expmap_se3(delta_i) * T;
        }
        if(!option_.use_point) return;
        for(int j = 0; j < n_points; j++)
        {
            Eigen::Vector3d delta_j = delta_x.segment(6*n_poses + 3*j, 3);
            Eigen::Vector3d p = p3ds_[j];
            p3ds_[j] = p + delta_j;
        }
    }

    Eigen::Matrix4d expmap_se3(Eigen::VectorXd x)
    {
        Eigen::Matrix3d R, V, s, I = Eigen::Matrix3d::Identity();
        Eigen::Vector3d t, w;
        Eigen::Matrix4d T = Eigen::Matrix4d::Identity();
        w = x.tail(3);
        t = x.head(3);
        double theta = w.norm();
        if( theta < 0.00000001 )
            R = I;
        else{
            s = skew(w)/theta;
            R = I + s * sin(theta) + s * s * (1.0f-cos(theta));
            V = I + s * (1.0f - cos(theta)) / theta + s * s * (theta - sin(theta)) / theta;
            t = V * t;
        }
        T.block(0,0,3,4) << R, t;
        return T;
    }

    Eigen::Matrix3d skew(Eigen::Vector3d v)
    {
        Eigen::Matrix3d skew = Eigen::Matrix3d::Zero();
        skew << 0, -v(2), v(1),
                v(2), 0, -v(0),
                -v(1), v(0), 0;
        return skew;
    }
private:
    Option option_;
    Eigen::Matrix3d camera_info_;

    std::vector<Eigen::Vector2d> p2ds_;
    std::vector<Eigen::Vector3d> p3ds_;
    std::vector<Eigen::Matrix4d> poses_;
};


#endif // __BA_OPTIMIZE_H__