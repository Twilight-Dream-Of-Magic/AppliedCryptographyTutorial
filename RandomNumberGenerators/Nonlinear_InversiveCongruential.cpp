#include "./PseudoRandomNumberGenerators.hpp"

namespace PseudoRandomNumberGenerators
{
	std::int64_t Nonlinear_InversiveCongruential(std::uint64_t Seed, std::uint64_t Iterations)
	{
		if (Seed == 0)
			Seed = 1;

		//https://en.wikipedia.org/wiki/Inversive_congruential_generator
		//https://www.johndcook.com/blog/2020/02/19/inverse-congruence-rng/
		constexpr std::int64_t A = 5520335699031059059, B = 2752743153957480735;
		constexpr std::int64_t PrimeNumber = 9223372036854775783;

		auto lambda_InversiveCongruentialGenerator = [](std::int64_t RandomNumber) -> std::int64_t
		{
			if (RandomNumber == 0)
				return B;
			else
			{
				//Inverse multiplicative moduled by PrimeNumber

				std::int64_t X = RandomNumber;
				std::int64_t Y = 1;
				std::int64_t Q = PrimeNumber - 2; //Fermat's little theorem <-> a ^ (p - 2)
				while (Q > 0)
				{
					Y *= static_cast<std::int64_t>(std::pow(RandomNumber, Q % 2));
					Y %= PrimeNumber;
					RandomNumber = static_cast<std::int64_t>(std::pow(RandomNumber, 2));
					RandomNumber %= PrimeNumber;
					Q >>= 1;
				}

				RandomNumber = (A * Y + B) % PrimeNumber;
			}

			return RandomNumber;
		};

		std::int64_t RandomNumber = static_cast<std::int64_t>(Seed);

		for (std::uint64_t count = 0; count < Iterations; ++count)
		{
			RandomNumber = lambda_InversiveCongruentialGenerator(RandomNumber);

			//std::cout << "This is random number: " << static_cast<std::uint64_t>(RandomNumber) << std::endl;
		}
		//std::cout << std::endl;

		return RandomNumber;
	}
}