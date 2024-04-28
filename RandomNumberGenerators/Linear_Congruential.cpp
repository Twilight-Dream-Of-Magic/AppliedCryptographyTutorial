#include "./PseudoRandomNumberGenerators.hpp"

namespace PseudoRandomNumberGenerators
{
	std::uint64_t Linear_Congruential(std::uint64_t Seed, std::uint64_t Iterations)
	{
		if (Seed == 0)
			Seed = 1;

		//https://en.wikipedia.org/wiki/Linear_congruential_generator
		constexpr std::uint64_t A = 6364136223846793005, B = 1442695040888963407;
		std::uint64_t RandomNumber = Seed;

		for (std::uint64_t count = 0; count < Iterations; ++count)
		{
			RandomNumber = (A * RandomNumber + B) % std::numeric_limits<std::uint64_t>::max();
			//std::cout << "This is random number: " << RandomNumber << std::endl;
		}
		//std::cout << std::endl;

		return RandomNumber;
	}
}