#include "TerrainProducer.h"
#include "Scene/SceneDatabase.h"

namespace cdtools
{

void TerrainProducer::Execute(SceneDatabase* pSceneDatabase)
{
	Mesh mesh(MeshID(0U), "TerrainMesh", 4U, 2U);
	mesh.SetVertexColorSetCount(0);
	mesh.SetVertexUVSetCount(0);

	mesh.SetPolygon(0, VertexID(0U), VertexID(1U), VertexID(2U));
	mesh.SetPolygon(1, VertexID(0U), VertexID(2U), VertexID(3U));

	mesh.SetVertexPosition(0, Point(-10.0f, 0.0f, 10.0f));
	mesh.SetVertexPosition(1, Point(10.0f, 0.0f, 10.0f));
	mesh.SetVertexPosition(2, Point(10.0f, 0.0f, -10.0f));
	mesh.SetVertexPosition(3, Point(-10.0f, 0.0f, -10.0f));

	mesh.SetVertexNormal(0, Direction(0.0f, 1.0f, 0.0f));
	mesh.SetVertexNormal(1, Direction(0.0f, 1.0f, 0.0f));
	mesh.SetVertexNormal(2, Direction(0.0f, 1.0f, 0.0f));
	mesh.SetVertexNormal(3, Direction(0.0f, 1.0f, 0.0f));

	pSceneDatabase->AddMesh(std::move(mesh));
}

}