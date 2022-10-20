#include "Processor.h"

#include "Consumer/IConsumer.h"
#include "Producer/IProducer.h"
#include "Scene/SceneDatabase.h"

#include <cassert>

namespace cdtools
{

Processor::Processor(IProducer* pProducer, IConsumer* pConsumer) :
	m_pProducer(pProducer),
	m_pConsumer(pConsumer),
	m_pSceneDatabase(std::make_unique<SceneDatabase>())
{
	assert(pProducer && "pProducer is invalid.");
	assert(pConsumer && "pConsumer is invalid.");
}

Processor::~Processor()
{
}

void Processor::Run()
{
	m_pProducer->Execute(m_pSceneDatabase.get());
	m_pConsumer->Execute(m_pSceneDatabase.get());
}

}