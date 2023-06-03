#ifndef __OCTREE_H__
#define __OCTREE_H__

#include <vector>
#include <map>
#include <Eigen/Core>
#include "core/tt_assert.h"

using Point3D = Eigen::Vector3d;


class Octree
{
public:
    Octree(const Point3D& center, const Point3D& half_dimension)
        : center_(center), half_dimension_(half_dimension), data_(nullptr){
        for (int i = 0; i < 8; ++i){
            children_[i] = nullptr;
        }
    }

    Octree(const Octree& other)
        : center_(other.center_), half_dimension_(other.half_dimension_), data_(other.data_){
    }

    ~Octree(){
        for (int i = 0; i < 8; ++i)
            delete children_[i];
    }

    void Insert(const Point3D& p)
    {
        tt_assert(IsInBoundary(p));

        if (IsLeaf())
        {
            if(data_ == nullptr){
                data_ = new Point3D(p);
                return;
            }else{
                Point3D* old_data = data_;
                data_ = nullptr;
                for (int i = 0; i < 8; ++i)
                {
                    Point3D new_center = center_;
                    new_center.x() += half_dimension_.x() * (i & 4 ? .5f : -.5f);
                    new_center.y() += half_dimension_.y() * (i & 2 ? .5f : -.5f);
                    new_center.z() += half_dimension_.z() * (i & 1 ? .5f : -.5f);
                    children_[i] = new Octree(new_center, half_dimension_ * .5f);
                }
                children_[GetOctant(*old_data)]->Insert(*old_data);
                children_[GetOctant(p)]->Insert(p);
            }
        }else{
            children_[GetOctant(p)]->Insert(p);
        }
    }
    std::vector<Point3D> GetPointsInsideSphere(const Point3D& center, double radius) const
    {
        std::map<double, Point3D> results;
        GetPointsInsideSphere(center, radius, results);
        std::vector<Point3D> ret;
        for (auto it = results.begin(); it != results.end(); ++it)
            ret.push_back(it->second);
        return ret;
    }
private:
    void GetPointsInsideSphere(const Point3D& center, double radius, std::map<double, Point3D>& results) const
    {
        if (IsLeaf())
        {
            if (data_ != nullptr)
            {
                double dist = (center - *data_).norm();
                if (dist <= radius){ results[dist] = *data_;}
            }
        }
        else
        {
            for (int i = 0; i < 8; ++i)
            {
                Point3D new_center = center_;
                new_center.x() += half_dimension_.x() * (i & 4 ? .5f : -.5f);
                new_center.y() += half_dimension_.y() * (i & 2 ? .5f : -.5f);
                new_center.z() += half_dimension_.z() * (i & 1 ? .5f : -.5f);
                double dist = (center - new_center).norm();
                if (dist <= radius)
                    children_[i]->GetPointsInsideSphere(center, radius, results);
            }
        }
    }

    bool IsLeaf() const { return children_[0] == nullptr; }

    int GetOctant(const Point3D& p) const
    {
        int oct = 0;
        if (p.x() >= center_.x()) oct |= 4;
        if (p.y() >= center_.y()) oct |= 2;
        if (p.z() >= center_.z()) oct |= 1;
        return oct;
    }

    bool IsInBoundary(const Point3D& p) const
    {
        return (p - center_).cwiseAbs().maxCoeff() <= half_dimension_.maxCoeff();
    }
private:
    Point3D center_;
    Point3D half_dimension_;
    Octree* children_[8];
    Point3D* data_;
};

#endif // __OCTREE_H__