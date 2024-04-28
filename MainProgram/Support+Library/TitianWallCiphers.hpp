#pragma once

#include <cstdint>
#include <cassert>

#include <iostream>
#include <iomanip>

#include <limits>
#include <exception>

#include <array>
#include <vector>
#include <tuple>

inline bool is_system_little_endian()
{
	const int value { 0x01 };
	const void * address { static_cast<const void *>(&value) };
	const unsigned char * least_significant_address { static_cast<const unsigned char *>(address) };

	return (*least_significant_address == 0x01);
}

struct CryptographicFunctions
{
protected:

	//Concatenation of Fibonacci numbers., π, φ, e
	const std::array<uint32_t, 4> MathMagicNumbers {0x01B70C8E,0x243F6A88,0x9E3779B9,0xB7E15162};

	/*
		This byte-substitution box: Strict avalanche criterion is satisfied !
		ByteDataSecurityTestData Transparency Order Is: 7.85956
		ByteDataSecurityTestData Nonlinearity Is: 112
		ByteDataSecurityTestData Propagation Characteristics Is: 8
		ByteDataSecurityTestData Delta Uniformity Is: 4
		ByteDataSecurityTestData Robustness Is: 0.984375
		ByteDataSecurityTestData Signal To Noise Ratio/Differential Power Analysis Is: 10.3062
		ByteDataSecurityTestData Absolute Value Indicatorer Is: 32
		ByteDataSecurityTestData Sum Of Square Value Indicator Is: 67584
		ByteDataSecurityTestData Algebraic Degree Is: 8
		ByteDataSecurityTestData Algebraic Immunity Degree Is: 4
	*/
	static constexpr std::array<std::uint8_t, 256> ByteSubstitutionBoxA
	{
		0xE2, 0x4E, 0x54, 0xFC, 0x94, 0xC2, 0x4A, 0xCC, 0x62, 0x0D, 0x6A, 0x46, 0x3C, 0x4D, 0x8B, 0xD1,
		0x5E, 0xFA, 0x64, 0xCB, 0xB4, 0x97, 0xBE, 0x2B, 0xBC, 0x77, 0x2E, 0x03, 0xD3, 0x19, 0x59, 0xC1,
		0x1D, 0x06, 0x41, 0x6B, 0x55, 0xF0, 0x99, 0x69, 0xEA, 0x9C, 0x18, 0xAE, 0x63, 0xDF, 0xE7, 0xBB,
		0x00, 0x73, 0x66, 0xFB, 0x96, 0x4C, 0x85, 0xE4, 0x3A, 0x09, 0x45, 0xAA, 0x0F, 0xEE, 0x10, 0xEB,
		0x2D, 0x7F, 0xF4, 0x29, 0xAC, 0xCF, 0xAD, 0x91, 0x8D, 0x78, 0xC8, 0x95, 0xF9, 0x2F, 0xCE, 0xCD,
		0x08, 0x7A, 0x88, 0x38, 0x5C, 0x83, 0x2A, 0x28, 0x47, 0xDB, 0xB8, 0xC7, 0x93, 0xA4, 0x12, 0x53,
		0xFF, 0x87, 0x0E, 0x31, 0x36, 0x21, 0x58, 0x48, 0x01, 0x8E, 0x37, 0x74, 0x32, 0xCA, 0xE9, 0xB1,
		0xB7, 0xAB, 0x0C, 0xD7, 0xC4, 0x56, 0x42, 0x26, 0x07, 0x98, 0x60, 0xD9, 0xB6, 0xB9, 0x11, 0x40,
		0xEC, 0x20, 0x8C, 0xBD, 0xA0, 0xC9, 0x84, 0x04, 0x49, 0x23, 0xF1, 0x4F, 0x50, 0x1F, 0x13, 0xDC,
		0xD8, 0xC0, 0x9E, 0x57, 0xE3, 0xC3, 0x7B, 0x65, 0x3B, 0x02, 0x8F, 0x3E, 0xE8, 0x25, 0x92, 0xE5,
		0x15, 0xDD, 0xFD, 0x17, 0xA9, 0xBF, 0xD4, 0x9A, 0x7E, 0xC5, 0x39, 0x67, 0xFE, 0x76, 0x9D, 0x43,
		0xA7, 0xE1, 0xD0, 0xF5, 0x68, 0xF2, 0x1B, 0x34, 0x70, 0x05, 0xA3, 0x8A, 0xD5, 0x79, 0x86, 0xA8,
		0x30, 0xC6, 0x51, 0x4B, 0x1E, 0xA6, 0x27, 0xF6, 0x35, 0xD2, 0x6E, 0x24, 0x16, 0x82, 0x5F, 0xDA,
		0xE6, 0x75, 0xA2, 0xEF, 0x2C, 0xB2, 0x1C, 0x9F, 0x5D, 0x6F, 0x80, 0x0A, 0x72, 0x44, 0x9B, 0x6C,
		0x90, 0x0B, 0x5B, 0x33, 0x7D, 0x5A, 0x52, 0xF3, 0x61, 0xA1, 0xF7, 0xB0, 0xD6, 0x3F, 0x7C, 0x6D,
		0xED, 0x14, 0xE0, 0xA5, 0x3D, 0x22, 0xB3, 0xF8, 0x89, 0xDE, 0x71, 0x1A, 0xAF, 0xBA, 0xB5, 0x81
	};

