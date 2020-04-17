#pragma once

#include <Eigen/Geometry>

namespace PT::Math {

constexpr inline double PI   = 3.1415926535897932;
constexpr inline double PI_2 = PI / 2.0;
constexpr inline double PI_3 = PI / 3.0;
constexpr inline double PI_4 = PI / 4.0;

constexpr inline double DEG_TO_RAD = (PI / 180);
constexpr inline double RAD_TO_DEG = (180 / PI);

typedef Eigen::Array3d  Color;
typedef Eigen::Vector3d Vector;
typedef Eigen::Affine3d Matrix;

/**
 * Returns an angle-axis rotation matrix
 * axis should be normalized
 * theta is in RADIANS
 */
Matrix AngleAxisMatrix(const Vector& axis, const double theta) {
    Matrix mat;
    mat = Eigen::AngleAxisd(theta, axis);
    return mat;
}

/**
 * Returns a matrix representing an euler angle
 * Pitch/yaw/roll are in RADIANS
 */
Matrix EulerAngleMatrix(const double pitch, const double yaw, const double roll) {
    Matrix mat;

    mat = Eigen::AngleAxisd(pitch, Eigen::Vector3d::UnitX()) *
          Eigen::AngleAxisd(yaw, Eigen::Vector3d::UnitY()) *
          Eigen::AngleAxisd(roll, Eigen::Vector3d::UnitZ());

    return mat;
}

} // namespace PT::Math
