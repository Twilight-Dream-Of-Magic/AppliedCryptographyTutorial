#pragma once

#include <vector>
#include <algorithm>

#include <iterator>
#include <stdexcept>
#include <utility>
#include <type_traits>

namespace TwilightDream
{
	struct BooleanBitWrapper
	{
		uint32_t bits;

		BooleanBitWrapper();
		BooleanBitWrapper( uint32_t value );
		~BooleanBitWrapper();

		// 按位与操作
		void bit_and( uint32_t other );
		// 按位或操作
		void bit_or( uint32_t other );

		// 按位非操作
		void bit_not();

		// 按位异或操作
		void bit_xor( uint32_t other );
		// 按位同或操作
		void bit_not_xor( uint32_t other );

		// 按位非与操作
		void bit_not_and( uint32_t other );

		// 按位非或操作
		void bit_not_or( uint32_t other );

		// 按位左移操作
		void bit_leftshift( int shift );
		// 按位右移操作
		void bit_rightshift( int shift );

		// 设置所有位为给定的布尔值
		void bit_set( bool value );

		// 设置指定索引的位为给定的布尔值
		void bit_set( bool value, int index );

		// 翻转指定索引的位
		void bit_flip( size_t index );

		// 获取指定索引的位的布尔值
		bool bit_get( int index ) const;

		// 统计比特'1'的数量
		size_t count_bits() const;

		BooleanBitWrapper operator=( const BooleanBitWrapper& other );

		operator uint32_t() const noexcept;

		friend bool operator==( const BooleanBitWrapper& left, const BooleanBitWrapper& right );

		friend bool operator!=( const BooleanBitWrapper& left, const BooleanBitWrapper& right );
	};
}