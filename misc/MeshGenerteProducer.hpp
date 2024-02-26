#pragma once

#include "Framework/IProducer.h"
#include "Math/Box.hpp"
#include "Math/Plane.hpp"
#include "Math/Sphere.hpp"
#include "Math/MeshGenerator.h"
#include "Scene/SceneDatabase.h"
#include "Scene/VertexFormat.h"

#include <string>

namespace cdtools
{

class MeshGenerteProducer final : public IProducer
{
public:
	MeshGenerteProducer() = delete;
	explicit MeshGenerteProducer(const char* pFilePath) : m_filePath(pFilePath) {}
	MeshGenerteProducer(const MeshGenerteProducer&) = delete;
	MeshGenerteProducer& operator=(const MeshGenerteProducer&) = delete;
	MeshGenerteProducer(MeshGenerteProducer&&) = delete;
	MeshGenerteProducer& operator=(MeshGenerteProducer&&) = delete;
	virtual ~MeshGenerteProducer() {}

	virtual void Execute(cd::SceneDatabase* pSceneDatabase) override
	{
		pSceneDatabase->SetName("MeshGenerator");

		cd::Sphere sphere(cd::Point(0.0f, 0.0f, 0.0f), 2.0f);
		//cd::Box box(cd::Vec3f(-1.0f, -1.0f, -1.0f), cd::Vec3f(1.0f, 1.0f, 1.0f));
		cd::VertexFormat vertexFormat;
		vertexFormat.AddVertexAttributeLayout(cd::VertexAttributeType::Position, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddVertexAttributeLayout(cd::VertexAttributeType::Normal, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddVertexAttributeLayout(cd::VertexAttributeType::UV, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddVertexAttributeLayout(cd::VertexAttributeType::Tangent, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		vertexFormat.AddVertexAttributeLayout(cd::VertexAttributeType::Bitangent, cd::GetAttributeValueType<cd::Point::ValueType>(), 3);
		std::optional<cd::Mesh> optMesh = cd::MeshGenerator::Generate(sphere, 20, 20, vertexFormat);
		if (!optMesh.has_value())
		{
			return;
		}

		cd::Mesh& mesh = optMesh.value();
		mesh.SetID(cd::MeshID(0U));
		pSceneDatabase->SetMeshCount(1U);
		pSceneDatabase->AddMesh(MoveTemp(mesh));
	}

private:
	std::string m_filePath;
};

}