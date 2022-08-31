#include "FbxProducer.h"

namespace cdtools
{

FbxProducer::FbxProducer(std::string filePath) :
	m_filePath(std::move(filePath))
{
}

void FbxProducer::Execute(SceneDatabase* pSceneDatabase)
{
	pSceneDatabase;
}

}