#pragma once

#include "DynamicBitSet.hpp"

#include <cstdint>
#include <cstdlib>
#include <cassert>

#include <iostream>
#include <iomanip>

#include <vector>
#include <algorithm>

#include <random>
#include <chrono>
#include <stdexcept>

struct BooleanBitWrapper
{
	uint32_t bits;

	BooleanBitWrapper() : bits( 0 ) {}
	BooleanBitWrapper( uint32_t value ) : bits( value ) {}
	~BooleanBitWrapper()
	{
		bits = 0;
	}

	// 按位与操作
	void bit_and( uint32_t other )
	{
		bits &= other;
	}

	// 按位或操作
	void bit_or( uint32_t other )
	{
		bits |= other;
	}

	// 按位非操作
	void bit_not()
	{
		bits = ~bits;
	}

	// 按位异或操作
	void bit_xor( uint32_t other )
	{
		bits ^= other;
	}

	// 按位同或操作
	void bit_not_xor( uint32_t other )
	{
		bits = ~( bits ^ other );
	}

	// 按位非与操作
	void bit_not_and( uint32_t other )
	{
		bits = ~( bits & other );
	}

	// 按位非或操作
	void bit_not_or( uint32_t other )
	{
		bits = ~( bits | other );
	}

	// 按位左移操作
	void bit_leftshift( int shift )
	{
		bits <<= shift;
	}

	// 按位右移操作
	void bit_rightshift( int shift )
	{
		bits >>= shift;
	}

	// 设置所有位为给定的布尔值
	void bit_set( bool value )
	{
		bits = value ? 0xFFFFFFFF : 0;
	}

	// 设置指定索引的位为给定的布尔值
	void bit_set( bool value, int index )
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
	void bit_flip( size_t index )
	{
		bits ^= ( 1 << index );
	}

	// 获取指定索引的位的布尔值
	bool bit_get( int index ) const
	{
		return ( bits >> index ) & 1;
	}

	// 统计比特'1'的数量
	size_t count_bits() const
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

	BooleanBitWrapper operator=( const BooleanBitWrapper& other )
	{
		if ( this == &other )
		{
			return *this;
		}

		bits = other.bits;
		return *this;
	}

	friend bool operator==( const BooleanBitWrapper& left, const BooleanBitWrapper& right )
	{
		return left.bits == right.bits;
	}

	friend bool operator!=( const BooleanBitWrapper& left, const BooleanBitWrapper& right )
	{
		return left.bits != right.bits;
	}
};

class DynamicBitSet
{
public:
	DynamicBitSet()
	{
		// 默认构造函数，可能进行一些初始化操作
		data_size = 0;
		data_capacity = 0;
		data_chunk_count = 0;
	}

	DynamicBitSet( size_t initial_bit_capacity, bool fill_bit )
		:
		bitset(needed_chunks(initial_bit_capacity), fill_bit ? BooleanBitWrapper( 0xFFFFFFFF ) : BooleanBitWrapper( 0x00000000 ))
	{
		// 设置实际的比特大小
		data_chunk_count = bitset.size();
		data_size = initial_bit_capacity;
		data_capacity = bitset.size() * 32;

		// 如果有多余的比特，设置它们
		size_t extra_bits = initial_bit_capacity % 32;  // 假设每个块有32位
		if (extra_bits != 0) {
			uint32_t mask = (1 << extra_bits) - 1;  // 创建一个掩码，用于保留需要的比特
			this->bitset.back().bits &= mask;  // 使用掩码来清除(MSB)多余的比特
		}
	}

	DynamicBitSet( const std::vector<bool>& bool_vector )
		:
		bitset(needed_chunks(bool_vector.size()), BooleanBitWrapper( 0x00000000 ))
	{
		// 设置实际的比特大小
		data_chunk_count = bitset.size();
		data_size = bool_vector.size();
		data_capacity = bitset.size() * 32;
		
		for ( size_t i = 0; i < bool_vector.size(); ++i )
		{
			(*this)[ i ] = bool_vector[ i ];
		}
	}

	// 接受二进制字符串作为参数的构造函数
	DynamicBitSet(const std::string& binaryString)
	{
		std::string binary(binaryString.size(), 0);
		std::reverse_copy(binaryString.begin(), binaryString.end(), binary.begin());
		resize( binaryString.size() );
		for ( size_t i = 0; i < binary.size(); ++i )
		{
			set_bit( binary[ i ] == '1', i );
		}

		// 设置实际的比特大小
		data_chunk_count = bitset.size();
		data_size = this->valid_number_of_bits();
		data_capacity = bitset.size() * 32;
	}

	DynamicBitSet( const std::string& string, int formatted )
	{
		switch ( formatted )
		{
			case 2:	 // 二进制
			{
				std::string binary(string.size(), 0);
				std::reverse_copy(string.begin(), string.end(), binary.begin());
				resize( string.size() );
				for ( size_t i = 0; i < binary.size(); ++i )
				{
					set_bit( binary[ i ] == '1', i );
				}
				break;
			}
			case 10:  // 十进制
			{
				std::string binary = decimal_to_binary( string );
				//std::cout << binary << std::endl;
				resize( binary.size() );
				for ( size_t i = 0; i < binary.size(); ++i )
				{
					set_bit( binary[ i ] == '1', i );
				}
			}
			break;
			case 16:  // 十六进制
			{
				std::string binary = hexadecimal_to_binary( string );
				resize( binary.size() );
				for ( size_t i = 0; i < binary.size(); ++i )
				{
					set_bit( binary[ i ] == '1', i );
				}
			}
			break;
		default:
			throw std::invalid_argument( "Invalid format specifier" );
		}
	}

	DynamicBitSet( uint32_t value )
		:
		bitset(1, BooleanBitWrapper( 0x00000000 ))
	{
		bitset[0].bits = value;

		// 更新成员变量
		this->data_chunk_count = 1;
		this->data_size = this->valid_number_of_bits();
		this->data_capacity = 32;
	}

	DynamicBitSet(const std::vector<uint32_t>& values)
		: 
		bitset(values.size(), BooleanBitWrapper(0x00000000)),
		data_chunk_count(values.size()),
		data_capacity(values.size() * 32),
		data_size(values.size() * 32)  // 初始化为最大可能的大小
	{
		for (size_t i = 0; i < values.size(); ++i)
		{
			bitset[i].bits = values[i];
		}

		this->data_size = this->valid_number_of_bits();
	}

	DynamicBitSet( uint64_t value )
		:
		bitset(2, BooleanBitWrapper( 0x00000000 ))
	{
		bitset[0].bits = value & 0x0000FFFFF;
		bitset[1].bits = value >> 32;

		// 更新成员变量
		this->data_chunk_count = 2;
		this->data_size = this->valid_number_of_bits();
		this->data_capacity = 64;
	}

	DynamicBitSet( const std::vector<uint64_t>& values )
		:
		bitset(values.size() * 2, BooleanBitWrapper(0x00000000)),
		data_chunk_count(values.size() * 2), // 两个32位块组成一个64位块
		data_capacity(values.size() * 64)
	{
		this->data_size = values.size() * 64;
	
		for (size_t i = 0; i < values.size(); ++i)
		{
			uint32_t lower_bits = static_cast<uint32_t>(values[i] & 0xFFFFFFFF);
			uint32_t upper_bits = static_cast<uint32_t>((values[i] >> 32) & 0xFFFFFFFF);
		
			bitset[i * 2].bits = lower_bits;
			bitset[i * 2 + 1].bits = upper_bits;
		}

		// 更新 data_size
		this->data_size = this->valid_number_of_bits();
	}

	DynamicBitSet( const std::initializer_list<uint32_t>& values)
			: bitset( values.size(), BooleanBitWrapper( 0x00000000 ) ), data_chunk_count( values.size() ), data_capacity( values.size() * 32 ), data_size( values.size() * 32 )
	{
		std::copy(values.begin(), values.end(), bitset.begin());

		this->data_size = this->valid_number_of_bits();
	}

	DynamicBitSet( const std::initializer_list<uint64_t>& values )
		:
		bitset( values.size() * 2, BooleanBitWrapper( 0x00000000 ) ),
		data_chunk_count( values.size() ),  // 两个32位块组成一个64位块
		data_capacity( values.size() * 64 ),  // 64 bits per chunk
		data_size( values.size() * 64 )
	{
		auto iter = bitset.begin();
		for ( uint64_t value : values )
		{
			uint32_t lower = static_cast<uint32_t>( value & 0xFFFFFFFF );
			uint32_t upper = static_cast<uint32_t>( ( value >> 32 ) & 0xFFFFFFFF );

			if(iter != bitset.end())
			{
				*iter = BooleanBitWrapper( lower );
				++iter;
				*iter = BooleanBitWrapper( upper );
				++iter;
			}
		}

		this->data_size = this->valid_number_of_bits();
	}

	DynamicBitSet( const BooleanBitWrapper& wrapper )
	{
		bitset.push_back( wrapper );

		this->data_chunk_count = bitset.size();
		this->data_size = this->valid_number_of_bits();
		this->data_capacity = bitset.size() * 32;
	}

	DynamicBitSet( const std::vector<BooleanBitWrapper>& wrapper_bool_vector )
		: bitset( wrapper_bool_vector )
	{
		// 更新成员变量
		this->data_chunk_count = bitset.size();
		this->data_size = this->valid_number_of_bits();
		this->data_capacity = bitset.size() * 32;
	}

	DynamicBitSet( const DynamicBitSet& other ) noexcept 
		: bitset( other.bitset ), data_capacity( other.data_capacity ), data_size( other.data_size ), data_chunk_count( other.data_chunk_count )
	{
		// 这里你可能还想进行一些额外的复制操作
	}

	DynamicBitSet& operator=( const DynamicBitSet& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;  // 返回 *this 以处理自赋值 case
		}
		this->bitset = other.bitset;

		this->data_chunk_count = other.data_chunk_count;
		this->data_capacity = other.data_capacity;
		this->data_size = other.data_size;