	/*
		This byte-substitution box: Strict avalanche criterion is satisfied !
		ByteDataSecurityTestData Transparency Order Is: 7.85221
		ByteDataSecurityTestData Nonlinearity Is: 112
		ByteDataSecurityTestData Propagation Characteristics Is: 8
		ByteDataSecurityTestData Delta Uniformity Is: 4
		ByteDataSecurityTestData Robustness Is: 0.984375
		ByteDataSecurityTestData Signal To Noise Ratio/Differential Power Analysis Is: 9.23235
		ByteDataSecurityTestData Absolute Value Indicatorer Is: 32
		ByteDataSecurityTestData Sum Of Square Value Indicator Is: 67584
		ByteDataSecurityTestData Algebraic Degree Is: 8
		ByteDataSecurityTestData Algebraic Immunity Degree Is: 4
	*/
	static constexpr std::array<std::uint8_t, 256> ByteSubstitutionBoxB
	{
		0xE2, 0x0D, 0x3E, 0x94, 0x1D, 0x02, 0x48, 0x71, 0x1C, 0x93, 0xA8, 0x69, 0xB7, 0x90, 0xAA, 0x5C, 
		0x37, 0x5A, 0xDB, 0x75, 0xFD, 0x64, 0x8D, 0xD3, 0x49, 0x12, 0xCB, 0xE0, 0xC6, 0x9A, 0x16, 0xDF, 
		0x33, 0x08, 0xAE, 0xD0, 0xFF, 0xB3, 0x29, 0x34, 0x56, 0xE9, 0x20, 0x7F, 0x44, 0x2F, 0xFA, 0xDC, 
		0x9C, 0x4E, 0x8A, 0x46, 0xDD, 0x42, 0xD9, 0x6A, 0x70, 0xF3, 0xF5, 0x8C, 0x09, 0x72, 0x7C, 0x9F, 
		0xB0, 0x1B, 0x96, 0x62, 0x45, 0x10, 0xEA, 0xA0, 0x6D, 0xA7, 0xCA, 0x3F, 0xAC, 0x0B, 0x23, 0x57, 
		0x28, 0x5B, 0xF7, 0xB4, 0x82, 0x9E, 0x17, 0xEC, 0x31, 0xA9, 0x14, 0xA2, 0xC5, 0x1E, 0x6C, 0x4F, 
		0x4D, 0x55, 0x0F, 0xBB, 0xD7, 0xC0, 0x0A, 0xE1, 0x47, 0xAF, 0x89, 0x26, 0xC4, 0xCD, 0x9D, 0x2C, 
		0x81, 0x3B, 0xEB, 0xF9, 0x53, 0x5E, 0x6F, 0x95, 0xBD, 0x27, 0xBA, 0xFB, 0x07, 0xA5, 0x5D, 0xED, 
		0xDA, 0x2A, 0xA4, 0x99, 0x73, 0x01, 0x98, 0x13, 0x1A, 0xA3, 0xB1, 0xBF, 0xE7, 0x15, 0xF8, 0x78, 
		0x0E, 0x9B, 0x6B, 0x67, 0xF6, 0xD8, 0x36, 0x61, 0x7E, 0xFC, 0x86, 0x40, 0x92, 0x52, 0x03, 0x97, 
		0x87, 0xB9, 0x85, 0x8E, 0x68, 0x06, 0x59, 0xC9, 0xD2, 0xD1, 0x76, 0xC1, 0x22, 0x39, 0x5F, 0xE3, 
		0x8B, 0xA6, 0xD6, 0x2B, 0x32, 0xBE, 0xC3, 0xE6, 0x60, 0x7A, 0x0C, 0xF4, 0x25, 0x41, 0x24, 0x54, 
		0x1F, 0xF0, 0x38, 0xAB, 0x05, 0x83, 0xCF, 0x58, 0x79, 0x3C, 0xC8, 0x7D, 0xAD, 0x51, 0xF2, 0xB2, 
		0x21, 0x43, 0x6E, 0xEF, 0xC7, 0x18, 0x3A, 0x88, 0x4B, 0x2E, 0x65, 0xDE, 0x66, 0xB6, 0x04, 0x30, 
		0xC2, 0x4A, 0xB5, 0x19, 0xCC, 0xFE, 0xD5, 0x84, 0x80, 0x8F, 0x2D, 0xE8, 0x35, 0xF1, 0x63, 0x4C, 
		0x77, 0x91, 0x11, 0xB8, 0xE4, 0xCE, 0xEE, 0xA1, 0x00, 0xD4, 0x50, 0xBC, 0x3D, 0x7B, 0x74, 0xE5
	};

