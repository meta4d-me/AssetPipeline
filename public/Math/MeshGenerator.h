#pragma once

#include "Base/Export.h"

#include <inttypes.h>
#include <optional>
#include <vector>

namespace cd
{

template<typename T, std::size_t N>
class TBox;

class Mesh;

template<typename T>
class TPlane;

template<typename T>
class TSphere;

class VertexFormat;

enum class CullFace
{
	ClockWise,
	CounterClockWise
};

// MeshGenerator helps to generate vertex buffer and index buffer from an implicit math class.
// Default : Left hand axis and counter clock wise
class CORE_API MeshGenerator final
{
public:
	// Utility class doesn't allow to construct.
	MeshGenerator() = delete;
	MeshGenerator(const MeshGenerator&) = delete;
	MeshGenerator& operator=(const MeshGenerator&) = delete;
	MeshGenerator(MeshGenerator&&) = delete;
	MeshGenerator& operator=(MeshGenerator&&) = delete;
	~MeshGenerator() = delete;

	static std::optional<Mesh> Generate(const TBox<float, 3>& box, const VertexFormat& vertexFormat, bool useCounterWiseForFrontFace = true);
	static std::optional<Mesh> Generate(const TSphere<float>& sphere, uint32_t numStacks, uint32_t numSlices, const VertexFormat& vertexFormat);
};

}