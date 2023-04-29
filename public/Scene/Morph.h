#pragma once

#include "Base/Export.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <vector>

namespace cd
{

class VertexFormat;
class MorphImpl;

class CORE_API Morph final
{
public:
	Morph() = delete;
	explicit Morph(InputArchive& inputArchive);
	explicit Morph(InputArchiveSwapBytes & inputArchive);
	explicit Morph(uint32_t vertexCount);
	explicit Morph(MorphID id, const char* pName, uint32_t vertexCount);
	Morph(const Morph&) = delete;
	Morph& operator=(const Morph&) = delete;
	Morph(Morph&&);
	Morph& operator=(Morph&&);
	~Morph();

	void Init(uint32_t vertexCount);
	void Init(MorphID meshID, const char* pName, uint32_t vertexCount);
	MorphID GetID() const;
	const char* GetName() const;

	void SetWeight(float weight);
	float GetWeight() const;

	uint32_t GetVertexCount() const;

	void SetVertexSourceID(uint32_t vertexIndex, uint32_t sourceID);
	VertexID GetVertexSourceID(uint32_t vertexIndex) const;
	std::vector<VertexID>& GetVertexSourceIDs();
	const std::vector<VertexID>& GetVertexSourceIDs() const;

	void SetVertexPosition(uint32_t vertexIndex, const Point& position);
	Point& GetVertexPosition(uint32_t vertexIndex);
	const Point& GetVertexPosition(uint32_t vertexIndex) const;
	std::vector<Point>& GetVertexPositions();
	const std::vector<Point>& GetVertexPositions() const;

	void SetVertexNormal(uint32_t vertexIndex, const Direction& normal);
	Direction& GetVertexNormal(uint32_t vertexIndex);
	const Direction& GetVertexNormal(uint32_t vertexIndex) const;
	std::vector<Direction>& GetVertexNormals();
	const std::vector<Direction>& GetVertexNormals() const;

	void SetVertexTangent(uint32_t vertexIndex, const Direction& tangent);
	Direction& GetVertexTangent(uint32_t vertexIndex);
	const Direction& GetVertexTangent(uint32_t vertexIndex) const;
	std::vector<Direction>& GetVertexTangents();
	const std::vector<Direction>& GetVertexTangents() const;

	void SetVertexBiTangent(uint32_t vertexIndex, const Direction& biTangent);
	Direction& GetVertexBiTangent(uint32_t vertexIndex);
	const Direction& GetVertexBiTangent(uint32_t vertexIndex) const;
	std::vector<Direction>& GetVertexBiTangents();
	const std::vector<Direction>& GetVertexBiTangents() const;

	Morph& operator<<(InputArchive& inputArchive);
	Morph& operator<<(InputArchiveSwapBytes& inputArchive);
	const Morph& operator>>(OutputArchive& outputArchive) const;
	const Morph& operator>>(OutputArchiveSwapBytes& outputArchive) const;

private:
	MorphImpl* m_pMorphImpl = nullptr;
};

}