#pragma once

#include "../MainProgram/Support+Library/Support-Library.hpp"

namespace StreamDataCryption::ISAAC
{
	class ISAAC_32Bit
	{
	public:
		bool IsPlusVersion = false;

		void Seed(std::string Seed, bool IsUseSeed);
		void Seed(std::string Seed, std::uint32_t register_a, std::uint32_t register_b);
		void Seed(std::uint32_t register_a, std::uint32_t register_b, std::span<const std::uint32_t> extra_seeds);
		std::uint32_t Generate();

		ISAAC_32Bit()
			: ALPHA(8)
		{
			
		}

		ISAAC_32Bit(uint64_t alpha)
			: ALPHA((alpha % 8 != 0) ? 8 : alpha)
		{
			
		}

	private:
		void InitializeState(bool IsUseSeed);
		void UpdateState();

		//Log2 of array size
		const std::uint32_t ALPHA = 0;
		const std::uint32_t STATE_SIZE = 1 << ALPHA;
		std::vector<std::uint32_t> StateArray = std::vector<std::uint32_t>(STATE_SIZE, 0);
		std::vector<std::uint32_t> ResultArray = std::vector<std::uint32_t>(STATE_SIZE, 0);
		std::uint32_t RoundCounter = 0;

		//Registers (Seeds)
		std::uint32_t a = 0;
		std::uint32_t b = 0;

		//State counter
		std::uint32_t c = 0;
	};

	class ISAAC_64Bit
	{
	public:
		bool IsPlusVersion = false;

		void Seed(std::string Seed, bool IsUseSeed);
		void Seed(std::string Seed, std::uint64_t register_a, std::uint64_t register_b);
		void Seed(std::uint64_t register_a, std::uint64_t register_b, std::span<const std::uint64_t> extra_seeds);
		std::uint64_t Generate();

		ISAAC_64Bit()
			: ALPHA(8)
		{
			
		}

		ISAAC_64Bit(uint64_t alpha)
			: ALPHA((alpha % 8 != 0) ? 8 : alpha)
		{
			
		}

	private:
		void InitializeState(bool IsUseSeed);
		void UpdateState();

		//Log2 of array size
		const std::uint32_t ALPHA = 0;
		const std::uint32_t STATE_SIZE = 1 << ALPHA;
		std::vector<std::uint64_t> StateArray = std::vector<std::uint64_t>(STATE_SIZE, 0);
		std::vector<std::uint64_t> ResultArray = std::vector<std::uint64_t>(STATE_SIZE, 0);
		std::uint32_t RoundCounter = 0;

		//Registers (Seeds)
		std::uint64_t a = 0;
		std::uint64_t b = 0;

		//State counter
		std::uint64_t c = 0;
	};
}