/**
* @brief	スレッドセーフなコンテナクラス 定義
*/

#pragma once

//===== インクルード =====
#include <mutex>
#include <list>

//===== クラス定義 =====
/**
* @brief	スレッドセーフなコンテナクラス
*/
template<class T>
class ThreadSafeContainer
{
	//=== メンバ変数
private:

	//! ミューテックス
	std::mutex		m_Mutex;

	//! リスト
	std::list<T>	m_List;

	//=== メンバ関数
public:

	/**
	* @brief	デフォルトコンストラクタ
	*/
	ThreadSafeContainer(void);

	/**
	* @brief	デストラクタ
	*/
	virtual ~ThreadSafeContainer(void);

	/**
	* @brief	コピーコンストラクタ
	*/
	ThreadSafeContainer(const ThreadSafeContainer<T>&) = delete;

	/**
	* @brief	ムーブコンストラクタ
	*/
	ThreadSafeContainer(ThreadSafeContainer<T>&&) = delete;

	/**
	* @brief	プッシュ
	*/
	void Push(const T& Param);

	/**
	* @brief	ポップ
	*/
	T Pop(void);

	/**
	* @brief	要素数取得
	*/
	size_t GetSize(void);
};

//===== インクルード =====
#include "ThreadSafeContainer.inl"