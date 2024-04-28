#include "Rivest.h"

namespace StreamDataCryption::RC4
{
	std::vector<std::uint8_t> RivestCipher4::GenerateKeyStream(std::size_t Count)
	{
		std::vector<std::uint8_t> KeyStream;

		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		for ( std::uint64_t Round = 0; Round < Count; ++Round )
		{
			//i = i + 1 (mod 256)
			LeftIndex = Modulo256Addition( LeftIndex, 1 );
			//j = j + S[i] (mod 256)
			RightIndex = Modulo256Addition( RightIndex, KeyState[ LeftIndex ] );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			//S[(S[i] + S[j] (mod 256))]
			KeyStream.push_back( KeyState[ Modulo256Addition( KeyState[ LeftIndex ], KeyState[ RightIndex ] ) ] );
		}

		return KeyStream;
	}

	void RivestCipher4::KeyScheduling(std::span<const std::uint8_t> Keys)
	{
		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		while ( LeftIndex < 256 )
		{
			//j = (j + (S[i] + Keys[i (mod 256)] (mod 256) ) (mod 256))
			RightIndex = Modulo256Addition( RightIndex, Modulo256Addition( KeyState[ LeftIndex ], Keys[ LeftIndex % Keys.size() ] ) );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			++LeftIndex;
		}

		LeftIndex = 0;
		RightIndex = 0;
	}

	std::vector<std::uint8_t> RivestCipher4A::GenerateKeyStream(std::size_t Count)
	{
		std::vector<std::uint8_t> KeyStream;

		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;
		std::uint32_t RightIndex2 = 0;

		for ( std::uint64_t Round = 0; Round < Count; ++Round )
		{
			//i = i + 1 (mod 256)
			LeftIndex = Modulo256Addition( LeftIndex, 1 );
			//j = j + S1[i] (mod 256)
			RightIndex = Modulo256Addition( RightIndex, KeyState[ LeftIndex ] );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			//S2[S1[i] + S1[j] (mod 256)]
			KeyStream.push_back( KeyState2[ Modulo256Addition(KeyState[ LeftIndex ], KeyState[ RightIndex ]) ] );
			//j2 = j2 + S2[i] (mod 256)
			RightIndex2 = Modulo256Addition( RightIndex2, KeyState2[ LeftIndex ] );
			std::swap( KeyState2[ LeftIndex ], KeyState2[ RightIndex ] );
			//S1[S2[i] + S2[j2] (mod 256)]
			KeyStream.push_back( KeyState[ Modulo256Addition(KeyState2[ LeftIndex ], KeyState2[ RightIndex ]) ] );
		}

		return KeyStream;
	}

	void RivestCipher4A::KeyScheduling(std::span<const std::uint8_t> Keys)
	{
		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		while ( LeftIndex < 256 )
		{
			RightIndex = Modulo256Addition( RightIndex, Modulo256Addition( KeyState[ LeftIndex ], Keys[ LeftIndex % Keys.size() ] ) );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			++LeftIndex;
		}

		LeftIndex = 0;
		RightIndex = 0;

		while ( LeftIndex < 256 )
		{
			RightIndex = Modulo256Addition( RightIndex, Modulo256Addition( KeyState[ LeftIndex ], Keys[ LeftIndex % Keys.size() ] ) );
			std::swap( KeyState2[ LeftIndex ], KeyState2[ RightIndex ] );
			++LeftIndex;
		}
		LeftIndex = 0;
		RightIndex = 0;
	}

