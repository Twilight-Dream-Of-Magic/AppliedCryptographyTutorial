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

	写完整版本
	
	std::vector<std::uint8_t> RivestCipher4Star::KeyScheduling(std::span<const std::uint8_t> Keys)
	{
		// ---------- 本地工具：PHT 正反对、稳定平方模素数 ----------
		auto PseudoHadamardTransformForward32 = [](std::uint32_t LeftValue, std::uint32_t RightValue) -> std::pair<std::uint32_t, std::uint32_t>
		{
			// Forward PHT: (x, y) -> (x + y, x + 2y)
			const std::uint32_t ForwardLeftValue  = LeftValue + RightValue;
			const std::uint32_t ForwardRightValue = LeftValue + (RightValue << 1);
			return { ForwardLeftValue, ForwardRightValue };
		};

		auto PseudoHadamardTransformBackward32 = [](std::uint32_t LeftValue, std::uint32_t RightValue) -> std::pair<std::uint32_t, std::uint32_t>
		{
			// Backward form (按你原式保留): (u, v) -> (v - u, 2u - v)
			const std::uint32_t BackwardLeftValue  = RightValue - LeftValue;
			const std::uint32_t BackwardRightValue = (LeftValue << 1) - RightValue;
			return { BackwardLeftValue, BackwardRightValue };
		};

		auto SquareModuloPrimeNumber32Bit = [&](std::uint64_t Value) -> std::uint64_t
		{
			const std::uint64_t ReducedValue = Value % static_cast<std::uint64_t>(PrimeNumber32Bit);

	#if defined(__SIZEOF_INT128__)
			const unsigned __int128 Product = static_cast<unsigned __int128>(ReducedValue) * static_cast<unsigned __int128>(ReducedValue);
			return static_cast<std::uint64_t>(Product % static_cast<unsigned __int128>(PrimeNumber32Bit));
	#else
			// ReducedValue < 2^32，因此 64-bit 乘法不会溢出
			return (ReducedValue * ReducedValue) % static_cast<std::uint64_t>(PrimeNumber32Bit);
	#endif
		};

		// ---------- 0) 初始化 KeyState 为 0..255 ----------
		for (std::uint32_t ByteIndex = 0; ByteIndex < 256; ++ByteIndex)
		{
			KeyState[ByteIndex] = static_cast<std::uint8_t>(ByteIndex);
		}

		// ---------- 1) 处理空 Key：给确定行为 ----------
		if (Keys.empty())
		{
			static constexpr std::uint8_t ZeroKeyByte = 0;
			Keys = std::span<const std::uint8_t>(&ZeroKeyByte, 1);
		}

		// ---------- 2) 固定 16 字节 KeyBlock（你原设计：交错拼接） ----------
		std::array<std::uint8_t, 16> KeyBlock{};
		for (std::uint32_t KeyBlockIndex = 0; KeyBlockIndex < 16; ++KeyBlockIndex)
		{
			KeyBlock[KeyBlockIndex] = Keys[KeyBlockIndex % Keys.size()];
		}

		// ---------- 3) Cross-bit concat：交错拼 A/B ----------
		std::uint64_t ConcatenatedValueA = 0;
		std::uint64_t ConcatenatedValueB = 0;

		for (std::uint32_t KeyBlockIndex = 0; KeyBlockIndex < 16; ++KeyBlockIndex)
		{
			if (KeyBlockIndex & 1u)
			{
				ConcatenatedValueA = (ConcatenatedValueA << 8) | static_cast<std::uint64_t>(KeyBlock[KeyBlockIndex]);
			}
			else
			{
				ConcatenatedValueB = (ConcatenatedValueB << 8) | static_cast<std::uint64_t>(KeyBlock[KeyBlockIndex]);
			}
		}

		// ---------- 4) Folded Multiply：真 64x64->128 ----------
		const std::pair<std::uint64_t, std::uint64_t> FoldedMultiplyOutput =
			RivestCipher4Star::LongNumberMultiply(ConcatenatedValueA, ConcatenatedValueB);

		const std::uint64_t FoldedMultiplyHighPart = FoldedMultiplyOutput.first;
		const std::uint64_t FoldedMultiplyLowPart  = FoldedMultiplyOutput.second;

		// ---------- 5) MT19937_64：必须 key 可复现 ----------
		const std::uint64_t DeterministicSeed =
			(FoldedMultiplyHighPart ^ FoldedMultiplyLowPart) ^
			0xA5A5A5A5A5A5A5A5ull ^
			(static_cast<std::uint64_t>(Keys.size()) << 56);

		std::mt19937_64 PseudoRandomNumberGeneratorMT19937_64(DeterministicSeed);

		std::uint64_t RandomDataA = PseudoRandomNumberGeneratorMT19937_64();
		std::uint64_t RandomDataB = PseudoRandomNumberGeneratorMT19937_64();
		std::uint64_t RandomDataC = PseudoRandomNumberGeneratorMT19937_64();
		std::uint64_t RandomDataD = PseudoRandomNumberGeneratorMT19937_64();

		// ---------- 6) 生成 RandomDataArray：固定成本填满 256 word ----------
		std::array<std::uint32_t, 256> RandomDataArray{};

		for (std::uint32_t BlockIndex = 0; BlockIndex < 64; ++BlockIndex)
		{
			const std::uint32_t KeyIndex0 = (BlockIndex * 2u) % static_cast<std::uint32_t>(Keys.size());
			const std::uint32_t KeyIndex1 = (KeyIndex0 + 1u) % static_cast<std::uint32_t>(Keys.size());

			const std::uint8_t KeyByte0 = Keys[KeyIndex0];
			const std::uint8_t KeyByte1 = Keys[KeyIndex1];

			// RDA = KeyByte0 * KeyByte1
			RandomDataA = static_cast<std::uint64_t>(KeyByte0) * static_cast<std::uint64_t>(KeyByte1);

			// RDC = RDB + RDA
			RandomDataC = RandomDataB + RandomDataA;

			// Marsaglia-ish xorshift mixing（保留结构）
			RandomDataA ^= (RandomDataB << 23) ^ (RandomDataD << 59);
			RandomDataB ^= (RandomDataC << 5)  ^ (RandomDataA >> 41);
			RandomDataC ^= (RandomDataD << 37) ^ (RandomDataB >> 28);
			RandomDataD ^= (RandomDataA << 47) ^ (RandomDataC >> 16);

			// 平方模素数搅拌（保留）
			RandomDataA += SquareModuloPrimeNumber32Bit(RandomDataD);
			RandomDataB += SquareModuloPrimeNumber32Bit(RandomDataA);
			RandomDataC += SquareModuloPrimeNumber32Bit(RandomDataB);
			RandomDataD += SquareModuloPrimeNumber32Bit(RandomDataC);

			// 按你原“D, C, B, A”顺序落到 32-bit word（折叠 64->32）
			const std::uint32_t WriteBaseIndex = BlockIndex * 4u;

			RandomDataArray[WriteBaseIndex + 0] = static_cast<std::uint32_t>((RandomDataD ^ (RandomDataD >> 32)) & 0xFFFFFFFFu);
			RandomDataArray[WriteBaseIndex + 1] = static_cast<std::uint32_t>((RandomDataC ^ (RandomDataC >> 32)) & 0xFFFFFFFFu);
			RandomDataArray[WriteBaseIndex + 2] = static_cast<std::uint32_t>((RandomDataB ^ (RandomDataB >> 32)) & 0xFFFFFFFFu);
			RandomDataArray[WriteBaseIndex + 3] = static_cast<std::uint32_t>((RandomDataA ^ (RandomDataA >> 32)) & 0xFFFFFFFFu);
		}

		// ---------- 7) Forward PHT 用于播种 + 洗牌（Fisher–Yates） ----------
		const std::uint64_t ForwardSeedLeftValue  = RandomDataA + RandomDataB;
		const std::uint64_t ForwardSeedRightValue = RandomDataA + RandomDataB * 2;

		PseudoRandomNumberGeneratorMT19937_64.seed(ForwardSeedLeftValue ^ ForwardSeedRightValue);

		for (std::uint32_t ShuffleIndex = 255; ShuffleIndex > 0; --ShuffleIndex)
		{
			const std::uint32_t SwapIndex =
				static_cast<std::uint32_t>(PseudoRandomNumberGeneratorMT19937_64() % (static_cast<std::uint64_t>(ShuffleIndex) + 1ull));

			std::swap(RandomDataArray[ShuffleIndex], RandomDataArray[SwapIndex]);
		}

		// ---------- 8) 双 S 盒 + 门级折叠（你钉死：绝不移除） ----------
		for (auto& RandomData : RandomDataArray)
		{
			const std::uint32_t Bit0 =
				static_cast<std::uint32_t>(ByteSubstitutionBoxA[ ByteSubstitutionBoxA[(RandomData >> 24) & 0xFFu] ]) << 24;
			const std::uint32_t Bit1 =
				static_cast<std::uint32_t>(ByteSubstitutionBoxA[ ByteSubstitutionBoxA[(RandomData >> 16) & 0xFFu] ]) << 16;
			const std::uint32_t Bit2 =
				static_cast<std::uint32_t>(ByteSubstitutionBoxA[ ByteSubstitutionBoxA[(RandomData >>  8) & 0xFFu] ]) <<  8;
			const std::uint32_t Bit3 =
				static_cast<std::uint32_t>(ByteSubstitutionBoxA[ ByteSubstitutionBoxA[(RandomData	  ) & 0xFFu] ]);

			// ✅ 门级折叠：原样保留
			RandomData = (Bit0 & Bit1) ^ (Bit2 | Bit3);
		}

		// ---------- 9) 用 RandomDataArray 驱动 KeyState：正向 + 反向 PHT 的“交叉重洗”（按你指定的 swap 对） ----------
		for (const auto& RandomData : RandomDataArray)
		{
			std::uint32_t XorShiftState = RandomData;

			XorShiftState ^= (XorShiftState << 17);
			const std::uint32_t IndexValueA = XorShiftState;

			XorShiftState ^= (XorShiftState >> 15);
			const std::uint32_t IndexValueB = XorShiftState;

			XorShiftState ^= (XorShiftState << 26);
			const std::uint32_t IndexValueC = XorShiftState;

			const std::uint32_t IndexValueD = IndexValueA - IndexValueB - IndexValueC;

			const std::uint32_t LeftValue  = (IndexValueB ^ IndexValueD);
			const std::uint32_t RightValue = (IndexValueB & IndexValueC) + (IndexValueA | IndexValueD);

			const auto ForwardPair  = PseudoHadamardTransformForward32(LeftValue, RightValue);
			const auto BackwardPair = PseudoHadamardTransformBackward32(LeftValue, RightValue);

			// ✅ 你要的“交叉重洗”：
			std::swap(KeyState[ForwardPair.first  & 0xFFu], KeyState[BackwardPair.second & 0xFFu]);
			std::swap(KeyState[BackwardPair.first & 0xFFu], KeyState[ForwardPair.second  & 0xFFu]);
		}

		// ---------- 10) 生成 PermutationTable：只生成一次（无二次重洗） ----------
		for (std::uint32_t TableIndex = 0; TableIndex < 256; ++TableIndex)
		{
			PermutationTable[TableIndex] = KeyState[ KeyState[TableIndex] ];
		}

		// ---------- 11) 返回生成好的 PermutationTable ----------
		return std::vector<std::uint8_t>(PermutationTable.begin(), PermutationTable.end());
	}

	static inline std::uint8_t ReduceModulo251_Byte(std::uint8_t Value)
	{
		// Value: 0..255，最多减一次
		std::int16_t CandidateValue = static_cast<std::int16_t>(static_cast<std::int16_t>(Value) - 251);
		CandidateValue += (CandidateValue >> 15) & 251; // 负数则加回 251
		return static_cast<std::uint8_t>(CandidateValue);
	}

	static inline std::uint8_t ReduceModulo251_Sum(std::uint8_t LeftValue, std::uint8_t RightValue)
	{
		// Sum: 0..510，最多减两次
		std::int16_t CandidateValue = static_cast<std::int16_t>(static_cast<std::int16_t>(LeftValue) + static_cast<std::int16_t>(RightValue) - 251);
		CandidateValue += (CandidateValue >> 15) & 251;

		CandidateValue = static_cast<std::int16_t>(CandidateValue - 251);
		CandidateValue += (CandidateValue >> 15) & 251;

		return static_cast<std::uint8_t>(CandidateValue);
	}

	std::vector<std::uint8_t> RivestCipher4Star::GenerateKeyStream(std::size_t Count)
	{
		std::vector<std::uint8_t> KeyStream(Count);

		// 用 8-bit 索引让溢出自然回绕，避免 %256
		std::uint8_t LeftIndex = 0;
		std::uint8_t RightIndex = 0;

		// 本地指针加速（少一点边界检查开销）
		std::uint8_t* KeyStatePointer = KeyState.data();
		const std::uint8_t* PermutationTablePointer = PermutationTable.data();

		for (std::size_t RoundIndex = 0; RoundIndex < Count; ++RoundIndex)
		{
			const std::uint8_t LeftValue = KeyStatePointer[LeftIndex];
			const std::uint8_t RightValue = KeyStatePointer[RightIndex];

			// 你原本的“差分 + (和 mod 251)”结构：保留
			const std::uint8_t SumModuloPrime = ReduceModulo251_Sum(LeftValue, RightValue);
			const std::uint8_t OutputIndex =
				static_cast<std::uint8_t>((LeftValue - RightValue) ^ SumModuloPrime);

			// 你原本的“(Right >> 1) | ((Left % 251) << 7)”：
			// (Left % 251) << 7 等价于 ((Left % 251) & 1) << 7
			const std::uint8_t LeftReducedModuloPrime = ReduceModulo251_Byte(LeftValue);
			const std::uint8_t MostSignificantBit = static_cast<std::uint8_t>((LeftReducedModuloPrime & 1u) << 7);
			const std::uint8_t RotatedByteData = static_cast<std::uint8_t>((RightValue >> 1) | MostSignificantBit);

			// 你原本的“乘法 mod 256”其实就是取低 8 位：保留但更快
			const std::uint8_t MultipliedByteData =
				static_cast<std::uint8_t>(
					static_cast<std::uint16_t>(LeftIndex + RotatedByteData) *
					static_cast<std::uint16_t>(RightIndex + RotatedByteData)
				);

			LeftIndex = static_cast<std::uint8_t>(LeftIndex + MultipliedByteData);
			RightIndex = static_cast<std::uint8_t>(RightIndex + PermutationTablePointer[LeftIndex]);

			// RC4-ish swap：保留
			std::swap(KeyStatePointer[LeftIndex], KeyStatePointer[RightIndex]);

			KeyStream[RoundIndex] = KeyStatePointer[OutputIndex];
		}

		return KeyStream;
	}
}