	//Although it is a key-dependent substitution box, it is a 32-bit elemental pseudo-random number
	//虽然是依赖密钥的替换盒，但是它是32比特的元素伪随机数
	std::array<std::uint32_t, 128> KDSB {};

	uint32_t Bits32RotateLeft(uint32_t number, uint32_t bit)
	{
		return (number << (bit % 32)) | (number >> (32 - (bit % 32)));
	}

	uint32_t Bits32RotateRight(uint32_t number, uint32_t bit)
	{
		return (number >> (bit % 32)) | (number << (32 - (bit % 32)));
	}

	//Nonlinear Boolean function
	inline uint32_t FF(uint32_t A, uint32_t B, uint32_t C, size_t Index, size_t ArraySize)
	{
		if(Index < ((ArraySize / 4) * 3))
			return A ^ B ^ C;
		else
			return (A & B) | (A & C) | (B & C);
	}

	inline uint32_t GG(uint32_t A, uint32_t B, uint32_t C, size_t Index, size_t ArraySize)
	{
		if(Index < ((ArraySize / 4) * 3))
			return A ^ B ^ C;
		else
			return (A & B) | (~A & C);
	}

	//Linear Transform function
	inline uint32_t L( uint32_t number )
	{
		return number ^ Bits32RotateLeft( number, 2 ) ^ Bits32RotateLeft( number, 10 ) ^ Bits32RotateLeft( number, 18 ) ^ Bits32RotateLeft( number, 24 );
	}

	inline uint32_t L2( uint32_t number )
	{
		return number ^ Bits32RotateLeft( number, 13 ) ^ Bits32RotateLeft( number, 23 );
	}

	void MixWithAddSubtract(size_t Counter, uint64_t& RandomIndex)
	{
		//混合函数，模加和模减, 数学常数
		RandomIndex ^= KDSB[Counter % KDSB.size()] + MathMagicNumbers[Counter % 4];
		KDSB[Counter % KDSB.size()] += KDSB[(Counter + 1) % KDSB.size()] - MathMagicNumbers[(RandomIndex + Counter) % 4];
		RandomIndex ^= KDSB[(Counter + 1) % KDSB.size()] + MathMagicNumbers[(RandomIndex - Counter) % 4];
		KDSB[(Counter + 1) % KDSB.size()] -= KDSB[Counter % KDSB.size()] - MathMagicNumbers[(RandomIndex + Counter) % 4];
	}

	void RandomAccessMix(size_t Counter, uint64_t& RandomIndex)
	{
		//混合函数，模加和模减, 数学常数
		//使用RandomPosition随机访问
		size_t RandomPosition = RandomIndex % KDSB.size();
		KDSB[Counter % KDSB.size()] ^= KDSB[RandomPosition];
		KDSB[Counter % KDSB.size()] += KDSB[Counter % KDSB.size()] - MathMagicNumbers[RandomPosition % 4];
	}

