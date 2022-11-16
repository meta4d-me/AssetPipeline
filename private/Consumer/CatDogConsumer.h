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
	virtual void Execute(const SceneDatabase* pSceneDatabase) override;

	ExportMode GetExportMode() const { return m_exportMode; }
	void SetExportMode(ExportMode mode) { m_exportMode = mode; }

private:
	void ExportPureBinary(const SceneDatabase* pSceneDatabase);
	void ExportXmlBinary(const SceneDatabase* pSceneDatabase);

private:
	ExportMode m_exportMode;
};

}