	std::vector<std::uint8_t> RivestCipher4_VMPC::GenerateKeyStream(std::size_t Count)
	{
		std::vector<std::uint8_t> KeyStream;

		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;
		std::uint32_t A = 0;
		std::uint32_t B = 0;

		for (std::uint64_t Round = 0; Round < Count; ++Round)
		{
			//a = S[i]
			A = KeyState[LeftIndex];
			//j = S[j + a (mod 256)]
			RightIndex = KeyState[Modulo256Addition(RightIndex, A)];
			//S[S[S[j] + 1 (mod 256)]]
			KeyStream.push_back(KeyState[KeyState[Modulo256Addition(KeyState[RightIndex], 1)]]);
			//b = S[j]
			B = KeyState[RightIndex];
			//S[i] = b
			KeyState[LeftIndex] = B;
			//S[j] = a
			KeyState[RightIndex] = A;
			//i = i + 1 (mod 256)
			LeftIndex = Modulo256Addition(LeftIndex, 1);
		}

		return KeyStream;
	}

	void RivestCipher4_VMPC::KeyScheduling(std::span<const std::uint8_t> Keys)
	{
		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		while ( LeftIndex < 256 )
		{
			RightIndex = Modulo256Addition( RightIndex, Modulo256Addition( KeyState[ LeftIndex ], Keys[ LeftIndex % Keys.size() ] ) );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			++LeftIndex;
		}

		LeftIndex = 0;
		RightIndex = 0;
	}

	std::vector<std::uint8_t> RivestCipher4Plus::GenerateKeyStream(std::size_t Count)
	{
		std::vector<std::uint8_t> KeyStream;

		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		for ( std::uint64_t Round = 0; Round < Count; ++Round )
		{
			//i = i + 1 (mod 256)
			LeftIndex = Modulo256Addition( LeftIndex, 1 );
			//a = S[i]
			std::uint8_t A = KeyState[ LeftIndex ];
			//j = j + a  (mod 256)
			RightIndex = Modulo256Addition( A, RightIndex );

			std::uint8_t B = KeyState[ RightIndex ]; //b = S[j]
			KeyState[ RightIndex ] = KeyState[ LeftIndex ]; //S[j] = S[i]
			KeyState[ LeftIndex ] = B; //S[i] = b

			//c = S[(i >> 3) ⊕ (j << 5) (mod 256)] + S[(i << 5) ⊕ (j >> 3) (mod 256)] (mod 256)
			std::uint8_t C = Modulo256Addition( KeyState[ ( (LeftIndex >> 3) ^ (RightIndex << 5) ) % 256 ], KeyState[ ( (LeftIndex << 5) ^ (RightIndex >> 3) ) % 256 ] );
			//(S[a + b (mod 256)] + S[c ⊕ 0xAA (mod 256)] (mod 256)) ⊕ S[j + b (mod 256)]
			KeyStream.push_back( ( Modulo256Addition(KeyState[Modulo256Addition(A, B)], KeyState[C ^ 0xAA]) ) ^ KeyState[ Modulo256Addition( RightIndex, B ) ] );
		}

		return KeyStream;
	}

	void RivestCipher4Plus::KeyScheduling(std::span<const std::uint8_t> Keys)
	{
		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		while ( LeftIndex < 256 )
		{
			RightIndex = Modulo256Addition( RightIndex, Modulo256Addition( KeyState[ LeftIndex ], Keys[ LeftIndex % Keys.size() ] ) );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			++LeftIndex;
		}

		LeftIndex = 0;
		RightIndex = 0;
	}

	std::vector<std::uint8_t> RivestCipher4_Spritz::GenerateKeyStream(std::size_t Count)
	{
		std::vector<std::uint8_t> KeyStream;

		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;
		std::uint32_t TemporaryValue = 0;
		std::uint32_t KeyStreamValue = 0;

		for ( std::uint64_t Round = 0; Round < Count; ++Round )
		{
			//i = i + w (mod 256)
			LeftIndex = Modulo256Addition( LeftIndex, 251 );
			//j = (k + S[j + S[i] (mod 256)] (mod 256))
			RightIndex = Modulo256Addition( TemporaryValue, KeyState[ Modulo256Addition( RightIndex, KeyState[ LeftIndex ] ) ] );
			//k = (k + (i + S[j] (mod 256)) (mod 256))
			TemporaryValue = Modulo256Addition( TemporaryValue, Modulo256Addition( LeftIndex, KeyState[ RightIndex ] ) );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			//z = S[j + S[i + S[z + k (mod 256)] (mod 256)] (mod 256)]
			KeyStreamValue = KeyState[ Modulo256Addition( RightIndex, KeyState[ Modulo256Addition(LeftIndex, KeyState[ Modulo256Addition( KeyStreamValue, TemporaryValue ) ]) ] ) ];
			KeyStream.push_back( KeyStreamValue );
		}

		return KeyStream;
	}

