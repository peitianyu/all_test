#ifndef __OCTREE_H__
#define __OCTREE_H__

#include <vector>
#include <functional>

template <typename PosType, typename DataType, size_t DIM>
class Octree {
public:
    constexpr static size_t child_num_ = 1 << DIM;
    struct Node
    {
        Node *childs[child_num_];
        PosType center;
        DataType data;
        size_t depth;

        Node() : center(PosType()), data(DataType()), depth(0) {
            for (size_t i = 0; i < child_num_; ++i) childs[i] = nullptr;
        }

        Node(const PosType& center, const DataType& data, size_t depth) 
            : center(center), data(data), depth(depth) {
            for (size_t i = 0; i < child_num_; ++i) childs[i] = nullptr;
        }

        ~Node() { 
            for (size_t i = 0; i < child_num_; ++i) {
                if (childs[i] != nullptr) {
                    delete childs[i];
                }
            }
        }
    };

    struct Boundary
    {
        PosType min;
        PosType max;

        Boundary(PosType min, PosType max) : min(min), max(max) { }
        Boundary() : min(PosType::Zero()), max(PosType::Zero()) { }

        constexpr PosType size() const { return max - min; }
        constexpr PosType center() const { return (max + min) / 2; }
    };

    Octree(const PosType& min, const PosType& max, size_t depth) 
        :boundary_(Boundary(min, max)), max_depth_(depth)
    {
        root_ = new Node(boundary_.center(), DataType(), 0);
    }

    ~Octree()
    {   
        delete root_;
    }

    void insert(const PosType& pos, const DataType& data)
    {
        insert(root_, pos, data);
    }

    Node *find(const PosType& pos)
    {
        return find(root_, pos, max_depth_);
    }

    Node *find(const PosType& pos, const size_t& depth)
    {
        return find(root_, pos, depth);
    }

    void find_boundary(Node* node, Boundary& boundary)
    {
        PosType half_size = boundary_.size() / (1 << (node->depth + 1));
        boundary.min = node->center - half_size;
        boundary.max = node->center + half_size;
    }

    void visual(std::function<void(Node* node)> func = nullptr)
    {
        traverse(root_, func);
    }

protected:
    void traverse(Node *node, std::function<void(Node* node)> func)
    {
        if (node == nullptr) return;
        func(node);
        for (size_t i = 0; i < child_num_; i++) {
            if(node->childs[i] != nullptr){
                traverse(node->childs[i], func);
            }
        }
    }

    void insert(Node *node, const PosType& pos, const DataType& data)
    {
        if (node->depth+1 == max_depth_) return;

        size_t index = find_index(pos, node);

        if (node->childs[index] == nullptr) {
            node->childs[index] = new Node(find_center(pos, node), data, node->depth + 1);
        }else{
            node->childs[index]->data = update(node->childs[index]->data, data);
        }

        insert(node->childs[index], pos, data);
    }
    
    virtual DataType update(DataType& old_data, const DataType& new_data)
    {
        return (new_data+old_data);
    }

    Node *find(Node *node, const PosType& pos, const size_t& depth)
    {
        if (node->depth == depth) return node;

        size_t index = find_index(pos, node);

        if (node->childs[index] == nullptr) return node;

        return find(node->childs[index], pos, depth);
    }
private:
    /******************************************************
	 *	Cells id are assigned as their position 
	 *   from the center (+ greater than center, - lower than center)
	 *
	 *		for 3D case									for 2D case
	 *
	 *	  	0	1	2	3	4	5	6	7		  	0	1	2	3
	 * 	x:	-	+	-	+	-	+	-	+		x:	-	+	-	+
	 * 	y:	-	-	+	+	-	-	+	+		y:	-	-	+	+	
	 * 	z:	-	-	-	-	+	+	+	+
	 *
	 *****************************************************/
    size_t find_index(const PosType& pos, const Node *node)
    {
        size_t index = 0;
        for (size_t i = 0; i < DIM; ++i) {
            if (pos[i] > node->center[i]) index |= (1 << i);
        }
        return index;
    }

    PosType find_center(const PosType& pos, const Node *node)
    {
        PosType center = node->center;
        PosType half_size = boundary_.size() / (1 << (node->depth + 2));
        for (size_t i = 0; i < DIM; ++i) {
            center[i] = (pos[i] > node->center[i]) ? center[i] + half_size[i] : center[i] - half_size[i];
        }

        return center;
    }
private:
    Boundary boundary_;
    size_t max_depth_;
    Node *root_;
};

template<typename PosType, typename DataType> using QuadTree = Octree<PosType, DataType, 2>;
template<typename PosType, typename DataType> using OctTree = Octree<PosType, DataType, 3>;

#endif // __OCTREE_H__