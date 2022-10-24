/**
* @brief	スレッドセーフなコンテナクラス 実装
*/

//===== インクルード =====
#include "ThreadSafeContainer.h"

//===== クラス定義 =====

template<class T>
ThreadSafeContainer<T>::ThreadSafeContainer(void)
{

}

template<class T>
ThreadSafeContainer<T>::~ThreadSafeContainer(void)
{

}

template<class T>
void ThreadSafeContainer<T>::Push(const T& Param)
{
	std::lock_guard<std::mutex> Lock(m_Mutex);
	m_List.push_back(Param);
}

template<class T>
T ThreadSafeContainer<T>::Pop(void)
{
	std::lock_guard<std::mutex> Lock(m_Mutex);
	auto Param = m_List.front();
	m_List.pop_front();

	return Param;
}

template<class T>
size_t ThreadSafeContainer<T>::GetSize(void)
{
	return m_List.size();
}