		return *this;
	}

	DynamicBitSet( DynamicBitSet&& other ) noexcept 
		: bitset( std::move( other.bitset ) ), data_capacity( std::move( other.data_capacity ) ), data_size( std::move( other.data_size ) ), data_chunk_count( std::move( other.data_chunk_count ) )
	{
		// 这里你可能还想进行一些额外的移动操作
	}

	DynamicBitSet& operator=( DynamicBitSet&& other ) noexcept
	{
		if ( this == &other )
		{
			return *this;  // 返回 *this 以处理自赋值 case
		}
		bitset = std::move( other.bitset );

		this->data_capacity = std::move( other.data_capacity );
		this->data_size = std::move( other.data_size );
		this->data_chunk_count = std::move( other.data_chunk_count );

		return *this;
	}

	~DynamicBitSet()
	{
		// 默认析构函数，如果需要，可以进行一些清理操作
		data_size = 0;
		data_capacity = 0;
		data_chunk_count = 0;
	}

	// 比特集使用内存物理容量是不是空
	bool empty()
	{
		if ( bitset.capacity() > 0 )
			if ( bitset.size() > 0 )
				return false;
		return true;
	}

	bool is_alone_chunk() const
	{
		return bitset.size() == 1;
	}

	// 计算实际有效比特数量(自适应比特大小)
	size_t valid_number_of_bits() const
	{
		// 内联函数：用于计算一个 32 位整数 x 的二进制表示中从最高位开始到第一个非零位之间，连续零的数量。
		auto LeadingZeros = [](uint32_t x) -> uint32_t {
			// 执行位扩展（smearing）操作，将最高位的 1 向右扩展，填充所有低位。
			// 这样做是为了方便后续计算 1 的数量。
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;

			// 计算 x 中 1's 的数量。
			// 下面的操作是经典的位操作技巧，用于快速计算一个整数的二进制表示中 1's 的数量。
			x -= (x >> 1) & 0x55555555;
			x = (x >> 2 & 0x33333333) + (x & 0x33333333);
			x = ((x >> 4) + x) & 0x0f0f0f0f;
			x += x >> 8;
			x += x >> 16;

			// 计算 uint32_t 类型的位数，这是一个编译时常量。
			// 对于 32 位整数，这将是 32。
			// 从 uint32_t 类型的总位数（通常是 32）中减去 1's 的数量，得到从最高位开始到第一个非0位的0's的数量。
			return ( sizeof(uint32_t) * CHAR_BIT ) - ( x & 0x0000003f );
		};

		// 从最高有效位（MSB）的 wrapper 开始检查
		for (size_t wrapperIndex = bitset.size(); wrapperIndex > 0 && wrapperIndex != size_t(-1); --wrapperIndex)
		{
			uint32_t currentWrapperBits = bitset[wrapperIndex - 1].bits;

			// 如果当前 wrapper 不全为零，则进一步检查
			if (currentWrapperBits != 0)
			{
				// 使用 LeadingZeros 函数来找到从最高位开始到第一个非零位之间的零的数量
				// 然后计算并返回实际使用的比特数量
				return (wrapperIndex - 1) * 32 + (uint32_t(32) - LeadingZeros(currentWrapperBits));
			}
		}

		// 如果所有位都是 0，则实际使用的比特数量为 0
		return 0;
	}

	// 被记录的比特数量(大小)
	size_t bit_size() const
	{
		return this->data_size;
	}

	// "虚拟容量"（即，在不调整底层 std::vector<BooleanBitWrapper>::size() 大小的情况下，可以存储的最大比特位数）
	// 被记录的比特集"容量"（以BooleanBitWrapper为单位的比特数 * std::vector<BooleanBitWrapper>::size()）
	size_t bit_capacity() const
	{
		return this->data_capacity;
	}

	//被记录的比特集数量
	size_t chunk_count() const
	{
		return this->data_chunk_count;
	}

	// 获取比特集使用内存物理容量（以BooleanBitWrapper为单位）
	size_t memory_capacity() const
	{
		return bitset.capacity();
	}

	// 检查是否所有的位都被设置
	bool all() const
	{
		for ( const auto& wrapper : bitset )
		{
			if ( wrapper.bits != 0xFFFFFFFF )
			{  // 如果有任何一个位没有被设置
				return false;
			}
		}
		return true;
	}

	// 检查是否有任何位被设置
	bool any() const
	{
		for ( const auto& wrapper : bitset )
		{
			if ( wrapper.bits != 0 )
			{  // 如果有任何一个位被设置
				return true;
			}
		}
		return false;
	}

	// 检查是否没有位被设置
	bool none() const
	{
		for ( const auto& wrapper : bitset )
		{
			if ( wrapper.bits != 0 )
			{  // 如果有任何一个位被设置
				return false;
			}
		}
		return true;
	}

	// 设置指定索引的位为给定的布尔值
	void set_bit( bool value, size_t index )
	{
		if ( index >= this->data_size )
		{
			throw std::out_of_range( "Index out of range from set bit" );
		}
		size_t wrapperIndex = index / 32;
		size_t bitIndex = index % 32;
		bitset[ wrapperIndex ].bit_set( value, bitIndex );
	}

	// 获取指定索引的位的布尔值
	bool get_bit( size_t index ) const
	{
		if ( index >= this->data_size )
		{
			throw std::out_of_range( "Index out of range from get bit" );
		}
		size_t wrapperIndex = index / 32;
		size_t bitIndex = index % 32;
		return bitset[ wrapperIndex ].bit_get( bitIndex );
	}

	class BitReference
	{
	public:
		BitReference( std::vector<BooleanBitWrapper>& bitset, size_t index ) : m_bitset( bitset ), m_index( index ), chunk_mask_true_( 1 << ( index % 32 ) ), chunk_mask_false_( ~( 1 << ( index % 32 ) ) ) {}

		BitReference operator=( const BitReference& other )
		{
			if ( this == &other )
			{
				return *this;
			}

			// 复制成员变量
			this->m_bitset = other.m_bitset;
			this->m_index = other.m_index;
			this->chunk_mask_true_ = other.chunk_mask_true_;
			this->chunk_mask_false_ = other.chunk_mask_false_;

			return *this;
		}

		// 用于赋值
		BitReference& operator=( bool value )
		{
			value ? m_bitset[ m_index / 32 ].bit_or( chunk_mask_true_ ) : m_bitset[ m_index / 32 ].bit_and( chunk_mask_false_ );
			return *this;
		}

		// 用于读取
		operator bool() const
		{
			return m_bitset[ m_index / 32 ].bits & chunk_mask_true_;
		}

		// 按位与赋值
		BitReference& operator&=( bool value )
		{
			if ( !value )
			{
				m_bitset[ m_index / 32 ].bit_and( chunk_mask_false_ );
			}
			return *this;
		}

		// 按位或赋值
		BitReference& operator|=( bool value )
		{
			if ( value )
			{
				m_bitset[ m_index / 32 ].bit_or( chunk_mask_true_ );
			}
			return *this;
		}

		// 按位异或赋值
		BitReference& operator^=( bool value )
		{
			if ( value )
			{
				m_bitset[ m_index / 32 ].bit_xor( chunk_mask_true_ );
			}
			return *this;
		}

		// 按位非
		bool operator~() const
		{
			return !( m_bitset[ m_index / 32 ].bits & chunk_mask_true_ );
		}

	private:
		std::vector<BooleanBitWrapper>& m_bitset;
		size_t							m_index;
		uint32_t						chunk_mask_true_;
		uint32_t						chunk_mask_false_;
	};

	// Subscript Operator for non-const DynamicBitSet
	BitReference operator[]( size_t index )
	{
		if(index >= this->data_size)
			throw std::out_of_range("Index out of range");

		return BitReference( this->bitset, index );
	}

	// Subscript Operator for non-const DynamicBitSet
	bool operator[]( size_t index ) const
	{
		if(index >= this->data_size)
			throw std::out_of_range("Index out of range");

		return this->bitset[index / 32].bit_get(index % 32);
	}

	// 正向迭代器
	class ForwardIterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = bool;
		using difference_type = std::ptrdiff_t;
		using pointer = bool*;
		using reference = BitReference;

		ForwardIterator( std::vector<BooleanBitWrapper>* bitset, size_t index ) : m_bitset( bitset ), m_index( index ) {}

		ForwardIterator& operator=( const ForwardIterator& other )
		{
			if ( this == &other )
			{
				return *this;
			}
			this->m_bitset = other.m_bitset;
			this->m_index = other.m_index;
			return *this;
		}

		reference operator*()
		{
			return BitReference( *m_bitset, m_index );
		}

		ForwardIterator& operator++()
		{
			++m_index;
			return *this;
		}

		ForwardIterator operator++( int )
		{
			ForwardIterator tmp( *this );
			++m_index;
			return tmp;
		}

		ForwardIterator& operator--()
		{
			if ( m_index == 0 )
			{
				throw std::out_of_range( "Iterator cannot be decremented past the beginning." );
			}
			--m_index;
			return *this;
		}

		ForwardIterator operator--( int )
		{
			ForwardIterator tmp( *this );
			--( *this );
			return tmp;
		}

		bool operator==( const ForwardIterator& rhs ) const
		{
			return ( m_bitset == rhs.m_bitset ) && ( m_index == rhs.m_index );
		}

		bool operator!=( const ForwardIterator& rhs ) const
		{
			return !( *this == rhs );
		}

		bool operator<( const ForwardIterator& rhs ) const
		{
			return m_index < rhs.m_index;
		}

		bool operator>( const ForwardIterator& rhs ) const
		{
			return m_index > rhs.m_index;
		}

		bool operator<=( const ForwardIterator& rhs ) const
		{
			return m_index <= rhs.m_index;
		}

		bool operator>=( const ForwardIterator& rhs ) const
		{
			return m_index >= rhs.m_index;
		}

		ForwardIterator operator+( difference_type n ) const
		{
			return ForwardIterator( m_bitset, m_index + n );
		}

		ForwardIterator operator-( difference_type n ) const
		{
			return ForwardIterator( m_bitset, m_index - n );
		}

	private:
		std::vector<BooleanBitWrapper>* m_bitset;
		size_t							m_index;
	};

	// 反向迭代器
	class BackwardIterator
	{
	public:
		using iterator_category = std::random_access_iterator_tag;
		using value_type = bool;
		using difference_type = std::ptrdiff_t;
		using pointer = bool*;
		using reference = BitReference;

		BackwardIterator( std::vector<BooleanBitWrapper>* bitset, size_t index ) : m_bitset( bitset ), m_index( index ) {}

		BackwardIterator& operator=( const BackwardIterator& other )
		{
			if ( this == &other )
			{
				return *this;
			}
			this->m_bitset = other.m_bitset;
			this->m_index = other.m_index;
			return *this;
		}

		reference operator*()
		{
			return BitReference( *m_bitset, m_index );
		}

		BackwardIterator& operator++()
		{
			if ( m_index == size_t( -1 ) )
			{
				throw std::out_of_range( "Iterator cannot be decremented past the beginning." );
			}
			--m_index;
			return *this;
		}

		BackwardIterator operator++( int )
		{
			BackwardIterator tmp( *this );
			--( *this );
			return tmp;
		}

		BackwardIterator& operator--()
		{
			++m_index;
			return *this;
		}

		BackwardIterator operator--( int )
		{
			BackwardIterator tmp( *this );
			++( *this );
			return tmp;
		}

		bool operator==( const BackwardIterator& rhs ) const
		{
			return ( m_bitset == rhs.m_bitset ) && ( m_index == rhs.m_index );
		}

		bool operator!=( const BackwardIterator& rhs ) const
		{
			return !( *this == rhs );
		}

		bool operator<( const BackwardIterator& rhs ) const
		{
			return m_index < rhs.m_index;
		}

		bool operator>( const BackwardIterator& rhs ) const
		{
			return m_index > rhs.m_index;
		}

		bool operator<=( const BackwardIterator& rhs ) const
		{
			return m_index <= rhs.m_index;
		}

		bool operator>=( const BackwardIterator& rhs ) const
		{
			return m_index >= rhs.m_index;
		}

		BackwardIterator operator+( difference_type n ) const
		{
			return BackwardIterator( m_bitset, m_index - n );
		}

		BackwardIterator operator-( difference_type n ) const
		{
			return BackwardIterator( m_bitset, m_index + n );
		}

	private:
		std::vector<BooleanBitWrapper>* m_bitset;
		size_t							m_index;
	};

	ForwardIterator begin()
	{
		return ForwardIterator( &this->bitset, 0 );
	}

	ForwardIterator end()
	{
		return ForwardIterator( &this->bitset, bit_size() );
	}

	BackwardIterator rbegin()
	{
		return BackwardIterator( &this->bitset, bit_size() - 1 );
	}

	BackwardIterator rend()
	{
		return BackwardIterator( &this->bitset, size_t( -1 ) );	 // 使用 size_t(-1) 表示无效索引
	}

	/* 最低有效位（LSB）是在最前面{(Bitchunk[0] >> 0) & 1}，而最高有效位（MSB）是在最后面{(Bitchunk[Bitchunk.size() - 1] >> 32 - 1) & 1)} */

	// 获取子集
	DynamicBitSet subset(size_t start, size_t end) const
	{
		if (end > this->data_size || start > end)
		{
			throw std::out_of_range("Invalid range for subset");
		}
		DynamicBitSet result(end - start, false);
		for (size_t i = start; i < end; ++i)
		{
			result[i - start] = (*this)[i];
		}
		return result;
	}

	/**
	 * Concatenates two DynamicBitSet objects.
	 *
	 * This function combines the bits of two DynamicBitSet objects to create a new
	 * DynamicBitSet where the bits of the "current" object are appended after the
	 * bits of the "other" object.
	 *
	 * @param current The DynamicBitSet whose least significant bits will be placed at the beginning.
	 * @param other The DynamicBitSet whose most significant bits will be placed after the "current" bits.
	 * @return A new DynamicBitSet containing the concatenated bits.
	 *
	 * @note The bit positions within the resulting DynamicBitSet are organized as follows:
	 *       [other's MSB] ... [other's LSB] [current's MSB] ... [current's LSB]
	 *
	 * @note The input DynamicBitSet objects are not modified.
	 *
	 * @note The returned DynamicBitSet's size will be the sum of the sizes of the input DynamicBitSet objects.
	 */
	friend DynamicBitSet bitset_concat(const DynamicBitSet& current, const DynamicBitSet& other)
	{
		DynamicBitSet result(current.data_size + other.data_size, false);

		// 首先，复制另一个 DynamicBitSet 的所有数据到结果的开始位置
		for (size_t i = 0; i < other.data_size; ++i) {
			result[i] = other[i];
		}

		// 然后，复制当前 DynamicBitSet 的所有数据到结果的偏移位置
		size_t offset = other.data_size;  // 偏移量等于 other 的大小
		for (size_t i = 0; i < current.data_size; ++i) {
			result[offset + i] = current[i];
		}

		return result;
	}

	// 在基于 LSB 位置 处 向左 插入 比特
	void insert(bool value, size_t index) {
		if (bitset.empty()) {
			 // 如果没有比特块，保持安全数据后，直接返回
			bitset.push_back(value);
			data_size = value;
			data_capacity = 32;
			data_chunk_count = 1;
			return;
		}

		if(this->data_size == 0) {
			if(bitset[0].bits == 0) {
				if(this->hamming_weight()) {
					this->data_size = this->valid_number_of_bits();
				}

				if(!value && this->data_size == 0)
					return;
				else if(value && this->data_size == 0)
				{
					/* 特殊处理：bitset不为0 和 data_size 为0，并且且没有任何有效个比特1数据被存储。 */

					//更新btiset容量大小。
					bitset.resize(needed_chunks(bitset.size() * 32 + 1), BooleanBitWrapper( 0x00000000 ));

					//修复错误记录的 data_size
					this->data_size = bitset.size() * 32 + 1;

					if (index >= this->data_size) {
						throw std::out_of_range("Index out of range");
					}
					
					if (index == 0)
					{
						// 如果 index 为 0（即 LSB）
						(*this)[1] = true; // 设置 原 LSB + 1 为 1
						return;
					}
					else if(index == bitset.size() * 32 - 1)
					{
						// 如果 index 为 bitset.size() * 32 - 1（即 MSB）
						bitset[bitset.size() - 1].bits |= 0x00000001;  // 设置 原 MSB + 1 为 1
						return;
					}

					//如果要插入中间位置
					( *this )[ index ] = true;
					return;
				}
			}
		}
		
		// 如果 index 为 this->data_size - 1（即 MSB），则 value 必须为 true
		if (index == this->data_size - 1 && value == false) {
			throw std::invalid_argument("Cannot insert a zero at the MSB position.");
		}
		
		if (index >= this->data_size) {
			throw std::out_of_range("Index out of range");
		}

		if ( this->data_size == 0 && this->data_chunk_count == 0 && value)
		{
			//Add bit chunk
			resize(1);
			if(value)
				( *this )[ 0 ] = true;
			return;
		}

		if(data_size == 1 && this->data_chunk_count == 1)
		{
			if(bitset[0].bits == 0x00000000)
			{
				bitset[0].bits = value ? 0x00000001 : 0x00000000;
			}
			else if(bitset[0].bits == 0x00000001)
			{
				bitset[0].bits <<= 1;
				bitset[0].bits |= value;
				++data_size;
			}
			return;
		}

		// 扩展数据大小
		resize(this->data_size + 1);

		DynamicBitSet subset1 = this->subset(0, index);
		DynamicBitSet subset2 = this->subset(index, this->data_size);

		subset1.push_back(value);  // 在LSB端添加值

		*this = bitset_concat(subset2, subset1);  // 注意顺序，因为我们是在LSB端添加
	}

	// 在基于 LSB 位置 处 向左 擦除 比特
	void erase(size_t index) {
		if (bitset.empty()) {
			 // 如果没有比特块，保持安全数据后，直接返回
			bitset.push_back(0);
			data_size = 0;
			data_capacity = 32;
			data_chunk_count = 1;
			return;
		}

		if(this->data_size == 0)
		{
			if(bitset[0].bits == 0)
			{
				if(this->hamming_weight())
					this->data_size = this->valid_number_of_bits();
				else
					return;
			}
		}
		
		if (index >= this->data_size) {
			throw std::out_of_range("Index out of range");
		}

		// 如果 index 为 0（即 LSB）并且没有其他比特为1
		if (index == 0 && this->hamming_weight() <= 1) {
			throw std::invalid_argument("Cannot erase the only set bit at the LSB position.");
		}

		if(data_size == 1 && this->data_chunk_count == 1)
		{
			//Remove bit chunk
			resize(0);
			return;
		}

		DynamicBitSet subset1 = this->subset(0, index);
		DynamicBitSet subset2 = this->subset(index + 1, this->data_size);

		*this = bitset_concat(subset2, subset1);  // 注意顺序，因为我们是在LSB端删除
	}

	// 在基于 MSB 位置 处 向右 插入 比特
	void reverse_insert(bool value, size_t backward_index) {
		if (bitset.empty()) {
			 // 如果没有比特块，保持安全数据后，直接返回
			bitset.push_back(value);
			data_size = value;
			data_capacity = 32;
			data_chunk_count = 1;
			return;
		}
		
		if(this->data_size == 0) {
			if(bitset[0].bits == 0) {
				if(this->hamming_weight()) {
					this->data_size = this->valid_number_of_bits();
				}

				if(!value && this->data_size == 0)
					return;
				else if(value && this->data_size == 0)
				{
					/* 特殊处理：bitset不为0 和 data_size 为0，并且且没有任何有效个比特1数据被存储。 */

					//更新btiset容量大小。
					bitset.resize(needed_chunks(bitset.size() * 32 + 1), BooleanBitWrapper( 0x00000000 ));

					//修复错误记录的 data_size
					this->data_size = bitset.size() * 32 + 1;

					if (this->data_size - 1 - backward_index >= this->data_size) {
						throw std::out_of_range("Index out of range");
					}
					
					if (this->data_size - 1 - backward_index == 0)
					{
						// 如果 backward_index 为 0（即 MSB）
						(*this)[this->data_size - 2] = true; // 设置 原 MSB - 1 为 1
						return;
					}
					else if(this->data_size - 1 - backward_index == bitset.size() * 32 - 1)
					{
						// 如果 backward_index 为 bitset.size() * 32 - 1（即 LSB）
						bitset[0].bits |= 0x00000001;  // 设置 原 LSB - 1 为 1
						return;
					}

					//如果要插入中间位置
					( *this )[ this->data_size - 1 - backward_index ] = true;
					return;
				}
			}
		}

		// 通常情况：将从 MSB 计数转换为从 LSB 计数的索引
		// 将从 MSB 计数转换为从 LSB 计数的索引
		size_t forward_index = this->data_size - 1 - backward_index;

		if (forward_index == this->data_size - 1 && value == false) {
			throw std::invalid_argument("Cannot insert a zero at the MSB position.");
		}

		if (forward_index >= this->data_size) {
			throw std::out_of_range("Index out of range");
		}
		
		if ( this->data_size == 0 && this->data_chunk_count == 0 && value)
		{
			//Add bit chunk
			resize(1);
			if(value)
				( *this )[ 0 ] = true;
			return;
		}

		if(data_size == 1 && this->data_chunk_count == 1)
		{
			if(bitset[0].bits == 0x00000000)
			{
				bitset[0].bits = value ? 0x00000001 : 0x00000000;
			}
			else if(bitset[0].bits == 0x00000001)
			{
				bitset[0].bits <<= 1;
				bitset[0].bits |= value;
				++data_size;
			}
			return;
		}

		// 扩展数据大小
		resize(this->data_size + 1);

		DynamicBitSet subset1 = this->subset(0, forward_index);
		DynamicBitSet subset2 = this->subset(forward_index, this->data_size);

		subset2.push_back(value);  // 在MSB端添加值

		*this = bitset_concat(subset2, subset1);  // 注意顺序，因为我们是在MSB端添加
	}

	// 在基于 MSB 位置 处 向右 擦除 比特
	void reverse_erase(size_t backward_index) {
		if (bitset.empty()) {
			 // 如果没有比特块，保持安全数据后，直接返回
			bitset.push_back(0);
			data_size = 0;
			data_capacity = 32;
			data_chunk_count = 1;
			return;
		}

		if(this->data_size == 0)
		{
			if(bitset[0].bits == 0)
			{
				if(this->hamming_weight())
					this->data_size = this->valid_number_of_bits();
				else
					return;
			}
		}
		
		// 将从 MSB 计数转换为从 LSB 计数的索引
		size_t forward_index = this->data_size - 1 - backward_index;

		if (forward_index >= this->data_size) {
			throw std::out_of_range("Index out of range");
		}

		// 如果 index 为 0（即 MSB）
		if (forward_index == this->data_size - 1) {
			*this = this->subset(0, this->data_size - 1);
			return;
		}

		if(data_size == 1 && this->data_chunk_count == 1)
		{
			//Remove bit chunk
			resize(0);
			return;
		}

		DynamicBitSet subset1 = this->subset(0, forward_index);
		DynamicBitSet subset2 = this->subset(forward_index + 1, this->data_size);

		*this = bitset_concat(subset2, subset1);  // 注意顺序，因为我们是在MSB端删除
	}

	// 追加一个位到 MSB（最重要位）
	void push_front(bool value) {
		if (bitset.empty()) {
			 // 如果没有比特块，保持安全数据后，直接返回
			bitset.push_back(value);
			data_size = 0;
			data_capacity = 32;
			data_chunk_count = 1;
			return;
		}
		
		size_t new_bit_size = data_size + 1;
		
		// 增加数据大小
		resize(new_bit_size);

		// 检查是否需要添加新的块
		if (data_size > bitset.size() * 32) {
			bitset.push_back(BooleanBitWrapper(0));
			data_capacity += 32;
			++data_chunk_count;
		}

		// 将所有位向左移动一位以创建空间
		this->left_shift(1);

		data_size = new_bit_size;

		// 在 MSB 处设置新值
		(*this)[data_size - 1] = value;
	}

	// 删除一个位 MSB（最重要位）
	void pop_front() {
		if (bitset.empty()) {
			 // 如果没有比特块，保持安全数据后，直接返回
			bitset.push_back(0);
			data_size = 0;
			data_capacity = 32;
			data_chunk_count = 1;
			return;
		}

		size_t new_bit_size = data_size - 1;

		// 将所有位向右移动一位以覆盖 MSB
		this->right_shift(1);

		// 减少数据大小
		resize(new_bit_size);

		data_size = new_bit_size;

		// 检查是否需要删除一个块
		if (bitset.size() > blocks_required(data_size)) {
			bitset.pop_back();
			data_capacity -= 32;
			--data_chunk_count;
		}
	}

	// 追加一个位到 LSB（最不重要位）
	void push_back(bool value) {
		size_t new_bit_size = data_size + 1;
		
		// 增加数据大小
		resize(new_bit_size);

		// 检查是否需要添加新的块
		if (data_size > bitset.size() * 32) {
			bitset.push_back(BooleanBitWrapper(0));
			data_capacity += 32;
			++data_chunk_count;
		}

		// 将所有位向左移动一位以使新值成为 LSB
		// 这个会计算当前的实际有效比特大小，所以之后还要再次覆盖一次数据大小
		this->left_shift(1);

		data_size = new_bit_size;

		// 在 LSB 处设置新值
		(*this)[0] = value;

		assert(check_consistency(1));
	}

	// 删除一个位 LSB（最不重要位）
	void pop_back() {
		size_t new_bit_size = data_size - 1;
		
		// 将所有位向右移动一位以覆盖 LSB
		// 这个会计算当前的实际有效比特大小，所以之后还要再次覆盖一次数据大小
		this->right_shift(1);

		// 减少数据大小
		resize(new_bit_size);

		data_size = new_bit_size;

		// 检查是否需要删除一个块
		if (bitset.size() > blocks_required(data_size)) {
			bitset.pop_back();
			data_capacity -= 32;
			--data_chunk_count;
		}

		assert(check_consistency(1));
	}

	// 翻转指定位置的比特位
	DynamicBitSet& flip( size_t position )
	{
		if ( position >= this->data_size )
		{
			throw std::out_of_range( "Filp bit: Position out of range" );
		}
		bitset[ position / 32 ].bit_flip( position % 32 );
		return *this;
	}

	void set()
	{
		for ( auto& chunk : bitset )
		{
			chunk.bits = 0xFFFFFFFF;
		}

		this->data_size = this->data_capacity;
	}

	void set( size_t pos, size_t len, bool value )
	{
		assert( pos < bit_size() );
		if ( len == 0 )
		{
			return;
		}
		assert( pos + len - 1 < bit_size() );

		const size_t first_chunk = pos / 32;
		const size_t last_chunk = ( pos + len - 1 ) / 32;
		const size_t first_bit_index = pos % 32;
		const size_t last_bit_index = ( pos + len - 1 ) % 32;

		uint32_t mask;

		if ( first_chunk == last_chunk )
		{
			mask = ( ( 1 << ( last_bit_index - first_bit_index + 1 ) ) - 1 ) << first_bit_index;
			if ( value )
			{
				bitset[ first_chunk ].bits |= mask;
			}
			else
			{
				bitset[ first_chunk ].bits &= ~mask;
			}
		}
		else
		{
			// First chunk
			mask = ( 1 << ( 32 - first_bit_index ) ) - 1;
			if ( value )
			{
				bitset[ first_chunk ].bits |= mask << first_bit_index;
			}
			else
			{
				bitset[ first_chunk ].bits &= ~( mask << first_bit_index );
			}

			// Middle chunks
			mask = value ? 0xFFFFFFFF : 0x00000000;
			for ( size_t i = first_chunk + 1; i < last_chunk; ++i )
			{
				bitset[ i ] = mask;
			}

			// Last chunk
			mask = ( 1 << ( last_bit_index + 1 ) ) - 1;
			if ( value )
			{
				bitset[ last_chunk ].bits |= mask;
			}
			else
			{
				bitset[ last_chunk ].bits &= ~mask;
			}
		}
	}

	void reset()
	{
		for ( auto& chunk : bitset )
		{
			chunk.bits = 0;
		}

		this->data_size = 0;
	}

	void reset( size_t pos, size_t len )
	{
		set( pos, len, false );
	}

	void reset( size_t pos )
	{
		( *this )[ pos ] = false;
	}

	// 按位与操作 (&=)
	void and_operation( const DynamicBitSet& other )
	{
		size_t min_size = std::min( this->data_chunk_count, other.data_chunk_count );
		for ( size_t i = 0; i < min_size; ++i )
		{
			this->bitset[ i ].bit_and( other.bitset[ i ].bits );
		}

		// 如果 this->bitset 比 other.bitset 长，将多余的部分设置为0
		if (this->data_chunk_count > other.data_chunk_count)
		{
			std::fill(this->bitset.begin() + min_size, this->bitset.end(), BooleanBitWrapper(0));
		}

		this->data_size = this->valid_number_of_bits();
	}

	// 按位或操作 (|=)
	void or_operation( const DynamicBitSet& other )
	{
		size_t min_size = std::min( this->data_chunk_count, other.data_chunk_count );
		for ( size_t i = 0; i < min_size; ++i )
		{
			this->bitset[ i ].bit_or( other.bitset[ i ].bits );
		}

		// 如果需要，扩展 this->bitset
		if (this->data_chunk_count < other.data_chunk_count)
		{
			this->bitset.resize(other.data_chunk_count);
			std::copy(other.bitset.begin() + min_size, other.bitset.end(), this->bitset.begin() + min_size);
		}

		this->data_size = this->valid_number_of_bits();
	}

	// 按位非操作 (~=) / 翻转所有位
	void not_operation()
	{
		for ( auto& wrapper : bitset )
		{
			wrapper.bit_not();
		}

		this->data_size = this->valid_number_of_bits();
	}

	// 按位异或操作 (^=)
	void xor_operation( const DynamicBitSet& other )
	{
		size_t min_size = std::min( this->data_chunk_count, other.data_chunk_count );
		for ( size_t i = 0; i < min_size; ++i )
		{
			this->bitset[ i ].bit_xor( other.bitset[ i ].bits );
		}

		// 如果需要，扩展 this->bitset
		if (this->data_chunk_count < other.data_chunk_count)
		{
			this->bitset.resize(other.data_chunk_count);
			std::copy(other.bitset.begin() + min_size, other.bitset.end(), this->bitset.begin() + min_size);
		}

		this->data_size = this->valid_number_of_bits();
	}

	// 左移操作 (<<=)
	DynamicBitSet& left_shift( size_t shift )
	{
		assert( shift > 0 );
		assert( shift < bitset.size() * 32 );

		const size_t blocks_shift = shift / 32;
		const size_t bits_offset = shift % 32;

		if ( bits_offset == 0 )
		{
			for ( size_t i = bitset.size() - 1; i >= blocks_shift; --i )
			{
				bitset[ i ] = bitset[ i - blocks_shift ];
			}
		}
		else
		{
			const size_t reverse_bits_offset = 32 - bits_offset;
			for ( size_t i = bitset.size() - 1; i > blocks_shift; --i )
			{
				bitset[ i ] = uint32_t( ( bitset[ i - blocks_shift ].bits << bits_offset ) | uint32_t( bitset[ i - blocks_shift - 1 ].bits >> reverse_bits_offset ) );
			}
			bitset[ blocks_shift ] = uint32_t( bitset[ 0 ].bits << bits_offset );
		}

		// set bit that came at the right to 0 in unmodified blocks
		std::fill( std::begin( bitset ), std::begin( bitset ) + static_cast<typename decltype( bitset )::difference_type>( blocks_shift ), 0x00000000 );

		this->data_size = this->valid_number_of_bits();

		return *this;
	}

	// 右移操作 (>>=)
	DynamicBitSet& right_shift( size_t shift )
	{
		assert( shift > 0 );
		assert( shift < bitset.size() * 32 );

		const size_t blocks_shift = shift / 32;
		const size_t bits_offset = shift % 32;
		const size_t last_block_to_shift = bitset.size() - blocks_shift - 1;

		if ( bits_offset == 0 )
		{
			for ( size_t i = 0; i <= last_block_to_shift; ++i )
			{
				bitset[ i ] = bitset[ i + blocks_shift ];
			}
		}
		else
		{
			const size_t reverse_bits_offset = 32 - bits_offset;
			for ( size_t i = 0; i < last_block_to_shift; ++i )
			{
				bitset[ i ] = uint32_t( ( bitset[ i + blocks_shift ].bits >> bits_offset ) | uint32_t( bitset[ i + blocks_shift + 1 ].bits << reverse_bits_offset ) );
			}
			bitset[ last_block_to_shift ] = uint32_t( bitset[ bitset.size() - 1 ].bits >> bits_offset );
		}

		// set bit that came at the left to 0 in unmodified blocks
		std::fill( std::begin( bitset ) + static_cast<typename decltype( bitset )::difference_type>( last_block_to_shift + 1 ), std::end( bitset ), 0x00000000 );

		this->data_size = this->valid_number_of_bits();

		return *this;
	}

	// 按位左旋转 (<<<=)
	void rotate_left( size_t shift )
	{
		size_t actualSize = bit_size();
		if ( shift == 0 || actualSize == 0 )
		{
			return;
		}

		shift %= actualSize;

		DynamicBitSet temp = this->subset( 0, shift );	// 保存最左侧的 'shift' 位
		left_shift( shift );							// 执行左移操作
		for ( size_t i = 0; i < shift; ++i )
		{
			set_bit( temp.get_bit( i ), actualSize - shift + i );  // 将保存的位移到右侧
		}

		this->data_size = this->valid_number_of_bits();
	}

	// 按位右旋转 (>>>=)
	void rotate_right( size_t shift )
	{
		size_t actualSize = bit_size();
		if ( shift == 0 || actualSize == 0 )
		{
			return;
		}

		shift %= actualSize;

		DynamicBitSet temp = this->subset( actualSize - shift, actualSize );  // 保存最右侧的 'shift' 位
		right_shift( shift );												  // 执行右移操作
		for ( size_t i = 0; i < shift; ++i )
		{
			set_bit( temp.get_bit( i ), i );  // 将保存的位移到左侧
		}

		this->data_size = this->valid_number_of_bits();
	}

	// 计算设置为 true 的位数 (汉明权重)
	size_t hamming_weight() const
	{
		size_t total_count = 0;
		for ( const auto& chunk : bitset )
		{
			total_count += chunk.count_bits();
		}
		return total_count;
	}

	size_t hamming_distance(const DynamicBitSet& other) const
	{
		if (this->data_chunk_count != other.data_chunk_count)
		{
			throw std::invalid_argument("Chunk count must be equal for Hamming distance");
		}

		size_t distance = 0;

		for (size_t i = 0; i < this->data_chunk_count; ++i)
		{
			uint32_t xor_result = this->bitset[i].bits ^ other.bitset[i].bits;

			// 计算 xor_result 中比特1的个数
			distance += BooleanBitWrapper(xor_result).count_bits();
		}

		return distance;
	}

	// for_each函数接口
    template <typename Func>
    void for_each_block(Func func)
	{
		for (size_t i = 0; i < data_chunk_count; ++i)
		{
			func(bitset[i]);
		}
	}

	// 预分配内存
	void reserve( size_t nunber_bit_size )
	{
		bitset.reserve( needed_chunks( nunber_bit_size ) );
		this->data_capacity = bitset.capacity() * 32;
	}

	// 重新分配比特大小 (可能调整 bit chunk 数量)
	void resize( std::size_t update_capacity_and_size, bool fill_bit = false )
	{
		if(data_size == 0 && update_capacity_and_size == 1)
		{
			bitset.push_back(BooleanBitWrapper(fill_bit));

			this->data_capacity = bitset.size() * 32;
			this->data_size = update_capacity_and_size;
			this->data_chunk_count = bitset.size();

			return;
		}
		else if(data_size == 1 && update_capacity_and_size == 0)
		{
			bitset.pop_back();

			this->data_capacity = bitset.size() * 32;
			this->data_size = update_capacity_and_size;
			this->data_chunk_count = bitset.size();

			return;
		}

		std::size_t current_bit_capacity = this->bit_capacity();											// 获取当前比特"虚拟容量"大小
		std::size_t chunk_size_with_update_bit_capacity = this->needed_chunks( update_capacity_and_size );	// 计算需要的BooleanBitWrapper数量

		// 在进行任何更改之前，设置新添加的位在最后一个不完整的块中
		set_unused_bits( fill_bit );

		/*
		// 如果新大小大于当前容量大小，扩展并填充新比特
		if ( update_capacity_and_size > current_bit_capacity )
		{
			bitset.resize( chunk_size_with_update_bit_capacity );  // 调整BooleanBitWrapper的数量
			if ( fill_bit )
			{
				for ( std::size_t i = current_bit_capacity; i < update_capacity_and_size; ++i )
				{
					set_bit( true, i );	 // 现在可以安全地设置新的比特
				}
			}
		}
		// 如果新大小小于当前容量大小，缩减并清除多余的比特
		else if ( update_capacity_and_size < current_bit_capacity )
		{
			for ( std::size_t i = update_capacity_and_size; i < current_bit_capacity; ++i )
			{
				set_bit( false, i );  // 现在可以安全地清除多余的比特
			}
			bitset.resize( chunk_size_with_update_bit_capacity );  // 调整BooleanBitWrapper的数量
		}
		*/
		bitset.resize( chunk_size_with_update_bit_capacity, fill_bit ? BooleanBitWrapper( 0xFFFFFFFF ) : BooleanBitWrapper( 0x00000000 ) );

		this->data_capacity = bitset.size() * 32;
		this->data_size = update_capacity_and_size;
		this->data_chunk_count = bitset.size();
	}

	// 释放多余的内存容量
	void shrink_to_fit()
	{
		bitset.shrink_to_fit();

		this->data_capacity = bitset.size() * 32;
		this->data_size = this->valid_number_of_bits();
		this->data_chunk_count = bitset.size();
	}

	// 清空比特集
	void clear()
	{
		if ( !empty() )
		{
			bitset.clear();
			bitset.shrink_to_fit();

			// 更新成员变量
			this->data_size = 0;
			this->data_chunk_count = 0;
			this->data_capacity = 0;
		}
	}

	// Equality Operator
	bool operator==( const DynamicBitSet& other ) const
	{
		if ( memory_capacity() != other.memory_capacity() )
			return false;
		return (bitset == other.bitset) && (data_chunk_count == other.data_chunk_count);
	}

	// Inequality Operator
	bool operator!=( const DynamicBitSet& other ) const
	{
		return !( *this == other );
	}

	// Bitwise AND Operator
	DynamicBitSet operator&( const DynamicBitSet& other )
	{
		DynamicBitSet result = *this;
		result.and_operation( other );
		return result;
	}

	// Bitwise OR Operator
	DynamicBitSet operator|( const DynamicBitSet& other )
	{
		DynamicBitSet result = *this;
		result.or_operation( other );
		return result;
	}

	// Bitwise NOT Operator
	DynamicBitSet operator~()
	{
		DynamicBitSet result = *this;
		result.not_operation();
		return result;
	}

	// Bitwise XOR Operator
	DynamicBitSet operator^( const DynamicBitSet& other )
	{
		DynamicBitSet result = *this;
		result.xor_operation( other );
		return result;
	}

	// Bitwise AND-Assignment Operator
	DynamicBitSet& operator&=( const DynamicBitSet& other )
	{
		this->and_operation( other );
		return *this;
	}

	// Bitwise OR-Assignment Operator
	DynamicBitSet& operator|=( const DynamicBitSet& other )
	{
		this->or_operation( other );
		return *this;
	}

	// Bitwise XOR-Assignment Operator
	DynamicBitSet& operator^=( const DynamicBitSet& other )
	{
		this->xor_operation( other );
		return *this;
	}

	// Left Shift Operator
	DynamicBitSet operator<<( size_t shift )
	{
		DynamicBitSet result( *this ) ;
		result <<= shift;
		return result;
	}

	// Right Shift Operator
	DynamicBitSet operator>>( size_t shift )
	{
		DynamicBitSet result( *this ) ;
		result >>= shift;
		return result;
	}

	// Left Shift-Assignment Operator
	DynamicBitSet& operator<<=( size_t shift )
	{
		if ( shift != 0 )
		{
			if ( shift >= this->data_size )
			{
				reset();
			}
			else
			{
				left_shift( shift );
				sanitize();	 // unused bits can have changed, reset them to 0
			}
		}
		return *this;
	}

	// Right Shift-Assignment Operator
	DynamicBitSet& operator>>=( size_t shift )
	{
		if ( shift != 0 )
		{
			if ( shift >= this->data_size )
			{
				reset();
			}
			else
			{
				right_shift( shift );
			}
		}
		return *this;
	}

	friend DynamicBitSet operator&( const DynamicBitSet& object, const uint32_t number )
	{
		if(object.data_chunk_count > 0)
		{
			DynamicBitSet result(uint32_t(0));
			result.bitset[0].bits = object.bitset[0].bits & number;
			return result;
		}
	}

	friend void operator&=( DynamicBitSet& object, const uint32_t number )
	{
		if(object.data_chunk_count > 0)
		{
			object.bitset[0].bits &= number;
		}
	}

	friend DynamicBitSet operator|( const DynamicBitSet& object, const uint32_t number )
	{
		if(object.data_chunk_count > 0)
		{
			DynamicBitSet result(uint32_t(0));
			result.bitset[0].bits = object.bitset[0].bits | number;
			return result;
		}
	}

	friend void operator|=( DynamicBitSet& object, const uint32_t number )
	{
		if(object.data_chunk_count > 0)
		{
			object.bitset[0].bits |= number;
		}
	}

	friend DynamicBitSet operator^( const DynamicBitSet& object, const uint32_t number )
	{
		if(object.data_chunk_count > 0)
		{
			DynamicBitSet result(uint32_t(0));
			result.bitset[0].bits = object.bitset[0].bits ^ number;
			return result;
		}
	}

	friend void operator^=( DynamicBitSet& object, const uint32_t number )
	{
		if(object.data_chunk_count > 0)
		{
			object.bitset[0].bits ^= number;
		}
	}

	// Convert to hexadecimal string (raw mode)
	std::vector<std::string> string_hexadecimal_raw_array() const
	{
		std::vector<std::string> hexStrings;
		for (size_t i = 0; i < this->data_chunk_count; ++i)
		{
			std::ostringstream oss;
			oss << std::setfill('0') << std::setw(8) << std::hex << bitset[i].bits;
			hexStrings.push_back(oss.str());
		}
		return hexStrings;
	}

	// Convert to hexadecimal string (hugenumber mode)
	std::string string_hexadecimal_hugenumber() const
	{
		return binary_to_hexadecimal( format_binary_string( false ) );
	}

	// Convert to decimal string (raw mode)
	std::vector<std::string> string_decimal_raw_array() const
	{
		std::vector<std::string> decimalStrings;
		for (size_t i = 0; i < this->data_chunk_count; ++i)
		{
			decimalStrings.push_back(std::to_string(bitset[i].bits));
		}
		return decimalStrings;
	}

	// Convert to decimal string (hugenumber mode)
	std::string string_decimal_hugenumber() const
	{
		std::string binary_string = format_binary_string( true );
		//std::cout << binary_string << std::endl;
		return binary_to_decimal( binary_string );
	}

	// Convert to binary string
	std::string format_binary_string( bool include_leading_zeros = false ) const
	{
		std::string result;
	 
		// 获取总容量
		size_t total_capacity = bit_capacity();

		// 计算需要有效的比特数量
		size_t number_bits = bit_size();

		// 预分配字符串的大小
		result.reserve( total_capacity );

		// 已处理的比特数
		size_t processed_bits = 0;

		// 遍历每个数据块（每个数据块包含32个比特）
		for ( size_t i = 0; i < this->data_chunk_count; ++i )
		{
			// 遍历数据块中的每个比特
			for ( int j = 0; j < 32; ++j )
			{
				// 从数据块中提取比特
				bool bit = ( bitset[ i ].bits >> j ) & 1;

				// 如果已经处理了所有有效的比特，就退出循环
				if ( processed_bits >= number_bits )
				{
					break;
				}

				// 将比特添加到结果字符串中
				result += ( bit ? '1' : '0' );

				// 更新已处理的比特数
				++processed_bits;
			}
		}

		if(result.size() > 1)
			std::reverse(result.begin(), result.end());

		// 如果不包括前导零，并且结果字符串以 '0' 开头，则移除所有前导零
		if ( !include_leading_zeros && !result.empty() && result[ 0 ] == '0' )
		{
			std::string copied;

			auto first_non_zero = result.find_first_not_of( '0' );
			if ( first_non_zero != std::string::npos )
			{
				copied = result.substr( first_non_zero );
			}
			else
			{
				// 如果字符串全是 '0'，则只保留一个 '0'
				copied = "0";
			}

			return copied;
		}

		return result;
	}

