#include "ProcessorImpl.h"

#include "Framework/IConsumer.h"
#include "Framework/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>

namespace cdtools
{

ProcessorImpl::ProcessorImpl(IProducer* pProducer, IConsumer* pConsumer, cd::SceneDatabase* pHostSceneDatabase) :
	m_pProducer(pProducer),
	m_pConsumer(pConsumer)
{
	assert(pProducer && "pProducer is invalid.");
	assert(pConsumer && "pConsumer is invalid.");

	if (nullptr == pHostSceneDatabase)
	{
		// Processor will create a SceneDatabase managed by itself.
		m_pLocalSceneDatabase = std::make_unique<cd::SceneDatabase>();
		m_pCurrentSceneDatabase = m_pLocalSceneDatabase.get();
	}
	else
	{
		// The SceneDatabase is from outside resource.
		m_pCurrentSceneDatabase = pHostSceneDatabase;
	}
}

ProcessorImpl::~ProcessorImpl()
{
}

void ProcessorImpl::Run()
{
	m_pProducer->Execute(m_pCurrentSceneDatabase);
	m_pConsumer->Execute(m_pCurrentSceneDatabase);
}

}