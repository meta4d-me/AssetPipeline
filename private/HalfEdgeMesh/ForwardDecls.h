#pragma once

#include "Math/Vector.hpp"
#include "Scene/ObjectID.h"

#include <functional>
#include <list>

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

/*
	Some containers need to know how to compare two iterators (std::map)
	Here we just say that one iterator comes before another if the address of the
	object it points to is smaller. (You should not have to worry about this!)
*/
#define COMPARE_BY_ADDRESS(Ref) \
	inline bool operator<(Ref const& i, Ref const& j) \
	{ \
		return &*i < &*j; \
	}

COMPARE_BY_ADDRESS(HalfEdgeRef);
COMPARE_BY_ADDRESS(VertexRef);
COMPARE_BY_ADDRESS(EdgeRef);
COMPARE_BY_ADDRESS(FaceRef);
COMPARE_BY_ADDRESS(HalfEdgeCRef);
COMPARE_BY_ADDRESS(VertexCRef);
COMPARE_BY_ADDRESS(EdgeCRef);
COMPARE_BY_ADDRESS(FaceCRef);
#undef COMPARE_BY_ADDRESS

}

/*
	Some containers need to know how to hash references (std::unordered_map).
	Here we simply hash the address of the element, since these are stable.
*/
namespace std
{

#define HASH_BY_ADDRESS(T) \
	template<> struct hash<T> \
	{ \
		uint64_t operator()(T const& key) const \
		{ \
			static const std::hash<decltype(&*key)> h; \
			return h(&*key); \
		} \
	}

HASH_BY_ADDRESS(cd::hem::VertexRef);
HASH_BY_ADDRESS(cd::hem::EdgeRef);
HASH_BY_ADDRESS(cd::hem::FaceRef);
HASH_BY_ADDRESS(cd::hem::HalfEdgeRef);
HASH_BY_ADDRESS(cd::hem::VertexCRef);
HASH_BY_ADDRESS(cd::hem::EdgeCRef);
HASH_BY_ADDRESS(cd::hem::FaceCRef);
HASH_BY_ADDRESS(cd::hem::HalfEdgeCRef);
#undef HASH_BY_ADDRESS

} // namespace std