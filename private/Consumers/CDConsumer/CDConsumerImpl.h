#pragma once

#include "Base/Endian.h"
#include "Base/EnumOptions.hpp"
#include "Base/Template.h"
#include "Consumers/CDConsumer/CDConsumerOptions.h"
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
	explicit CDConsumerImpl(std::string filePath) : m_exportMode(ExportMode::PureBinary), m_filePath(cd::MoveTemp(filePath)) {}
	CDConsumerImpl(const CDConsumerImpl&) = delete;
	CDConsumerImpl& operator=(const CDConsumerImpl&) = delete;
	CDConsumerImpl(CDConsumerImpl&&) = delete;
	CDConsumerImpl& operator=(CDConsumerImpl&&) = delete;
	~CDConsumerImpl() = default;
	void Execute(const cd::SceneDatabase* pSceneDatabase);

	ExportMode GetExportMode() const { return m_exportMode; }
	void SetExportMode(ExportMode mode) { m_exportMode = mode; }

	cd::EndianType GetTargetEndian() const { return m_targetEndian; }
	void SetTargetEndian(cd::EndianType endian) { m_targetEndian = endian; }

	void ExportPureBinary(const cd::SceneDatabase* pSceneDatabase);
	void ExportXmlBinary(const cd::SceneDatabase* pSceneDatabase);

	cd::EnumOptions<CDConsumerOptions>& GetOptions() { return m_options; }
	const cd::EnumOptions<CDConsumerOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(CDConsumerOptions option) const { return m_options.IsEnabled(option); }

private:
	ExportMode m_exportMode;
	cd::EnumOptions<CDConsumerOptions> m_options;

	cd::EndianType m_targetEndian = cd::Endian::GetNative();
	std::string m_filePath;
};

}