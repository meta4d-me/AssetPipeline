#pragma once

#include "Base/Endian.h"
#include "Base/EnumOptions.hpp"
#include "Base/Template.h"
#include "Producers/CDProducer/CDProducerOptions.h"

#include <string>

namespace cd
{

class SceneDatabase;

}

namespace cdtools
{

class CDProducerImpl final
{
public:
	CDProducerImpl() = delete;
	explicit CDProducerImpl(std::string filePath) : m_filePath(cd::MoveTemp(filePath)) {}
	CDProducerImpl(const CDProducerImpl&) = delete;
	CDProducerImpl& operator=(const CDProducerImpl&) = delete;
	CDProducerImpl(CDProducerImpl&&) = delete;
	CDProducerImpl& operator=(CDProducerImpl&&) = delete;
	~CDProducerImpl() = default;
	void Execute(cd::SceneDatabase* pSceneDatabase);

	cd::EnumOptions<CDProducerOptions>& GetOptions() { return m_options; }
	const cd::EnumOptions<CDProducerOptions>& GetOptions() const { return m_options; }
	bool IsOptionEnabled(CDProducerOptions option) const { return m_options.IsEnabled(option); }

private:
	std::string m_filePath;
	cd::EnumOptions<CDProducerOptions> m_options;
};

}