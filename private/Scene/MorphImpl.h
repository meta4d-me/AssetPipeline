#pragma once

#include "Base/Template.h"
#include "IO/InputArchive.hpp"
#include "IO/OutputArchive.hpp"
#include "Scene/ObjectID.h"

#include <array>
#include <string>
#include <vector>

namespace cd
{

class MorphImpl final
{
public:
	MorphImpl() = delete;

	template<bool SwapBytesOrder>
	explicit MorphImpl(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		*this << inputArchive;
	}

	explicit MorphImpl(uint32_t vertexCount);
	explicit MorphImpl(MorphID id, MeshID sourceMeshID, std::string name, uint32_t vertexCount);

	MorphImpl(const MorphImpl&) = default;
	MorphImpl& operator=(const MorphImpl&) = default;
	MorphImpl(MorphImpl&&) = default;
	MorphImpl& operator=(MorphImpl&&) = default;
	~MorphImpl() = default;

	void Init(uint32_t vertexCount);
	void Init(MorphID id, std::string name, uint32_t vertexCount);
	
	IMPLEMENT_ID_APIS(MorphID, m_id);
	IMPLEMENT_NAME_APIS(m_name);

	MeshID GetSourceMeshID() const { return m_sourceMeshID; }

	void SetWeight(float weight) { m_weight = weight; }
	float& GetWeight() { return m_weight; } 
	float GetWeight() const { return m_weight; }

	uint32_t GetVertexCount() const { return m_vertexCount; }

	IMPLEMENT_VECTOR_DATA_APIS(VertexSourceID, m_vertexSourceIDs);
	IMPLEMENT_VECTOR_DATA_APIS(VertexPosition, m_vertexPositions);
	IMPLEMENT_VECTOR_DATA_APIS(VertexNormal, m_vertexNormals);
	IMPLEMENT_VECTOR_DATA_APIS(VertexTangent, m_vertexTangents);
	IMPLEMENT_VECTOR_DATA_APIS(VertexBiTangent, m_vertexBiTangents);

	template<bool SwapBytesOrder>
	MorphImpl& operator<<(TInputArchive<SwapBytesOrder>& inputArchive)
	{
		std::string name;
		uint32_t id;
		uint32_t vertexCount;
		inputArchive >> name >> id >> vertexCount;

		Init(MorphID(id), MoveTemp(name), vertexCount);
		inputArchive.ImportBuffer(GetVertexPositions().data());
		inputArchive.ImportBuffer(GetVertexNormals().data());
		inputArchive.ImportBuffer(GetVertexTangents().data());
		inputArchive.ImportBuffer(GetVertexBiTangents().data());

		return *this;
	}

	template<bool SwapBytesOrder>
	const MorphImpl& operator>>(TOutputArchive<SwapBytesOrder>& outputArchive) const
	{
		outputArchive << GetName() << GetID().Data() << GetVertexCount();
		outputArchive.ExportBuffer(GetVertexPositions().data(), GetVertexPositions().size());
		outputArchive.ExportBuffer(GetVertexNormals().data(), GetVertexNormals().size());
		outputArchive.ExportBuffer(GetVertexTangents().data(), GetVertexTangents().size());
		outputArchive.ExportBuffer(GetVertexBiTangents().data(), GetVertexBiTangents().size());

		return *this;
	}

private:
	std::string					m_name;
	MorphID						m_id;
	MeshID						m_sourceMeshID;
	float						m_weight;

	uint32_t					m_vertexCount;
	std::vector<VertexID>		m_vertexSourceIDs;

	// vertex geometry data
	std::vector<Point>			m_vertexPositions;
	std::vector<Direction>		m_vertexNormals;
	std::vector<Direction>		m_vertexTangents;
	std::vector<Direction>		m_vertexBiTangents;
};

}