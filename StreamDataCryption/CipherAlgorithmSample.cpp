#include "StreamDataCryption.hpp"
#include "../RandomNumberGenerators/PseudoRandomNumberGenerators.hpp"

namespace StreamDataCryption::AlgorithmSample
{
	class StreamCipherBasedAES
	{
	private:

		//Xoshiro(1024 Bit) Pseudo-random number generator
		//Xoshiro(1024 比特) 伪随机数生成器
		PseudoRandomNumberGenerators::xorshiro1024 PRNG;

		//Keystream state array
		//密钥流状态数组
		std::array<std::uint64_t, 128> KeyStreamState {};

		//Advanced encryption/decryption standard byte substitution box
		//高级加密/解密标准的字节替换盒
		std::array<std::uint8_t, 256> AES_BOX
		{
			0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
			0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
			0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
			0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
			0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
			0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
			0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
			0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
			0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
			0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
			0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
			0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
			0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
			0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
			0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
			0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
		};

		//Generate pseudo-random numbers
		//生成伪随机数
		void GenerateDataWithPRNG(std::span<std::uint64_t> Datas)
		{
			for (auto& Data : Datas)
				Data = PRNG();
		}

		//Byte substitution (non-linear function/obfuscation)
		//字节替换 (非线性函数/混淆)
		std::uint8_t ByteSubtition(std::uint8_t Byte)
		{
			Byte = AES_BOX[AES_BOX[Byte]];

			return Byte;
		}

		//Arrays data shuffle(Approximate nonlinear function/diffusion)
		//数组数据洗牌(近似非线性函数/扩散)
		void ShuffleArray(std::span<std::uint64_t> Datas)
		{
			for (std::size_t index = 0; index < Datas.size(); ++index)
			{
				std::swap(Datas[index], Datas[PRNG() % Datas.size()]);
			}
		}

	public:

		std::vector<std::uint64_t> GenerateKeyStream(std::size_t GeneratedSize)
		{
			std::size_t ElementIndex = 0;
			std::vector<std::uint64_t> KeyStream;

			for(std::size_t GenerateCount = 0; GenerateCount < GeneratedSize; GenerateCount += this->KeyStreamState.size());
			{
				for (size_t round = 0; round < 128; round++)
				{
					this->GenerateDataWithPRNG(this->KeyStreamState);
			
					//View arrays as a range of memory views.
					//把数组作为一个内存视图范围看待
					std::span<std::uint64_t> KeyStreamStateSpan(this->KeyStreamState);

					for(std::size_t OffsetIndex = 0; OffsetIndex < KeyStreamStateSpan.size(); OffsetIndex += 4)
					{
						//Gets the view of the sub-memory range.
						//获取子内存范围的视图
						std::span<std::uint64_t> KeyStreamStateSubSpan = KeyStreamStateSpan.subspan(OffsetIndex, 4);
				
						//Pseudo-hadamard transform (forward)
						//伪哈达玛德变换(正向)
						std::uint64_t a = KeyStreamStateSubSpan[0] + KeyStreamStateSubSpan[1];
						std::uint64_t b = KeyStreamStateSubSpan[0] + 2 * KeyStreamStateSubSpan[1];
						KeyStreamStateSubSpan[0] = a;
						KeyStreamStateSubSpan[1] = b;

						//Pseudo-hadamard transform (backward)
						//伪哈达玛德变换(反向)
						std::uint64_t c = KeyStreamStateSubSpan[3] - KeyStreamStateSubSpan[2];
						std::uint64_t d = 2 * KeyStreamStateSubSpan[2] - KeyStreamStateSubSpan[3];
						KeyStreamStateSubSpan[2] = c;
						KeyStreamStateSubSpan[3] = d;

						a = b = c = d = 0;
					}
			
					this->ShuffleArray(this->KeyStreamState);
			
					for(std::size_t Index = 0; Index < KeyStreamState.size(); ++Index)
					{
						std::uint64_t Bit64 = KeyStreamState[Index];

						//Moves 64 bits of data to bits in different locations using bitwise operations. This way the conversion to 8 bits of a byte is truncated to 8 bits of a different 8 part
						//将64比特的数据用位运算移动到比特不同位置。这样转换到字节的8个比特的时候，会截断到不同的8个部分的8个比特
						Bit64 = static_cast<std::uint64_t>(ByteSubtition(Bit64 >> 56))
						| static_cast<std::uint64_t>(ByteSubtition(Bit64 >> 48))
						| static_cast<std::uint64_t>(ByteSubtition(Bit64 >> 40))
						| static_cast<std::uint64_t>(ByteSubtition(Bit64 >> 32))
						| static_cast<std::uint64_t>(ByteSubtition(Bit64 >> 24))
						| static_cast<std::uint64_t>(ByteSubtition(Bit64 >> 16))
						| static_cast<std::uint64_t>(ByteSubtition(Bit64 >> 8))
						| static_cast<std::uint64_t>(ByteSubtition(Bit64));

						 KeyStreamState[Index] = Bit64;
					}
			
					this->ShuffleArray(this->KeyStreamState);
				}

				std::copy(KeyStreamState.begin(), KeyStreamState.end(), std::back_inserter(KeyStream));
			}

			return KeyStream;
		}

		//Resets the stream cryptograph state.
		//重置流密码器状态。
		void Reset()
		{
			constexpr std::array<std::uint64_t, 128> ZeroData {};

			::memcpy(KeyStreamState.data(), ZeroData.data(), KeyStreamState.size() * sizeof(std::uint64_t));
		}

		explicit StreamCipherBasedAES(std::uint64_t seed)
			:
			PRNG(seed)
		{
		
		}

		StreamCipherBasedAES() = delete;
		~StreamCipherBasedAES() = default;
	};
}