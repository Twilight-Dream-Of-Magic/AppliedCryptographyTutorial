#include "./PseudoRandomNumberGenerators.hpp"

namespace PseudoRandomNumberGenerators
{
	ComplementaryMultiplyWithCarry::ComplementaryMultiplyWithCarry(std::uint64_t Seed)
		: StateIndex(STATE_SIZE - 1), Increment64BitValue(9223372036854775783ULL)
	{
		std::array<std::uint64_t, STATE_SIZE / 2> StateArray{};

		StateArray[0] = Linear_Congruential(Seed, 1);
		StateArray[1] = Linear_Congruential(StateArray[0], 1);
		
		//Initialization state functions for the 64-bit version of Mersenne Twister (Edited)
		for (std::uint32_t Index = 2; Index < StateArray.size(); ++Index)
			StateArray[Index] = 0x5851F42D4C957F2DULL * (StateArray[Index - 1] ^ (StateArray[Index - 2] >> 62)) + Index;

		for (std::uint32_t Index = 0; Index < StateArray.size(); Index += 2)
		{
			this->StateArray[Index] = static_cast<std::uint32_t>( StateArray[Index] >> 32 );
			this->StateArray[Index + 1] = static_cast<std::uint32_t>( StateArray[Index] & 0x00000000FFFFFFFF );
		}

		memory_set_no_optimize_function<0x00>(StateArray.data(), sizeof(std::uint64_t) * StateArray.size());
	}

	ComplementaryMultiplyWithCarry::ComplementaryMultiplyWithCarry(std::uint32_t Seed)
		: StateIndex(STATE_SIZE - 1), Increment32BitValue(809430660)
	{
		std::uint64_t NewSeed = Linear_Congruential(Seed, 1);

		this->StateArray[0] = NewSeed & 0x00000000FFFFFFFFFFFFFULL;
		this->StateArray[1] = NewSeed >> 32;
		
		//Initialization state functions for the 32-bit version of Mersenne Twister (Edited)
		for (std::uint32_t Index = 2; Index < STATE_SIZE; ++Index)
			this->StateArray[Index] = 0x6C078965UL * (this->StateArray[Index - 1] ^ (this->StateArray[Index - 2] >> 30)) + Index;
	}

	ComplementaryMultiplyWithCarry::~ComplementaryMultiplyWithCarry()
	{
		this->Increment64BitValue = 0;
		this->Increment32BitValue = 0;
		this->StateIndex = 0;
		memory_set_no_optimize_function<0x00>(this->StateArray.data(), sizeof(std::uint32_t) * this->StateArray.size());
	}

	void ComplementaryMultiplyWithCarry::ResetState(std::uint64_t Seed)
	{
		this->Increment64BitValue = 0;
		this->Increment32BitValue = 0;
		this->StateIndex = 0;
		memory_set_no_optimize_function<0x00>(this->StateArray.data(), sizeof(std::uint32_t) * this->StateArray.size());

		std::uint64_t NewSeed = Linear_Congruential(Seed, 1);

		this->StateArray[0] = NewSeed & 0x00000000FFFFFFFFFFFFFULL;
		this->StateArray[1] = NewSeed >> 32;
		
		//Initialization state functions for the 32-bit version of Mersenne Twister (Edited)
		for (std::uint32_t Index = 2; Index < STATE_SIZE; ++Index)
			this->StateArray[Index] = 0x6C078965UL * (this->StateArray[Index - 1] ^ (this->StateArray[Index - 2] >> 30)) + Index;
	}

	std::uint64_t ComplementaryMultiplyWithCarry::NumberGeneration64Bit(std::uint64_t Iterations)
	{
		if (Iterations == 0)
			Iterations = 1;

		constexpr std::uint64_t Modulus64BitValue = 0xFFFFFFFFFFFFFFFEULL;
		constexpr std::uint32_t Modulus32BitValue = 0xFFFFFFFEULL;

		std::uint64_t SubtractionValue = 0;
		std::uint64_t RandomNumber = 0;

		for (std::uint64_t Count = 0; Count < Iterations; ++Count)
		{
			this->StateIndex = (this->StateIndex + 1) & (STATE_SIZE - 1);
			std::uint64_t TemporaryState = MultiplierValue * this->StateArray[this->StateIndex] + this->Increment64BitValue;
			
			this->Increment64BitValue = TemporaryState & 0x00000000FFFFFFFF;
			SubtractionValue = TemporaryState + this->Increment64BitValue;

			if (SubtractionValue < this->Increment64BitValue)
			{
				++SubtractionValue;
				++(this->Increment64BitValue);
			}

			std::uint64_t X = StateArray[this->StateIndex];
			StateArray[this->StateIndex] = static_cast<std::uint32_t>(Modulus64BitValue - SubtractionValue >> 32);
			RandomNumber = (static_cast<std::uint64_t>(X) << 32) | Modulus32BitValue - StateArray[this->StateIndex];

			std::cout << "Now Random Value of the 64 bit is:" << RandomNumber << std::endl;
		}

		return RandomNumber;
	}

	std::uint32_t ComplementaryMultiplyWithCarry::NumberGeneration32Bit(std::uint64_t Iterations)
	{
		if (Iterations == 0)
			Iterations = 1;

		constexpr std::uint32_t Modulus32BitValue = 0xFFFFFFFEULL;

		for (std::uint64_t Count = 0; Count < Iterations; ++Count)
		{
			this->StateIndex = (this->StateIndex + 1) & (STATE_SIZE - 1);
			std::uint64_t TemporaryState = MultiplierValue * this->StateArray[this->StateIndex] + this->Increment32BitValue;
			
			this->Increment32BitValue = static_cast<std::uint32_t>(TemporaryState >> 32);
			std::uint32_t SubtractionValue = static_cast<std::uint32_t>( TemporaryState + static_cast<std::uint64_t>(this->Increment32BitValue) & 0x00000000FFFFFFFF);

			if (SubtractionValue < this->Increment32BitValue)
			{
				++SubtractionValue;
				++(this->Increment32BitValue);
			}

			StateArray[this->StateIndex] = Modulus32BitValue - SubtractionValue;

			std::cout << "Now Random Value of the 32 bit is:" << StateArray[this->StateIndex] << std::endl;
		}

		return StateArray[this->StateIndex];
	}
}