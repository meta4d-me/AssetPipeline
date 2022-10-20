#pragma once

#include "Consumer/BaseConsumer.h"

namespace cdtools
{

class PhysxConsumer final : public BaseConsumer
{
public:
	using BaseConsumer::BaseConsumer;
	virtual void Execute(const SceneDatabase* pSceneDatabase) override;

	/// <summary>
	/// Generate convex or not.
	/// </summary>
	void ActivateConvexService() { m_bWantConvex = true; }
	bool IsConvexServiceActive() const { return m_bWantConvex; }

private:
	bool m_bWantConvex = false;
};

}