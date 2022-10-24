//!	@file		chapter1.cpp
//!	@brief		マルチスレッド課題　第1章
//! @note		Copyright (c) FromSoftware, Inc.

//-----------------------------------------------------------------------------
// File Dependencies
//-----------------------------------------------------------------------------

#include <stdio.h>
#include <functions.h>
#include <crtdbg.h>

#include <process.h>
#include <mutex>
#include <list>

//-----------------------------------------------------------------------------
// Macro Definitions
//-----------------------------------------------------------------------------

#define N		100000
#define	MIN		1
#define	MAX		13
#define RESULT	833197		// この計算はこの値になります

//-----------------------------------------------------------------------------
// Using Namespace
//-----------------------------------------------------------------------------
using namespace ex02_MultiThread::common;

//-----------------------------------------------------------------------------
// Namespace
//-----------------------------------------------------------------------------
namespace ex02_MultiThread
{
	namespace chapter1
	{
		//-----------------------------------------------------------------------------
		// Structures
		//-----------------------------------------------------------------------------
		struct Parameter
		{
			int x, y, z;
		};

		//-----------------------------------------------------------------------------
		// Global Variables
		//-----------------------------------------------------------------------------
		std::list<Parameter>	g_Parameters;
		std::mutex				g_Mutex;

		//-----------------------------------------------------------------------------
		// Implements
		//-----------------------------------------------------------------------------
		/**************************************************************************//**
			@brief		パラメータ生成を行います。
			@return		常に0
		*//***************************************************************************/
		unsigned WINAPI GenParameter(LPVOID pArgs)
		{
			// パラメータ生成
			for (int i = 0; i < N; ++i)
			{
				Parameter Param;

				Param.x = Random(MIN, MAX);
				Param.y = Random(MIN, MAX);
				Param.z = Random(MIN, MAX);

				// アクセス権を獲得できるまでブロック
				g_Mutex.lock();
				g_Parameters.push_back(Param);
				g_Mutex.unlock();
			}

			::_endthreadex(0);
			return 0;
		}

		/**************************************************************************//**
			@brief		作業を行います。
			@return		作業の結果
			@todo		この関数を改造し、同じ結果をより高速に返せるようにしてください。
		*//***************************************************************************/
#if 1
		int DoWork()
		{
			int sum = 0;

			// パラメータ生成開始
			auto Thread = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::GenParameter, NULL, 0, NULL);
			if (Thread == 0 || reinterpret_cast<long>(Thread) == -1L)
			{
				// スレッド起動失敗
				return -1;
			}

			// 少し待たないとパラメータが適切に設定されていない
			// Sleep()は15ms単位でしか待機時間を決められない
			// 15msもあればある程度はパラメータの生成も終わっているはず(全て終わってなくてもいい)
			::Sleep(15);

			// Tarai計算
			for (int i = 0; i < N; ++i)
			{
				// アクセス権を獲得できるまでブロック
				g_Mutex.lock();
				auto Param = g_Parameters.front();	// 先頭を削除したいのでコピー
				g_Parameters.pop_front();
				g_Mutex.unlock();

				sum += Tarai(Param.x, Param.y, Param.z);
			}

			::WaitForSingleObject(Thread, INFINITE);
			::CloseHandle(Thread);

			return sum;
		}
#else
		int DoWork()
		{
			int sum = 0;

			for (int i = 0; i < N; i++)
			{
				int x = Random(MIN, MAX);
				int y = Random(MIN, MAX);
				int z = Random(MIN, MAX);

				sum += Tarai(x, y, z);
			}

			return sum;
		}
#endif
	}
}

/**************************************************************************//**
	@brief		プログラム開始関数です。
*//***************************************************************************/
void main()
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	
	unsigned __int64 startTime = GetCPUCounter();
	int result = ex02_MultiThread::chapter1::DoWork();
	unsigned __int64 endTime = GetCPUCounter();

	printf( "time : %f\n", (double)(endTime - startTime) / (double)GetCPUFrequency() );
	printf( "result : %d\n", result );
}