#pragma once

#include "DynamicBitSet.hpp"

inline void testBooleanBitWrapper()
{
	using namespace TwilightDream;
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
	using namespace TwilightDream;
	// 创建一个 DynamicBitSet 实例并初始化一些值
	DynamicBitSet dbset;
	dbset.push_back( true );
	dbset.push_back( false );
	dbset.push_back( true );
	dbset.push_back( false );

	std::cout << "Testing push back:" << std::endl;
	std::cout << dbset.format_binary_string( true ) << std::endl;

	// 测试正向迭代器
	std::cout << "Testing forward iterator:" << std::endl;
	for ( auto it = dbset.begin(); it != dbset.end(); ++it )
	{
		std::cout << (bool)*it << " ";
	}
	std::cout << std::endl;

	// 测试反向迭代器
	std::cout << "Testing reverse iterator:" << std::endl;
	for ( auto it = dbset.rbegin(); it != dbset.rend(); ++it )
	{
		std::cout << (bool)*it << " ";
	}
	std::cout << std::endl;

	dbset.push_back( true );
	dbset.push_back( true );
	dbset.push_back( false );
	dbset.push_back( false );

	std::cout << "Testing push back:" << std::endl;
	std::cout << dbset.format_binary_string( true ) << std::endl;

	// 测试正向迭代器
	std::cout << "Testing forward iterator:" << std::endl;
	for ( auto it = dbset.begin(); it != dbset.end(); ++it )
	{
		std::cout << (bool)*it << " ";
	}
	std::cout << std::endl;

	// 测试反向迭代器
	std::cout << "Testing reverse iterator:" << std::endl;
	for ( auto it = dbset.rbegin(); it != dbset.rend(); ++it )
	{
		std::cout << (bool)*it << " ";
	}
	std::cout << std::endl;

	// 测试正向迭代器
	std::cout << "Testing (Modified) forward iterator:" << std::endl;
	for ( auto it = dbset.begin(); it + 1 != dbset.end(); ++it )
	{
		*it = *(it + 1);
		std::cout << (bool)*it << " ";
	}
	std::cout << std::endl;

	// 测试反向迭代器
	std::cout << "Testing (Modified) reverse iterator:" << std::endl;
	for ( auto it = dbset.rbegin(); it + 1 != dbset.rend(); ++it )
	{
		*it = *(it + 1);
		std::cout << (bool)*it << " ";
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
	//assert( dbset2.format_binary_string( true ) == "010101010101010101010101010101010101010101" );

	// 删除 LSB
	dbset2.erase( 0 );
	std::cout << "Erase Result: " << dbset2.format_binary_string( true ) << std::endl;
	//assert( dbset2.format_binary_string( true ) == "01010101010101010101010101010101010101010" );

	// 删除 MSB
	dbset2.erase( dbset2.bit_size() - 1 );
	std::cout << "Erase Result: " << dbset2.format_binary_string( true ) << std::endl;
	//assert( dbset2.format_binary_string( true ) == "1010101010101010101010101010101010101010" );

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
	using namespace TwilightDream;
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
	using namespace TwilightDream;
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

	std::string	  output;

	//FIXME!!!
	//std::string	  long_binary = "000000000000000000000000000000010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010101010";
	//DynamicBitSet db1( long_binary, 2 );
	//output = db1.format_binary_string( false );
	//std::cout << "input:  " << long_binary << std::endl;
	//std::cout << "output: " << output << std::endl;
	//assert( output == long_binary );
	//std::cout << "Test for long binary string passed." << std::endl;

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
	using namespace TwilightDream;
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
	using namespace TwilightDream;
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
	using namespace TwilightDream;
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
	std::cout << "C Right Shifted 17: " << (BigNumberC >> 17).format_binary_string(true) << std::endl;
	std::cout << "C Left Shifted 17: " << (BigNumberC << 17).format_binary_string(true) << std::endl;
	
	DynamicBitSet BigNumberC_RL(BigNumberC);
	BigNumberC_RL.rotate_left(75);
	std::cout << "C Left Rotated 75: " << BigNumberC_RL.format_binary_string(true) << std::endl;
	DynamicBitSet BigNumberC_RR(BigNumberC);
	BigNumberC_RR.rotate_right(75);
	std::cout << "C Right Rotated 75: " << BigNumberC_RR.format_binary_string(true) << std::endl;

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
	/*
		Current tested: of the bit-set class based on dynamically allocated array storage for 32-bit chunks
		
		Functions with big problems (which may need to be redesigned):

		push_front
		pop_front

		insert
		erase
		reverse_insert
		reverse_insert

		Functions not yet implemented:

		rotate_left
		rotate_right
	*/

	testBooleanBitWrapper();
	testDynamicBitSet();
	testConstructors();
	testConversions();
	testLargeData();
	testRandomData();
	testOperatorsAndModifications();
}