//!	@file		functions.h
//!	@brief		マルチスレッド課題で利用する共通関数群を定義します。
//! @note		Copyright (c) FromSoftware, Inc.

#ifndef __MultiThread_functions_h__
#define __MultiThread_functions_h__

//-----------------------------------------------------------------------------
// File Dependencies
//-----------------------------------------------------------------------------

#include <windows.h>
#include <stdlib.h>
#include <assert.h>

//-----------------------------------------------------------------------------
// Implements
//-----------------------------------------------------------------------------
namespace ex02_MultiThread
{
	namespace common
	{
		/**************************************************************************//**
			@brief		たらいまわし関数です。
			@param		[in] x		x値
			@param		[in] x		y値
			@param		[in] x		z値
			@return		関数の結果
		*//***************************************************************************/
		inline int Tarai( int x, int y, int z )
		{
			if( x <= y )
			{
				return y;
			}

			return Tarai(
						Tarai( x - 1, y, z ),
						Tarai( y - 1, z, x ),
						Tarai( z - 1, x, y )
						);
		}

		/**************************************************************************//**
			@brief		指定範囲で乱数を返します。
			@param		[in] min	乱数が取る最小値
			@param		[in] max	乱数が取る最大値
			@return		区間[min,max]の値
		*//***************************************************************************/
		inline int Random( int min, int max )
		{
			//rand()使用は意図的。課題の結果を維持するため使用を継続する
			return min + (max - min) * rand() / RAND_MAX;
		}

		/**************************************************************************//**
			@brief		CPU時間を取得します。
			@return		CPU時間
		*//***************************************************************************/
		inline unsigned __int64 GetCPUCounter()
		{
			LARGE_INTEGER result;
			BOOL ret = ::QueryPerformanceCounter( &result );
			assert( ret );
			return result.QuadPart;
		}

		/**************************************************************************//**
			@brief		CPU時間が1秒間に進む値を取得します。
			@return		CPU時間が1秒間に進む値
		*//***************************************************************************/
		inline unsigned __int64 GetCPUFrequency()
		{
			LARGE_INTEGER result;
			BOOL ret = ::QueryPerformanceFrequency( &result );
			assert( ret );
			return result.QuadPart;
		}
	}
}
#endif // __MultiThread_functions_h__
