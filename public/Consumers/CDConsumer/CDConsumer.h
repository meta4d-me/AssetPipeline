#pragma once

#include "Base/Endian.h"
#include "ExportMode.h"
#include "Framework/IConsumer.h"

namespace cdtools
{

class CDConsumerImpl;

class TOOL_API CDConsumer final : public IConsumer
{
public:
	CDConsumer() = delete;
	explicit CDConsumer(const char* pFilePath);
	CDConsumer(const CDConsumer&) = delete;
	CDConsumer& operator=(const CDConsumer&) = delete;
	CDConsumer(CDConsumer&&) = delete;
	CDConsumer& operator=(CDConsumer&&) = delete;
	virtual ~CDConsumer();
	virtual void Execute(const cd::SceneDatabase* pSceneDatabase) override;

	ExportMode GetExportMode() const;
	void SetExportMode(ExportMode mode);

private:
	void ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, cd::EndianType targetEndian = cd::Endian::GetNative());
	void ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase, cd::EndianType targetEndian = cd::Endian::GetNative());

private:
	CDConsumerImpl* m_pCDConsumerImpl;
};

}