	uint32_t NLFSR(uint32_t Register)
	{
		//Referenced: https://en.wikipedia.org/wiki/KeeLoq
		for ( size_t NLFSR_ROUND = 0; NLFSR_ROUND < 64; ++NLFSR_ROUND )
		{
			//PrimerNumbers: 2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61
			bool BinaryA = ((Register >> 31) & 1);
			bool BinaryB = ((Register >> 29 - 1) & 1);
			bool BinaryC = ((Register >> 23) & 1);
			bool BinaryD = ((Register >> 19 - 2) & 1);
			bool BinaryE = ((Register >> 13) & 1);
			bool BinaryF = ((Register >> 7 - 3) & 1);
			bool BinaryG = ((Register >> 1) & 1);

			bool BinaryH = ((Register >> 16) & 1) ^ (Register & 1);

			//Right Shift Register
			Register >>= 1;
		
			//Feedback Function
			bool FeedBack0 = (BinaryB != BinaryG) != (BinaryA != BinaryF);
			bool FeedBack1 = (BinaryA && BinaryD) != (BinaryA && BinaryG);
			bool FeedBack2 = (BinaryB && BinaryC) != (BinaryB && BinaryD) != (BinaryB && BinaryE);
			bool FeedBack3 = (BinaryE && BinaryF) == (BinaryD && BinaryF) == (BinaryC && BinaryF);
			bool FeedBack4 = (BinaryF && BinaryG) == (BinaryE && BinaryG) == (BinaryD && BinaryG);
			bool FeedBack5 = (BinaryA && BinaryB && BinaryG) != (BinaryA && BinaryD && BinaryG) != (BinaryA && BinaryF && BinaryG);
			bool FeedBack6 = (BinaryA && BinaryB && BinaryC) != (BinaryB && BinaryC && BinaryD) != (BinaryC && BinaryD && BinaryE) != (BinaryD && BinaryE && BinaryF) != (BinaryE && BinaryF && BinaryG);
			bool FeedBack7 = (BinaryA && BinaryC && BinaryE && BinaryG) != (BinaryB && BinaryD && BinaryF);

			bool FeedBack = BinaryH != FeedBack0 != FeedBack1 != FeedBack2 != FeedBack3 != FeedBack4 != FeedBack5 != FeedBack6 != FeedBack7;
			Register ^= static_cast<uint64_t>( FeedBack ) << 31;
			//std::cout << "Register: "<< Register << std::endl;
			//std::cout << "Register (hex): " << std::hex << Register << std::endl;
		}

		return Register;
	}

	void ComplexMix(size_t Counter, uint64_t& RandomIndex)
	{
		uint32_t RegisterLeft = RandomIndex >> 32;
		uint32_t RegisterRight = RandomIndex & 0x00000000FFFFFFFF;
		
		RegisterLeft = NLFSR(RegisterLeft);
		RegisterRight = NLFSR(RegisterRight);

		RandomIndex = uint64_t(RegisterRight) << 32 | uint64_t(RegisterLeft);

		KDSB[Counter % KDSB.size()] ^= KDSB[(Counter - 2 + KDSB.size()) % KDSB.size()] ^ KDSB[(Counter - 1) % KDSB.size()];
		
		//GG, FF, L 函数
		KDSB[Counter % KDSB.size()] += GG(Counter, KDSB[(Counter - 1) % KDSB.size()], RandomIndex % std::numeric_limits<uint32_t>::max(), Counter, KDSB.size());
		KDSB[Counter % KDSB.size()] -= FF(KDSB[(Counter - 3) % KDSB.size()], L(KDSB[(Counter - 2) % KDSB.size()]), KDSB[(Counter - 1) % KDSB.size()], Counter, KDSB.size());

		//混合函数，模加和模减, 数学常数
		//L, L2 函数
		RandomIndex ^= L(KDSB[Counter % KDSB.size()] - MathMagicNumbers[Counter % 4]);
		KDSB[(Counter + 1) % KDSB.size()] -= L2(KDSB[Counter % KDSB.size()] + MathMagicNumbers[(RandomIndex - Counter) % 4]);
		RandomIndex ^= L2(KDSB[Counter % KDSB.size()] - MathMagicNumbers[(RandomIndex + Counter) % 4]);
		KDSB[(Counter + 2) % KDSB.size()] += L(KDSB[(Counter + 1) % KDSB.size()] + MathMagicNumbers[(RandomIndex - Counter) % 4]);
	}

public:

