#include "SM4.hpp"

namespace ChinaShangYongMiMa::SM4
{
	void SM4::key_ext( const std::array<uint8_t, key_size>& key, std::array<uint32_t, 32>& rk, bool encrypt )
	{
		std::array<uint32_t, 36> k;

		// 初始密钥
		k[ 0 ] = ( static_cast<uint32_t>( key[ 0 ] ) << 24 ) | ( static_cast<uint32_t>( key[ 1 ] ) << 16 ) | ( static_cast<uint32_t>( key[ 2 ] ) << 8 ) | ( static_cast<uint32_t>( key[ 3 ] ) );
		k[ 1 ] = ( static_cast<uint32_t>( key[ 4 ] ) << 24 ) | ( static_cast<uint32_t>( key[ 5 ] ) << 16 ) | ( static_cast<uint32_t>( key[ 6 ] ) << 8 ) | ( static_cast<uint32_t>( key[ 7 ] ) );
		k[ 2 ] = ( static_cast<uint32_t>( key[ 8 ] ) << 24 ) | ( static_cast<uint32_t>( key[ 9 ] ) << 16 ) | ( static_cast<uint32_t>( key[ 10 ] ) << 8 ) | ( static_cast<uint32_t>( key[ 11 ] ) );
		k[ 3 ] = ( static_cast<uint32_t>( key[ 12 ] ) << 24 ) | ( static_cast<uint32_t>( key[ 13 ] ) << 16 ) | ( static_cast<uint32_t>( key[ 14 ] ) << 8 ) | ( static_cast<uint32_t>( key[ 15 ] ) );

		// 与系统参数 FK 异或
		k[ 0 ] ^= fk[ 0 ];
		k[ 1 ] ^= fk[ 1 ];
		k[ 2 ] ^= fk[ 2 ];
		k[ 3 ] ^= fk[ 3 ];

		// 计算子密钥 rk
		for ( std::size_t i = 0; i < 32; ++i )
		{
			k[ i + 4 ] = k[ i ] ^ l2( byte_sub( k[ i + 1 ] ^ k[ i + 2 ] ^ k[ i + 3 ] ^ ck[ i ] ) );
			rk[ i ] = k[ i + 4 ];
		}

		// 如果是解密操作，反转子密钥顺序
		if ( !encrypt )
		{
			std::reverse( rk.begin(), rk.end() );
		}
	}

	void SM4::crypt( const std::array<uint8_t, block_size>& input, std::array<uint8_t, block_size>& output, const std::array<uint32_t, 32>& rk )
	{
		uint32_t x0, x1, x2, x3;
		x0 = ( static_cast<uint32_t>( input[ 0 ] ) << 24 ) | ( static_cast<uint32_t>( input[ 1 ] ) << 16 ) | ( static_cast<uint32_t>( input[ 2 ] ) << 8 ) | static_cast<uint32_t>( input[ 3 ] );
		x1 = ( static_cast<uint32_t>( input[ 4 ] ) << 24 ) | ( static_cast<uint32_t>( input[ 5 ] ) << 16 ) | ( static_cast<uint32_t>( input[ 6 ] ) << 8 ) | static_cast<uint32_t>( input[ 7 ] );
		x2 = ( static_cast<uint32_t>( input[ 8 ] ) << 24 ) | ( static_cast<uint32_t>( input[ 9 ] ) << 16 ) | ( static_cast<uint32_t>( input[ 10 ] ) << 8 ) | static_cast<uint32_t>( input[ 11 ] );
		x3 = ( static_cast<uint32_t>( input[ 12 ] ) << 24 ) | ( static_cast<uint32_t>( input[ 13 ] ) << 16 ) | ( static_cast<uint32_t>( input[ 14 ] ) << 8 ) | static_cast<uint32_t>( input[ 15 ] );

		for ( std::size_t r = 0; r < 32; r += 4 )
		{
			uint32_t mid = x1 ^ x2 ^ x3 ^ rk[ r ];
			mid = byte_sub( mid );
			x0 ^= l1( mid );

			mid = x2 ^ x3 ^ x0 ^ rk[ r + 1 ];
			mid = byte_sub( mid );
			x1 ^= l1( mid );

			mid = x3 ^ x0 ^ x1 ^ rk[ r + 2 ];
			mid = byte_sub( mid );
			x2 ^= l1( mid );

			mid = x0 ^ x1 ^ x2 ^ rk[ r + 3 ];
			mid = byte_sub( mid );
			x3 ^= l1( mid );
		}

		output[ 0 ] = static_cast<uint8_t>( x3 >> 24 );
		output[ 1 ] = static_cast<uint8_t>( x3 >> 16 );
		output[ 2 ] = static_cast<uint8_t>( x3 >> 8 );
		output[ 3 ] = static_cast<uint8_t>( x3 );
		output[ 4 ] = static_cast<uint8_t>( x2 >> 24 );
		output[ 5 ] = static_cast<uint8_t>( x2 >> 16 );
		output[ 6 ] = static_cast<uint8_t>( x2 >> 8 );
		output[ 7 ] = static_cast<uint8_t>( x2 );
		output[ 8 ] = static_cast<uint8_t>( x1 >> 24 );
		output[ 9 ] = static_cast<uint8_t>( x1 >> 16 );
		output[ 10 ] = static_cast<uint8_t>( x1 >> 8 );
		output[ 11 ] = static_cast<uint8_t>( x1 );
		output[ 12 ] = static_cast<uint8_t>( x0 >> 24 );
		output[ 13 ] = static_cast<uint8_t>( x0 >> 16 );
		output[ 14 ] = static_cast<uint8_t>( x0 >> 8 );
		output[ 15 ] = static_cast<uint8_t>( x0 );
	}
}