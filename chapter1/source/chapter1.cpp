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

#define NUM_CALC_THREAD		 2				// tarai()計算スレッドの個数
#define NUM_GEN_PARAM_THREAD 1				// パラメータ生成スレッドの個数(rand()がスレッドセーフじゃないので1固定)
#define NUM_THREAD			 (NUM_CALC_THREAD + NUM_GEN_PARAM_THREAD)

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
			for (int i = 0; i < N / NUM_GEN_PARAM_THREAD; ++i)
			{
				Parameter Param;

				Param.x = Random(MIN, MAX);
				Param.y = Random(MIN, MAX);
				Param.z = Random(MIN, MAX);

				g_Parameter.Push(Param);
			}

			::_endthreadex(0);
			return 0;
		}

		/**************************************************************************//**
			@brief		計算を行います
			@return		Tarai関数で計算した結果
		*//***************************************************************************/
		unsigned WINAPI CalcTarai(LPVOID pArgs)
		{
			// 計算
			int sum = 0;
			for (int i = 0; i < N / NUM_CALC_THREAD; ++i)
			{
				// 要素が無ければカウントを進めずにループ
				if (g_Parameter.GetSize() <= 0)
				{
					--i;
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
			HANDLE ThreadHandles[NUM_THREAD];

			// パラメータ生成開始
			for (int i = 0; i < NUM_GEN_PARAM_THREAD; ++i)
			{
				ThreadHandles[NUM_CALC_THREAD + i] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::GenParameter, NULL, 0, NULL);
				if (ThreadHandles[NUM_CALC_THREAD + i] == 0 || reinterpret_cast<long>(ThreadHandles[NUM_CALC_THREAD + i]) == -1L)
				{
					// スレッド起動失敗
					return -1;
				}
			}

			// 計算開始
			for (int i = 0; i < NUM_CALC_THREAD; ++i)
			{
				ThreadHandles[i] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::CalcTarai, NULL, 0, NULL);
				if (ThreadHandles[i] == 0 || reinterpret_cast<long>(ThreadHandles[i]) == -1L)
				{
					// スレッド起動失敗
					return -1;
				}
			}

			// 全ての処理が終わるまで待機
			::WaitForMultipleObjects(NUM_THREAD, ThreadHandles, TRUE, INFINITE);
			int sum = 0;
			for (int i = 0; i < NUM_CALC_THREAD; ++i)
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