	void KeySchedule(const std::vector<std::uint8_t>& KeyBytes)
	{
		if (KeyBytes.empty())
		{
			return;
		}

		std::vector<std::uint8_t> KeyBytesBuffer(KeyBytes);
		
		while(KeyBytesBuffer.size() % 4 != 0)
		{
			KeyBytesBuffer.push_back(0x00);
		}
		
		std::vector<std::uint32_t> KeyBlocks(KeyBytesBuffer.size() / 4, 0);

		bool is_system_little_endian_flag = is_system_little_endian();

		uint32_t KeyBlock = 0;
		for ( size_t i = 0, j = 0; i < KeyBytesBuffer.size(); i += 4 )
		{
			/*
				Key preprocessing - Apply non-linear functions - Byte substitution box
				密钥预处理 - 应用非线性函数 - 字节替换盒
			*/

			uint8_t& ByteA = KeyBytesBuffer[i];
			uint8_t& ByteB = KeyBytesBuffer[i + 1];
			uint8_t& ByteC = KeyBytesBuffer[i + 2];
			uint8_t& ByteD = KeyBytesBuffer[i + 3];

			//The original byte is the number of rows, The first substitution finds the number of columns, and the second substitution finds the required value.
			//原字节是行数,第一次替代找到列数,第二次替代找到需要的值
			ByteA = ByteSubstitutionBoxA[ByteSubstitutionBoxA[ByteA]];
			ByteB = ByteSubstitutionBoxA[ByteSubstitutionBoxA[ByteB]];
			ByteC = ByteSubstitutionBoxB[ByteSubstitutionBoxB[ByteC]];
			ByteD = ByteSubstitutionBoxB[ByteSubstitutionBoxB[ByteD]];

			//Byte Order Change: D C B A -> D B C A 

			//4 8-bits packing into 1 32-bits
			KeyBlock = uint32_t(ByteD) << 24 | uint32_t(ByteB) << 16 | uint32_t(ByteC) << 8 | ByteA;

			if(!is_system_little_endian_flag)
			{
				//Byte swap 32-bits
				KeyBlock = ((KeyBlock & 0x000000FF) << 24) | ((KeyBlock & 0x0000FF00) << 8) | ((KeyBlock & 0x00FF0000) >> 8) | ((KeyBlock & 0xFF000000) >> 24);
			}
			KeyBlocks[j] = KeyBlock;
			++j;
		}

		uint64_t RandomIndex = 0;

		//PRNG State Register
		std::array<std::uint32_t, 128> State {};

		size_t index = 0;
		while (index < KeyBlocks.size())
		{
			// 每次从KeyBlocks中取出128个32位块放入KDSB
			size_t offset = std::min(index + KDSB.size(), KeyBlocks.size());

			//If the structure has indeed executed a round, the status is fed back to the next round of 128 32-bit key blocks.
			//如果该结构确实已经执行了一轮，则将状态反馈到下一轮的128个32位密钥块。
			if(index > 0)
			{
				//Left half bits Mix use MODULAR-ADDITION
				//Right half bits Mix use XOR
				for(size_t i = 0, j = index; i < KDSB.size() && j < offset; ++i, ++j)
				{
					KeyBlocks[j] += State[i] & uint32_t(0xFFFF0000);
					KeyBlocks[j] ^= State[i] & uint32_t(0x0000FFFF);
				}
			}

			std::copy(KeyBlocks.begin() + index, KeyBlocks.begin() + offset, KDSB.begin());

			//Pseudo Random Function
			//伪随机函数
			for (size_t Round = 0; Round < 4; Round++)
			{
				for (size_t Counter = 0; Counter < KDSB.size(); Counter++)
				{
					MixWithAddSubtract(Counter, RandomIndex);
					RandomAccessMix(Counter, RandomIndex);
					ComplexMix(Counter, RandomIndex);
				}
			}

			if(index >= KDSB.size())
			{
				//Left half bits Mix use XOR
				//Right half bits Mix use MODULAR-ADDITION
				for(size_t i = index, j = 0; i < offset && j < KDSB.size(); ++i, ++j)
				{
					State[j] = KDSB[j];

					State[j] ^= KeyBlocks[i] & uint32_t(0xFFFF0000);
					State[j] += KeyBlocks[i] & uint32_t(0x0000FFFF);
				}
			}

			index += KDSB.size();

			// 如果不足128个32位块，执行一次4轮的伪随机函数后就结束
			if (offset - index < KDSB.size())
				break;
		}
	}
};

class TitanWallStreamCipher : public CryptographicFunctions
{
private:

	bool IsKeyUsed = false;

	//PRNG State Register
	std::array<std::uint32_t, 128> State {};

public:

	explicit TitanWallStreamCipher(const std::vector<std::uint8_t>& KeyBytes) 
	{
		// 生成子密钥
		KeySchedule(KeyBytes);
		IsKeyUsed = true;
	}

	~TitanWallStreamCipher()
	{
		ResetState();
	}

