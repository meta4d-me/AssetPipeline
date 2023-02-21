#pragma once

#include "Base/Template.h"

#include <chrono>
#include <string>

namespace cdtools
{

class PerformanceProfiler
{
public:
	PerformanceProfiler(std::string tag) :
		m_tag(cd::MoveTemp(tag))
	{
		m_startTimePoint = std::chrono::steady_clock::now();
	}

	~PerformanceProfiler()
	{
		std::chrono::steady_clock::time_point endTimePoint = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsedTime = endTimePoint - m_startTimePoint;
		printf("\n%s costs %f seconds\n", m_tag.c_str(), elapsedTime.count());
	}

private:
	std::string m_tag;
	std::chrono::steady_clock::time_point m_startTimePoint;
};

}