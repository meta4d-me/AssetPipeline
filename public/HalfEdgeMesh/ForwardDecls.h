#pragma once

#include "Math/Vector.hpp"
#include "Scene/Types.h"

#include <cassert>
#include <functional>
#include <list>
#include <optional>
#include <vector>

namespace cd::hem
{

class Vertex;
class Edge;
class HalfEdge;
class Face;

using VertexRef = std::list<Vertex>::iterator;
using VertexCRef = std::list<Vertex>::const_iterator;
using EdgeRef = std::list<Edge>::iterator;
using EdgeCRef = std::list<Edge>::const_iterator;
using HalfEdgeRef = std::list<HalfEdge>::iterator;
using HalfEdgeCRef = std::list<HalfEdge>::const_iterator;
using FaceRef = std::list<Face>::iterator;
using FaceCRef = std::list<Face>::const_iterator;

}