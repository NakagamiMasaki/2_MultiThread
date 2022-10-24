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

#define NUM_CALC_THREAD		 2				// tarai()�v�Z�X���b�h�̌�
#define NUM_GEN_PARAM_THREAD 1				// �p�����[�^�����X���b�h�̌�(rand()���X���b�h�Z�[�t����Ȃ��̂�1�Œ�)
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
			@brief		�p�����[�^�������s���܂��B
			@return		���0
		*//***************************************************************************/
		unsigned WINAPI GenParameter(LPVOID pArgs)
		{
			// �p�����[�^����
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
			@brief		�v�Z���s���܂�
			@return		Tarai�֐��Ōv�Z��������
		*//***************************************************************************/
		unsigned WINAPI CalcTarai(LPVOID pArgs)
		{
			// �v�Z
			int sum = 0;
			for (int i = 0; i < N / NUM_CALC_THREAD; ++i)
			{
				// �v�f��������΃J�E���g��i�߂��Ƀ��[�v
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
			@brief		��Ƃ��s���܂��B
			@return		��Ƃ̌���
			@todo		���̊֐����������A�������ʂ���荂���ɕԂ���悤�ɂ��Ă��������B
		*//***************************************************************************/
#if 1
		int DoWork()
		{
			// +1�̓p�����[�^�����p�X���b�h
			HANDLE ThreadHandles[NUM_THREAD];

			// �p�����[�^�����J�n
			for (int i = 0; i < NUM_GEN_PARAM_THREAD; ++i)
			{
				ThreadHandles[NUM_CALC_THREAD + i] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::GenParameter, NULL, 0, NULL);
				if (ThreadHandles[NUM_CALC_THREAD + i] == 0 || reinterpret_cast<long>(ThreadHandles[NUM_CALC_THREAD + i]) == -1L)
				{
					// �X���b�h�N�����s
					return -1;
				}
			}

			// �v�Z�J�n
			for (int i = 0; i < NUM_CALC_THREAD; ++i)
			{
				ThreadHandles[i] = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::CalcTarai, NULL, 0, NULL);
				if (ThreadHandles[i] == 0 || reinterpret_cast<long>(ThreadHandles[i]) == -1L)
				{
					// �X���b�h�N�����s
					return -1;
				}
			}

			// �S�Ă̏������I���܂őҋ@
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