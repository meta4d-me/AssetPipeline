#pragma once

#include <concepts>
#include <queue>

namespace cd
{

// IterablePriorityQueueProxy is a helper to iterate elements in std::priority_queue which mostly is used to debug.
// In C++, the best priority queue I think is std::multiset which supports update operation. (Just erase and insert again!)
// Emmmm, std::priority is nothing but a wrapper of std::make_heap/push_heap/pop_heap.
template<typename T>
concept use_vector_container = requires(T)
{
	{std::vector<typename T::value_type>{}} -> std::same_as<typename T::container_type>;
};

template<typename T> requires use_vector_container<T>
class IterablePriorityQueueProxy
{
public:
	IterablePriorityQueueProxy() = delete;
	IterablePriorityQueueProxy(const T& pq) : m_pPriorityQueue(&pq) {}
	IterablePriorityQueueProxy(const IterablePriorityQueueProxy&) = default;
	IterablePriorityQueueProxy& operator=(const IterablePriorityQueueProxy&) = default;
	IterablePriorityQueueProxy(IterablePriorityQueueProxy&&) = default;
	IterablePriorityQueueProxy& operator=(IterablePriorityQueueProxy&&) = default;
	~IterablePriorityQueueProxy() = default;

	auto begin() const { return &m_pPriorityQueue->top(); };
	auto end() const { return begin() + m_pPriorityQueue->size(); };

private:
	const T* m_pPriorityQueue = nullptr;
};

}