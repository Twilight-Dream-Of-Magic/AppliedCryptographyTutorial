#pragma once

#include "../MainProgram/Support+Library/Support-Library.hpp"

namespace PseudoRandomNumberGenerators
{
	template<std::integral DataType>
	class UniformRandomBitGenerator
	{
	public:
		using result_type = DataType;

		static constexpr result_type max() { return std::numeric_limits<result_type>::max(); }
		static constexpr result_type min() { return std::numeric_limits<result_type>::min(); }
	};
}

#include "XorshiftFamily.h"

namespace PseudoRandomNumberGenerators
{
	inline std::uint64_t Linear_Congruential(std::uint64_t Seed, std::uint64_t Iterations);

	inline std::int64_t Nonlinear_InversiveCongruential(std::uint64_t Seed, std::uint64_t Iterations);

	class BlumBlumShub
	{

	private:

		std::linear_congruential_engine<std::uint32_t, 134775813UL, 1UL, (1UL << 32) - 1> LCG;
		bool IsPrimeNumber(std::uint64_t Number);
		std::uint64_t FastPowerAlgorithmWithModulus(std::uint64_t BaseNumber, std::uint64_t ExponentNumber, std::uint64_t ModulusNumber);

		std::uint32_t Q = 0, P = 0;
		std::uint64_t N = 0;
		std::uint64_t X = 0;

	public:

		void ResetState(std::uint64_t Seed);
		std::uint64_t NumberGeneration(std::uint64_t Iterations);

		BlumBlumShub() = delete;

		BlumBlumShub(std::uint64_t Seed)
			: LCG(Seed)
		{
			if(Seed == 0);
				this->ResetState(1);
			this->ResetState(Seed);
		}

		~BlumBlumShub() = default;
	};

	class ComplementaryMultiplyWithCarry
	{

	private:
		static constexpr std::uint32_t STATE_SIZE = 4096;
		std::array<std::uint32_t, STATE_SIZE> StateArray{};
		std::uint32_t StateIndex;

		/*
			The value of the multiply constant can have an impact on the period and statistical properties of the generator.
			In general, a larger multiply constant may lead to a longer period, but may also result in poorer statistical properties, such as a higher correlation between consecutive random numbers.

			Therefore, it is generally recommended to use a value for the multiply constant that has been carefully chosen to balance these factors.
			The value of 18782ULL is one such value that has been found to work well in practice.

			However, if you wish to use a different value for the multiply constant, you can try using a value that is close to a power of 2, such as 16382ULL or 32764ULL.
			These values may result in a generator with similar statistical properties to the original generator, but with a longer period.

			It is also possible to use a constant that is the result of a calculation, such as a hash of a seed value or a combination of multiple constants.
			However, be aware that the statistical properties of the generator may be less predictable if you use a custom multiply constant.
		*/
		static constexpr std::uint64_t MultiplierValue = 18782ULL; //18446744073709551500ULL

		/*
			In the Complementary-multiply-with-carry (CMWC) pseudo-random number generator, the value of the c(Increment) variable is typically chosen to be a small constant, such as 362436 or 809430660, to ensure that the generator has a long period.

			However, it is possible to use a larger constant for the c(Increment) variable if desired. One way to choose a larger constant is to use a prime number that is close to a power of 2. 
			For example, you could use the following constant:
			@code
			uint64_t Increment = (1ULL << 63) - 25;
			@endcode

			This constant is a prime number that is close to the largest 64-bit unsigned integer (2^64 - 1), which may help to ensure that the generator has a long period.

			Alternatively, you could use a constant that is the result of a calculation, such as a hash of a seed value or a combination of multiple constants.
			For example:
			@code
			uint64_t Increment = (1ULL << 63) - (seed ^ (seed >> 32));
			@endcode
		*/
		std::uint32_t Increment32BitValue;
		std::uint64_t Increment64BitValue;

