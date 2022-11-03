#include "CatDogConsumer.h"

#include "Scene/SceneDatabase.h"

#include <fstream>

namespace cdtools
{

void CatDogConsumer::Execute(const SceneDatabase* pSceneDatabase)
{
	std::ofstream fout(m_filePath, std::ios::out | std::ios::binary);
	pSceneDatabase->ExportBinary(fout);
	fout.close();
}

}