	void GeneratePseudoRandomBytes(std::vector<std::uint8_t>& Bytes)
	{
		if(Bytes.empty())
			return;

		if(!IsKeyUsed)
			return;

		uint64_t RandomIndex = 0;

		for ( size_t i = 0; i < KDSB.size(); i++ )
		{
			KDSB[i] += State[i] & 0xFFFF0000;
			KDSB[i] ^= State[i] & 0x0000FFFF;

			//Pseudo Random Function
			//伪随机函数
			for (size_t Round = 0; Round < 4; Round++)
			{
				for (size_t Counter = 0; Counter < KDSB.size(); Counter++)
				{
					MixWithAddSubtract(Counter, RandomIndex);
					RandomAccessMix(Counter, RandomIndex);
					ComplexMix(Counter, RandomIndex);
				}
			}

			State[i] ^= KDSB[i] & 0xFFFF0000;
			State[i] += KDSB[i] & 0x0000FFFF;
		}

		// 1 32-bits unpacking into 4 8-bits
		for (size_t i = 0; i < State.size() && (i * 4) < Bytes.size(); i++)
		{
			std::uint32_t value = State[i];
			for (int j = 0; j < 4 && ((i * 4) + j) < Bytes.size(); j++)
			{
				Bytes[i * 4 + j] = static_cast<std::uint8_t>(value & 0xFF);
				value >>= 8;
			}
		}
	}

	void InitialState(const std::vector<std::uint8_t>& KeyBytes)
	{
		ResetState();
		KeySchedule(KeyBytes);
		IsKeyUsed = true;
	}

	void ResetState()
	{
		const std::vector<uint32_t> ZeroNumbers(128, 0x00);

		memmove(State.data(), ZeroNumbers.data(), 128 * sizeof(uint32_t));
		memmove(KDSB.data(), ZeroNumbers.data(), 128 * sizeof(uint32_t));
		IsKeyUsed = false;
	}
};

class TitanWallBlockCipher : public CryptographicFunctions
{
private:
	// 伪哈达玛德变换
	void PHT(std::uint32_t& a, std::uint32_t& b)
	{
		std::uint32_t temp_a = a;
		// 32位模运算
		a = (a + b) & 0xFFFFFFFF;
		b = (temp_a + 2 * b) & 0xFFFFFFFF;
	}

	// 伪哈达玛德变换的逆变换
	void InversePHT(std::uint32_t& a, std::uint32_t& b)
	{
		std::uint32_t temp_a = a;
		// 32位模运算
		a = (2 * a - b) & 0xFFFFFFFF;
		b = (b - temp_a) & 0xFFFFFFFF;
	}

public:

	explicit TitanWallBlockCipher(const std::vector<std::uint8_t>& KeyBytes) 
	{
		// 生成子密钥
		KeySchedule(KeyBytes);
	}

	~TitanWallBlockCipher()
	{
		const std::vector<uint32_t> ZeroNumbers(128, 0x00);
		memmove(KDSB.data(), ZeroNumbers.data(), 128 * sizeof(uint32_t));
	}

