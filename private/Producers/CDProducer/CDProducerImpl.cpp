#include "CDProducerImpl.h"

#include "IO/InputArchive.hpp"
#include "Scene/SceneDatabase.h"

#include <fstream>

namespace cdtools
{

void CDProducerImpl::Execute(cd::SceneDatabase* pSceneDatabase)
{
	std::ifstream fin(m_filePath, std::ios::in | std::ios::binary);
	
	uint8_t fileEndian;
	fin.read(reinterpret_cast<char*>(&fileEndian), sizeof(uint8_t));

	if (fileEndian != cd::Endian::GetNative())
	{
		cd::InputArchiveSwapBytes inputArchive(&fin);
		*pSceneDatabase << inputArchive;

		// Warnings!! You can get better performance by using correct endian instead.
		// If you don't care about performance in your case, it is OK to swap bytes.
		// assert(fileEndian == platformEndian && "Please input asset file with same endian.");
	}
	else
	{
		cd::InputArchive inputArchive(&fin);
		*pSceneDatabase << inputArchive;
	}
	
	fin.close();
}

}