#include "./PseudoRandomNumberGenerators.hpp"

namespace PseudoRandomNumberGenerators
{
	bool BlumBlumShub::IsPrimeNumber(std::uint64_t Number)
	{
		if (Number <= 1)
			return false;
		if (Number <= 3)
			return true;
		if (Number % 2 == 0 || Number % 3 == 0)
			return false;
		for (std::uint64_t Index = 5; Index * Index <= Number; Index += 6)
			if (Number % Index == 0 || Number % (Index + 2) == 0)
				return false;
		return true;
	}

	std::uint64_t BlumBlumShub::FastPowerAlgorithmWithModulus(std::uint64_t BaseNumber, std::uint64_t ExponentNumber, std::uint64_t ModulusNumber)
	{
		BaseNumber %= ModulusNumber;
		std::uint64_t Result = 1;
		while (ExponentNumber > 0)
		{
			if (ExponentNumber & 1)
				Result = (Result * BaseNumber) % ModulusNumber;
			BaseNumber = (BaseNumber * BaseNumber) % ModulusNumber;
			ExponentNumber >>= 1;
		}
		return Result;
	}

	void BlumBlumShub::ResetState(std::uint64_t Seed)
	{
		// Initialize the PRNG with the seed value.
		LCG.seed(Seed);

		// Choose two large prime numbers p and q that are congruent to 3 mod 4.
		do
		{
			P = static_cast<std::uint32_t>(LCG());
		} while (!this->IsPrimeNumber(P) || P % 4 != 3);

		do
		{
			Q = static_cast<std::uint32_t>(LCG());
		} while (!this->IsPrimeNumber(Q) || Q % 4 != 3);

		// Compute n = p * q.
		N = static_cast<std::uint64_t>(P) * static_cast<std::uint64_t>(Q);

		// Choose a random seed value X that is co-prime to N and not 1 or 0.
		do
		{
			X = ( static_cast<std::uint64_t>(LCG()) << 32 ) | static_cast<std::uint64_t>(LCG());
		} while (std::gcd(X, N) != 1 || X == 1 || X == 0);
	}

	std::uint64_t BlumBlumShub::NumberGeneration(std::uint64_t Iterations)
	{
		/* Generate a sequence of random numbers using the BBS algorithm. */

		// StateNumber is X mod N
		std::uint64_t RandomNumber = X % N;

		for (std::uint64_t Count = 0; Count < Iterations; ++Count)
		{
			// Compute RandomNumber' = pow(RandomNumber, 2) mod N
			RandomNumber = this->FastPowerAlgorithmWithModulus(RandomNumber, 2, N);

			std::cout << "Now Random Value of the 64 bit is:" << RandomNumber << std::endl;
		}

		return RandomNumber;
	}
}