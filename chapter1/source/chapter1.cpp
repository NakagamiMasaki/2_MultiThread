//!	@file		chapter1.cpp
//!	@brief		�}���`�X���b�h�ۑ�@��1��
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
#define RESULT	833197		// ���̌v�Z�͂��̒l�ɂȂ�܂�

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
			@brief		�p�����[�^�������s���܂��B
			@return		���0
		*//***************************************************************************/
		unsigned WINAPI GenParameter(LPVOID pArgs)
		{
			// �p�����[�^����
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
			// ���[�v�͈͊m��
			int* pThreadID = reinterpret_cast<int*>(pArgs);
			const int BasePos = N / NUM_THREAD * *pThreadID;
			const int LoopCount = N / NUM_THREAD + BasePos;

			// �v�Z
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
			@brief		��Ƃ��s���܂��B
			@return		��Ƃ̌���
			@todo		���̊֐����������A�������ʂ���荂���ɕԂ���悤�ɂ��Ă��������B
		*//***************************************************************************/
#if 1
		int DoWork()
		{
			// +1�̓p�����[�^�����p�X���b�h
			HANDLE ThreadHandles[NUM_THREAD + 1];

			// �p�����[�^�����J�n
			ThreadHandles[NUM_THREAD] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::GenParameter, NULL, 0, NULL);
			if (ThreadHandles == 0 || reinterpret_cast<long>(ThreadHandles) == -1L)
			{
				// �X���b�h�N�����s
				return -1;
			}

			// �����҂��Ȃ��ƃp�����[�^���K�؂ɐݒ肳��Ă��Ȃ�
			// Sleep()��15ms�P�ʂł����ҋ@���Ԃ����߂��Ȃ�
			// 15ms������΂�����x�̓p�����[�^�̐������I����Ă���͂�(�S�ďI����ĂȂ��Ă�����)
			::Sleep(15);

			// �v�Z�J�n
			int ThreadID[NUM_THREAD] = { 0, 1 };
			for (int i = 0; i < NUM_THREAD; ++i)
			{
				ThreadHandles[i] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::CalcTarai, &ThreadID[i], 0, NULL);
				if (ThreadHandles == 0 || reinterpret_cast<long>(ThreadHandles) == -1L)
				{
					// �X���b�h�N�����s
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
	@brief		�v���O�����J�n�֐��ł��B
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