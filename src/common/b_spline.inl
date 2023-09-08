#include "b_spline.h"

template <class V, class T>
inline
BSpline<V,T>::BSpline (void)
    : n(3)
{
}

template <class V, class T>
bool
BSpline<V,T>::empty (void) const
{
    return this->points.empty();
}

template <class V, class T>
inline void
BSpline<V,T>::set_degree (int degree)
{
    this->n = degree;
}

template <class V, class T>
inline int
BSpline<V,T>::get_degree (void) const
{
    return this->n;
}

template <class V, class T>
inline void
BSpline<V,T>::reserve (std::size_t n_points)
{
    this->points.reserve(n_points);
    this->knots.reserve(n_points + n + 1);
}

template <class V, class T>
inline void
BSpline<V,T>::add_point (V const& p)
{
    this->points.push_back(p);
}

template <class V, class T>
inline void
BSpline<V,T>::add_knot (T const& t)
{
    this->knots.push_back(t);
}

template <class V, class T>
inline void
BSpline<V,T>::uniform_knots (T const& min, T const& max)
{
    T width = max - min;
    int n_knots = this->points.size() + this->n + 1;
    int segments = this->points.size() - this->n;
    this->knots.clear();
    this->knots.reserve(n_knots);

    for (int i = 0; i < this->n; ++i)
        this->knots.push_back(min);
    for (int i = 0; i < segments + 1; ++i)
        this->knots.push_back(min + T(i) * width / T(segments));
    for (int i = 0; i < this->n - 1; ++i)
        this->knots.push_back(max);
    if (this->n > 0)
        this->knots.push_back(max + T(1));

#if 0
    std::cout << "Made knots: ";
    for (std::size_t i = 0; i < this->knots.size(); ++i)
        std::cout << this->knots[i] << " ";
    std::cout << std::endl;
#endif
}


template <class V, class T>
inline void
BSpline<V,T>::scale_knots (T const& min, T const& max)
{
    T first = this->knots[this->n];
    T const& last = this->knots[this->knots.size() - this->n - 1];
    T scale = (max - min) / (last - first);
    for (std::size_t i = 0; i < this->knots.size(); ++i)
        this->knots[i] = (this->knots[i] - first) * scale;
#if 0
    std::cout << "Made knots: ";
    for (std::size_t i = 0; i < this->knots.size(); ++i)
        std::cout << this->knots[i] << " ";
    std::cout << std::endl;
#endif
}

template <class V, class T>
inline typename BSpline<V,T>::PointVector const&
BSpline<V,T>::get_points (void) const
{
    return this->points;
}

template <class V, class T>
inline typename BSpline<V,T>::KnotVector const&
BSpline<V,T>::get_knots (void) const
{
    return this->knots;
}

template <class V, class T>
inline V
BSpline<V,T>::evaluate (T const& t) const
{
    /* FIXME: inefficient */
    /* TODO: fast lookup through tables*/
    V p = this->points[0] * this->deboor(0, this->n, t);
    for (std::size_t i = 1; i < this->points.size(); ++i)
        p += this->points[i] * this->deboor(i, this->n, t);
    return p;
}

template <class V, class T>
inline T
BSpline<V,T>::deboor (int i, int k, T const& x) const
{
    if (k == 0) {
        return (x >= this->knots[i] && x < this->knots[i+1]) ? T(1) : T(0);
    }

    float d1 = this->knots[i+k] - this->knots[i];
    float d2 = this->knots[i+k+1] - this->knots[i+1];
    T v1 = d1 > T(0) ? (x - this->knots[i]) / d1 : T(0);
    T v2 = d2 > T(0) ? (this->knots[i+k+1] - x) / d2 : T(0);
    return v1 * deboor(i, k-1, x) + v2 * deboor(i+1, k-1, x);
}

template <class V, class T>
inline void
BSpline<V,T>::transform (Eigen::Matrix4f const& transf)
{
    for (std::size_t i = 0; i < points.size(); ++i)
        points[i] = transf * points[i];
}