private:
	//Bit chunks
	std::vector<BooleanBitWrapper> bitset;

	size_t data_size = 0;
	size_t data_capacity = 0;
	size_t data_chunk_count = 0;

	void sanitize()
	{
		size_t shift = data_size % 32;
		if ( shift > 0 )
		{
			last_block().bit_set( false, shift );
		}
	}

	bool check_unused_bits( int direction ) const noexcept
	{
		const size_t extra_bits = data_size % 32;
		if ( extra_bits > 0 )
		{
			if ( direction == 1 )
			{
				return ( ( last_block().bits & ( 0x00000001 << extra_bits ) ) == 0x00000000 );
			}
			else if ( direction == -1 )
			{
				return ( ( last_block().bits & ( 0xFFFFFFFF << extra_bits ) ) == 0x00000000 );
			}
		}
		return true;
	}

	bool check_size() const noexcept
	{
		return blocks_required( data_size ) == bitset.size();
	}

	bool check_capacity() const noexcept
	{
		return data_capacity == data_chunk_count * 32;
	}

	bool check_consistency( int direction ) const noexcept
	{
		if ( direction == 1 )
		{
			return check_unused_bits( 1 ) && check_size() && check_capacity();
		}
		else if ( direction == -1 )
		{
			return check_unused_bits( -1 ) && check_size() && check_capacity();
		}
		else
		{
			// 乱传参数，报错
			std::cerr << "Invalid direction parameter for check_consistency. Use 1 for back operations and -1 for front operations." << std::endl;
			return false;
		}
	}

	size_t blocks_required( size_t bits ) const
	{
		return ( bits + 31 ) / 32;	// 假设每个数据块有 32 位
	}

	BooleanBitWrapper& last_block()
	{
		return bitset[ bitset.size() - 1 ];
	}

	const BooleanBitWrapper& last_block() const
	{
		return bitset[ bitset.size() - 1 ];
	}

	// 计算需要的BooleanBitWrapper数量
	std::size_t needed_chunks( const std::size_t bit_size ) const noexcept
	{
		return bit_size / 32 + ( bit_size % 32 > 0 );
	}

	// 设置未使用的位
	void set_unused_bits( bool value )
	{
		if ( bitset.empty() )
			return;

		std::size_t bit_pos = bit_size() % 32;	// 假设每个BooleanBitWrapper包含32位

		if ( bit_pos )
		{
			uint32_t  mask = 0xFFFFFFFF << bit_pos;
			uint32_t& data = bitset[ bitset.size() - 1 ].bits;
			if ( value )
			{
				data |= mask;
			}
			else
			{
				data &= ~mask;
			}
		}
	}

	std::string binary_to_hexadecimal( const std::string& binary ) const
	{
		static const char hex_chars[] = "0123456789ABCDEF";
		std::string		  result;
		size_t			  length = binary.size();
		uint32_t		  remainder = length % 4;

		std::string padded_binary = binary;
		if ( remainder > 0 )
		{
			// Pad the binary string to the left with zeros
			padded_binary = std::string( 4 - remainder, '0' ) + binary;
			length += 4 - remainder;
		}

		for ( size_t i = 0; i < length; i += 4 )
		{
			std::string chunk = padded_binary.substr( i, 4 );
			uint32_t	decimal_value = 0;
			for ( char c : chunk )
			{
				decimal_value = ( decimal_value << 1 ) | ( c - '0' );
			}
			result += hex_chars[ decimal_value ];
		}

		return result;
	}

	std::string hexadecimal_to_binary( const std::string& hexadecimal ) const
	{
		std::string binary;
		for ( char hex : hexadecimal )
		{
			uint32_t value;
			if ( '0' <= hex && hex <= '9' )
			{
				value = hex - '0';
			}
			else if ( 'A' <= hex && hex <= 'F' )
			{
				value = hex - 'A' + 10;
			}
			else if ( 'a' <= hex && hex <= 'f' )
			{
				value = hex - 'a' + 10;
			}
			else
			{
				throw std::invalid_argument( "Invalid hexadecimal digit" );
			}
			for ( int i = 3; i >= 0; --i )
			{
				binary += ( value >> i ) & 1 ? '1' : '0';
			}
		}

		// Usually, we are used to put the most significant bit first.
		// Reverse the binary string to get the correct bit order
		std::reverse(binary.begin(), binary.end());
		return binary;
	}

	std::string HighPrecisionNumberAdd( const std::string& a, const std::string& b ) const
	{
		std::string result;
		uint32_t	carry = 0;
		for ( uint32_t i = 0; i < std::max( a.size(), b.size() ); ++i )
		{
			uint32_t sum = carry;
			if ( i < a.size() )
				sum += a[ a.size() - 1 - i ] - '0';
			if ( i < b.size() )
				sum += b[ b.size() - 1 - i ] - '0';
			carry = sum / 10;
			sum = sum % 10;
			result += std::to_string( sum );
		}
		if ( carry )
			result += std::to_string( carry );

		std::reverse( result.begin(), result.end() );
		return result;
	}

	std::string decimal_to_binary( const std::string& decimal ) const
	{
		std::string binary;
		std::string number = decimal;
		while ( number != "0" )
		{
			std::string quotient;
			uint32_t	remainder = 0;
			for ( char digit : number )
			{
				uint32_t current_number = remainder * 10 + ( digit - '0' );
				quotient += ( current_number / 2 ) + '0';
				remainder = current_number % 2;
			}
			binary = std::to_string( remainder ) + binary;

			// Remove leading zeros from quotient
			auto it = quotient.begin();
			while ( it != quotient.end() && *it == '0' )
			{
				++it;
			}
			quotient = std::string( it, quotient.end() );

			// If quotient is empty or all zeros, set number to "0" to exit the loop
			if ( quotient.empty() || std::all_of( quotient.begin(), quotient.end(), []( char c ) { return c == '0'; } ) )
			{
				number = "0";
			}
			else
			{
				number = quotient;
			}
		}

		// Usually, we are used to put the most significant bit first.
		// Reverse the binary string to get the correct bit order
		std::reverse(binary.begin(), binary.end());

		return binary.empty() ? "0" : binary;
	}

	std::string binary_to_decimal( const std::string& binary ) const
	{
		std::string decimal = "0";
		for ( const char bit : binary )
		{
			decimal = HighPrecisionNumberAdd( decimal, decimal );  // equivalent to decimal *= 2
			if ( bit == '1' )
			{
				decimal = HighPrecisionNumberAdd( decimal, "1" );  // equivalent to decimal += 1
			}
		}
		return decimal;
	}
};

