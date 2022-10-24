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
#include "ThreadSafeContainer.h"

//-----------------------------------------------------------------------------
// Macro Definitions
//-----------------------------------------------------------------------------

#define N		100000
#define	MIN		1
#define	MAX		13
#define RESULT	833197		// この計算はこの値になります

#define NUM_THREAD 2

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
		ThreadSafeContainer<Parameter>	g_Parameter;

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
			}

			::_endthreadex(0);
			return 0;
		}

		unsigned WINAPI CalcTarai(LPVOID pArgs)
		{
			// ループ範囲確定
			int* pThreadID = reinterpret_cast<int*>(pArgs);
			const int BasePos = N / NUM_THREAD * *pThreadID;
			const int LoopCount = N / NUM_THREAD + BasePos;

			// 計算
			int sum = 0;
			for (int i = BasePos; i < LoopCount; ++i)
			{
				if (g_Parameter.GetSize() <= 0)
				{
					
					continue;
				}

				auto Param = g_Parameter.Pop();
				sum += Tarai(Param.x, Param.y, Param.z);
			}

			_endthreadex(sum);
			return sum;
		}

		/**************************************************************************//**
			@brief		作業を行います。
			@return		作業の結果
			@todo		この関数を改造し、同じ結果をより高速に返せるようにしてください。
		*//***************************************************************************/
#if 1
		int DoWork()
		{
			// +1はパラメータ生成用スレッド
			HANDLE ThreadHandles[NUM_THREAD + 1];

			// パラメータ生成開始
			ThreadHandles[NUM_THREAD] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::GenParameter, NULL, 0, NULL);
			if (ThreadHandles == 0 || reinterpret_cast<long>(ThreadHandles) == -1L)
			{
				// スレッド起動失敗
				return -1;
			}

			// 少し待たないとパラメータが適切に設定されていない
			// Sleep()は15ms単位でしか待機時間を決められない
			// 15msもあればある程度はパラメータの生成も終わっているはず(全て終わってなくてもいい)
			::Sleep(15);

			// 計算開始
			int ThreadID[NUM_THREAD] = { 0, 1 };
			for (int i = 0; i < NUM_THREAD; ++i)
			{
				ThreadHandles[i] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::CalcTarai, &ThreadID[i], 0, NULL);
				if (ThreadHandles == 0 || reinterpret_cast<long>(ThreadHandles) == -1L)
				{
					// スレッド起動失敗
					return -1;
				}
			}

			::WaitForMultipleObjects(NUM_THREAD + 1, ThreadHandles, TRUE, INFINITE);
			int sum = 0;
			for (int i = 0; i < NUM_THREAD + 1; ++i)
			{
				DWORD ExitCode = 0;
				if (GetExitCodeThread(ThreadHandles[i], &ExitCode))
				{
					sum += static_cast<int>(ExitCode);
				}

				::CloseHandle(ThreadHandles[i]);
			}

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