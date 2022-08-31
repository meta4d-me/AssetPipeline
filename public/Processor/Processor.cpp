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
	m_pSceneDatabase(new SceneDatabase)
{
	assert(pProducer && "pProducer is invalid.");
	assert(pConsumer && "pConsumer is invalid.");
}

Processor::~Processor()
{
	delete m_pSceneDatabase;
}

void Processor::Run()
{
	m_pProducer->Execute(m_pSceneDatabase);
	m_pConsumer->Execute(m_pSceneDatabase);
}

}