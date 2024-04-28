#include "./PseudoRandomNumberGenerators.hpp"

namespace PseudoRandomNumberGenerators
{
	/* This is 64 bit version */

	MersenneTwister64Bit::MersenneTwister64Bit(std::uint64_t Seed)
		: StateIndex(STATE_SIZE)
	{
		this->StateArray[0] = Seed;
		for (std::uint32_t Index = 1; Index < STATE_SIZE; Index++)
			this->StateArray[Index] = 0x5851F42D4C957F2DULL * (this->StateArray[Index - 1] ^ (this->StateArray[Index - 1] >> WORD_SIZE - 2)) + Index;
	}

	MersenneTwister64Bit::~MersenneTwister64Bit()
	{
		this->StateIndex = 0;
		memory_set_no_optimize_function<0x00>(StateArray.data(), sizeof(std::uint64_t) * StateArray.size());
	}

	void MersenneTwister64Bit::TwistState()
	{
		constexpr std::uint64_t LOWER_MASK = (1ULL << SEPARTION_POINT) - 1ULL;
		constexpr std::uint64_t UPPER_MASK = ~(LOWER_MASK);

		for (std::uint32_t Index = 0; Index < STATE_SIZE; Index++)
		{
			//State the top and bottom halves of two different figures for pittance splicing. Then in order to form the vector part of the data and the matrix part that follows for matrix multiplication.
			std::uint64_t TemporaryState
			= (this->StateArray[Index] & UPPER_MASK) 
			| (this->StateArray[(Index + 1) % STATE_SIZE] & LOWER_MASK);

			//x' = x * Matrix
			std::uint64_t GF2MatrixMultipled = (TemporaryState >> 1);
			if ((TemporaryState & 1) == 1)
				GF2MatrixMultipled ^= TWIST_MATRIX_MAGIC_NUMBER;
			this->StateArray[Index] = this->StateArray[(Index + MIDDLE_WORD_OFFSET) % STATE_SIZE] ^ GF2MatrixMultipled;
		}

		this->StateIndex = 0;
	}

	void MersenneTwister64Bit::ResetState(std::uint64_t Seed)
	{
		this->StateIndex = 0;
		memory_set_no_optimize_function<0x00>(StateArray.data(), sizeof(std::uint64_t) * StateArray.size());

		this->StateArray[0] = Seed;
		for (std::uint32_t Index = 1; Index < STATE_SIZE; Index++)
			this->StateArray[Index] = 0x5851F42D4C957F2DULL * (this->StateArray[Index - 1] ^ (this->StateArray[Index - 1] >> WORD_SIZE - 2)) + Index;
	}

	std::uint64_t MersenneTwister64Bit::NumberGeneration(std::uint64_t Iterations)
	{
		if (Iterations == 0)
			Iterations = 1;

		std::uint64_t RandomNumber = 0;

		for (std::uint64_t Count = 0; Count < Iterations; ++Count)
		{
			if (this->StateIndex >= STATE_SIZE)
				this->TwistState();

			RandomNumber = this->StateArray[this->StateIndex];

			//y' = y * Matrix (Invertible matrix)
			RandomNumber ^= (RandomNumber >> TRANSFORM_BITSHIFT_C) & TRANSFORM_BITMASK_C;
			RandomNumber ^= (RandomNumber << TRANSFORM_BITSHIFT_A) & TRANSFORM_BITMASK_A;
			RandomNumber ^= (RandomNumber << TRANSFORM_BITSHIFT_B) & TRANSFORM_BITMASK_B;
			RandomNumber ^= (RandomNumber >> TRANSFORM_BITSHIFT_D);

			++(this->StateIndex);
		}

		return RandomNumber;
	}

	/* This is 32 bit version */

	MersenneTwister32Bit::MersenneTwister32Bit(std::uint32_t Seed)
		: StateIndex(STATE_SIZE)
	{
		this->StateArray[0] = Seed;
		for (std::uint32_t Index = 1; Index < STATE_SIZE; Index++)
			this->StateArray[Index] = 0x6C078965UL * (this->StateArray[Index - 1] ^ (this->StateArray[Index - 1] >> WORD_SIZE - 2)) + Index;
	}

	MersenneTwister32Bit::~MersenneTwister32Bit()
	{
		this->StateIndex = 0;
		memory_set_no_optimize_function<0x00>(StateArray.data(), sizeof(std::uint32_t) * StateArray.size());
	}

	void MersenneTwister32Bit::TwistState()
	{
		constexpr std::uint32_t LOWER_MASK = (1UL << SEPARTION_POINT) - 1UL;
		constexpr std::uint32_t UPPER_MASK = ~LOWER_MASK;

		for (std::uint32_t Index = 0; Index < STATE_SIZE; Index++)
		{
			//State the top and bottom halves of two different figures for pittance splicing. Then in order to form the vector part of the data and the matrix part that follows for matrix multiplication.
			std::uint64_t TemporaryState
			= (this->StateArray[Index] & UPPER_MASK) 
			| (this->StateArray[(Index + 1) % STATE_SIZE] & LOWER_MASK);
			std::uint64_t GF2MatrixMultipled = (TemporaryState >> 1);

			//x' = x * Matrix 
			if ((TemporaryState & 1) == 1)
				GF2MatrixMultipled ^= TWIST_MATRIX_MAGIC_NUMBER;
			this->StateArray[Index] = this->StateArray[(Index + MIDDLE_WORD_OFFSET) % STATE_SIZE] ^ GF2MatrixMultipled;
		}

		this->StateIndex = 0;
	}

	void MersenneTwister32Bit::ResetState(std::uint32_t Seed)
	{
		this->StateIndex = 0;
		memory_set_no_optimize_function<0x00>(StateArray.data(), sizeof(std::uint32_t) * StateArray.size());

		this->StateArray[0] = Seed;
		for (std::uint32_t Index = 1; Index < STATE_SIZE; Index++)
			this->StateArray[Index] = 0x6C078965UL * (this->StateArray[Index - 1] ^ (this->StateArray[Index - 1] >> WORD_SIZE - 2)) + Index;
	}

	std::uint32_t MersenneTwister32Bit::NumberGeneration(std::uint64_t Iterations)
	{
		if (Iterations == 0)
			Iterations = 1;

		std::uint32_t RandomNumber = 0;

		for (std::uint64_t Count = 0; Count < Iterations; ++Count)
		{
			if (this->StateIndex >= STATE_SIZE)
				this->TwistState();

			RandomNumber = this->StateArray[this->StateIndex];

			//y' = y * Matrix (Invertible matrix)
			RandomNumber ^= (RandomNumber >> TRANSFORM_BITSHIFT_C) & TRANSFORM_BITMASK_C;
			RandomNumber ^= (RandomNumber << TRANSFORM_BITSHIFT_A) & TRANSFORM_BITMASK_A;
			RandomNumber ^= (RandomNumber << TRANSFORM_BITSHIFT_B) & TRANSFORM_BITMASK_B;
			RandomNumber ^= (RandomNumber >> TRANSFORM_BITSHIFT_D);

			++(this->StateIndex);
		}

		return RandomNumber;
	}
}