#include "BooleanBitWrapper.hpp"

namespace TwilightDream
{
	/* BooleanBitWrapper */

	BooleanBitWrapper::BooleanBitWrapper() : bits( 0 ) {}
	BooleanBitWrapper::BooleanBitWrapper( uint32_t value ) : bits( value ) {}
	BooleanBitWrapper::~BooleanBitWrapper()
	{
		bits = 0;
	}

	// 按位与操作
	void BooleanBitWrapper::bit_and( uint32_t other )
	{
		bits &= other;
	}

	// 按位或操作
	void BooleanBitWrapper::bit_or( uint32_t other )
	{
		bits |= other;
	}

	// 按位非操作
	void BooleanBitWrapper::bit_not()
	{
		bits = ~bits;
	}

	// 按位异或操作
	void BooleanBitWrapper::bit_xor( uint32_t other )
	{
		bits ^= other;
	}

	// 按位同或操作
	void BooleanBitWrapper::bit_not_xor( uint32_t other )
	{
		bits = ~( bits ^ other );
	}

	// 按位非与操作
	void BooleanBitWrapper::bit_not_and( uint32_t other )
	{
		bits = ~( bits & other );
	}

	// 按位非或操作
	void BooleanBitWrapper::bit_not_or( uint32_t other )
	{
		bits = ~( bits | other );
	}

	// 按位左移操作
	void BooleanBitWrapper::bit_leftshift( int shift )
	{
		bits <<= shift;
	}

	// 按位右移操作
	void BooleanBitWrapper::bit_rightshift( int shift )
	{
		bits >>= shift;
	}

	// 设置所有位为给定的布尔值
	void BooleanBitWrapper::bit_set( bool value )
	{
		bits = value ? 0xFFFFFFFF : 0;
	}

	// 设置指定索引的位为给定的布尔值
	void BooleanBitWrapper::bit_set( bool value, int index )
	{
		if ( value )
		{
			bits |= ( 1 << index );
		}
		else
		{
			bits &= ~( 1 << index );
		}
	}

	// 翻转指定索引的位
	void BooleanBitWrapper::bit_flip( size_t index )
	{
		bits ^= ( 1 << index );
	}

	// 获取指定索引的位的布尔值
	bool BooleanBitWrapper::bit_get( int index ) const
	{
		return ( bits >> index ) & 1;
	}

	// 统计比特'1'的数量
	size_t BooleanBitWrapper::count_bits() const
	{
		uint32_t n = bits;

		// 将相邻的位分组，每两位一组，然后用这两位中较低的一位表示这一组中置位的数量（0或1或2）
		// 例如: 0b1101 (原始数值) 变成 0b0100
		n = n - ( ( n >> 1 ) & 0x55555555 );

		// 将相邻的两组位（即4位）合并为一组，然后用这一组中较低的两位表示这一组中置位的数量（0到4）
		// 例如: 0b0100 (来自上一步) 变成 0b0010
		n = ( n & 0x33333333 ) + ( ( n >> 2 ) & 0x33333333 );

		// 将相邻的两组位（即8位）合并为一组，然后用这一组中较低的4位表示这一组中置位的数量（0到8）
		// 并且我们通过和 0x0F0F0F0F 相与，消除了不需要的位
		n = ( n + ( n >> 4 ) ) & 0x0F0F0F0F;

		// 将32位数中的所有8位组合并，得到一个8位数，这个8位数的低8位表示原32位数中置位的数量（0到32）
		n = n + ( n >> 8 );

		// 同上，但这次是将两个8位数合并为一个16位数
		n = n + ( n >> 16 );

		// 使用与操作消除不需要的位，返回计数结果
		return n & 0x3F;
	}

	BooleanBitWrapper::operator uint32_t() const noexcept
	{
		return bits;
	}

	BooleanBitWrapper BooleanBitWrapper::operator=( const BooleanBitWrapper& other )
	{
		if ( this == &other )
		{
			return *this;
		}

		bits = other.bits;
		return *this;
	}

	bool operator==( const BooleanBitWrapper& left, const BooleanBitWrapper& right )
	{
		return left.bits == right.bits;
	}

	bool operator!=( const BooleanBitWrapper& left, const BooleanBitWrapper& right )
	{
		return left.bits != right.bits;
	}
}