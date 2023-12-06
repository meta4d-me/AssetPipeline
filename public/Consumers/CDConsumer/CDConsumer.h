#pragma once

#include "Base/Endian.h"
#include "Consumers/CDConsumer/CDConsumerOptions.h"
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

	cd::EndianType GetTargetEndian() const;
	void SetTargetEndian(cd::EndianType endian);

	void EnableOption(CDConsumerOptions option);
	void DisableOption(CDConsumerOptions option);
	bool IsOptionEnabled(CDConsumerOptions option) const;

private:
	void ExportPureBinary(const cd::SceneDatabase* pSceneDatabase);
	void ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase);

private:
	CDConsumerImpl* m_pCDConsumerImpl;
};

}