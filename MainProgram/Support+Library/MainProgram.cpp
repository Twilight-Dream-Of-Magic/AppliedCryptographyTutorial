#include "../Support+Library/Support-Library.hpp"

//namespace CommonToolkit
#include "CommonToolkit.hpp"

//namespace CommmonSecurity
#include "../../RandomNumberGenerators/SeedAndDistribution.hpp"

//namespace RandomNumberGenerators
#include "../../RandomNumberGenerators/PseudoRandomNumberGenerators.hpp"

//namespace StreamDataCryption
#include "../../StreamDataCryption/StreamDataCryption.hpp"
#include "../../StreamDataCryption/ChinaShangYongMiMa/ZUC.hpp"

//namespace BlockDataCryption
#include "../../BlockDataCryption/BlockDataCryption.hpp"

#include "UnitTester.hpp"
//#include "DynamicBitset.hpp"
#include "TitianWallCiphers.hpp"

auto main(int argument_cout, char* argument_vector[]) -> int
{
	std::system("chcp 65001");

	ChinaShangYongMiMa::ZUC::TestZUC();

	//BlockDataCryption::TinyEncryptionAlgorithmGroup::UnitTest();
	//TestTitanWallBlockCipher();
	//TestTitanWallStreamCipher();
	//AllTestBitset();

	#if 0

	StreamDataCryption::ISAAC::ISAAC_32Bit ISAAC;
	
	std::string TestMessage = "a Top Secret secret";
	const std::string ConstantTestMessage = TestMessage;
	std::cout << TestMessage << std::endl;
	
	//XOR Encrypt
	ISAAC.Seed("this is my secret key", true);
	for( auto& Character : TestMessage )
	{
		//Generate a random character in printable ASCII range
		Character ^= (ISAAC.Generate() % 95 + 32);
	}

	for (uint32_t i = 0; i < TestMessage.size(); i++) 
		std::cout << std::setfill ('0') << std::setw(2) << std::hex << (int) TestMessage[i];
	std::cout << '\n' << TestMessage << std::endl;

	//XOR Decrypt
	ISAAC.Seed("this is my secret key", true);
	for( auto& Character : TestMessage )
	{
		//Generate a random character in printable ASCII range
		Character ^= (ISAAC.Generate() % 95 + 32);
	}

	for (uint32_t i = 0; i < TestMessage.size(); i++) 
		std::cout << std::setfill ('0') << std::setw(2) << std::hex << (int) TestMessage[i];
	std::cout << '\n' << TestMessage << std::endl;

	#endif

	#if 0

	if constexpr(false)
	{
		/*
			a, b, c, d ∈ FiniteField(2, 32)
			1475, 308611737, 1246536229, 3695 ∈ FiniteField(2, 32)

			b' = b ⊕ ((a + d) <<< 7)
			c' = c ⊕ ((a + b') <<< 9)
			d' = d ⊕ ((c' + b') <<< 13)
			a' = a ⊕ ((c' + d') <<< 18)
		*/
		constexpr std::uint32_t a = 1475U, b = 308611737U, c = 1246536229U, d = 3695U;

		constexpr std::uint32_t bb = b ^ std::rotl((a + d), 7);
		constexpr std::uint32_t cc = c ^ std::rotl((a + bb), 9);
		constexpr std::uint32_t dd = d ^ std::rotl((cc + bb), 13);
		constexpr std::uint32_t aa = a ^ std::rotl((cc + dd), 18);

		std::cout << "a:\n" << std::bitset<32>(a).to_string() << '\n';
		std::cout << "d:\n" << std::bitset<32>(d).to_string() << '\n';
		std::cout << "b:\n" << std::bitset<32>(b).to_string() << '\n';
		std::cout << "a + d:\n" << std::bitset<32>(a + d).to_string() << '\n';
		std::cout << "(a + d) <<< 7:\n" << std::bitset<32>(std::rotl((a + d), 7)).to_string() << '\n';
		std::cout << "b ⊕ ((a + d) <<< 7):\n" << std::bitset<32>(bb).to_string() << '\n';

		std::cout << '\n';

		std::cout << "a:\n" << std::bitset<32>(a).to_string() << '\n';
		std::cout << "bb:\n" << std::bitset<32>(bb).to_string() << '\n';
		std::cout << "c:\n" << std::bitset<32>(c).to_string() << '\n';
		std::cout << "a + bb:\n" << std::bitset<32>(a + bb).to_string() << '\n';
		std::cout << "(a + bb) <<< 9:\n" << std::bitset<32>(std::rotl((a + bb), 9)).to_string() << '\n';
		std::cout << "c ⊕ ((a + bb) <<< 9):\n" << std::bitset<32>(cc).to_string() << '\n';

		std::cout << '\n';

		std::cout << "cc:\n" << std::bitset<32>(cc).to_string() << '\n';
		std::cout << "bb:\n" << std::bitset<32>(bb).to_string() << '\n';
		std::cout << "d:\n" << std::bitset<32>(d).to_string() << '\n';
		std::cout << "cc + bb:\n" << std::bitset<32>(cc + bb).to_string() << '\n';
		std::cout << "(cc + bb) <<< 13:\n" << std::bitset<32>(std::rotl((cc + bb), 13)).to_string() << '\n';
		std::cout << "d ⊕ ((a + d) <<< 13):\n" << std::bitset<32>(dd).to_string() << '\n';

		std::cout << '\n';

		std::cout << "cc:\n" << std::bitset<32>(cc).to_string() << '\n';
		std::cout << "dd:\n" << std::bitset<32>(dd).to_string() << '\n';
		std::cout << "a:\n" << std::bitset<32>(a).to_string() << '\n';
		std::cout << "cc + dd:\n" << std::bitset<32>(cc + dd).to_string() << '\n';
		std::cout << "(cc + dd) <<< 18:\n" << std::bitset<32>(std::rotl((cc + dd), 18)).to_string() << '\n';
		std::cout << "a ⊕ ((cc + dd) <<< 18):\n" << std::bitset<32>(aa).to_string() << '\n';
	}

	std::cout << '\n';

	if constexpr(false)
	{
		/*
			a, b, c, d ∈ FiniteField(2, 32)
			1475, 308611737, 1246536229, 3695 ∈ FiniteField(2, 32)

			a' = a + b
			d' = d ⊕ a'
			d'' = d' <<< 16

			c' = c + d''
			b' = b ⊕ c'
			b'' = b' <<< 12

			a'' = a' + b''
			d''' = d'' ⊕ a''
			d'''' = d''' <<< 8

			c'' = c' + d''''
			b''' = b'' ⊕ c''
			b'''' = b''' <<< 7
		*/
		constexpr std::uint32_t a = 1475U, b = 308611737U, c = 1246536229U, d = 3695U;

		std::cout << "a:\n" << std::bitset<32>(a).to_string() << '\n';
		std::cout << "b:\n" << std::bitset<32>(b).to_string() << '\n';
		std::cout << "c:\n" << std::bitset<32>(c).to_string() << '\n';
		std::cout << "d:\n" << std::bitset<32>(d).to_string() << '\n';

		std::cout << '\n';

		constexpr std::uint32_t aa = a + b;
		constexpr std::uint32_t dd = d ^ aa;
		constexpr std::uint32_t ddd = std::rotl(dd, 16);

		std::cout << "aa = a + b:\n" << std::bitset<32>(aa).to_string() << '\n';
		std::cout << "dd = d ⊕ aa:\n" << std::bitset<32>(dd).to_string() << '\n';
		std::cout << "ddd = dd <<< 16:\n" << std::bitset<32>(ddd).to_string() << '\n';

		std::cout << '\n';

		constexpr std::uint32_t cc = c + ddd;
		constexpr std::uint32_t bb = b ^ cc;
		constexpr std::uint32_t bbb = std::rotl(bb, 12);

		std::cout << "cc = c + ddd:\n" << std::bitset<32>(cc).to_string() << '\n';
		std::cout << "bb = b ⊕ cc:\n" << std::bitset<32>(bb).to_string() << '\n';
		std::cout << "bbb = bb <<< 12:\n" << std::bitset<32>(bbb).to_string() << '\n';

		std::cout << '\n';

		constexpr std::uint32_t aaa = aa + bbb;
		constexpr std::uint32_t dddd = ddd ^ aaa;
		constexpr std::uint32_t ddddd = std::rotl(dddd, 8);

		std::cout << "aaa = aa + bbb:\n" << std::bitset<32>(aaa).to_string() << '\n';
		std::cout << "dddd = ddd ⊕ aaa:\n" << std::bitset<32>(dddd).to_string() << '\n';
		std::cout << "ddddd = dddd <<< 8:\n" << std::bitset<32>(ddddd).to_string() << '\n';

		std::cout << '\n';

		constexpr std::uint32_t ccc = cc + ddddd;
		constexpr std::uint32_t bbbb = bbb ^ ccc;
		constexpr std::uint32_t bbbbb = std::rotl(bbbb, 7);

		std::cout << "ccc = cc + ddddd:\n" << std::bitset<32>(ccc).to_string() << '\n';
		std::cout << "bbbb = bbb ⊕ ccc:\n" << std::bitset<32>(bbbb).to_string() << '\n';
		std::cout << "bbbbb = bbbb <<< 7:\n" << std::bitset<32>(bbbbb).to_string() << '\n';
	}

	std::cout.tie(0)->sync_with_stdio(false);

	//UnitTester::Test_StreamCryptograph_MyRivestCipher4Star();

	//UnitTester::Show_StreamCryptograph_MyRivestCipher4Star();

	if constexpr(false)
	{
		std::vector<std::uint8_t> Keys
		{
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00
		};

		std::vector<std::uint8_t> NumberOnces
		{
			0x00, 0x00, 0x00, 0x00,
			0x00, 0x00, 0x00, 0x00
		};

		const auto backup_io_format_flags = std::cout.flags();

		StreamDataCryption::DJB::Salsa20 Salsa20Cipher(Keys, NumberOnces);
		auto ByteKeyStream = Salsa20Cipher.GenerateKeyStream(180);

		std::uint32_t ByteCounter = 0;
		std::cout << "Key Stream Byte Data With Salsa20:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}
		ByteCounter = 0;
		std::cout << "--------------------------------" << std::endl;

		StreamDataCryption::DJB::Chacha20 Chacha20Cipher(Keys, NumberOnces);
		ByteKeyStream = Chacha20Cipher.GenerateKeyStream(180);
	
		ByteCounter = 0;
		std::cout << "Key Stream Byte Data With Chacha20:" << std::endl;
		for ( const auto& ByteKey : ByteKeyStream )
		{
			std::cout << "0x" << std::setfill( '0' ) << std::setw( sizeof( std::uint8_t ) * 2 ) << std::hex << static_cast<std::uint32_t>( ByteKey ) << ',' << '\t';
			++ByteCounter;
			if ( ByteCounter % 16 == 0 )
			{
				std::cout << '\n';
			}
		}
		ByteCounter = 0;

		std::cout.flags(backup_io_format_flags);
	}

	//StreamDataCryption::DJB::Salsa20 Salsa20Cipher;
	//Salsa20Cipher.WeakerTest();

	//StreamDataCryption::DJB::Chacha20 Chacha20Cipher;
	//Chacha20Cipher.WeakerTest();

	#endif

	std::cout << std::endl;

	#if 0

	PseudoRandomNumberGenerators::AdditionWithCarry<std::uint64_t, 11, 28> AWC(1);

	//1 -> 1634221231264927721
	//2 -> 18064990775153012038
	auto TestNumber64BitOnAWC = AWC();
	std::cout << "TestNumber of the bit on AdditionWithCarry Function is :" << TestNumber64BitOnAWC << std::endl;

	PseudoRandomNumberGenerators::SubtractWithBorrow<std::uint64_t, 5, 62> SWB(1);

	//1 -> 12341784191568880448
	//2 -> 5768763937965491514
	auto TestNumber64BitOnSWB = SWB();
	std::cout << "TestNumber of the bit on SubtractWithBorrow Function is :" << TestNumber64BitOnSWB << std::endl;

	PseudoRandomNumberGenerators::SubtractWithCarry<std::uint64_t, 5, 62> SWC(1);

	//1 -> 8582135542310973860
	//2 -> 16554923195729802669
	auto TestNumber64BitOnSWC = SWC();
	std::cout << "TestNumber of the bit on SubtractWithCarry Function is :" << TestNumber64BitOnSWC << std::endl;

	PseudoRandomNumberGenerators::BlumBlumShub BBS;

	//1 -> 6643 (Iteration is 128)
	//2 -> 20200781 (Iteration is 128)
	std::uint64_t TestNumber64BitOnBBS = BBS.NumberGeneration(14569, 128);
	std::cout << "TestNumber of the 64 bit on BlumBlumShub Function is :" << TestNumber64BitOnBBS << std::endl;

	PseudoRandomNumberGenerators::ComplementaryMultiplyWithCarry CMWC((std::uint32_t)2);

	//1 -> 1061661061 (Iteration is 128)
	//2 -> 194730313 (Iteration is 128)
	std::uint32_t TestNumber32Bit = CMWC.NumberGeneration32Bit(128);
	std::cout << "TestNumber of the 32 bit is :" << TestNumber32Bit << std::endl;

	//1 -> 15304230332744613086 (Iteration is 128)
	//2 -> 4283982144046371081 (Iteration is 128)
	std::uint64_t TestNumber64Bit = CMWC.NumberGeneration64Bit(128);
	std::cout << "TestNumber of the 64 bit is :" << TestNumber64Bit << std::endl;

	#endif

	#if 0

	PseudoRandomNumberGenerators::MersenneTwister32Bit MT19937_32Bit(1);
	
	for ( size_t round = 0; round < 100; round++ )
	{
		std::cout << "Mersenne Twister of the 32 bit data is :" << MT19937_32Bit.NumberGeneration(1) << std::endl;
	}

	PseudoRandomNumberGenerators::MersenneTwister64Bit MT19937_64Bit(1);
	for ( size_t round = 0; round < 100; round++ )
	{
		std::cout << "Mersenne Twister of the 64 bit data is :" << MT19937_64Bit.NumberGeneration(1) << std::endl;
	}

	#endif

	#if 0

	PseudoRandomNumberGenerators::Xorshift::XorShift32 XOR_SHIFT_32(1);
	PseudoRandomNumberGenerators::Xorshift::XorShift64 XOR_SHIFT_64(1);

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShift32 use the 32 bit state is :" << XOR_SHIFT_32.NumberGeneration32Bit(1) << std::endl;
	}

	std::cout << "----------------------------------------------------------" << std::endl;

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShift64 use the 64 bit state is :" << XOR_SHIFT_64.NumberGeneration64Bit(1) << std::endl;
	}

	PseudoRandomNumberGenerators::Xorshift::XorShift128Star XOR_SHIFT_128(1);
	PseudoRandomNumberGenerators::Xorshift::XorShift1024Star XOR_SHIFT_1024(1);

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShift1024* use the 64 bit state is :" << XOR_SHIFT_1024.NumberGeneration64Bit(1) << std::endl;
	}

	std::cout << "----------------------------------------------------------" << std::endl;

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShift128* use the 64 bit state is :" << XOR_SHIFT_128.NumberGeneration64Bit(1) << std::endl;
	}

	PseudoRandomNumberGenerators::xorshiro128 XOR_SHIFT_ROTATE_128(1);
	PseudoRandomNumberGenerators::xorshiro256 XOR_SHIFT_ROTATE_256(1);
	PseudoRandomNumberGenerators::xorshiro512 XOR_SHIFT_ROTATE_512(1);
	PseudoRandomNumberGenerators::xorshiro1024 XOR_SHIFT_ROTATE_1024(1);

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShiftRotate use the 128 bit state is :" << XOR_SHIFT_ROTATE_128() << std::endl;
	}

	std::cout << "----------------------------------------------------------" << std::endl;

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShiftRotate use the 256 bit state is :" << XOR_SHIFT_ROTATE_256() << std::endl;
	}

	std::cout << "----------------------------------------------------------" << std::endl;

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShiftRotate use the 512 bit state is :" << XOR_SHIFT_ROTATE_512() << std::endl;
	}

	std::cout << "----------------------------------------------------------" << std::endl;

	for ( size_t round = 0; round < 128; round++ )
	{
		std::cout << "XorShiftRotate use the 1024 bit state is :" << XOR_SHIFT_ROTATE_1024() << std::endl;
	}

	std::cout << std::endl;

	#endif
	
	#if defined(TEST_DYNAMIC_BITSET)

	std::string		  original_binary_string = std::string( "10101100100010110001010010000101011101000111110110010010001010010100001010001001101011001100111110011111011001001000111010100101" );
	DynamicBitsetData tdsm_bitset( original_binary_string );
	std::string		  test_binary_string = tdsm_bitset.ToString( DynamicBitsetData::StringFormatType::Binary );

	bool CheckIsSame = original_binary_string == test_binary_string;

	std::string original_decimal_string = std::string( "1946785468763217741156874856156245762214184788152245747889566858" );
	tdsm_bitset.FromString( original_decimal_string, DynamicBitsetData::StringFormatType::Decimal );
	std::string test_decimal_string = tdsm_bitset.ToString( DynamicBitsetData::StringFormatType::Decimal );

	CheckIsSame = original_decimal_string == test_decimal_string;

	#endif

	//LargeNumber test_large_number = "789456123789456123789456123789456123"_BIGINTEGER;

	system("pause");

	return 0;
}