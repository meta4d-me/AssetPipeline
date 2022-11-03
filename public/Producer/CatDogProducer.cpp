#include "CatDogProducer.h"

#include "Scene/SceneDatabase.h"

#include <fstream>

namespace cdtools
{

void CatDogProducer::Execute(SceneDatabase* pSceneDatabase)
{
	std::ifstream fin(m_filePath, std::ios::in | std::ios::binary);
	pSceneDatabase->ImportBinary(fin);
	fin.close();
}

}