	void RivestCipher4_Spritz::KeyScheduling(std::span<const std::uint8_t> Keys)
	{
		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		while ( LeftIndex < 256 )
		{
			RightIndex = Modulo256Addition( RightIndex, Modulo256Addition( KeyState[ LeftIndex ], Keys[ LeftIndex % Keys.size() ] ) );
			std::swap( KeyState[ LeftIndex ], KeyState[ RightIndex ] );
			++LeftIndex;
		}

		LeftIndex = 0;
		RightIndex = 0;
	}

	std::pair<uint64_t, uint64_t> RivestCipher4Star::LongNumberMultiply(uint64_t x, uint64_t y)
	{
		// Initialize variables for result, multiplier and multiplicand
		std::uint64_t result_high = 0; // high part of result
		std::uint64_t result_low = 0; // low part of result
		std::uint64_t multiplier = x; // multiplier operand
		std::uint64_t multiplicand = y; // multiplicand operand

		// Loop over the bits of multiplier from least significant to most significant
		for (std::uint64_t i = 0; i < 64; i++)
		{
			// If the current bit of multiplier is one
			if (multiplier & 1)
			{
				// Add multiplicand to result_low using XOR operation
				result_low ^= multiplicand;
				// If there is a carry from adding multiplicand to result_low
				if (result_low < multiplicand)
				{
					// Add one to result_high using XOR operation
					result_high ^= 1;
				}
				// Add any remaining bits of multiplicand to result_high using OR operation
				result_high |= (multiplicand >> 63);
			}
			// Right shift multiplier by one bit using logical shift operator (>>)
			multiplier >>= 1;
			// Left shift multiplicand by one bit using logical shift operator (<<)
			multiplicand <<= 1;
		}

		// Return a pair of high part and low part of result
		return std::make_pair(result_high, result_low);
	}

	std::vector<std::uint8_t> RivestCipher4Star::GenerateKeyStream(std::size_t Count)
	{
		std::vector<std::uint8_t> KeyStream;

		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		for (std::uint64_t Round = 0; Round < Count; ++Round)
		{
			// XOR the two least significant bits of the two state variables
			std::uint8_t KeyByteData = (KeyState[LeftIndex] - KeyState[RightIndex]) ^ ((KeyState[RightIndex] + KeyState[LeftIndex]) % 251);

			// Perform a bitwise rotation
			std::uint8_t RotatedByteData = (KeyState[RightIndex] >> 1) | ((KeyState[LeftIndex] % 251) << 7);

			// Multiply the rotated byte by a constant and take the lowest 8 bits
			std::uint8_t MultipliedByteData = ((LeftIndex + RotatedByteData) * (RightIndex + RotatedByteData)) % 256;

			// Use the multiplied byte to calculate the new LeftIndex
			LeftIndex = Modulo256Addition(LeftIndex, MultipliedByteData);

			// Use the PermutationTable to calculate the new RightIndex
			RightIndex = Modulo256Addition(RightIndex, PermutationTable[LeftIndex]);

			// Swap the two state variables
			std::swap(KeyState[LeftIndex], KeyState[RightIndex]);

			// Push the byte to the keystream
			KeyStream.push_back(KeyState[KeyByteData]);
		}

		for (std::uint32_t i = 0, j = 0; i < 256; i++)
		{
			j = Modulo256Addition(j, Modulo256Addition(KeyState[i % KeyState.size()], PermutationTable[i]));
			std::swap(PermutationTable[i], PermutationTable[j]);
		}

		return KeyStream;
	}

