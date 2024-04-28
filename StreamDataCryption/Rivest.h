#pragma once

#include "../MainProgram/Support+Library/Support-Library.hpp"

namespace StreamDataCryption::RC4
{
	struct AlgorithmBase
	{
		//Pseudo-random generation algorithm (PRGA)
		virtual std::vector<std::uint8_t> GenerateKeyStream(std::size_t Count) = 0;

		//Key-scheduling algorithm (KSA)
		virtual void KeyScheduling(std::span<const std::uint8_t> Keys) = 0;

		std::uint32_t Modulo256Addition(std::uint32_t a, std::uint32_t b)
		{
			return (a + b) % 256;
		}
	};
}

namespace StreamDataCryption::RC4
{
	// Stream cipher - Rivest Cipher 4
	class RivestCipher4 : public AlgorithmBase
	{

	private:
		std::array<std::uint8_t, 256> KeyState{};

	public:

		//Pseudo-random generation algorithm (PRGA)
		virtual std::vector<std::uint8_t> GenerateKeyStream(std::size_t Count) override final;

		//Key-scheduling algorithm (KSA)
		virtual void KeyScheduling(std::span<const std::uint8_t> Keys) override final;

		explicit RivestCipher4()
			:
			AlgorithmBase()
		{
			std::uint8_t ByteData = 0;
			for (auto& Key : KeyState)
			{
				Key = ByteData;
				++ByteData;
			}
			ByteData = 0;
		}

		virtual ~RivestCipher4() = default;
	};

	// Stream cipher - Rivest Cipher 4A
	class RivestCipher4A : public AlgorithmBase
	{

	private:
		std::array<std::uint8_t, 256> KeyState{};
		std::array<std::uint8_t, 256> KeyState2{};

	public:

		//Pseudo-random generation algorithm (PRGA)
		virtual std::vector<std::uint8_t> GenerateKeyStream(std::size_t Count) override final;

		//Key-scheduling algorithm (KSA)
		virtual void KeyScheduling(std::span<const std::uint8_t> Keys) override final;

		explicit RivestCipher4A()
			:
			AlgorithmBase()
		{
			std::uint8_t ByteData = 0;
			std::array<std::uint8_t, 256> TemporaryKeyState{};
			for (std::uint32_t Index = 0; Index < 256; ++Index)
			{
				KeyState[Index] = ByteData;
				KeyState2[Index] = ByteData;
				++ByteData;
			}
			ByteData = 0;
		}

		virtual ~RivestCipher4A() = default;
	};

	// Stream cipher - Rivest Cipher 4 with Variably Modified Permutation Composition
	class RivestCipher4_VMPC : public AlgorithmBase
	{

	private:
		std::array<std::uint8_t, 256> KeyState{};

	public:

		//Pseudo-random generation algorithm (PRGA)
		virtual std::vector<std::uint8_t> GenerateKeyStream(std::size_t Count) override final;

		//Key-scheduling algorithm (KSA)
		virtual void KeyScheduling(std::span<const std::uint8_t> Keys) override final;

		explicit RivestCipher4_VMPC()
			:
			AlgorithmBase()
		{
			std::uint8_t ByteData = 0;
			for (auto& Key : KeyState)
			{
				Key = ByteData;
				++ByteData;
			}
			ByteData = 0;
		}

		virtual ~RivestCipher4_VMPC() = default;
	};

	// Stream cipher - Rivest Cipher 4 Plus
	// https://eprint.iacr.org/2008/396
	class RivestCipher4Plus : public AlgorithmBase
	{

	private:
		std::array<std::uint8_t, 256> KeyState{};

	public:

		//Pseudo-random generation algorithm (PRGA)
		virtual std::vector<std::uint8_t> GenerateKeyStream(std::size_t Count) override final;

		//Key-scheduling algorithm (KSA)
		virtual void KeyScheduling(std::span<const std::uint8_t> Keys) override final;

		explicit RivestCipher4Plus()
			:
			AlgorithmBase()
		{
			std::uint8_t ByteData = 0;
			for (auto& Key : KeyState)
			{
				Key = ByteData;
				++ByteData;
			}
			ByteData = 0;
		}

		virtual ~RivestCipher4Plus() = default;
	};

	// Stream cipher - Rivest Cipher 4 - Spritz
	class RivestCipher4_Spritz : public AlgorithmBase
	{

	private:
		std::array<std::uint8_t, 256> KeyState{};

	public:

		//Pseudo-random generation algorithm (PRGA)
		virtual std::vector<std::uint8_t> GenerateKeyStream(std::size_t Count) override final;

		//Key-scheduling algorithm (KSA)
		virtual void KeyScheduling(std::span<const std::uint8_t> Keys) override final;

		explicit RivestCipher4_Spritz()
			:
			AlgorithmBase()
		{
			std::uint8_t ByteData = 0;
			for (auto& Key : KeyState)
			{
				Key = ByteData;
				++ByteData;
			}
			ByteData = 0;
		}

		virtual ~RivestCipher4_Spritz() = default;
	};

	// Stream cipher - Rivest Cipher 4 with Complex Edited by Twilight-Dream
	class RivestCipher4Star : public AlgorithmBase
	{

	private:
		std::array<std::uint8_t, 256> KeyState{};
		// Permutation table based on Keystate
		std::array<std::uint8_t, 256> PermutationTable{};
		static constexpr std::uint32_t PrimeNumber32Bit = 4294967291;

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

		// A function that multiplies two 64-bit unsigned integers
		// and returns a pair of 64-bit unsigned integers
		// representing the high part and low part of the product
		std::pair<uint64_t, uint64_t> LongNumberMultiply(uint64_t x, uint64_t y);

	public:
		//RC4* - Complex Pseudo-random generation algorithm (PRGA) 
		std::vector<std::uint8_t> GenerateKeyStream(std::size_t Count) override final;

		//RC4* - Complex Key-scheduling algorithm (KSA)
		void KeyScheduling(std::span<const std::uint8_t> Keys) override final;

		explicit RivestCipher4Star()
			:
			AlgorithmBase()
		{
			std::uint8_t ByteData = 0;
			for (auto& Key : KeyState)
			{
				Key = ByteData;
				++ByteData;
			}
			ByteData = 0;
		}

		virtual ~RivestCipher4Star() = default;
	};
}