	public:
		void ResetState(std::uint64_t Seed);
		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations);
		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations);

		ComplementaryMultiplyWithCarry() = delete;

		explicit ComplementaryMultiplyWithCarry(std::uint64_t Seed);
		explicit ComplementaryMultiplyWithCarry(std::uint32_t Seed);
		~ComplementaryMultiplyWithCarry();

	};

	class MersenneTwister64Bit
	{

	private:
		//word size (in number of bits)
		static constexpr std::uint32_t WORD_SIZE = 64;
		
		//degree of recurrence
		static constexpr std::uint32_t STATE_SIZE = 312;
		
		//middle word, an offset used in the recurrence relation defining the series x
		//1 ≤ middle word ＜ degree of recurrence
		static constexpr std::uint32_t MIDDLE_WORD_OFFSET = 156;

		//separation point of one word, or the number of bits of the lower bitmask
		//0 ≤ separation point ≤ word size - 1
		static constexpr std::uint32_t SEPARTION_POINT = 31;

		//coefficients of the rational normal form twist matrix
		static constexpr std::uint64_t TWIST_MATRIX_MAGIC_NUMBER = 0xB5026F5AA96619E9ULL;

		//TwistedGaloisFeedbackRegister(SEPARTION_POINT)
		//Mersenne Twister bit mask for this applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITMASK_A = 0x71D67FFFEDA60000ULL;
		static constexpr std::uint64_t TRANSFORM_BITMASK_B = 0xFFF7EEE000000000ULL;

		//TwistedGaloisFeedbackRegister(SEPARTION_POINT)
		//Mersenne Twister bit shift for this applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_A = 17;
		//TwistedGaloisFeedbackRegister(SEPARTION_POINT)
		//Mersenne Twister bit shift for this applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_B = 37;

		//Mersenne Twister additional bit shifts for applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_C = 29;
		//Mersenne Twister additional bit shifts for applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_D = 43;
		//Mersenne Twister additional bit masks for applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITMASK_C = 0x5555555555555555ULL;

		std::array<std::uint64_t, STATE_SIZE> StateArray {};
		std::uint32_t StateIndex;

		void TwistState();

	public:
		void ResetState(std::uint64_t Seed);
		std::uint64_t NumberGeneration(std::uint64_t Iterations);

		MersenneTwister64Bit() = delete;

		explicit MersenneTwister64Bit(std::uint64_t Seed);
		~MersenneTwister64Bit();
	};

	class MersenneTwister32Bit
	{

	private:
		//word size (in number of bits)
		static constexpr std::uint32_t WORD_SIZE = 32;
		
		//degree of recurrence
		static constexpr std::uint32_t STATE_SIZE = 624;
		
		//middle word, an offset used in the recurrence relation defining the series x
		//1 ≤ middle word ＜ degree of recurrence
		static constexpr std::uint32_t MIDDLE_WORD_OFFSET = 397;
		
		//separation point of one word, or the number of bits of the lower bitmask
		//0 ≤ separation point ≤ word size - 1
		static constexpr std::uint32_t SEPARTION_POINT = 31;

		//coefficients of the rational normal form twist matrix
		static constexpr std::uint32_t TWIST_MATRIX_MAGIC_NUMBER = 0x9908B0DFUL;

		//TwistedGaloisFeedbackRegister(SEPARTION_POINT)
		//Mersenne Twister bit mask for this applying tempering transform
		static constexpr std::uint32_t TRANSFORM_BITMASK_A = 0x9D2C5680UL;
		//TwistedGaloisFeedbackRegister(SEPARTION_POINT)
		//Mersenne Twister bit mask for this applying tempering transform
		static constexpr std::uint32_t TRANSFORM_BITMASK_B = 0xEFC60000UL;

		//TwistedGaloisFeedbackRegister(SEPARTION_POINT)
		//Mersenne Twister bit shift for this applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_A = 7;
		//TwistedGaloisFeedbackRegister(SEPARTION_POINT)
		//Mersenne Twister bit shift for this applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_B = 15;

		//Mersenne Twister additional bit shifts for applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_C = 11;
		//Mersenne Twister additional bit shifts for applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITSHIFT_D = 18;
		//Mersenne Twister additional bit masks for applying tempering transform
		static constexpr std::uint64_t TRANSFORM_BITMASK_C = 0xFFFFFFFFUL;

		std::array<std::uint32_t, STATE_SIZE> StateArray{};
		std::uint32_t StateIndex;

		void TwistState();

	public:
		void ResetState(std::uint32_t Seed);
		std::uint32_t NumberGeneration(std::uint64_t Iterations);

		MersenneTwister32Bit() = delete;

		explicit MersenneTwister32Bit(std::uint32_t Seed);
		~MersenneTwister32Bit();
	};

	enum AlgorithmNames : std::uint64_t
	{
		BLUM_BLUM_SHUB = 0,
		COMPLEMENTARY_MULTIPLY_WITH_CARRY = 1,
		MERSENNE_TWISTER = 2,
	};

	class ManagerPRNG32Bit : public UniformRandomBitGenerator<std::uint32_t>
	{
	private:
		std::unordered_set<AlgorithmNames> AlgorithmNameHashSet;

		//Pointer to Instances
		std::unique_ptr<ComplementaryMultiplyWithCarry> ComplementaryMultiplyWithCarryPointer = nullptr;
		std::unique_ptr<MersenneTwister32Bit> MersenneTwisterPointer = nullptr;

		std::uint32_t CurrentSeed = 0;

	public:
		void add_algorithm(AlgorithmNames AlgorithmName)
		{
			if(AlgorithmNameHashSet.find(AlgorithmName) == AlgorithmNameHashSet.end())
			{
				AlgorithmNameHashSet.insert(AlgorithmName);

				switch (AlgorithmName)
				{
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						ComplementaryMultiplyWithCarryPointer = std::make_unique<ComplementaryMultiplyWithCarry>(CurrentSeed);
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						MersenneTwisterPointer = std::make_unique<MersenneTwister32Bit>(CurrentSeed);
					}

					default:
						break;
				}
			}
		}

		void remove_algorithm(AlgorithmNames AlgorithmName)
		{
			if(AlgorithmNameHashSet.find(AlgorithmName) != AlgorithmNameHashSet.end())
			{
				AlgorithmNameHashSet.erase(AlgorithmName);

				switch (AlgorithmName)
				{
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						ComplementaryMultiplyWithCarryPointer.reset();
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						MersenneTwisterPointer.reset();
						break;
					}

					default:
						break;
				}
			}
		}

		void seed(AlgorithmNames AlgorithmName, std::uint32_t SeedValue)
		{
			CurrentSeed = SeedValue;

			if(AlgorithmNameHashSet.find(AlgorithmName) != AlgorithmNameHashSet.end())
			{
				switch (AlgorithmName)
				{
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						ComplementaryMultiplyWithCarryPointer->ResetState(CurrentSeed);
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						MersenneTwisterPointer->ResetState(CurrentSeed);
						break;
					}

					default:
						break;
				}
			}
		}

		std::uint32_t operator()(AlgorithmNames AlgorithmName, std::uint64_t Iterations)
		{
			if(AlgorithmNameHashSet.find(AlgorithmName) != AlgorithmNameHashSet.end())
			{
				std::uint32_t Value = 0;

				switch (AlgorithmName)
				{
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						Value = ComplementaryMultiplyWithCarryPointer->NumberGeneration32Bit(Iterations);
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						Value = MersenneTwisterPointer->NumberGeneration(Iterations);
						break;
					}

					default:
						break;
				}

				return Value;
			}
		}

		void discard(AlgorithmNames AlgorithmName, std::size_t Round, std::uint64_t Iterations)
		{
			while (--Round)
			{
				operator()(AlgorithmName, Iterations);
			}
		}

		ManagerPRNG32Bit(std::uint32_t SeedValue) 
			: CurrentSeed(SeedValue)
		{
			if(CurrentSeed == 0)
				CurrentSeed = 1;
		}

		~ManagerPRNG32Bit() = default;
	};

	class ManagerPRNG64Bit : public UniformRandomBitGenerator<std::uint64_t>
	{
	private:
		std::unordered_set<AlgorithmNames> AlgorithmNameHashSet;

		//Pointer to Instances
		std::unique_ptr<BlumBlumShub> BlumBlumShubPointer = nullptr;
		std::unique_ptr<ComplementaryMultiplyWithCarry> ComplementaryMultiplyWithCarryPointer = nullptr;
		std::unique_ptr<MersenneTwister64Bit> MersenneTwisterPointer = nullptr;

		std::uint64_t CurrentSeed = 0;

	public:
		void add_algorithm(AlgorithmNames AlgorithmName)
		{
			if(AlgorithmNameHashSet.find(AlgorithmName) == AlgorithmNameHashSet.end())
			{
				AlgorithmNameHashSet.insert(AlgorithmName);

				switch (AlgorithmName)
				{
					case AlgorithmNames::BLUM_BLUM_SHUB:
					{
						BlumBlumShubPointer = std::make_unique<BlumBlumShub>(CurrentSeed);
						break;
					}
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						ComplementaryMultiplyWithCarryPointer = std::make_unique<ComplementaryMultiplyWithCarry>(CurrentSeed);
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						MersenneTwisterPointer = std::make_unique<MersenneTwister64Bit>(CurrentSeed);
						break;
					}

					default:
						break;
				}
			}

			
		}

		void remove_algorithm(AlgorithmNames AlgorithmName)
		{
			if(AlgorithmNameHashSet.find(AlgorithmName) != AlgorithmNameHashSet.end())
			{
				AlgorithmNameHashSet.erase(AlgorithmName);

				switch (AlgorithmName)
				{
					case AlgorithmNames::BLUM_BLUM_SHUB:
					{
						BlumBlumShubPointer.reset();
						break;
					}
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						ComplementaryMultiplyWithCarryPointer.reset();
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						MersenneTwisterPointer.reset();
						break;
					}

					default:
						break;
				}
			}
		}

		void seed(AlgorithmNames AlgorithmName, std::uint64_t SeedValue)
		{
			CurrentSeed = SeedValue;

			if(AlgorithmNameHashSet.find(AlgorithmName) != AlgorithmNameHashSet.end())
			{
				switch (AlgorithmName)
				{
					case AlgorithmNames::BLUM_BLUM_SHUB:
					{
						BlumBlumShubPointer->ResetState(CurrentSeed);
						break;
					}
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						ComplementaryMultiplyWithCarryPointer->ResetState(CurrentSeed);
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						MersenneTwisterPointer->ResetState(CurrentSeed);
						break;
					}

					default:
						break;
				}
			}
		}

		std::uint64_t operator()(AlgorithmNames AlgorithmName, std::uint64_t Iterations)
		{
			if(AlgorithmNameHashSet.find(AlgorithmName) != AlgorithmNameHashSet.end())
			{
				std::uint64_t Value = 0;

				switch (AlgorithmName)
				{
					case AlgorithmNames::BLUM_BLUM_SHUB:
					{
						Value = BlumBlumShubPointer->NumberGeneration(Iterations);
						break;
					}
					case AlgorithmNames::COMPLEMENTARY_MULTIPLY_WITH_CARRY:
					{
						Value = ComplementaryMultiplyWithCarryPointer->NumberGeneration64Bit(Iterations);
						break;
					}
					case AlgorithmNames::MERSENNE_TWISTER:
					{
						Value = MersenneTwisterPointer->NumberGeneration(Iterations);
						break;
					}

					default:
						break;
				}

				return Value;
			}
		}

		void discard(AlgorithmNames AlgorithmName, std::size_t Round, std::uint64_t Iterations)
		{
			while (--Round)
			{
				operator()(AlgorithmName, Iterations);
			}
		}

		ManagerPRNG64Bit(std::uint64_t SeedValue)
			: CurrentSeed(SeedValue)
		{
			if(CurrentSeed == 0)
				CurrentSeed = 1;
		}

		~ManagerPRNG64Bit() = default;
	};

	/*
		Reference Papers : https://christoph-conrads.name/faster-ranlux-pseudo-random-number-generators/

		A New Class of Random Number Generators by George Marsaglia, Arif Zaman
		DOI: 10.1214/aoap/1177005878

		GENERATOR AWC(AdditionWithCarry):
		WORD SIZE W	|	LONG LAG R	|	SHORT LAG S	|	TIME TO CHAOS T_C	|	BASE 2 LOGARITHM OF PERIOD LENGTH
			16				9				2					10								139
			16				18				13					16								287
			32				8				3					22								255
			32				16				3					17								509
			32				29				17					27								927
			64				25				14					51								< 1600
			64				28				11					44								< 1792

		SWB(Method II)(SubtractWithBorrow):
		WORD SIZE W	|	LONG LAG R	|	SHORT LAG S	|	TIME TO CHAOS T_C	|	BASE 2 LOGARITHM OF PERIOD LENGTH
			32				19				16					32								> 47
			64				15				2					30								959
			64				17				14					62								< 1088
			64				62				5					21								> 165

		SWB(Method I)(SubtractWithBorrow):
		WORD SIZE W	|	LONG LAG R	|	SHORT LAG S	|	TIME TO CHAOS T_C	|	BASE 2 LOGARITHM OF PERIOD LENGTH
			16				5				3					15								71
			16				11				3					11								169
			16				11				5					13								167
			24				10				24					18								570
			32				7				3					25								217
			32				16				9					27								506
			32				17				3					17								534
			32				21				6					19								664
			64				13				7					51								818
	*/

	template<typename DataType, std::size_t ShortLag, std::size_t LongLag>
	class AdditionWithCarry : UniformRandomBitGenerator<DataType>
	{

	public:
		//The prototype implementation
		static_assert(std::is_integral<DataType>::value, "");
		static_assert(!std::is_signed<DataType>::value, "");
		static_assert(ShortLag > 0U, "");
		static_assert(LongLag > ShortLag, "");

		using result_type = DataType;
		
		static constexpr auto long_lag = LongLag;
		static constexpr auto short_lag = ShortLag;
		
		// Set the modulus and result modulus based on the type of DataType
		static constexpr DataType modulus_value =
			std::is_same_v<DataType, std::uint64_t>
			? (std::uint64_t(1) << 64) - 2
			: std::is_same_v<DataType, std::uint32_t>
			? (std::uint32_t(1) << 32) - 2
			: std::is_same_v<DataType, std::uint16_t>
			? (std::uint16_t(1) << 16) - 2
			: std::is_same_v<DataType, std::uint8_t>
			? (std::uint8_t(1) << 8) - 2
			: 0;

		static constexpr DataType result_modulus_value =
			std::is_same_v<DataType, std::uint64_t>
			? 18446744073709551557ULL
			: std::is_same_v<DataType, std::uint32_t>
			? 4294967291UL
			: std::is_same_v<DataType, std::uint16_t>
			? 65521U
			: std::is_same_v<DataType, std::uint8_t>
			? 251U
			: 0U;

		result_type operator()()
		{
			// Compute the indices for the previous values
			std::size_t IndexA = (this->StateIndex - long_lag) % long_lag;
			std::size_t IndexB = (this->StateIndex - short_lag) % long_lag;

			// Compute the next random value using the Addition-With-Carry method
			auto Result = this->StateSequence[IndexA] + this->StateSequence[IndexB] + this->StateCarryValue;
			this->StateCarryValue = (static_cast<std::int64_t>(Result) >= static_cast<std::int64_t>(modulus_value)) ? 1U : 0U;
			if (this->StateCarryValue == 1U)
				Result %= modulus_value;

			this->StateIndex = (this->StateIndex + 1U == long_lag ? 0U : this->StateIndex + 1U);
			this->StateSequence[this->StateIndex] = Result;
			
			return Result % result_modulus_value;
		}

		void discard(std::size_t Round)
		{
			//Discard the pseudo-random numbers generated by the long_lag round
			for (std::size_t Round = 0; Round < long_lag; ++Round)
				(*this)();
		}

		AdditionWithCarry(DataType Seed = 123456U)
		{
			if constexpr (std::numeric_limits<DataType>::digits > 32)
			{
				std::linear_congruential_engine<std::uint64_t, 6364136223846793005ULL, 1442695040888963407ULL, (1ULL << 64) - 1> LCG(Seed);

				for (std::size_t Index = 0; Index < long_lag; ++Index)
					this->StateSequence[Index] = static_cast<DataType>(LCG());
			}
			else
			{
				std::linear_congruential_engine<std::uint32_t, 134775813UL, 1UL, (1UL << 32) - 1> LCG(Seed);

				for (std::size_t Index = 0; Index < long_lag; ++Index)
					this->StateSequence[Index] = static_cast<DataType>(LCG());
			}

			//Discard the pseudo-random numbers generated by the long_lag round
			for (std::size_t Round = 0; Round < long_lag; ++Round)
				(*this)();
		}

		~AdditionWithCarry()
		{
			memory_set_no_optimize_function<0x00>(StateSequence.data(), sizeof(DataType) * StateSequence.size());
			this->StateCarryValue = 0;
			this->StateIndex = 0;
		}

	private:
		std::array<DataType, long_lag> StateSequence{};
		std::uint8_t StateCarryValue = 0;
		std::size_t StateIndex = 0;
	};

	template<typename DataType, std::size_t ShortLag, std::size_t LongLag>
	struct SubtractWithBorrow : UniformRandomBitGenerator<DataType>
	{
	public:
		//The prototype implementation
		static_assert(std::is_integral<DataType>::value, "");
		static_assert(!std::is_signed<DataType>::value, "");
		static_assert(ShortLag > 0U, "");
		static_assert(LongLag > ShortLag, "");

		using result_type = DataType;

		static constexpr auto long_lag = LongLag;
		static constexpr auto short_lag = ShortLag;

		// Set the modulus and result modulus based on the type of DataType
		static constexpr DataType modulus_value =
			std::is_same_v<DataType, std::uint64_t>
			? (std::uint64_t(1) << 64) - 2
			: std::is_same_v<DataType, std::uint32_t>
			? (std::uint32_t(1) << 32) - 2
			: std::is_same_v<DataType, std::uint16_t>
			? (std::uint16_t(1) << 16) - 2
			: std::is_same_v<DataType, std::uint8_t>
			? (std::uint8_t(1) << 8) - 2
			: 0;

		static constexpr DataType result_modulus_value =
			std::is_same_v<DataType, std::uint64_t>
			? 18446744073709551557ULL
			: std::is_same_v<DataType, std::uint32_t>
			? 4294967291UL
			: std::is_same_v<DataType, std::uint16_t>
			? 65521U
			: std::is_same_v<DataType, std::uint8_t>
			? 251U
			: 0U;

		result_type operator()()
		{
			// Compute the indices for the previous values
			std::size_t IndexA = (this->StateIndex - long_lag) % long_lag;
			std::size_t IndexB = (this->StateIndex - short_lag) % long_lag;

			// Compute the next random value using the Subtract-With-Borrow method
			auto Result = this->StateSequence[IndexA] - this->StateSequence[IndexB] - this->StateBorrowValue;
			this->StateBorrowValue = (static_cast<std::int64_t>(Result) < static_cast<std::int64_t>(0)) ? 1U : 0U;
			if (this->StateBorrowValue == 1U)
				Result %= modulus_value;

			this->StateIndex = (this->StateIndex + 1U == long_lag ? 0U : this->StateIndex + 1U);
			this->StateSequence[this->StateIndex] = Result;
			
			return Result % result_modulus_value;
		}

		void discard(std::size_t Round)
		{
			//Discard the pseudo-random numbers generated by the long_lag round
			for (std::size_t Round = 0; Round < long_lag; ++Round)
				(*this)();
		}

		// Initialize the state array with seed value
		SubtractWithBorrow(DataType Seed = 123456U)
		{
			if constexpr (std::numeric_limits<DataType>::digits > 32)
			{
				std::linear_congruential_engine<std::uint64_t, 6364136223846793005ULL, 1442695040888963407ULL, (1ULL << 64) - 1> LCG(Seed);

				for (std::size_t Index = 0; Index < long_lag; ++Index)
					this->StateSequence[Index] = static_cast<DataType>( LCG() );
			}
			else
			{
				std::linear_congruential_engine<std::uint32_t, 134775813UL, 1UL, (1UL << 32) - 1> LCG(Seed);
				
				for (std::size_t Index = 0; Index < long_lag; ++Index)
					this->StateSequence[Index] = static_cast<DataType>(LCG());
			}

			//Discard the pseudo-random numbers generated by the long_lag round
			for (std::size_t Round = 0; Round < long_lag; ++Round)
				(*this)();
		}

		~SubtractWithBorrow()
		{
			memory_set_no_optimize_function<0x00>(StateSequence.data(), sizeof(DataType) * StateSequence.size());
			this->StateBorrowValue = 0;
			this->StateIndex = 0;
		}

	private:
		std::array<DataType, long_lag> StateSequence{};
		std::uint8_t StateBorrowValue = 0;
		std::size_t StateIndex = 0;
	};

	template<typename DataType, std::size_t ShortLag, std::size_t LongLag>
	class SubtractWithCarry : UniformRandomBitGenerator<DataType>
	{
	public:
		//The prototype implementation
		static_assert(std::is_integral<DataType>::value, "");
		static_assert(!std::is_signed<DataType>::value, "");
		static_assert(ShortLag > 0U, "");
		static_assert(LongLag > ShortLag, "");

		using result_type = DataType;

		static constexpr auto long_lag = LongLag;
		static constexpr auto short_lag = ShortLag;

		// Set the modulus and result modulus based on the type of DataType
		static constexpr DataType modulus_value =
			std::is_same_v<DataType, std::uint64_t>
			? (std::uint64_t(1) << 64) - 2
			: std::is_same_v<DataType, std::uint32_t>
			? (std::uint32_t(1) << 32) - 2
			: std::is_same_v<DataType, std::uint16_t>
			? (std::uint16_t(1) << 16) - 2
			: std::is_same_v<DataType, std::uint8_t>
			? (std::uint8_t(1) << 8) - 2
			: 0;

		static constexpr DataType result_modulus_value =
			std::is_same_v<DataType, std::uint64_t>
			? 18446744073709551557ULL
			: std::is_same_v<DataType, std::uint32_t>
			? 4294967291UL
			: std::is_same_v<DataType, std::uint16_t>
			? 65521U
			: std::is_same_v<DataType, std::uint8_t>
			? 251U
			: 0U;

		result_type operator()()
		{
			my_cpp2020_assert(this->StateCarryValue == 0 || this->StateCarryValue == 1, "Invalied binary carry value!", std::source_location::current());

			std::size_t IndexA = (this->StateIndex - short_lag) % long_lag;
			std::size_t IndexB = (this->StateIndex - long_lag) % long_lag;

			// Compute the next random value using the Subtract-With-Carry method
			auto Result = this->StateSequence[IndexA] - this->StateSequence[IndexB] - this->StateCarryValue;
			this->StateCarryValue = (static_cast<std::int64_t>(Result) < static_cast<std::int64_t>(0)) ? 1U : 0U;
			if (this->StateCarryValue == 1U)
				Result %= modulus_value;

			this->StateIndex = this->StateIndex + 1U == long_lag ? 0U : this->StateIndex + 1U;
			this->StateSequence[this->StateIndex] = Result;

			return Result % result_modulus_value;
		}

		void discard(std::size_t Round)
		{
			//Discard the pseudo-random numbers generated by the long_lag round
			for (std::size_t Round = 0; Round < long_lag; ++Round)
				(*this)();
		}

		SubtractWithCarry(DataType Seed = 123456U)
		{
			if constexpr (std::numeric_limits<DataType>::digits > 32)
			{
				std::linear_congruential_engine<std::uint64_t, 6364136223846793005ULL, 1442695040888963407ULL, (1ULL << 64) - 1> LCG(Seed);

				for (std::size_t Index = 0; Index < long_lag; ++Index)
					this->StateSequence[Index] = static_cast<DataType>(LCG());
			}
			else
			{
				std::linear_congruential_engine<std::uint32_t, 134775813UL, 1UL, (1UL << 32) - 1> LCG(Seed);

				for (std::size_t Index = 0; Index < long_lag; ++Index)
					this->StateSequence[Index] = static_cast<DataType>(LCG());
			}

			//Discard the pseudo-random numbers generated by the long_lag round
			for (std::size_t Round = 0; Round < long_lag; ++Round)
				(*this)();
		}

		~SubtractWithCarry()
		{
			memory_set_no_optimize_function<0x00>(StateSequence.data(), sizeof(DataType) * StateSequence.size());
			this->StateCarryValue = 0;
			this->StateIndex = 0;
		}

	private:
		std::array<DataType, long_lag> StateSequence{};
		std::uint8_t StateCarryValue = 0;
		std::size_t StateIndex = 0;
	};

	/*
		golden ratio is 0x9e3779b97f4a7c13 with 64 bit number
	*/

	// An implementation of xorshiro (https://vigna.di.unimi.it/xorshift/)
	// wrapped to fit the C++11 RandomNumberGenerator requirements.
	// This allows us to use it with all the other facilities in <random>.
	//
	// Credits go to David Blackman and Sebastiano Vigna.
	//
	// TODO: make generic? (parameterise scrambler/width/hyperparameters/etc.)
	// Not as easy to do nicely as it might sound,
	// and this as it is is good enough for my purposes.

	struct xorshiro128 : UniformRandomBitGenerator<std::uint64_t>
	{
		static constexpr std::uint32_t num_state_words = 2;
		using state_type = std::array<std::uint64_t, num_state_words>;

		// cannot initialize with an all-zero state
		constexpr xorshiro128() noexcept
			: state { 12, 34 }
		{
		}

		// using SplitMix64 generator to initialize the state;
		// using a different generator helps prevent seed correlation
		explicit constexpr xorshiro128( result_type seed ) noexcept
		{
			auto splitmix64 = [ seed_value = seed ]() mutable {
				auto z = ( seed_value += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;
				return z ^ ( z >> 31 );
			};
			std::ranges::generate( state, splitmix64 );
		}

		explicit xorshiro128( std::initializer_list<result_type> initializer_list_args )
		{
			*this = xorshiro128(initializer_list_args.begin(), initializer_list_args.end());
		}

		template <std::input_or_output_iterator SeedDataIteratorType>
		requires
		( 
			not std::convertible_to<SeedDataIteratorType, result_type>
		)
		explicit xorshiro128( SeedDataIteratorType&& begin, SeedDataIteratorType&& end )
		{
			std::vector<result_type> seed_vector { begin, end };
			this->generate_number_state_seeds( seed_vector );
			seed_vector.clear();
			seed_vector.shrink_to_fit();
		}

		explicit xorshiro128( std::span<const result_type> seed_span )
		{
			this->generate_number_state_seeds( seed_span );
		}

		explicit xorshiro128( std::seed_seq& s_q )
		{
			this->generate_number_state_seeds(s_q);
		}

		constexpr void seed() noexcept
		{
			*this = xorshiro128();
		}
		constexpr void seed( result_type s ) noexcept
		{
			*this = xorshiro128( s );
		}
		template <typename SeedSeq>
		requires( not std::convertible_to<SeedSeq, result_type> )
		constexpr void seed( SeedSeq& q )
		{
			*this = xorshiro128( q );
		}

		constexpr result_type operator()() noexcept
		{
			// xorshiro128+:
			/*
				const auto a = state[0];
				auto b = state[1];
				const auto result = a + b;

				b ^= a;
				state[0] = rotl(a, 24) ^ b ^ (b << 16); // a, b
				state[1] = rotl(b, 37); // c
			*/
			
			// xorshiro128++:
			/*
				const auto a = state[0];
				auto b = state[1];
				const auto result = std::rotl(a + b, 17) + a;

				b ^= a;
				state[0] = std::rotl(a, 49) ^ b ^ (b << 21); // a, b
				state[1] = std::rotl(b, 28); // c
			*/

			// xorshiro128**:
			const auto a = state[0];
			auto b = state[1];
			const auto result = std::rotl(a * 5, 7) * 9;

			b ^= a;
			state[0] = std::rotl(a, 24) ^ b ^ (b << 16); // a, b
			state[1] = std::rotl(b, 37); // c

			return result;
		}

		constexpr void discard( std::uint64_t round ) noexcept
		{
			if(round == 0)
				return;

			while ( round-- )
				operator()();
		}

		/*
			This is the jump function for the generator. 
			It is equivalent to 2^64 calls to operator()();
			It can be used to generate 2^64 non-overlapping subsequences for parallel computations.
		*/
		constexpr void jump() noexcept
		{
			constexpr std::uint64_t jump_table[] = {
				0xdf900294d8f554a5, 0x170865df4b3201fc
			};

			state_type temporary_state {};
			for ( std::uint32_t jump_table_index = 0; jump_table_index < std::ssize( jump_table ); jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( jump_table[ jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];
					}
					operator()();
				}
			}

			temporary_state[ 0 ] = state[ 0 ];
			temporary_state[ 1 ] = state[ 1 ];
		}

		/*
			This is the long-jump function for the generator.
			It is equivalent to 2^96 calls to operator()();
			It can be used to generate 2^32 starting points,
			From each of which jump() will generate 2^32 non-overlapping subsequences for parallel distributed computations. 
		*/
		constexpr void long_jump() noexcept
		{
			constexpr std::uint64_t long_jump_table[] = {
				0xd2a98b26625eee7b, 0xdddf9b1090aa7ac1
			};

			state_type temporary_state {};
			for ( std::uint32_t long_jump_table_index = 0; long_jump_table_index < std::ssize( long_jump_table ); long_jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( long_jump_table[ long_jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];

					}
					operator()();
				}
			}

			temporary_state[ 0 ] = state[ 0 ];
			temporary_state[ 1 ] = state[ 1 ];

		}

		constexpr bool operator==( const xorshiro128& ) const noexcept = default;

		template <typename CharT, typename Traits>
		friend std::basic_ostream<CharT, Traits>& operator<<( std::basic_ostream<CharT, Traits>& os, const xorshiro128& e )
		{
			os << e.state[ 0 ];
			for ( int i = 1; i < num_state_words; ++i )
			{
				os.put( os.widen( ' ' ) );
				os << e.state[ i ];
			}
			return os;
		}

		template <typename CharT, typename Traits>
		friend std::basic_istream<CharT, Traits&> operator>>( std::basic_istream<CharT, Traits>& is, xorshiro128& e )
		{
			xorshiro128 r;
			// TODO: what if ' ' is not considered whitespace?
			// Maybe more appropriate is to `.get` each space
			for ( auto& s : r.state )
				is >> s;
			if ( is )
				e = r;
			return is;
		}

	private:
		state_type state;

		void generate_number_state_seeds(std::seed_seq& s_q)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];
			s_q.generate( std::begin( this_temparory_state ), std::end( this_temparory_state ) );
			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}

		void generate_number_state_seeds(std::span<const result_type> seed_span)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];

			auto seed_span_begin = seed_span.begin();
			auto seed_span_end = seed_span.end();
			result_type seed = 0;
			auto splitmix64 = [&seed_span_begin, &seed_span_end, &seed]() mutable {
					
				auto z = (seed += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;

				if(seed_span_begin != seed_span_end)
				{
					++seed_span_begin;
				}

				return z ^ ( z >> 31 );
			};
			std::ranges::generate( this_temparory_state, splitmix64 );
			seed = 0;

			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}
	};

	struct xorshiro256 : UniformRandomBitGenerator<std::uint64_t>
	{
		static constexpr std::uint32_t num_state_words = 4;
		using state_type = std::array<std::uint64_t, num_state_words>;

		// cannot initialize with an all-zero state
		constexpr xorshiro256() noexcept
			: state { 12, 34 }
		{
		}

		// using SplitMix64 generator to initialize the state;
		// using a different generator helps prevent seed correlation
		explicit constexpr xorshiro256( result_type seed ) noexcept
		{
			auto splitmix64 = [ seed_value = seed ]() mutable {
				auto z = ( seed_value += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;
				return z ^ ( z >> 31 );
			};
			std::ranges::generate( state, splitmix64 );
		}

		explicit xorshiro256( std::initializer_list<result_type> initializer_list_args )
		{
			*this = xorshiro256(initializer_list_args.begin(), initializer_list_args.end());
		}

		template <std::input_or_output_iterator SeedDataIteratorType>
		requires
		( 
			not std::convertible_to<SeedDataIteratorType, result_type>
		)
		explicit xorshiro256( SeedDataIteratorType&& begin, SeedDataIteratorType&& end )
		{
			std::vector<result_type> seed_vector { begin, end };
			this->generate_number_state_seeds( seed_vector );
			seed_vector.clear();
			seed_vector.shrink_to_fit();
		}

		explicit xorshiro256( std::span<const result_type> seed_span )
		{
			this->generate_number_state_seeds( seed_span );
		}

		explicit xorshiro256( std::seed_seq& s_q )
		{
			this->generate_number_state_seeds(s_q);
		}

		constexpr void seed() noexcept
		{
			*this = xorshiro256();
		}
		constexpr void seed( result_type s ) noexcept
		{
			*this = xorshiro256( s );
		}
		template <typename SeedSeq>
		requires( not std::convertible_to<SeedSeq, result_type> )
		constexpr void seed( SeedSeq& q )
		{
			*this = xorshiro256( q );
		}

		constexpr result_type operator()() noexcept
		{
			// xorshiro256+:
			// const auto result = state[0] + state[3];
			// xorshiro256++:
			// const auto result = std::rotl(state[0] + state[3], 23) + state[0];

			// xorshiro256**:
			const auto result = std::rotl( state[ 1 ] * 5, 7 ) * 9;
			const auto t = state[ 1 ] << 17;

			state[ 2 ] ^= state[ 0 ];
			state[ 3 ] ^= state[ 1 ];
			state[ 1 ] ^= state[ 2 ];
			state[ 0 ] ^= state[ 3 ];

			state[ 2 ] ^= t;
			state[ 3 ] = std::rotl( state[ 3 ], 45 );

			return result;
		}

		constexpr void discard( std::uint64_t round ) noexcept
		{
			if(round == 0)
				return;

			while ( round-- )
				operator()();
		}

		/*
			This is the jump function for the generator.
			It is equivalent to 2^128 calls to operator()();
			It can be used to generate 2^128 non-overlapping subsequences for parallel computations.
		*/
		constexpr void jump() noexcept
		{
			constexpr std::uint64_t jump_table[] = {
				0x180ec6d33cfd0aba,
				0xd5a61266f0c9392c,
				0xa9582618e03fc9aa,
				0x39abdc4529b1661c,
			};

			state_type temporary_state {};
			for ( std::uint32_t jump_table_index = 0; jump_table_index < std::ssize( jump_table ); jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( jump_table[ jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];
						temporary_state[ 2 ] ^= state[ 2 ];
						temporary_state[ 3 ] ^= state[ 3 ];
					}
					operator()();
				}
			}

			state[ 0 ] = temporary_state[ 0 ];
			state[ 1 ] = temporary_state[ 1 ];
			state[ 2 ] = temporary_state[ 2 ];
			state[ 3 ] = temporary_state[ 3 ];
		}

		/*
			This is the jump function for the generator.
			It is equivalent to 2^192 calls to operator()();
			It can be used to generate 2^64 starting points,
			From each of which jump() will generate 2^64 non-overlapping subsequences for parallel distributed computations.
		*/
		constexpr void long_jump() noexcept
		{
			constexpr std::uint64_t long_jump_table[] = {
				0x76e15d3efefdcbbf,
				0xc5004e441c522fb3,
				0x77710069854ee241,
				0x39109bb02acbe635,
			};

			state_type temporary_state {};
			for ( std::uint32_t long_jump_table_index = 0; long_jump_table_index < std::ssize( long_jump_table ); long_jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( long_jump_table[ long_jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];
						temporary_state[ 2 ] ^= state[ 2 ];
						temporary_state[ 3 ] ^= state[ 3 ];
					}
					operator()();
				}
			}

			state[ 0 ] = temporary_state[ 0 ];
			state[ 1 ] = temporary_state[ 1 ];
			state[ 2 ] = temporary_state[ 2 ];
			state[ 3 ] = temporary_state[ 3 ];
		}

		constexpr bool operator==( const xorshiro256& ) const noexcept = default;

		template <typename CharT, typename Traits>
		friend std::basic_ostream<CharT, Traits>& operator<<( std::basic_ostream<CharT, Traits>& os, const xorshiro256& e )
		{
			os << e.state[ 0 ];
			for ( int i = 1; i < num_state_words; ++i )
			{
				os.put( os.widen( ' ' ) );
				os << e.state[ i ];
			}
			return os;
		}

		template <typename CharT, typename Traits>
		friend std::basic_istream<CharT, Traits&> operator>>( std::basic_istream<CharT, Traits>& is, xorshiro256& e )
		{
			xorshiro256 r;
			// TODO: what if ' ' is not considered whitespace?
			// Maybe more appropriate is to `.get` each space
			for ( auto& s : r.state )
				is >> s;
			if ( is )
				e = r;
			return is;
		}

	private:
		state_type state;

		void generate_number_state_seeds(std::seed_seq& s_q)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];
			s_q.generate( std::begin( this_temparory_state ), std::end( this_temparory_state ) );
			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}

		void generate_number_state_seeds(std::span<const result_type> seed_span)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];

			auto seed_span_begin = seed_span.begin();
			auto seed_span_end = seed_span.end();
			result_type seed = 0;
			auto splitmix64 = [&seed_span_begin, &seed_span_end, &seed]() mutable {
					
				auto z = (seed += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;

				if(seed_span_begin != seed_span_end)
				{
					++seed_span_begin;
				}

				return z ^ ( z >> 31 );
			};
			std::ranges::generate( this_temparory_state, splitmix64 );
			seed = 0;

			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}
	};

	struct xorshiro512 : UniformRandomBitGenerator<std::uint64_t>
	{
		static constexpr std::uint32_t num_state_words = 8;
		using state_type = std::array<std::uint64_t, num_state_words>;

		std::size_t state_position = 0;

		// cannot initialize with an all-zero state
		constexpr xorshiro512() noexcept
			: state { 12, 34 }
		{
		}

		// using SplitMix64 generator to initialize the state;
		// using a different generator helps prevent seed correlation
		explicit constexpr xorshiro512( result_type seed ) noexcept
		{
			auto splitmix64 = [ seed_value = seed ]() mutable {
				auto z = ( seed_value += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;
				return z ^ ( z >> 31 );
			};
			std::ranges::generate( state, splitmix64 );
		}

		explicit xorshiro512( std::initializer_list<result_type> initializer_list_args )
		{
			*this = xorshiro512(initializer_list_args.begin(), initializer_list_args.end());
		}

		template <std::input_or_output_iterator SeedDataIteratorType>
		requires
		( 
			not std::convertible_to<SeedDataIteratorType, result_type>
		)
		explicit xorshiro512( SeedDataIteratorType&& begin, SeedDataIteratorType&& end )
		{
			std::vector<result_type> seed_vector { begin, end };
			this->generate_number_state_seeds( seed_vector );
			seed_vector.clear();
			seed_vector.shrink_to_fit();
		}

		explicit xorshiro512( std::span<const result_type> seed_span )
		{
			this->generate_number_state_seeds( seed_span );
		}

		explicit xorshiro512( std::seed_seq& s_q )
		{
			this->generate_number_state_seeds(s_q);
		}

		constexpr void seed() noexcept
		{
			*this = xorshiro512();
		}
		constexpr void seed( result_type s ) noexcept
		{
			*this = xorshiro512( s );
		}
		template <typename SeedSeq>
		requires( not std::convertible_to<SeedSeq, result_type> )
		constexpr void seed( SeedSeq& q )
		{
			*this = xorshiro512( q );
		}

		constexpr result_type operator()() noexcept
		{
			const std::size_t this_state_position = this->state_position;
			this->state_position = (this->state_position + 1) & 15;
				
			// xorshiro512+:
			// const auto result = s[0] + s[2];
			// xorshiro512++:
			// const auto result = std::rotl(s[0] + s[2], 17) + s[2];

			// xorshiro512**:
			const auto result = std::rotl(state[1] * 5, 7) * 9;

			const auto t = state[1] << 11;

			state[2] ^= state[0];
			state[5] ^= state[1];
			state[1] ^= state[2];
			state[7] ^= state[3];
			state[3] ^= state[4];
			state[4] ^= state[5];
			state[0] ^= state[6];
			state[6] ^= state[7];

			state[6] ^= t;

			state[7] = std::rotl(state[7], 21);

			return result;
		}

		constexpr void discard( std::uint64_t round ) noexcept
		{
			if(round == 0)
				return;

			while ( round-- )
				operator()();
		}

		/*
			This is the jump function for the generator.
			It is equivalent to 2^256 calls to operator()();
			It can be used to generate 2^256 non-overlapping subsequences for parallel computations.
		*/
		constexpr void jump() noexcept
		{
			constexpr std::uint64_t jump_table[] = {
				0x33ed89b6e7a353f9, 0x760083d7955323be,
				0x2837f2fbb5f22fae, 0x4b8c5674d309511c,
				0xb11ac47a7ba28c25, 0xf1be7667092bcc1c,
				0x53851efdb6df0aaf, 0x1ebbc8b23eaf25db
			};

			state_type temporary_state {};
			for ( std::uint32_t jump_table_index = 0; jump_table_index < std::ssize( jump_table ); jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( jump_table[ jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];
						temporary_state[ 2 ] ^= state[ 2 ];
						temporary_state[ 3 ] ^= state[ 3 ];
						temporary_state[ 4 ] ^= state[ 4 ];
						temporary_state[ 5 ] ^= state[ 5 ];
						temporary_state[ 6 ] ^= state[ 6 ];
						temporary_state[ 7 ] ^= state[ 7 ];
					}
					operator()();
				}
			}

			temporary_state[ 0 ] = state[ 0 ];
			temporary_state[ 1 ] = state[ 1 ];
			temporary_state[ 2 ] = state[ 2 ];
			temporary_state[ 3 ] = state[ 3 ];
			temporary_state[ 4 ] = state[ 4 ];
			temporary_state[ 5 ] = state[ 5 ];
			temporary_state[ 6 ] = state[ 6 ];
			temporary_state[ 7 ] = state[ 7 ];
		}

		/*
			This is the long-jump function for the generator.
			It is equivalent to 2^384 calls to operator()();
			It can be used to generate 2^128 starting points,
			from each of which jump() will generate 2^128 non-overlapping subsequences for parallel distributed computations.
		*/
		constexpr void long_jump() noexcept
		{
			constexpr std::uint64_t long_jump_table[] = {
				0x11467fef8f921d28, 0xa2a819f2e79c8ea8,
				0xa8299fc284b3959a, 0xb4d347340ca63ee1,
				0x1cb0940bedbff6ce, 0xd956c5c4fa1f8e17,
				0x915e38fd4eda93bc, 0x5b3ccdfa5d7daca5
			};

			state_type temporary_state {};
			for ( std::uint32_t long_jump_table_index = 0; long_jump_table_index < std::ssize( long_jump_table ); long_jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( long_jump_table[ long_jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];
						temporary_state[ 2 ] ^= state[ 2 ];
						temporary_state[ 3 ] ^= state[ 3 ];
						temporary_state[ 4 ] ^= state[ 4 ];
						temporary_state[ 5 ] ^= state[ 5 ];
						temporary_state[ 6 ] ^= state[ 6 ];
						temporary_state[ 7 ] ^= state[ 7 ];

					}
					operator()();
				}
			}

			temporary_state[ 0 ] = state[ 0 ];
			temporary_state[ 1 ] = state[ 1 ];
			temporary_state[ 2 ] = state[ 2 ];
			temporary_state[ 3 ] = state[ 3 ];
			temporary_state[ 4 ] = state[ 4 ];
			temporary_state[ 5 ] = state[ 5 ];
			temporary_state[ 6 ] = state[ 6 ];
			temporary_state[ 7 ] = state[ 7 ];
		}

	private:
		state_type state;

		void generate_number_state_seeds(std::seed_seq& s_q)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];
			s_q.generate( std::begin( this_temparory_state ), std::end( this_temparory_state ) );
			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}

		void generate_number_state_seeds(std::span<const result_type> seed_span)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];

			auto seed_span_begin = seed_span.begin();
			auto seed_span_end = seed_span.end();
			result_type seed = 0;
			auto splitmix64 = [&seed_span_begin, &seed_span_end, &seed]() mutable {
					
				auto z = (seed += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;

				if(seed_span_begin != seed_span_end)
				{
					++seed_span_begin;
				}

				return z ^ ( z >> 31 );
			};
			std::ranges::generate( this_temparory_state, splitmix64 );
			seed = 0;

			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}
	};

	struct xorshiro1024 : UniformRandomBitGenerator<std::uint64_t>
	{
		static constexpr std::uint32_t num_state_words = 16;
		using state_type = std::array<std::uint64_t, num_state_words>;

		std::size_t state_position = 0;

		// cannot initialize with an all-zero state
		constexpr xorshiro1024() noexcept
			: state { 12, 34 }
		{
		}

		// using SplitMix64 generator to initialize the state;
		// using a different generator helps prevent seed correlation
		explicit constexpr xorshiro1024( result_type seed ) noexcept
		{
			auto splitmix64 = [ seed_value = seed ]() mutable {
				auto z = ( seed_value += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;
				return z ^ ( z >> 31 );
			};
			std::ranges::generate( state, splitmix64 );
		}

		explicit xorshiro1024( std::initializer_list<result_type> initializer_list_args )
		{
			*this = xorshiro1024(initializer_list_args.begin(), initializer_list_args.end());
		}

		template <std::input_or_output_iterator SeedDataIteratorType>
		requires
		( 
			not std::convertible_to<SeedDataIteratorType, result_type>
		)
		explicit xorshiro1024( SeedDataIteratorType&& begin, SeedDataIteratorType&& end )
		{
			std::vector<result_type> seed_vector { begin, end };
			this->generate_number_state_seeds( seed_vector );
			seed_vector.clear();
			seed_vector.shrink_to_fit();
		}

		explicit xorshiro1024( std::span<const result_type> seed_span )
		{
			this->generate_number_state_seeds( seed_span );
		}

		explicit xorshiro1024( std::seed_seq& s_q )
		{
			this->generate_number_state_seeds(s_q);
		}

		constexpr void seed() noexcept
		{
			*this = xorshiro1024();
		}
		constexpr void seed( result_type s ) noexcept
		{
			*this = xorshiro1024( s );
		}
		template <typename SeedSeq>
		requires( not std::convertible_to<SeedSeq, result_type> )
		constexpr void seed( SeedSeq& q )
		{
			*this = xorshiro1024( q );
		}

		constexpr result_type operator()() noexcept
		{
			const std::size_t this_state_position = this->state_position;
			this->state_position = (this->state_position + 1) & 15;
				
			// xorshiro1024++:
			// const auto result = std::rotl(a + b, 23) + a;
			// xorshiro1024*:
			// const auto result = a * 0x9e3779b97f4a7c13;

			// xorshiro1024**:
			const auto a = state[ this->state_position ];
			const auto result = std::rotl( a * 5, 7 ) * 9;
			auto b = state[ this_state_position ];

			b ^= a;
			state[this_state_position] = std::rotl( a, 25 ) ^ b ^ (b << 27);
			state[this->state_position] = std::rotl( b, 36 );

			return result;
		}

		constexpr void discard( std::uint64_t round ) noexcept
		{
			if(round == 0)
				return;

			while ( round-- )
				operator()();
		}

		/*
			This is the jump function for the generator.
			It is equivalent to 2^512 calls to operator()();
			It can be used to generate 2^512 non-overlapping subsequences for parallel computations.
		*/
		constexpr void jump() noexcept
		{
			constexpr std::uint64_t jump_table[] = {
				0x931197d8e3177f17, 0xb59422e0b9138c5f,
				0xf06a6afb49d668bb, 0xacb8a6412c8a1401,
				0x12304ec85f0b3468, 0xb7dfe7079209891e,
				0x405b7eec77d9eb14, 0x34ead68280c44e4a,
				0xe0e4ba3e0ac9e366, 0x8f46eda8348905b7,
				0x328bf4dbad90d6ff, 0xc8fd6fb31c9effc3,
				0xe899d452d4b67652, 0x45f387286ade3205,
				0x03864f454a8920bd, 0xa68fa28725b1b384
			};

			state_type temporary_state {};
			for ( std::uint32_t jump_table_index = 0; jump_table_index < std::ssize( jump_table ); jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( jump_table[ jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];
						temporary_state[ 2 ] ^= state[ 2 ];
						temporary_state[ 3 ] ^= state[ 3 ];
						temporary_state[ 4 ] ^= state[ 4 ];
						temporary_state[ 5 ] ^= state[ 5 ];
						temporary_state[ 6 ] ^= state[ 6 ];
						temporary_state[ 7 ] ^= state[ 7 ];
						temporary_state[ 8 ] ^= state[ 8 ];
						temporary_state[ 9 ] ^= state[ 9 ];
						temporary_state[ 10 ] ^= state[ 10 ];
						temporary_state[ 11 ] ^= state[ 11 ];
						temporary_state[ 12 ] ^= state[ 12 ];
						temporary_state[ 13 ] ^= state[ 13 ];
						temporary_state[ 14 ] ^= state[ 14 ];
						temporary_state[ 15 ] ^= state[ 15 ];
					}
					operator()();
				}
			}

			temporary_state[ 0 ] = state[ 0 ];
			temporary_state[ 1 ] = state[ 1 ];
			temporary_state[ 2 ] = state[ 2 ];
			temporary_state[ 3 ] = state[ 3 ];
			temporary_state[ 4 ] = state[ 4 ];
			temporary_state[ 5 ] = state[ 5 ];
			temporary_state[ 6 ] = state[ 6 ];
			temporary_state[ 7 ] = state[ 7 ];
			temporary_state[ 8 ] = state[ 8 ];
			temporary_state[ 9 ] = state[ 9 ];
			temporary_state[ 10 ] = state[ 10 ];
			temporary_state[ 11 ] = state[ 11 ];
			temporary_state[ 12 ] = state[ 12 ];
			temporary_state[ 13 ] = state[ 13 ];
			temporary_state[ 14 ] = state[ 14 ];
			temporary_state[ 15 ] = state[ 15 ];
		}

		/*
			This is the long-jump function for the generator.
			It is equivalent to 2^768 calls to operator()();
			It can be used to generate 2^256 starting points,
			from each of which jump() will generate 2^256 non-overlapping subsequences for parallel distributed computations.
		*/
		constexpr void long_jump() noexcept
		{
			constexpr std::uint64_t long_jump_table[] = {
				0x7374156360bbf00f, 0x4630c2efa3b3c1f6,
				0x6654183a892786b1, 0x94f7bfcbfb0f1661,
				0x27d8243d3d13eb2d, 0x9701730f3dfb300f,
				0x2f293baae6f604ad, 0xa661831cb60cd8b6,
				0x68280c77d9fe008c, 0x50554160f5ba9459,
				0x2fc20b17ec7b2a9a, 0x49189bbdc8ec9f8f,
				0x92a65bca41852cc1, 0xf46820dd0509c12a,
				0x52b00c35fbf92185, 0x1e5b3b7f589e03c1
			};

			state_type temporary_state {};
			for ( std::uint32_t long_jump_table_index = 0; long_jump_table_index < std::ssize( long_jump_table ); long_jump_table_index++ )
			{
				for ( std::uint32_t b = 0; b < 64; b++ )
				{
					if ( long_jump_table[ long_jump_table_index ] & ( static_cast<std::uint64_t>( 1 ) << b ) )
					{
						temporary_state[ 0 ] ^= state[ 0 ];
						temporary_state[ 1 ] ^= state[ 1 ];
						temporary_state[ 2 ] ^= state[ 2 ];
						temporary_state[ 3 ] ^= state[ 3 ];
						temporary_state[ 4 ] ^= state[ 4 ];
						temporary_state[ 5 ] ^= state[ 5 ];
						temporary_state[ 6 ] ^= state[ 6 ];
						temporary_state[ 7 ] ^= state[ 7 ];
						temporary_state[ 8 ] ^= state[ 8 ];
						temporary_state[ 9 ] ^= state[ 9 ];
						temporary_state[ 10 ] ^= state[ 10 ];
						temporary_state[ 11 ] ^= state[ 11 ];
						temporary_state[ 12 ] ^= state[ 12 ];
						temporary_state[ 13 ] ^= state[ 13 ];
						temporary_state[ 14 ] ^= state[ 14 ];
						temporary_state[ 15 ] ^= state[ 15 ];

					}
					operator()();
				}
			}

			temporary_state[ 0 ] = state[ 0 ];
			temporary_state[ 1 ] = state[ 1 ];
			temporary_state[ 2 ] = state[ 2 ];
			temporary_state[ 3 ] = state[ 3 ];
			temporary_state[ 4 ] = state[ 4 ];
			temporary_state[ 5 ] = state[ 5 ];
			temporary_state[ 6 ] = state[ 6 ];
			temporary_state[ 7 ] = state[ 7 ];
			temporary_state[ 8 ] = state[ 8 ];
			temporary_state[ 9 ] = state[ 9 ];
			temporary_state[ 10 ] = state[ 10 ];
			temporary_state[ 11 ] = state[ 11 ];
			temporary_state[ 12 ] = state[ 12 ];
			temporary_state[ 13 ] = state[ 13 ];
			temporary_state[ 14 ] = state[ 14 ];
			temporary_state[ 15 ] = state[ 15 ];
		}

	private:
		state_type state;

		void generate_number_state_seeds(std::seed_seq& s_q)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];
			s_q.generate( std::begin( this_temparory_state ), std::end( this_temparory_state ) );
			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}

		void generate_number_state_seeds(std::span<const result_type> seed_span)
		{
			std::uint32_t this_temparory_state[ num_state_words * 2 ];

			auto seed_span_begin = seed_span.begin();
			auto seed_span_end = seed_span.end();
			result_type seed = 0;
			auto splitmix64 = [&seed_span_begin, &seed_span_end, &seed]() mutable {
					
				auto z = (seed += 0x9e3779b97f4a7c15 );
				z = ( z ^ ( z >> 30 ) ) * 0xbf58476d1ce4e5b9;
				z = ( z ^ ( z >> 27 ) ) * 0x94d049bb133111eb;

				if(seed_span_begin != seed_span_end)
				{
					++seed_span_begin;
				}

				return z ^ ( z >> 31 );
			};
			std::ranges::generate( this_temparory_state, splitmix64 );
			seed = 0;

			for ( std::uint32_t index = 0; index < num_state_words; ++index )
			{
				state[ index ] = this_temparory_state[ index * 2 ];
				state[ index ] <<= 32;
				state[ index ] |= this_temparory_state[ index * 2 + 1 ];
			}
		}
	};
}