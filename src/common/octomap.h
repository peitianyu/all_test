#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Core>
#include <memory>

using Point3D = Eigen::Vector3d;

struct OctreeNode
{
    Point3D centor;
    double size;
    double log_odds;
    OctreeNode* children[8];
};

#define MAX_DEPTH 16
#define MIN_SIZE 0.01

class OctoMap
{
public:
    struct Option
    {
        uint max_depth = 16;
        double min_size = 0.01;
    };

    OctoMap(const Option& option)
        : option_(option)
    {
        root_ = std::make_shared<OctreeNode>();
    }

    void InsertPoint(const Point3D& point, double log_odds)
    {
        std::shared_ptr<OctreeNode> node = root_;
        Point3D curr_centor(0.5, 0.5, 0.5);
        double curr_size = 1.0;
        uint depth = 0;

        while (depth < option_.max_depth && curr_size > option_.min_size)
        {
            int octant_idx = GetOctant(point, curr_centor);
            OctreeNode* child = node->children[octant_idx];

            if (child == nullptr)
            {
                // Create new child if it doesn't exist
                child = new OctreeNode();
                child->centor = curr_centor + 0.5 * curr_size * Eigen::Vector3d(
                    (octant_idx & 4) ? 0.5 : -0.5,
                    (octant_idx & 2) ? 0.5 : -0.5,
                    (octant_idx & 1) ? 0.5 : -0.5
                );
                child->size = 0.5 * curr_size;
                child->log_odds = log_odds;
                for (int i = 0; i < 8; ++i)
                {
                    child->children[i] = nullptr;
                }
                node->children[octant_idx] = child;
                break;
            }
            else
            {
                // Descend to the next level
                node = std::shared_ptr<OctreeNode>(child);
                curr_centor = child->centor;
                curr_size = child->size;
                ++depth;
            }
        }

        if (depth == option_.max_depth || curr_size <= option_.min_size)
        {
            // Can't descend further, insert at current node
            node->log_odds += log_odds;
        }
    }

    // 遍历所有节点
    void Traverse()
    {
        std::vector<std::shared_ptr<OctreeNode>> stack;
        stack.push_back(root_);
        while (!stack.empty())
        {
            std::shared_ptr<OctreeNode> node = stack.back();
            stack.pop_back();
            std::cout << "centor: " << node->centor.transpose() << std::endl;
            std::cout << "size: " << node->size << std::endl;
            std::cout << "log_odds: " << node->log_odds << std::endl;
            for (int i = 0; i < 8; ++i)
            {
                if (node->children[i] != nullptr)
                {
                    stack.push_back(std::shared_ptr<OctreeNode>(node->children[i]));
                }
            }
        }
    }
private:
    int GetOctant(const Point3D& point, const Point3D& centor)
    {
        int idx = 0;
        if (point.x() > centor.x()) idx |= 4;
        if (point.y() > centor.y()) idx |= 2;
        if (point.z() > centor.z()) idx |= 1;
        return idx;
    }
private:
    Option option_;
    std::shared_ptr<OctreeNode> root_;
};