	void RivestCipher4Star::KeyScheduling(std::span<const std::uint8_t> Keys)
	{
		std::vector<std::uint32_t> RandomDataArray;

		//RDA, RDB, RDC, RDD
		std::uint64_t RandomDataA = 0;
		std::uint64_t RandomDataB = 0;
		std::uint64_t RandomDataC = 0;
		std::uint64_t RandomDataD = 0;

		std::pair<std::uint64_t, std::uint64_t> Two64Bit{ 0,0 };
		auto& [A, B] = Two64Bit;

		/*
			Key(8-bit array)
			Keys = Keys[0], Keys[1], Key[2], Keys[3], Keys[4], Keys[5]
			B(64-bit) = Keys[0],  Keys[2], Keys[4] ......
			A(64-bit) = Keys[1],  Keys[3], Keys[5] ......
		*/
		for (std::uint32_t i = 0; i < 16; i++)
		{
			//The cross bit concat
			if (i & 1)
			{
				A <<= 8;
				A |= static_cast<std::uint64_t>(Keys[i]);
			}
			{
				B <<= 8;
				B |= static_cast<std::uint64_t>(Keys[i]);
			}
		}

		std::mt19937_64 PRNG_MT19937(std::mt19937_64::default_seed);

		if (std::ranges::all_of(Keys.begin(), Keys.end(), [](std::uint8_t key) { return key == 0; }) == true)
		{
			RandomDataA = PRNG_MT19937();
			RandomDataB = RandomDataA * 2 + PRNG_MT19937();
			RandomDataC = RandomDataB * 2 + PRNG_MT19937();
			RandomDataD = RandomDataC * 2 + PRNG_MT19937();
		}
		else
		{
			//Folded Multiply Algorithm Step:
			//1. (A * B) -> A'(C_High) B'(C_Low)
			//2. A' bit_xor B'
			Two64Bit = LongNumberMultiply(A, B);
			PRNG_MT19937.seed(A ^ B);

			RandomDataA = PRNG_MT19937();
			RandomDataB = RandomDataA * 2 + PRNG_MT19937();
			RandomDataC = RandomDataB * 2 + PRNG_MT19937();
			RandomDataD = RandomDataC * 2 + PRNG_MT19937();
		}

		//Generate Random Data With Use Byte Key
		for (std::uint64_t KeyIndex = 0; KeyIndex < Keys.size(); KeyIndex += 2)
		{
			//RDA = Keys[i] * Keys[(i + 1) mod Keys.size()]
			//RDC = RDB + RDA (mod 2^64)
			RandomDataA = Keys[KeyIndex] * Keys[(KeyIndex + 1) % Keys.size()];
			std::uint64_t RandomDataC = RandomDataB + RandomDataA;

			// Perform bitwise mixing here (Marsaglia's Xorshift)

			RandomDataA ^= (RandomDataB << 23) ^ (RandomDataD << 59);
			RandomDataB ^= (RandomDataC << 5) ^ (RandomDataA >> 41);
			RandomDataC ^= (RandomDataD << 37) ^ (RandomDataB >> 28);
			RandomDataD ^= (RandomDataA << 47) ^ (RandomDataC >> 16);

			RandomDataA += ((RandomDataD * RandomDataD) % PrimeNumber32Bit);
			RandomDataB += ((RandomDataA * RandomDataA) % PrimeNumber32Bit);
			RandomDataC += ((RandomDataB * RandomDataB) % PrimeNumber32Bit);
			RandomDataD += ((RandomDataC * RandomDataC) % PrimeNumber32Bit);

			if (RandomDataD > std::numeric_limits<std::uint32_t>::max())
			{
				RandomDataArray.push_back(RandomDataD >> 32);
				RandomDataArray.push_back(RandomDataD & 0x00000000FFFFFFFF);
			}
			else
			{
				RandomDataArray.push_back(RandomDataD);
			}

			if (RandomDataC > std::numeric_limits<std::uint32_t>::max())
			{
				RandomDataArray.push_back(RandomDataC >> 32);
				RandomDataArray.push_back(RandomDataC & 0x00000000FFFFFFFF);
			}
			else
			{
				RandomDataArray.push_back(RandomDataC);
			}

			if (RandomDataB > std::numeric_limits<std::uint32_t>::max())
			{
				RandomDataArray.push_back(RandomDataB >> 32);
				RandomDataArray.push_back(RandomDataB & 0x00000000FFFFFFFF);
			}
			else
			{
				RandomDataArray.push_back(RandomDataB);
			}

			if (RandomDataA > std::numeric_limits<std::uint32_t>::max())
			{
				RandomDataArray.push_back(RandomDataA >> 32);
				RandomDataArray.push_back(RandomDataA & 0x00000000FFFFFFFF);
			}
			else
			{
				RandomDataArray.push_back(RandomDataA);
			}
		}

		//Pseudo-Hadamard Transformation (Forward)
		std::uint64_t RandomSeed = RandomDataA + RandomDataB;
		std::uint64_t RandomSeed2 = RandomDataA + RandomDataB * 2;

		PRNG_MT19937.seed(RandomSeed ^ RandomSeed2);

		//Shuffle Generated Random Data Array
		for (std::uint64_t RandomDataArrayIndex = 0; RandomDataArrayIndex < RandomDataArray.size(); ++RandomDataArrayIndex)
		{
			std::swap(RandomDataArray[RandomDataArrayIndex], RandomDataArray[PRNG_MT19937() % RandomDataArray.size()]);
		}

		//Apply Byte Substitution Box
		for (auto& RandomData : RandomDataArray)
		{
			std::uint32_t Bit0 = static_cast<std::uint32_t>(ByteSubstitutionBoxA[ByteSubstitutionBoxA[(RandomData >> 24) & 0xFF]]) << 24;
			std::uint32_t Bit1 = static_cast<std::uint32_t>(ByteSubstitutionBoxA[ByteSubstitutionBoxA[(RandomData >> 16) & 0xFF]]) << 16;
			std::uint32_t Bit2 = static_cast<std::uint32_t>(ByteSubstitutionBoxA[ByteSubstitutionBoxA[(RandomData >> 8) & 0xFF]]) << 8;
			std::uint32_t Bit3 = static_cast<std::uint32_t>(ByteSubstitutionBoxA[ByteSubstitutionBoxA[(RandomData) & 0xFF]]);
			RandomData = (Bit0 & Bit1) ^ (Bit2 | Bit3);
		}

		std::uint32_t IndexA = 0;
		std::uint32_t IndexB = 0;
		std::uint32_t IndexC = 0;
		std::uint32_t IndexD = 0;

		std::uint32_t LeftIndex = 0;
		std::uint32_t RightIndex = 0;

		//Fill Random Index And Apply Swap opertion to KeyState Array
		for (const auto& RandomData : RandomDataArray)
		{
			std::uint32_t XorShift = RandomData;
			XorShift ^= (XorShift << 17); //a
			IndexA = XorShift;
			XorShift ^= (XorShift >> 15); //b
			IndexB = XorShift;
			XorShift ^= (XorShift << 26); //c
			IndexC = XorShift;
			IndexD = IndexA - IndexB - IndexC;

			std::uint32_t L = (IndexB ^ IndexD);
			std::uint32_t R = (IndexB & IndexC) + (IndexA | IndexD);

			//Pseudo-Hadamard Transformation (Backward)
			LeftIndex = R - L;
			RightIndex = L * 2 - R;

			std::swap(KeyState[LeftIndex % 256], KeyState[RightIndex % 256]);
		}

		for (std::uint32_t i = 0; i < 256; i++)
		{
			PermutationTable[i] = KeyState[KeyState[i]];
		}
	}
}