inline void testBooleanBitWrapper()
{
	BooleanBitWrapper wrapper;

	// Test bit_set and bit_get
	wrapper.bit_set( true, 0 );
	assert( wrapper.bit_get( 0 ) == true );

	wrapper.bit_set( false, 0 );
	assert( wrapper.bit_get( 0 ) == false );

	// Test bit_and
	wrapper.bits = 0b00000000000000000000000000001010;
	wrapper.bit_and( 0b00000000000000000000000000001100 );
	assert( wrapper.bits == 0b00000000000000000000000000001000 );

	// Test bit_or
	wrapper.bits = 0b1010;
	wrapper.bit_or( 0b00000000000000000000000000001100 );
	assert( wrapper.bits == 0b00000000000000000000000000001110 );

	// Test bit_not
	wrapper.bits = 0b1010;
	wrapper.bit_not();
	assert( wrapper.bits == 0b11111111111111111111111111110101 );

	// Test bit_leftshift
	wrapper.bits = 0b00000000000000000000000000000001;
	wrapper.bit_leftshift( 2 );
	assert( wrapper.bits == 0b00000000000000000000000000000100 );

	// Test bit_rightshift
	wrapper.bits = 0b00000000000000000000000000000100;
	wrapper.bit_rightshift( 2 );
	assert( wrapper.bits == 0b00000000000000000000000000000001 );

	std::cout << "All BooleanBitWrapper tests passed!\n";
}

