#pragma once

#include "PseudoRandomNumberGenerators.hpp"

namespace PseudoRandomNumberGenerators::Xorshift
{
	void XorshiftWithExtraState(std::span<std::uint64_t> State, std::uint64_t& Result);

	void XorshiftStarWithExtraState(std::span<std::uint64_t> State, std::uint32_t& StateIndex, std::uint64_t& Result);

	//This is XorShift Base class function interface
	struct XorShift : UniformRandomBitGenerator<std::uint64_t>
	{

	public:
		virtual void ResetState(std::uint32_t Seed)
		{
			throw std::runtime_error("Base XorShift Class, No need to implement 'void ResetState(std::uint32_t Seed)' function!");
		}

		virtual void ResetState(std::uint64_t Seed)
		{
			throw std::runtime_error("Base XorShift Class, No need to implement 'void ResetState(std::uint64_t Seed)' function!");
		}

		virtual std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations)
		{
			throw std::runtime_error("Base XorShift Class, No need to implement 'std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations)' function!");
			return 0;
		}

		virtual std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations)
		{
			throw std::runtime_error("Base XorShift Class, No need to implement 'std::uint64_t NumberGeneration32Bit(std::uint64_t Iterations)' function!");
			return 0;
		}

		std::uint64_t SplitMix64(std::uint64_t Seed)
		{
			Seed += 0x9e3779b97f4a7c15;
			Seed = (Seed ^ (Seed >> 30)) * 0xbf58476d1ce4e5b9;
			Seed = (Seed ^ (Seed >> 27)) * 0x94d049bb133111eb;
			return Seed ^ (Seed >> 31);
		}

		XorShift() = default;
		virtual ~XorShift() = default;
	};

	/* XorShift32Bit Algorithms */

	class XorShift32 : XorShift
	{
	private:
		std::uint32_t State = 0;

	public:
		void ResetState(std::uint32_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State = Value >> 32;
			Value = 0;
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final;

		XorShift32() = delete;

		XorShift32(std::uint32_t SeedValue)
			: XorShift(), State(SeedValue)
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift32() = default;
	};

	class XorShift32Plus : XorShift
	{
	private:
		std::uint32_t State = 0;

	public:
		void ResetState(std::uint32_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State = Value >> 32;
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final;

		XorShift32Plus() = delete;

		XorShift32Plus(std::uint32_t SeedValue)
			: XorShift(), State(0)
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift32Plus() = default;
	};

	class XorShift32PlusPlus : XorShift
	{
	private:
		std::uint32_t State = 0;

	public:
		void ResetState(std::uint32_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State = Value >> 32;
			Value = 0;
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final;

		XorShift32PlusPlus() = delete;

		XorShift32PlusPlus(std::uint32_t SeedValue)
			: XorShift(), State(0)
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift32PlusPlus() = default;
	};

	class XorShift32Star : XorShift
	{
	private:
		std::uint32_t State = 0;

	public:
		void ResetState(std::uint32_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State = Value >> 32;
			Value = 0;
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final;

		XorShift32Star() = delete;

		XorShift32Star(std::uint32_t SeedValue)
			: XorShift(), State(SeedValue)
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift32Star() = default;
	};

	class XorShift32StarStar : XorShift
	{
	private:
		std::uint32_t State = 0;

	public:
		void ResetState(std::uint32_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State = Value >> 32;
			Value = 0;
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final;

		XorShift32StarStar() = delete;

		XorShift32StarStar(std::uint32_t SeedValue)
			: XorShift(), State(SeedValue)
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift32StarStar() = default;
	};

	/* XorShift64Bit Algorithms */

	class XorShift64 : XorShift
	{
	private:
		std::array<std::uint32_t, 2> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final;

		XorShift64() = delete;

		XorShift64(std::uint64_t SeedValue)
			: XorShift()
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift64() = default;
	};

	class XorShift64Plus : XorShift
	{
	private:
		std::array<std::uint32_t, 2> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final;

		XorShift64Plus() = delete;

		XorShift64Plus(std::uint64_t SeedValue)
			: XorShift()
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift64Plus() = default;
	};

	class XorShift64PlusPlus : XorShift
	{
	private:
		std::array<std::uint32_t, 2> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final;

		XorShift64PlusPlus() = delete;

		XorShift64PlusPlus(std::uint64_t SeedValue)
			: XorShift()
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}
		virtual ~XorShift64PlusPlus() = default;
	};

	class XorShift64Star : XorShift
	{
	private:
		std::array<std::uint32_t, 2> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final;

		XorShift64Star() = delete;

		XorShift64Star(std::uint64_t SeedValue)
			: XorShift()
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}
		virtual ~XorShift64Star() = default;
	};

	class XorShift64StarStar : XorShift
	{
	private:
		std::array<std::uint32_t, 2> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			std::uint64_t Value = this->SplitMix64(Seed);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final;

		XorShift64StarStar() = delete;

		XorShift64StarStar(std::uint64_t SeedValue)
			: XorShift()
		{
			std::uint64_t Value = this->SplitMix64(SeedValue);
			State[0] = Value >> 32;
			Value = this->SplitMix64(State[0]);
			State[1] = Value >> 32;
			Value = 0;
		}

		virtual ~XorShift64StarStar() = default;
	};

	/* XorShift(128/256/512/1024)Bit Algorithms */

	class XorShift128 : XorShift
	{
	private:
		std::array<std::uint64_t, 2> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);
			State[1] = this->SplitMix64(State[0]);
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber;
		}

		XorShift128() = delete;

		XorShift128(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift128() = default;
	};

	class XorShift128Star : XorShift
	{
	private:
		std::array<std::uint64_t, 2> State{};
		std::uint32_t StateIndex = 0;

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);
			State[1] = this->SplitMix64(State[0]);
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber;
		}

		XorShift128Star() = delete;

		XorShift128Star(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift128Star() = default;
	};

	class XorShift256 : XorShift
	{
	private:
		std::array<std::uint64_t, 4> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);

			for (size_t i = 0; i < State.size() - 1; i++)
			{
				State[i + 1] = this->SplitMix64(State[i]);
			}
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber;
		}

		XorShift256() = delete;

		XorShift256(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift256() = default;
	};

	class XorShift256Star : XorShift
	{
	private:
		std::array<std::uint64_t, 4> State{};
		std::uint32_t StateIndex = 0;

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);

			for (size_t i = 0; i < State.size() - 1; i++)
			{
				State[i + 1] = this->SplitMix64(State[i]);
			}
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber;
		}

		XorShift256Star() = delete;

		XorShift256Star(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift256Star() = default;
	};

	class XorShift512 : XorShift
	{
	private:
		std::array<std::uint64_t, 8> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);

			for (size_t i = 0; i < State.size() - 1; i++)
			{
				State[i + 1] = this->SplitMix64(State[i]);
			}
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber;
		}

		XorShift512() = delete;

		XorShift512(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift512() = default;
	};

	class XorShift512Star : XorShift
	{
	private:
		std::array<std::uint64_t, 8> State{};
		std::uint32_t StateIndex = 0;

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);

			for (size_t i = 0; i < State.size() - 1; i++)
			{
				State[i + 1] = this->SplitMix64(State[i]);
			}
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber;
		}

		XorShift512Star() = delete;

		XorShift512Star(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift512Star() = default;
	};

	class XorShift1024 : XorShift
	{
	private:
		std::array<std::uint64_t, 16> State{};

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);

			for (size_t i = 0; i < State.size() - 1; i++)
			{
				State[i + 1] = this->SplitMix64(State[i]);
			}
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftWithExtraState(State, RandomNumber);
			return RandomNumber;
		}

		XorShift1024() = delete;

		XorShift1024(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift1024() = default;
	};

	class XorShift1024Star : XorShift
	{
	private:
		std::array<std::uint64_t, 16> State{};
		std::uint32_t StateIndex = 0;

	public:
		void ResetState(std::uint64_t Seed) override final
		{
			if (Seed == 0)
				Seed = 1;

			State[0] = this->SplitMix64(Seed);

			for (size_t i = 0; i < State.size() - 1; i++)
			{
				State[i + 1] = this->SplitMix64(State[i]);
			}
		}

		std::uint32_t NumberGeneration32Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber >> 32;
		}

		std::uint64_t NumberGeneration64Bit(std::uint64_t Iterations) override final
		{
			std::uint64_t RandomNumber = 0;
			XorshiftStarWithExtraState(State, StateIndex, RandomNumber);
			return RandomNumber;
		}

		XorShift1024Star() = delete;

		XorShift1024Star(std::uint64_t SeedValue)
			: XorShift(), State{ 0, 0 }
		{
			this->ResetState(SeedValue);

			//Discard the same status for the first 128 rounds.
			for(std::uint32_t Round = 0; Round < 128; ++Round)
			{
				NumberGeneration64Bit(1);
			}
		}

		virtual ~XorShift1024Star() = default;
	};

}