	// 加密函数
	std::vector<std::uint32_t> Encrypt(const std::vector<std::uint32_t>& plaintext)
	{
		if(plaintext.size() % 8 != 0)
		{
			std::cout << "The number of data blocks cannot be aligned. Please perform preprocessing before proceeding." << std::endl;
			throw std::logic_error("The number of data blocks cannot be aligned. Please perform preprocessing before proceeding.");
		}

		std::vector<std::uint32_t> ciphertext(plaintext.size(), 0);

		const size_t SubkeysSize = KDSB.size();

		for (size_t i = 0; i < plaintext.size(); i += 8)
		{
			uint32_t A = plaintext[i];
			uint32_t B = plaintext[i + 1];
			uint32_t C = plaintext[i + 2];
			uint32_t D = plaintext[i + 3];
			uint32_t E = plaintext[i + 4];
			uint32_t F = plaintext[i + 5];
			uint32_t G = plaintext[i + 6];
			uint32_t H = plaintext[i + 7];

			B += KDSB[0];
			D += KDSB[1];
			F += KDSB[2];
			H += KDSB[3];

			//128 - 1 = 127
			//(127 - 3) / 2 = 62
			for (size_t j = 1; j <= 62; j++)
			{
				// 替换函数S
				std::uint32_t t = B ^ ((B << 1) + 1);
				std::uint32_t u = D ^ ((D << 2) + 1);
				std::uint32_t v = F ^ ((F << 3) + 1);
				std::uint32_t w = H ^ ((H << 4) + 1);

				A = Bits32RotateRight(A - t, w % 32) + KDSB[2 * j];
				C = Bits32RotateRight(C ^ u, v % 32) + KDSB[2 * j + 1];
				E = Bits32RotateRight(E ^ v, u % 32) + KDSB[2 * j + 2];
				G = Bits32RotateRight(G + w, t % 32) + KDSB[2 * j + 3];

				B += KDSB[j % KDSB.size()];
				D ^= KDSB[(j + 1) % KDSB.size()];
				F ^= KDSB[(j + 2) % KDSB.size()];
				H -= KDSB[(j + 3) % KDSB.size()];

				// 置换函数P
				std::tie(F, D, B, H, A, G, E, C) = std::make_tuple(A, B, C, D, E, F, G, H);

				// 伪哈达玛德变换
				PHT(A, B);
				PHT(C, D);
				PHT(E, F);
				PHT(G, H);
			}

			A += KDSB[SubkeysSize - 4];
			C += KDSB[SubkeysSize - 3];
			E += KDSB[SubkeysSize - 2];
			G += KDSB[SubkeysSize - 1];

			ciphertext[i] = A;
			ciphertext[i + 1] = B;
			ciphertext[i + 2] = C;
			ciphertext[i + 3] = D;
			ciphertext[i + 4] = E;
			ciphertext[i + 5] = F;
			ciphertext[i + 6] = G;
			ciphertext[i + 7] = H;
		}

		return ciphertext;
	}

	// 解密函数
	std::vector<std::uint32_t> Decrypt(const std::vector<std::uint32_t>& ciphertext)
	{
		if(ciphertext.size() % 8 != 0)
		{
			std::cout << "The number of data blocks cannot be aligned. Please perform preprocessing before proceeding." << std::endl;
			throw std::logic_error("The number of data blocks cannot be aligned. Please perform preprocessing before proceeding.");
		}

		std::vector<std::uint32_t> plaintext(ciphertext.size());

		const size_t SubkeysSize = KDSB.size();

		for (size_t i = 0; i < ciphertext.size(); i += 8)
		{
			uint32_t A = ciphertext[i];
			uint32_t B = ciphertext[i + 1];
			uint32_t C = ciphertext[i + 2];
			uint32_t D = ciphertext[i + 3];
			uint32_t E = ciphertext[i + 4];
			uint32_t F = ciphertext[i + 5];
			uint32_t G = ciphertext[i + 6];
			uint32_t H = ciphertext[i + 7];

			G -= KDSB[SubkeysSize - 1];
			E -= KDSB[SubkeysSize - 2];
			C -= KDSB[SubkeysSize - 3];
			A -= KDSB[SubkeysSize - 4];

			//128 - 1 = 127
			//(127 - 3) / 2 = 62
			for (size_t j = 62; j >= 1; j--)
			{
				// 伪哈达玛德变换的逆变换
				InversePHT(A, B);
				InversePHT(C, D);
				InversePHT(E, F);
				InversePHT(G, H);

				// 置换函数P的逆操作
				std::tie(A, B, C, D, E, F, G, H) = std::make_tuple(F, D, B, H, A, G, E, C);

				// 替换函数S的逆操作
				H += KDSB[(j + 3) % KDSB.size()];
				F ^= KDSB[(j + 2) % KDSB.size()];
				D ^= KDSB[(j + 1) % KDSB.size()];
				B -= KDSB[j % KDSB.size()];

				std::uint32_t t = B ^ ((B << 1) + 1);
				std::uint32_t u = D ^ ((D << 2) + 1);
				std::uint32_t v = F ^ ((F << 3) + 1);
				std::uint32_t w = H ^ ((H << 4) + 1);

				G = Bits32RotateLeft(G - KDSB[2 * j + 3], t % 32) - w;
				E = Bits32RotateLeft(E - KDSB[2 * j + 2], u % 32) ^ v;
				C = Bits32RotateLeft(C - KDSB[2 * j + 1], v % 32) ^ u;
				A = Bits32RotateLeft(A - KDSB[2 * j], w % 32) + t;
			}

			H -= KDSB[3];
			F -= KDSB[2];
			D -= KDSB[1];
			B -= KDSB[0];

			plaintext[i] = A;
			plaintext[i + 1] = B;
			plaintext[i + 2] = C;
			plaintext[i + 3] = D;
			plaintext[i + 4] = E;
			plaintext[i + 5] = F;
			plaintext[i + 6] = G;
			plaintext[i + 7] = H;
		}

		return plaintext;
	}
};


