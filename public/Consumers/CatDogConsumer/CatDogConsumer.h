#pragma once

#include "ExportMode.h"
#include "Framework/IConsumer.h"

#include <bit>

namespace cdtools
{

class CatDogConsumerImpl;

class TOOL_API CatDogConsumer final : public IConsumer
{
public:
	CatDogConsumer() = delete;
	explicit CatDogConsumer(const char* pFilePath);
	CatDogConsumer(const CatDogConsumer&) = delete;
	CatDogConsumer& operator=(const CatDogConsumer&) = delete;
	CatDogConsumer(CatDogConsumer&&) = delete;
	CatDogConsumer& operator=(CatDogConsumer&&) = delete;
	virtual ~CatDogConsumer();
	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override;

	ExportMode GetExportMode() const;
	void SetExportMode(ExportMode mode);

private:
	void ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, std::endian targetEndian = std::endian::native);
	void ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase);

private:
	CatDogConsumerImpl* m_pCatDogConsumerImpl;
};

}