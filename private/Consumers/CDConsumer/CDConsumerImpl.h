#pragma once

#include "Base/Endian.h"
#include "Base/Template.h"
#include "Consumers/CDConsumer/ExportMode.h"

#include <string>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class CDConsumerImpl final
{
public:
	CDConsumerImpl() = delete;
	explicit CDConsumerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)), m_exportMode(ExportMode::PureBinary) {}
	CDConsumerImpl(const CDConsumerImpl&) = delete;
	CDConsumerImpl& operator=(const CDConsumerImpl&) = delete;
	CDConsumerImpl(CDConsumerImpl&&) = delete;
	CDConsumerImpl& operator=(CDConsumerImpl&&) = delete;
	~CDConsumerImpl() = default;
	void Execute(const cd::SceneDatabase* pSceneDatabase);

	ExportMode GetExportMode() const { return m_exportMode; }
	void SetExportMode(ExportMode mode) { m_exportMode = mode; }

	void ExportPureBinary(const cd::SceneDatabase* pSceneDatabase, cd::EndianType targetEndian = cd::Endian::GetNative());
	void ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase, cd::EndianType targetEndian = cd::Endian::GetNative());

private:
	ExportMode m_exportMode;
	std::string m_filePath;
};

}