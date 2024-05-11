#ifndef __A_STAR_H__
#define __A_STAR_H__

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <cmath>
#include "simple_grid_map.h"

namespace path_plan
{

struct Point
{
    int x, y;

    Point(int _x, int _y) : x(_x), y(_y) {}

    bool operator<(const Point &other) const{
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

struct Node
{
    Point pos;
    float g;
    float h;
    Node *pre_node;

    Node(Point _pos)
        : pos(_pos), g(0), h(0), pre_node(nullptr)
    {}

    Node(Point _pos, float _g, float _h, Node *_pre_node)
        : pos(_pos), g(_g), h(_h), pre_node(_pre_node) { g += _pre_node->g; }

    bool cmp(Point new_pos) { return pos.x == new_pos.x && pos.y == new_pos.y;}

    float sum_cost() const { return g + h;}

    std::vector<Point> path() {
        std::vector<Point> path;
        Node *curr_node = this;
        while (curr_node != NULL) {
            path.push_back(curr_node->pos);
            curr_node = curr_node->pre_node;
        }
        std::reverse(path.begin(), path.end());
        return path;
    }
};

class Astar : public GridMap
{
public:
    Astar(int width, int height) : GridMap(width, height)
    {}

    Astar(const std::string& file_name) : GridMap(file_name)
    {}

    void get_size(int &width_, int &height_) const
    {
        width_ = width;
        height_ = height;
    }

    std::vector<Point> find_path(const Point &start, const Point &dest)
    {
        if(is_occ(start.x, start.y) || is_occ(dest.x, dest.y)) {
            std::cerr << "start or dest is occ" << std::endl;
            return {};
        }

        std::set<Point> closed_list;

        auto cmp = [](const Node *left, const Node *right) { return left->sum_cost() > right->sum_cost(); };
        std::priority_queue<Node *, std::vector<Node *>, decltype(cmp)> open_list(cmp);

        open_list.push(new Node(start)); // 给一个起点
        while (!open_list.empty())
        {
            Node *curr_node = open_list.top();
            open_list.pop();

            // 找到目标节点, 返回路径
            if (curr_node->cmp(dest)) {
                return curr_node->path();
            }
            
            // 更新closed_list
            if(closed_list.find(curr_node->pos) != closed_list.end())   continue;  
            else                                                closed_list.insert(curr_node->pos);        

            // 更新open_list
            static std::vector<Node> movements = get_motion_model(); // 四近邻/八近邻
            for (const Node &movement : movements)
            {
                Point new_pos = Point(curr_node->pos.x + movement.pos.x, curr_node->pos.y + movement.pos.y);
                
                // 若节点已经被探索过或者是障碍物则跳过
                if (!is_in(new_pos.x, new_pos.y)) continue;
                
                // 将更新后的节点加入openList
                open_list.push(new Node(new_pos, get_movement_cost(new_pos) , heuristic(new_pos, dest), curr_node));            // Astar
                // open_list.push(new Node(new_pos, get_movement_cost(new_pos), 0, curr_node));                                 // Dijkstra
            }
        }

        return std::vector<Point>();
    }
private:
    float get_movement_cost(const Point &pos)
    {
        if(is_occ(pos.x, pos.y)) return 10.0;

        return 0.0;
    }

    float heuristic(const Point &pos, const Point &dest)
    {
        float dx = pos.x - dest.x;
        float dy = pos.y - dest.y;
        return  std::sqrt(dx * dx + dy * dy);
    }

    std::vector<Node> get_motion_model()
    {
        return {Node(Point(1, 0)),
                Node(Point(0, 1)),
                Node(Point(-1, 0)),
                Node(Point(0, -1))};
    }
};

} // namespace path_plan












#endif // __A_STAR_H__