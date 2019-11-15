#ifndef EZGL_RECTANGLE_HPP
#define EZGL_RECTANGLE_HPP

#include <ezgl/point.hpp>

#include <algorithm>

namespace ezgl {

/**
 * Represents a rectangle as two diagonally opposite points.
 */
class rectangle {
public:
  /**
   * Create a rectangle from two diagonally opposite points.
   */
  rectangle(point2d origin, point2d top_right) : m_first(origin), m_second(top_right)
  {
  }

  /**
   * Create a rectangle with a given width and height.
   */
  rectangle(point2d origin, double width, double height) : m_first(origin), m_second(origin)
  {
    m_second.x += width;
    m_second.y += height;
  }

  /**
   * The minimum x-coordinate.
   */
  double left() const
  {
    return std::min(m_first.x, m_second.x);
  }

  /**
   * The maximum x-coordinate.
   */
  double right() const
  {
    return std::max(m_first.x, m_second.x);
  }

  /**
   * The minimum y-coordinate.
   */
  double bottom() const
  {
    return std::min(m_first.y, m_second.y);
  }

  /**
   * The maximum y-coordinate.
   */
  double top() const
  {
    return std::max(m_first.y, m_second.y);
  }

  /**
   * The minimum x-coordinate and the minimum y-coordinate.
   */
  point2d bottom_left() const
  {
    return {left(), bottom()};
  }

  /**
   * The minimum x-coordinate and the maximum y-coordinate.
   */
  point2d top_left() const
  {
    return {left(), top()};
  }

  /**
   * The maximum x-coordinate and the minimum y-coordinate.
   */
  point2d bottom_right() const
  {
    return {right(), bottom()};
  }

  /**
   * The maximum x-coordinate and the maximum y-coordinate.
   */
  point2d top_right() const
  {
    return {right(), top()};
  }

  /**
   * Test if the x and y values are within the rectangle.
   */
  bool contains(double x, double y) const
  {
    if(x < left() || right() < x || y < bottom() || top() < y) {
      return false;
    }

    return true;
  }

  /**
   * Test if the x and y values are within the rectangle.
   */
  bool contains(point2d point) const
  {
    return contains(point.x, point.y);
  }

  /**
   * The width of the rectangle.
   */
  double width() const
  {
    return right() - left();
  }

  /**
   * The height of the rectangle.
   */
  double height() const
  {
    return top() - bottom();
  }

  /**
   *
   * The area of the rectangle.
   */
  double area() const
  {
    return width() * height();
  }

  /**
   * The center of the rectangle in the x plane.
   */
  double center_x() const
  {
    return (right() + left()) * 0.5;
  }

  /**
   * The center of the rectangle in the y plane.
   */
  double center_y() const
  {
    return (top() + bottom()) * 0.5;
  }

  /**
   * The center of the recangle.
   */
  point2d center() const
  {
    return {center_x(), center_y()};
  }

  /**
   * Test for equality.
   */
  bool operator==(const rectangle &rhs) const
  {
    return m_first == rhs.m_first && m_second == rhs.m_second;
  }

  /**
   * Test for inequality.
   */
  bool operator!=(const rectangle &rhs) const
  {
    return !(rhs == *this);
  }

  /**
   * translate the rectangle by positive offsets.
   */
  friend rectangle &operator+=(rectangle &lhs, point2d const &rhs)
  {
    lhs.m_first += rhs;
    lhs.m_second += rhs;

    return lhs;
  }

  /**
   * translate the rectangle by negative offsets.
   */
  friend rectangle &operator-=(rectangle &lhs, point2d const &rhs)
  {
    lhs.m_first -= rhs;
    lhs.m_second -= rhs;

    return lhs;
  }

  /**
   * Create a new rectangle that is translated (negative offsets).
   */
  friend rectangle operator-(rectangle &lhs, point2d const &rhs)
  {
    return rectangle(lhs.m_first - rhs, lhs.m_second - rhs);
  }

  /**
   * Create a new rectangle that is translated (positive offsets).
   */
  friend rectangle operator+(rectangle &lhs, point2d const &rhs)
  {
    return rectangle(lhs.m_first + rhs, lhs.m_second + rhs);
  }

  /** The first point of the rectangle */
  point2d m_first;

  /** The second point of the rectangle */
  point2d m_second;
};
}

#endif //EZGL_RECTANGLE_HPP
