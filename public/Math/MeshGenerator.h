#pragma once

#include "Base/Export.h"

#include <optional>
#include <vector>

namespace cd
{

template<typename T>
class TBox;

class Mesh;

template<typename T>
class TPlane;

template<typename T>
class TSphere;

class VertexFormat;

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

	static std::optional<Mesh> Generate(const TBox<float>& box, const VertexFormat& vertexFormat);
	static std::optional<Mesh> GenerateNonUnique(const TBox<float>& box, const VertexFormat& vertexFormat);
	static std::optional<Mesh> Generate(const TPlane<float>& plane, float width, float height, const VertexFormat& vertexFormat);
	static std::optional<Mesh> Generate(const TSphere<float>& sphere, const VertexFormat& vertexFormat);
};

}