#include "ProcessorImpl.h"

#include "Framework/IConsumer.h"
#include "Framework/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>

namespace cdtools
{

ProcessorImpl::ProcessorImpl(IProducer* pProducer, IConsumer* pConsumer) :
	m_pProducer(pProducer),
	m_pConsumer(pConsumer),
	m_pSceneDatabase(std::make_unique<cd::SceneDatabase>())
{
	assert(pProducer && "pProducer is invalid.");
	assert(pConsumer && "pConsumer is invalid.");
}

ProcessorImpl::~ProcessorImpl()
{
}

void ProcessorImpl::Run()
{
	m_pProducer->Execute(m_pSceneDatabase.get());
	m_pConsumer->Execute(m_pSceneDatabase.get());
}

}