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

//-----------------------------------------------------------------------------
// Macro Definitions
//-----------------------------------------------------------------------------

#define N		100000
#define	MIN		1
#define	MAX		13
#define RESULT	833197		// ���̌v�Z�͂��̒l�ɂȂ�܂�

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

				// �A�N�Z�X�����l���ł���܂Ńu���b�N
				g_Mutex.lock();
				g_Parameters.push_back(Param);
				g_Mutex.unlock();
			}

			::_endthreadex(0);
			return 0;
		}

		/**************************************************************************//**
			@brief		��Ƃ��s���܂��B
			@return		��Ƃ̌���
			@todo		���̊֐����������A�������ʂ���荂���ɕԂ���悤�ɂ��Ă��������B
		*//***************************************************************************/
#if 1
		int DoWork()
		{
			int sum = 0;

			// �p�����[�^�����J�n
			auto Thread = (HANDLE)::_beginthreadex(NULL, 0, ex02_MultiThread::chapter1::GenParameter, NULL, 0, NULL);
			if (Thread == 0 || reinterpret_cast<long>(Thread) == -1L)
			{
				// �X���b�h�N�����s
				return -1;
			}

			// �����҂��Ȃ��ƃp�����[�^���K�؂ɐݒ肳��Ă��Ȃ�
			// Sleep()��15ms�P�ʂł����ҋ@���Ԃ����߂��Ȃ�
			// 15ms������΂�����x�̓p�����[�^�̐������I����Ă���͂�(�S�ďI����ĂȂ��Ă�����)
			::Sleep(15);

			// Tarai�v�Z
			for (int i = 0; i < N; ++i)
			{
				// �A�N�Z�X�����l���ł���܂Ńu���b�N
				g_Mutex.lock();
				auto Param = g_Parameters.front();	// �擪���폜�������̂ŃR�s�[
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