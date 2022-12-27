#pragma once

#include "Base/Template.h"
#include "ExportMode.h"

#include <string>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class CatDogConsumerImpl final
{
public:
	CatDogConsumerImpl() = delete;
	explicit CatDogConsumerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	CatDogConsumerImpl(const CatDogConsumerImpl&) = delete;
	CatDogConsumerImpl& operator=(const CatDogConsumerImpl&) = delete;
	CatDogConsumerImpl(CatDogConsumerImpl&&) = delete;
	CatDogConsumerImpl& operator=(CatDogConsumerImpl&&) = delete;
	~CatDogConsumerImpl() = default;
	void Execute(const cd::SceneDatabase* pSceneDatabase);

	ExportMode GetExportMode() const { return m_exportMode; }
	void SetExportMode(ExportMode mode) { m_exportMode = mode; }

	void ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, std::endian targetEndian = std::endian::native);

	// WIP : 1.Xml/Json in a same way to export. 2.endianess
	void ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase);

private:
	ExportMode m_exportMode;
	std::string m_filePath;
};

}