inline void testDynamicBitSet()
{
	// 创建一个 DynamicBitSet 实例并初始化一些值
	DynamicBitSet dbset;
	dbset.push_back( true );
	dbset.push_back( false );
	dbset.push_back( true );
	dbset.push_back( false );

	// 测试正向迭代器
	std::cout << "Testing forward iterator:" << std::endl;
	for ( auto it = dbset.begin(); it != dbset.end(); ++it )
	{
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	// 测试反向迭代器
	std::cout << "Testing reverse iterator:" << std::endl;
	for ( auto it = dbset.rbegin(); it != dbset.rend(); ++it )
	{
		std::cout << *it << " ";
	}
	std::cout << std::endl;

	DynamicBitSet dbset1;

	// 初始化测试
	assert( dbset1.bit_size() == 0 );
	std::cout << "Initialization test passed!\n";

	// 使用 push_back 添加一系列比特
	for ( int i = 0; i < 128; ++i )
	{
		dbset1.push_back( i % 2 == 0 );
		std::cout << "After push_back: " << dbset1.format_binary_string( true ) << std::endl;
		assert( dbset1.bit_size() == dbset1.valid_number_of_bits() );
	}

	// 使用 pop_back 减少一系列比特
	for ( int i = 128; i > 0; --i )
	{
		dbset1.pop_back();
		assert( dbset1.bit_size() == dbset1.valid_number_of_bits() );
		std::cout << "After pop_back: " << dbset1.format_binary_string( true ) << std::endl;
	}

	// 使用 push_front 添加一系列比特
	for ( int i = 0; i < 64; ++i )
	{
		dbset1.push_front(i);
		std::cout << "After push_front: " << dbset1.format_binary_string( true ) << std::endl;
		assert( dbset1.bit_size() == dbset1.valid_number_of_bits() );
	}

	// 使用 pop_front 减少一系列比特
	for ( int i = 64; i > 0; --i )
	{
		dbset1.pop_front();
		assert( dbset1.bit_size() == dbset1.valid_number_of_bits() );
		std::cout << "After pop_front: " << dbset1.format_binary_string( true ) << std::endl;
	}

	//最低有效位（LSB）是在最前面{(Bitchunk[0] >> 0) & 1}，而最高有效位（MSB）是在最后面{(Bitchunk[Bitchunk.size() - 1] >> 32 - 1) & 1)}

	DynamicBitSet dbset2;

	// 填充 40 个比特，交替为 0 和 1
	for ( int i = 0; i < 40; ++i )
	{
		dbset2.push_back( i % 2 == 0 );
	}
	std::cout << "Result: " << dbset2.format_binary_string( true ) << std::endl;
	assert( dbset2.format_binary_string( true ) == "1010101010101010101010101010101010101010" );

	/*测试正向版本的插入和擦除 insert_bit 和 erase_bit*/

	/* 测试非法操作*/
	try
	{
		dbset2.insert( false, 39 );	 // 尝试在 MSB 位置插入一个 0
	}
	catch ( const std::invalid_argument& e )
	{
		std::cout << "Exception caught: " << e.what() << std::endl;
	}
	std::cout << "Result: " << dbset2.format_binary_string( true ) << std::endl;

	/* 测试合法操作*/

	// 在 LSB+1 位置插入一个 1
	dbset2.insert( true, 1 );
	std::cout << "Insert Result: " << dbset2.format_binary_string( true ) << std::endl;
	assert( dbset2.format_binary_string( true ) == "010101010101010101010101010101010101010101" );

	// 删除 LSB
	dbset2.erase( 0 );
	std::cout << "Erase Result: " << dbset2.format_binary_string( true ) << std::endl;
	assert( dbset2.format_binary_string( true ) == "01010101010101010101010101010101010101010" );

	// 删除 MSB
	dbset2.erase( dbset2.bit_size() - 1 );
	std::cout << "Erase Result: " << dbset2.format_binary_string( true ) << std::endl;
	assert( dbset2.format_binary_string( true ) == "1010101010101010101010101010101010101010" );

	dbset2.clear();

	/*测试反向版本的插入和擦除 reverse_insert_bit 和 reverse_erase_bit*/

	/* 测试非法操作*/
	try
	{
		dbset2.reverse_insert( false, 0 );	// 尝试在 MSB 位置插入一个 0
	}
	catch ( const std::invalid_argument& e )
	{
		std::cout << "Exception caught: " << e.what() << std::endl;
	}
	std::cout << "Result: " << dbset2.format_binary_string( true ) << std::endl;

	/* 测试合法操作*/

	// 在 MSB-1 位置插入一个 1
	dbset2.reverse_insert( true, 1 );
	std::cout << "Reverse_Insert Result: " << dbset2.format_binary_string( true ) << std::endl;

	// 在 MSB 位置插入一个 1
	dbset2.reverse_insert( true, 0 );
	std::cout << "Reverse_Insert Result: " << dbset2.format_binary_string( true ) << std::endl;

	// 删除 MSB-1
	dbset2.reverse_erase( 1 );
	std::cout << "Reverse_Erase Result: " << dbset2.format_binary_string( true ) << std::endl;

	// 删除 MSB
	dbset2.reverse_erase( 0 );
	std::cout << "Reverse_Erase Result: " << dbset2.format_binary_string( true ) << std::endl;

	// 使用字符串初始化 DynamicBitSet 对象
	DynamicBitSet dbset3("1010101010101010");
	DynamicBitSet dbset4("1100110011001100");

	// 输出初始状态
	std::cout << "Initial dbset3: " << dbset3.format_binary_string(true) << std::endl;
	std::cout << "Initial dbset4: " << dbset4.format_binary_string(true) << std::endl;

	// 测试 and_operation
	dbset3.and_operation(dbset4);
	std::cout << "After and_operation: " << dbset3.format_binary_string(true) << std::endl;  // 预期输出：1000100010001000

	// 重新初始化 dbset3
	dbset3 = DynamicBitSet("1010101010101010");

	// 测试 or_operation
	dbset3.or_operation(dbset4);
	std::cout << "After or_operation: " << dbset3.format_binary_string(true) << std::endl;  // 预期输出：1110111011101110

	// 重新初始化 dbset3 和 dbset4
	dbset3 = DynamicBitSet("1010101010101010");
	dbset4 = DynamicBitSet("1100110011001100");

	// 测试 not_operation
	dbset3.not_operation();
	dbset4.not_operation();
	std::cout << "After not_operation on dbset3: " << dbset3.format_binary_string(true) << std::endl;  // 预期输出：0101010101010101
	std::cout << "After not_operation on dbset4: " << dbset4.format_binary_string(true) << std::endl;  // 预期输出：0011001100110011

	std::cout << "All DynamicBitSet tests passed!\n";
}

inline void testConstructors()
{
	// 测试默认构造函数
	DynamicBitSet db1;
	assert( db1.bit_size() == 0 );

	// 测试带有初始容量和填充位的构造函数
	DynamicBitSet db2( 5, true );
	assert( db2.bit_size() == 5 );
	assert( db2.get_bit( 0 ) == true );
	assert( db2.get_bit( 4 ) == true );

	// 测试从 std::vector<bool> 构造
	std::vector<bool> vec = { true, false, true };
	DynamicBitSet	  db3( vec );
	assert( db3.bit_size() == 3 );
	assert( db3.get_bit( 0 ) == true );
	assert( db3.get_bit( 1 ) == false );
	assert( db3.get_bit( 2 ) == true );

	// 测试从二进制字符串构造
	DynamicBitSet db4( "1101", 2 );
	assert( db4.bit_size() == 4 );
	assert( db4.get_bit( 0 ) == true );
	assert( db4.get_bit( 1 ) == false );
	assert( db4.get_bit( 2 ) == true );
	assert( db4.get_bit( 3 ) == true );

	// 测试从十进制字符串构造
	DynamicBitSet db5( "13", 10 );
	assert( db5.bit_size() == 4 );
	assert( db5.get_bit( 0 ) == true );
	assert( db5.get_bit( 1 ) == false );
	assert( db5.get_bit( 2 ) == true );
	assert( db5.get_bit( 3 ) == true );

	// 测试从十六进制字符串构造
	DynamicBitSet db6( "D", 16 );
	assert( db6.bit_size() == 4 );
	assert( db6.get_bit( 0 ) == true );
	assert( db6.get_bit( 1 ) == false );
	assert( db6.get_bit( 2 ) == true );
	assert( db6.get_bit( 3 ) == true );

	// 测试从 uint32_t 构造
	DynamicBitSet db7( ( uint32_t )13 );
	assert( db7.bit_size() == 4 );
	assert( db7.get_bit( 0 ) == true );
	assert( db7.get_bit( 1 ) == false );
	assert( db7.get_bit( 2 ) == true );
	assert( db7.get_bit( 3 ) == true );

	// 测试从 std::vector<uint32_t> 构造
	std::vector<uint32_t> vec32 = { 13, 7 };
	DynamicBitSet		  db8( vec32 );
	std::cout << "Result: " << db8.format_binary_string( true ) << std::endl;
	assert( db8.bit_size() == 35 );
	assert( db8.get_bit( 0 ) == true );
	assert( db8.get_bit( 1 ) == false );
	assert( db8.get_bit( 2 ) == true );
	assert( db8.get_bit( 3 ) == true );
	assert( db8.get_bit( 31 ) == false );
	assert( db8.get_bit( 32 ) == true );
	assert( db8.get_bit( 33 ) == true );
	assert( db8.get_bit( 34 ) == true );

	// 测试从 uint64_t 构造
	DynamicBitSet db9( ( uint64_t )13 );
	assert( db9.bit_size() == 4 );
	assert( db9.get_bit( 0 ) == true );
	assert( db9.get_bit( 1 ) == false );
	assert( db9.get_bit( 2 ) == true );
	assert( db9.get_bit( 3 ) == true );

	// 测试从 std::vector<uint64_t> 构造
	std::vector<uint64_t> vec64 = { 0b0100010000000100000000000100010000001000000000001000100001000100, 0b1000000001000000000001000000000110000000000100000000010000000010 };
	DynamicBitSet		  db10( vec64 );
	std::cout << "Result: " << db10.format_binary_string( true ) << std::endl;

	std::cout << "All constructor tests passed!\n";
}

inline void testConversions()
{
	std::string bigBinaryString = "1100101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101";

	DynamicBitSet db_big(bigBinaryString, 2);

	// 测试转换为二进制字符串
	assert(db_big.format_binary_string( true ) == bigBinaryString);
	assert(db_big.format_binary_string( false ) == "1100101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101");

	// 测试转换为十六进制字符串（原始模式）
	if(db_big.string_hexadecimal_raw_array() != std::vector<std::string>{"55555555", "55555555", "55555555", "65555555"})
		assert(false);

	// 测试转换为十进制字符串（原始模式）
	if(db_big.string_decimal_raw_array() != std::vector<std::string>{"1431655765", "1431655765", "1431655765", "1700091221"})
		assert(false);

	// 测试转换为十六进制字符串（大数模式）
	assert(db_big.string_hexadecimal_hugenumber() == "65555555555555555555555555555555");

	// 测试转换为十进制字符串（大数模式）
	assert(db_big.string_decimal_hugenumber() == "134695103572871475120919115441741583701");

	std::string	  long_binary = "000000000000000000000000000000010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010";
	DynamicBitSet db1( long_binary, 2 );
	std::string	  output = db1.format_binary_string( true );
	std::cout << "input:  " << long_binary << std::endl;
	std::cout << "output: " << output << std::endl;
	assert( output == long_binary );
	std::cout << "Test for long binary string passed." << std::endl;

	std::string	  long_decimal = "1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890";
	DynamicBitSet db2( long_decimal, 10 );
	output = db2.string_decimal_hugenumber();
	std::cout << "input: " << long_decimal << std::endl;
	std::cout << "output: " << output << std::endl;
	assert( output == long_decimal );
	std::cout << "Test for long decimal string passed." << std::endl;

	std::string	  long_hex = "ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890ABCDEF1234567890";
	DynamicBitSet db3( long_hex, 16 );
	output = db3.string_hexadecimal_hugenumber();
	std::cout << "input: " << long_hex << std::endl;
	std::cout << "output: " << output << std::endl;
	assert( output == long_hex );
	std::cout << "Test for long hexadecimal string passed." << std::endl;

	std::cout << "All conversion tests passed!\n";
}

inline void testLargeData()
{
	// 测试大规模数据
	const size_t  largeSize = 1e6;	// 例如，一百万位
	DynamicBitSet largeDb( largeSize, true );

	// 确保所有位都设置为 true
	for ( size_t i = 0; i < largeSize; ++i )
	{
		assert( largeDb.get_bit( i ) == true );
	}

	// 测试性能：设置所有位为 false
	auto start = std::chrono::high_resolution_clock::now();
	for ( size_t i = 0; i < largeSize; ++i )
	{
		largeDb.set_bit( false, i );
	}
	auto						  end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "Time taken to set " << largeSize << " bits to false: " << elapsed.count() << "s\n";

	// 确保所有位都设置为 false
	for ( size_t i = 0; i < largeSize; ++i )
	{
		assert( largeDb.get_bit( i ) == false );
	}

	// 测试从大规模 std::vector<uint64_t> 构造
	std::vector<uint64_t> largeVec( largeSize / 64, 0xFFFFFFFFFFFFFFFF );
	DynamicBitSet		  largeDb2( largeVec );

	// 确保所有位都设置为 true
	for ( size_t i = 0; i < largeSize; ++i )
	{
		assert( largeDb2.get_bit( i ) == true );
	}

	//std::cout << "largeDb: " << largeDb.format_binary_string( true ) << std::endl;
	//std::cout << "largeDb2: " << largeDb2.format_binary_string( true ) << std::endl;

	std::cout << "All large data tests passed!\n";
}

inline void testRandomData()
{
	std::random_device				rd;
	std::mt19937					gen( rd() );
	std::uniform_int_distribution<> dis( 0, 1 );

	const size_t	  size = 1000;	// 例如，一千位
	std::vector<bool> randomBits( size );

	// 生成随机数据
	for ( size_t i = 0; i < size; ++i )
	{
		randomBits[ i ] = dis( gen );
	}

	// 使用随机数据构建 DynamicBitSet
	DynamicBitSet db( randomBits );

	// 验证 DynamicBitSet 中的数据与原始随机数据匹配
	for ( size_t i = 0; i < size; ++i )
	{
		assert( db.get_bit( i ) == randomBits[ i ] );
	}

	// 打印不同的格式
	std::cout << "Binary (Raw Array): " << db.format_binary_string( true ) << std::endl;
	
	// 打印十六进制原始数组
	std::vector<std::string> hexRawArray = db.string_hexadecimal_raw_array();
	std::cout << "Hexadecimal (Raw Array): ";
	for (const auto& hexStr : hexRawArray) {
		std::cout << hexStr << " ";
	}
	std::cout << std::endl;

	// 打印十进制原始数组
	std::vector<std::string> decRawArray = db.string_decimal_raw_array();
	std::cout << "Decimal (Raw Array): ";
	for (const auto& decStr : decRawArray) {
		std::cout << decStr << " ";
	}
	std::cout << std::endl;

	std::cout << "Hexadecimal (Huge Number): " << db.string_hexadecimal_hugenumber() << std::endl;
	std::cout << "Decimal (Huge Number): " << db.string_decimal_hugenumber() << std::endl;

	std::cout << "All random data tests passed!\n";
}

struct BinaryStringOperation
{
	// 填充前导零以使两个字符串长度相同
	void PadLeadingZeros(std::string &str1, std::string &str2) {
		int len1 = str1.length();
		int len2 = str2.length();
		
		if (len1 < len2) {
			str1.insert(0, len2 - len1, '0');
		} else if (len1 > len2) {
			str2.insert(0, len1 - len2, '0');
		}
	}

	std::string bitstring_and(const std::string& a, const std::string& b) {
		std::string result;

		for (size_t i = 0; i < a.length(); ++i) {
			result.push_back((a[i] == '1' && b[i] == '1') ? '1' : '0');
		}
		return result;
	}

	std::string bitstring_or(const  std::string& a, const std::string& b) {
		
		std::string result;

		for (size_t i = 0; i < a.length(); ++i) {
			result.push_back((a[i] == '1' || b[i] == '1') ? '1' : '0');
		}
		return result;
	}

	std::string bitstring_not(const std::string& a) {
		std::string result;
		for (char c : a) {
			result.push_back(c == '1' ? '0' : '1');
		}
		return result;
	}

	std::string bitstring_xor(const  std::string& a, const  std::string& b) {
		std::string result;

		for (size_t i = 0; i < a.length(); ++i) {
			result.push_back( (a[i] != b[i]) ? '1' : '0');
		}
		return result;
	}
};

inline void testOperatorsAndModifications()
{
	// 测试构造函数和赋值运算符
	DynamicBitSet BigNumberA("125479658432147853691427564979735125197475847675544949173461857", 10);
	DynamicBitSet BigNumberB("547851144862556526651496461641321749161654982131654951649816549", 10);

	assert(BigNumberA == DynamicBitSet(BigNumberA));

	// 测试位运算符
	
	auto A_bits = BigNumberA.format_binary_string( true );
	auto B_bits = BigNumberB.format_binary_string( true );

	std::cout << "A : " << BigNumberA.string_decimal_hugenumber() << std::endl;
	std::cout << "A (binary): " << A_bits << std::endl;
	std::cout << "B : " << BigNumberB.string_decimal_hugenumber() << std::endl;
	std::cout << "B (binary): " << B_bits << std::endl;

	#if 0
	BinaryStringOperation BitwiseOperator;
	BitwiseOperator.PadLeadingZeros(A_bits, B_bits);
	auto AND_bits = BitwiseOperator.bitstring_and(A_bits, B_bits);
	auto OR_bits = BitwiseOperator.bitstring_or(A_bits, B_bits);
	auto A_NOT_bits = BitwiseOperator.bitstring_not(A_bits);
	auto B_NOT_bits = BitwiseOperator.bitstring_not(B_bits);
	auto XOR_bits = BitwiseOperator.bitstring_xor(A_bits, B_bits);
	#endif

	assert(BigNumberA.hamming_weight() == 90);
	assert(BigNumberB.hamming_weight() == 100);
	
	DynamicBitSet ANDAND = BigNumberA & BigNumberB;
	std::cout << "ANDAND : " << ANDAND.string_decimal_hugenumber() << std::endl;
	std::cout << "ANDAND (binary): " << ANDAND.format_binary_string() << std::endl;

	DynamicBitSet OROR = BigNumberA | BigNumberB;
	std::cout << "OROR : " << OROR.string_decimal_hugenumber() << std::endl;
	std::cout << "OROR (binary): " << OROR.format_binary_string() << std::endl;

	DynamicBitSet NOTNOTA = ~BigNumberA;
	DynamicBitSet NOTNOTB = ~BigNumberB;
	std::cout << "NOTNOTA : " << NOTNOTA.string_decimal_hugenumber() << std::endl;
	std::cout << "NOTNOTA (binary): " << NOTNOTA.format_binary_string() << std::endl;

	std::cout << "NOTNOTB : " << NOTNOTB.string_decimal_hugenumber() << std::endl;
	std::cout << "NOTNOTB (binary): " << NOTNOTB.format_binary_string() << std::endl;

	DynamicBitSet XORXOR = BigNumberA ^ BigNumberB;
	std::cout << "XORXOR : " << XORXOR.string_decimal_hugenumber() << std::endl;
	std::cout << "XORXOR (binary): " << XORXOR.format_binary_string() << std::endl;

	// 测试修改操作
	BigNumberA.set_bit(0, false);
	assert(BigNumberA.get_bit(0) == false);

	BigNumberA.flip(0);
	assert(BigNumberA.get_bit(0) == true);

	// 测试其他功能，比如子串、连接等
	DynamicBitSet AA = BigNumberA.subset(0, 16);
	DynamicBitSet BB = BigNumberB.subset(16, 32);
	DynamicBitSet CC = bitset_concat(AA, BB);
	assert(!CC.empty() && CC.bit_size() <= 32);

	std::cout << "A compare B hamming distance:" << BigNumberA.hamming_distance(BigNumberB) << std::endl;;

	BigNumberA.clear();
	assert(BigNumberA.hamming_weight() == 0);
	BigNumberB.clear();
	assert(BigNumberB.hamming_weight() == 0);

	DynamicBitSet BigNumberC("0000000010101001100100010011010010010101011001010101010010101010101101011001100110011001101001011001100011100110011011010011101010111");
	std::cout << "C (include_leading_zeros = true): " << BigNumberC.format_binary_string(true) << std::endl;
	std::cout << "C (include_leading_zeros = false): " << BigNumberC.format_binary_string(false) << std::endl;

	std::cout << "All operator and modification tests passed!\n";
}

//void test_long_uint32_vector()
//{
//	std::vector<uint32_t> long_vector( 1000, 4294967295 );	// 1000个全为1的32位整数
//	DynamicBitSet		  db( long_vector );
//	std::vector<uint32_t> output = db.to_uint32_vector();  // 假设你有这样一个方法
//	assert( output == long_vector );
//	std::cout << "Test for long std::vector<uint32_t> passed." << std::endl;
//}

inline void AllTestBitset()
{
	testBooleanBitWrapper();
	testDynamicBitSet();
	testConstructors();
	testConversions();
	testLargeData();
	testRandomData();
	testOperatorsAndModifications();
}