void TestTitanWallBlockCipher()
{
	// 测试数据
	std::vector<std::uint32_t> plaintext = {0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210, 0xAAAAAAAA, 0x55555555, 0x80000000, 0x11111111, 0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210, 0xAAAAAAAA, 0x55555555, 0x80000000, 0x11111111};
	std::vector<std::uint8_t> key = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};

	TitanWallBlockCipher cipher(key);

	// 加密
	std::vector<std::uint32_t> ciphertext = cipher.Encrypt(plaintext);

	// 解密
	std::vector<std::uint32_t> decryptedText = cipher.Decrypt(ciphertext);

	// 验证
	if(plaintext != decryptedText)
	{
		std::cout << "测试失败！" << std::endl;
		assert(false);
	}

	std::cout << "加密前的数据: ";
	for (auto val : plaintext)
		std::cout << std::hex << val << " ";
	std::cout << std::endl;

	std::cout << "加密后的数据: ";
	for (auto val : ciphertext)
		std::cout << std::hex << val << " ";
	std::cout << std::endl;

	std::cout << "解密后的数据: ";
	for (auto val : decryptedText)
		std::cout << std::hex << val << " ";
	std::cout << std::endl;

	std::cout << "测试成功！" << std::endl;

	// 输出KDSB的内容
	/*for (size_t i = 0; i < titanwall.KDSB.size(); ++i) {
		std::cout << std::dec << "KDSB[" << i << "] = " << std::hex << titanwall.KDSB[i] << std::endl;
	}*/
}

void TestTitanWallStreamCipher()
{
	// 1. Test Key Initialization
	std::vector<std::uint8_t> key = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0};
	TitanWallStreamCipher cipher(key);

	// 2. Test Pseudo Random Byte Generation
	std::vector<std::uint8_t> bytes(32, 0); // Initialize a vector of 32 bytes with zeros
	cipher.GeneratePseudoRandomBytes(bytes);

	// Ensure that the bytes have been modified
	bool isModified = false;
	for (auto byte : bytes)
	{
		if (byte != 0)
		{
			isModified = true;
			break;
		}
	}

	assert(isModified); // Assert that the bytes have been modified

	// 3. Test Reset State
	cipher.ResetState();
	cipher.InitialState(key);
	std::vector<std::uint8_t> resetBytes(32, 0);
	cipher.GeneratePseudoRandomBytes(resetBytes);

	// Ensure that the bytes are still zeros after reset
	bool isReset = true;
	for (auto byte : resetBytes)
	{
		if (byte != 0)
		{
			isReset = false;
			break;
		}
	}

	assert(!isReset); // Assert that the bytes have been modified even after reset

	// 4. Encrypt data
	std::string originalText = "Hello, TitanWall!";

	// Print Original Text
	std::cout << "Original Text: " << originalText << std::endl;

	std::vector<std::uint8_t> data(originalText.begin(), originalText.end());
	std::vector<std::uint8_t> random(originalText.size(), 0);
	std::vector<std::uint8_t> encryptedData(random.size(), 0);

	cipher.ResetState();
	cipher.InitialState(key);
	cipher.GeneratePseudoRandomBytes(random);
	for (size_t i = 0; i < data.size(); ++i)
	{
		encryptedData[i] = data[i] ^ random[i];
	}

	// Print encrypted data
	std::cout << "Encrypted Data: ";
	for (auto byte : encryptedData)
	{
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << std::endl;

	// 5. Decrypt data
	cipher.ResetState();
	cipher.InitialState(key);
	cipher.GeneratePseudoRandomBytes(random);
	std::vector<std::uint8_t> decryptedData(encryptedData.size(), 0);
	for (size_t i = 0; i < random.size(); ++i)
	{
		decryptedData[i] = encryptedData[i] ^ random[i];
	}

	// Print decrypted data
	std::cout << "Decrypted Data: ";
	for (auto byte : decryptedData)
	{
		std::cout << std::hex << static_cast<int>(byte) << " ";
	}
	std::cout << std::endl;

	std::string decryptedText(decryptedData.begin(), decryptedData.end());

	// Print Recovered Text
	std::cout << "Recovered Text: " << decryptedText << std::endl;

	// Ensure that the decrypted data matches the original plaintext
	assert(decryptedText == originalText);
}
