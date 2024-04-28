#include "ZUC.hpp"

namespace ChinaShangYongMiMa::ZUC
{
	ZUC::ZUC( const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv )
	{
		AlgorithmInitialize( key, iv );
	}

	void ZUC::bit_reorganization()
	{
		X[ 0 ] = ( ( lfsr[ 15 ] & 0x7FFF8000 ) << 1 ) | ( lfsr[ 14 ] & 0x0000FFFF ); //Hight16Bit Low16Bit
		X[ 1 ] = ( ( lfsr[ 11 ] & 0x0000FFFF ) << 16 ) | ( lfsr[ 9 ] >> 15 ); //Low16Bit Hight16Bit
		X[ 2 ] = ( ( lfsr[ 7 ] & 0x0000FFFF ) << 16 ) | ( lfsr[ 5 ] >> 15 ); //Low16Bit Hight16Bit
		X[ 3 ] = ( ( lfsr[ 2 ] & 0x0000FFFF ) << 16 ) | ( lfsr[ 0 ] >> 15 ); //Low16Bit Hight16Bit
	}

	void ZUC::lfsr_initialize( uint32_t u )
	{
		uint32_t v = lfsr_next();
		if(v == 0)
			v = 2147483647;

		lfsr_append( addition_uint31(v, u ) );
	}

	void ZUC::lfsr_work()
	{
		uint32_t lfsr_16 = lfsr_next();

		if(lfsr_16 == 0)
			lfsr_16 = 2147483647;

		lfsr_append( lfsr_16 );
	}

	uint32_t ZUC::lfsr_next()
	{
		uint32_t f = lfsr[ 0 ];
		f = addition_uint31( f, rotl_uint31( lfsr[ 0 ], 8 ) );
		f = addition_uint31( f, rotl_uint31( lfsr[ 4 ], 20 ) );
		f = addition_uint31( f, rotl_uint31( lfsr[ 10 ], 21 ) );
		f = addition_uint31( f, rotl_uint31( lfsr[ 13 ], 17 ) );
		f = addition_uint31( f, rotl_uint31( lfsr[ 15 ], 15 ) );
		return f;
	}
	
	void ZUC::lfsr_append( uint32_t f )
	{
		// Move elements to the left to simulate append
		for ( size_t i = 0; i < 15; ++i )
		{
			lfsr[ i ] = lfsr[ i + 1 ];
		}
		lfsr[ 15 ] = f;
	}

	uint32_t ZUC::f()
	{
		uint32_t Y = ( ( X[ 0 ] ^ r[ 0 ] ) + r[ 1 ] ) & 0xFFFFFFFF;
		uint32_t W1 = ( r[ 0 ] + X[ 1 ] ) & 0xFFFFFFFF;
		uint32_t W2 = r[ 1 ] ^ X[ 2 ];

		uint32_t u = l1( ( ( W1 & 0x0000FFFF ) << 16 ) | ( W2 >> 16 ) );
		uint32_t v = l2( ( ( W2 & 0x0000FFFF ) << 16 ) | ( W1 >> 16 ) );
		
		r[ 0 ] = make_uint32( S0[ u >> 24 ], S1[ ( u >> 16 ) & 0xFF ], S0[ ( u >> 8 ) & 0xFF ], S1[ u & 0xFF ] );
		r[ 1 ] = make_uint32( S0[ v >> 24 ], S1[ ( v >> 16 ) & 0xFF ], S0[ ( v >> 8 ) & 0xFF ], S1[ v & 0xFF ] );
		
		return Y;
	}

	void ZUC::AlgorithmInitialize( const std::vector<uint8_t>& key, const std::vector<uint8_t>& iv )
	{
		// Expand Key
		for ( size_t i = 0; i < 16; ++i )
		{
			lfsr[ i ] = make_uint31( key[ i ], D[ i ], iv[ i ] );
		}

		//Reset Memory Registers
		r[ 0 ] = 0;
		r[ 1 ] = 0;

		/* Algorithm Initialize */
		for ( int i = 0; i < 32; ++i )
		{
			bit_reorganization();
			uint32_t W = f();
			lfsr_initialize( W >> 1 );
		}
	}

	std::vector<uint32_t> ZUC::AlgorithmGenerateKeystream( size_t length )
	{
		std::vector<uint32_t> keystream_buffer;
		
		/* Algorithm Work */
		// Discard the output of F.
		bit_reorganization();
		f();
		lfsr_work();

		/* Algorithm Generate Keystream */

		auto generate_keystream = [ this ]()
		{
			bit_reorganization();
			uint32_t Z = f() ^ X[ 3 ];
			lfsr_work();
			return Z;
		};

		for ( size_t i = 0; i < length; ++i )
		{
			keystream_buffer.push_back( generate_keystream() );
		}
		return keystream_buffer;
	}

	std::vector<uint8_t> ZUC::AlgorithmEncrypt( const std::vector<uint8_t>& input )
	{
		size_t				 length = input.size();
		auto				 key_stream = AlgorithmGenerateKeystream( length );
		std::vector<uint8_t> output;
		for ( size_t i = 0; i < length; ++i )
		{
			output.push_back( input[ i ] ^ ( key_stream[ i ] & 0xFF ) );
		}
		return output;
	}
}  // namespace ChinaShangYongMiMa::ZUC