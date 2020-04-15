#pragma once

#include "eigen.hpp"

namespace PT {

struct Vertex {
    Math::Vector pos;
    Math::Vector normal;
    double       u = 0, v = 0;

    Vertex()
        : pos({0, 0, 0})
        , normal({0, 0, 0}) {}

    Vertex(const Math::Vector&& pos)
        : pos(pos)
        , normal({0, 0, 0}) {}

    Vertex(const double x, const double y, const double z)
        : pos(x, y, z)
        , normal({0, 0, 0}) {}

    Vertex(const Math::Vector&& pos, const Math::Vector&& normal)
        : pos(pos)
        , normal(normal) {}
};

struct Triangle {
    Vertex v0;
    Vertex v1;
    Vertex v2;

    Triangle(const Vertex&& v0, const Vertex&& v1, const Vertex&& v2)
        : v0(v0)
        , v1(v1)
        , v2(v2) {}

    bool IntersectRay(const Math::Vector& origin, const Math::Vector& dir, double& t) {
        auto edge1 = v1.pos - v0.pos;
        auto edge2 = v2.pos - v0.pos;
        auto h     = dir.cross(edge2);
        auto a     = edge1.dot(h);

        if (a > -1e-9 && a < 1e-9)
            return false;

        auto f = 1.0 / a;
        auto s = origin - v0.pos;
        auto u = f * s.dot(h);

        if (u < 0 || u > 1)
            return false;

        auto q = s.cross(edge1);
        auto v = f * dir.dot(q);

        if (v < 0 || (u + v) > 1)
            return false;

        double dist = f * edge2.dot(q);
        if (dist > 1e-9 && dist < t) {
            t = dist;
            return true;
        }

        return false;
    }
};

} // namespace PT