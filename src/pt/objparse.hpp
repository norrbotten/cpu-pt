#pragma once

#include <iostream>

#include <fstream>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "string-trim.hpp"
#include "triangle.hpp"

namespace PT {

// TODO: make this more robust and prettier
// this should probably also be in-place, and not have to store
// a bunch of intermediate vectors, so i can load fucklarge obj files
// this must be some of the ugliest fucking code i ever wrote
std::optional<std::vector<Triangle>> ParseObj(const char* filename) {
    std::ifstream input(filename);

    if (!input.good())
        return {};

    std::vector<Math::Vector>              vertex_positions;
    std::vector<Math::Vector>              vertex_normals;
    std::vector<std::pair<double, double>> vertex_texcoords;

    // storing indices in the vectors above, since its not specified
    // if the points have to be defined BEFORE they are used
    typedef std::array<size_t, 9> TriangleData; // 3 pos, 3 normals, 3 texcoords
    std::vector<TriangleData>     triangles;

    for (std::string line; std::getline(input, line);) {
        Util::trim(line);

        if (line.length() == 0 || line.at(0) == '#')
            continue;

        std::stringstream line_stream(line);

        std::string first;
        line_stream >> first; // read first 'word'

        if (first.compare("v") == 0) { // vertex pos
            double x, y, z, w = 1;
            line_stream >> x;
            line_stream >> y;
            line_stream >> z;

            if (!line_stream.eof()) // optional w scalar argument, defaults to 1
                line_stream >> w;

            vertex_positions.push_back(w * Math::Vector{x, y, z});
        }
        else if (first.compare("vt") == 0) { // vertex texture coord, skipping w here
            double u, v = 0;
            line_stream >> u;

            if (!line_stream.eof()) // apparently v is optional and defaults to 0
                line_stream >> v;

            vertex_texcoords.push_back({u, v});
        }
        else if (first.compare("vn") == 0) { // vertex normal
            double x, y, z;
            line_stream >> x;
            line_stream >> y;
            line_stream >> z;

            auto normal = Math::Vector{x, y, z};
            normal.normalize(); // not guaranteed to be normalized

            vertex_normals.push_back(normal);
        }
        else if (first.compare("f") == 0) { // face/polygon
            std::vector<size_t> vert_pos_indices;
            std::vector<size_t> vert_texcoord_indices;
            std::vector<size_t> vert_normal_indices;

            bool has_texcoord = false, has_normal = false;

            for (std::string segment; line_stream >> segment;) {
                const auto first_slash = segment.find('/');

                if (first_slash == std::string::npos) { // no slash found, this only has a pos index
                    vert_pos_indices.push_back(std::stoull(segment));
                }
                else if (segment.length() > first_slash) {
                    // add vert pos index
                    vert_pos_indices.push_back(std::stoull(segment.substr(0, first_slash)));

                    // search for a 2nd slash
                    const auto second_slash = segment.find('/', first_slash + 1);

                    if (second_slash == std::string::npos) { // no 2nd slash, no normals
                        const auto texcoord = segment.substr(first_slash + 1);
                        vert_texcoord_indices.push_back(std::stoull(texcoord));
                        has_texcoord = true;
                    }
                    else if (second_slash == (first_slash + 1)) { // no texcoord, only normal
                        const auto normal = segment.substr(second_slash + 1);
                        vert_normal_indices.push_back(std::stoull(normal));
                        has_normal = true;
                    }
                    else { // has texcoord and normal
                        if (segment.length() == second_slash)
                            return {};

                        const auto texcoord =
                            segment.substr(first_slash + 1, second_slash - first_slash - 1);

                        const auto normal = segment.substr(second_slash + 1);

                        vert_texcoord_indices.push_back(std::stoull(texcoord));
                        vert_normal_indices.push_back(std::stoull(normal));

                        has_texcoord = true;
                        has_normal   = true;
                    }
                }
                else {
                    // obj fucked
                    return {};
                }
            }

            // fill texcoord and normal vectors with -1 if the polygon doesnt have
            // either of them

            if (!has_texcoord) {
                vert_texcoord_indices.reserve(vert_pos_indices.size());
                for (size_t i = 0; i < vert_pos_indices.size(); i++)
                    vert_texcoord_indices.push_back(-1); // yes will underflow since its a size_t,
                                                         // but its not gonna have 2^64-1 points
            }

            if (!has_normal) {
                vert_normal_indices.reserve(vert_pos_indices.size());
                for (size_t i = 0; i < vert_pos_indices.size(); i++)
                    vert_normal_indices.push_back(-1);
            }

            // triangulate it, atm this is just crappy triangle fan
            // TODO: something that works for concave polys (reasonably flat ones)
            // IDEA: place em on a common plane and run ear clipping on it, then reproject into 3d

            for (size_t i = 2; i < vert_pos_indices.size(); i++) {
                // clang-format off
                triangles.push_back({vert_pos_indices.at(0),
                                     vert_pos_indices.at(i - 1),
                                     vert_pos_indices.at(i),
                                     vert_texcoord_indices.at(0),
                                     vert_texcoord_indices.at(i - 1),
                                     vert_texcoord_indices.at(i),
                                     vert_normal_indices.at(0),
                                     vert_normal_indices.at(i - 1),
                                     vert_normal_indices.at(i)});
                // clang-format on
            }
        }
        // ignore other parametric line stuff
    }

    // generate triangles
    std::vector<Triangle> result;
    result.reserve(triangles.size());

    for (auto& indices : triangles) {
        // -1 everywhere since obj files are indexed from 1

        auto vert1 = Vertex(Math::Vector(vertex_positions[indices[0] - 1]));
        auto vert2 = Vertex(Math::Vector(vertex_positions[indices[1] - 1]));
        auto vert3 = Vertex(Math::Vector(vertex_positions[indices[2] - 1]));

        if (indices[3] != (size_t)-1) { // has texcoord
            vert1.u = vertex_texcoords[indices[3] - 1].first;
            vert1.v = vertex_texcoords[indices[3] - 1].second;
            vert2.u = vertex_texcoords[indices[4] - 1].first;
            vert2.v = vertex_texcoords[indices[4] - 1].second;
            vert3.u = vertex_texcoords[indices[5] - 1].first;
            vert3.v = vertex_texcoords[indices[5] - 1].second;
        }
        else {
            vert1.u = 0;
            vert1.v = 0;
            vert2.u = 0;
            vert2.v = 1;
            vert3.u = 1;
            vert3.v = 1;
        }

        if (indices[6] != (size_t)-1) { // has normal
            vert1.normal = Math::Vector(vertex_normals[indices[6] - 1]);
            vert2.normal = Math::Vector(vertex_normals[indices[7] - 1]);
            vert3.normal = Math::Vector(vertex_normals[indices[8] - 1]);
        }
        else {
            // calculate a normal
            // TODO: use actual normal generating algo instead of calculating the triangles plane

            auto edge1  = vert3.pos - vert1.pos;
            auto edge2  = vert3.pos - vert2.pos;
            auto normal = edge1.cross(edge2).normalized();

            vert1.normal = Math::Vector(normal);
            vert2.normal = Math::Vector(normal);
            vert3.normal = Math::Vector(normal);
        }

        result.push_back(Triangle(std::move(vert1), std::move(vert2), std::move(vert3)));
    }

    return result;
}

} // namespace PT