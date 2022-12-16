#pragma once

#include "Consumer/BaseConsumer.h"

namespace cdtools
{

enum class ExportMode
{
	XmlBinary = 0,
	PureBinary,
};

class CatDogConsumer final : public BaseConsumer
{
public:
	using BaseConsumer::BaseConsumer;
	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override;

	ExportMode GetExportMode() const { return m_exportMode; }
	void SetExportMode(ExportMode mode) { m_exportMode = mode; }

private:
	void ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, std::endian targetEndian = std::endian::native);

	// WIP : 1.Xml/Json in a same way to export. 2.endianess
	void ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase);

private:
	ExportMode m_exportMode;
};

}