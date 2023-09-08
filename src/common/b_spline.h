#ifndef __BSPLINE_H__
#define __BSPLINE_H__

#include <vector>
#include <Eigen/Core>

template <class V, class T = float>
class BSpline
{
public:
    typedef std::vector<T> KnotVector;
    typedef std::vector<V> PointVector;

public:
    BSpline (void);

    /** Returns whether there are no points in this spline. */
    bool empty (void) const;

    /** Sets the degree of spline segments. */
    void set_degree (int degree);
    /** Returns the degree of the spline segments. */
    int get_degree (void) const;

    /** Reserves space for points and the knot vector. */
    void reserve (std::size_t n_points);

    /** Adds a point to the control point vector. */
    void add_point (V const& p);
    /** Adds a knot to the knot vector. */
    void add_knot (T const& t);
    /** Initializes the knot vector to be uniform. */
    void uniform_knots (T const& min, T const& max);
    /** Scales the knots such that evaluation is valid in [min, max]. */
    void scale_knots (T const& min, T const& max);

    /** Returns the point vector. */
    PointVector const& get_points (void) const;
    /** Returns the knot vector. */
    KnotVector const& get_knots (void) const;

    /** Evalutes the B-Spline. */
    V evaluate (T const& t) const;

    /** Transforms the B-Spline. */
    void transform (Eigen::Matrix4f const& transf);

private:
    /** De Boor algorithm to evaluate the basis polynomials. */
    T deboor (int i, int k, T const& x) const;

private:
    int n; ///< the degree of the polynom
    KnotVector knots; ///< knot vector with m entries
    PointVector points; ///< m-n-1 control points
};

#include "b_spline.inl"
#endif // __BSPLINE_H__
