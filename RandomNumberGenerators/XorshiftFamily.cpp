#include "XorshiftFamily.h"

//https://www.researchgate.net/publication/5142825_Xorshift_RNGs
//Xorshift RNGs
//DOI:10.18637/jss.v008.i14

//https://vigna.di.unimi.it/ftp/papers/xorshift.pdf
//https://vigna.di.unimi.it/ftp/papers/xorshiftplus.pdf

/* This generate is 32 bit number, with use 2^32 Bit Possibilities, The 32Bit version */

namespace PseudoRandomNumberGenerators::Xorshift
{
	std::uint32_t XorShift32::NumberGeneration32Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			State ^= State << 13;
			State ^= State >> 17;
			State ^= State << 15;
		}

		return State;
	}

	/* This generate is 32 bit number, with use 2^32 Bit Possibilities, The 32Bit+ version */

	std::uint32_t XorShift32Plus::NumberGeneration32Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			State ^= State << 13;
			State ^= State >> 17;
			State ^= State << 15;
		}

		return State + 0x9E3779B9;
	}

	/* This generate is 32 bit number, with use 2^32 Bit Possibilities, The 32Bit++ version */

	std::uint32_t XorShift32PlusPlus::NumberGeneration32Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			State ^= State << 13;
			State ^= State >> 17;
			State ^= State << 15;
			State += 0x9E3779B9;
		}

		return State + 0x9E3779B9;
	}

	/* This generate is 32 bit number, with use 2^32 Bit Possibilities, The 32Bit* version */

	std::uint32_t XorShift32Star::NumberGeneration32Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			State ^= State << 13;
			State ^= State >> 17;
			State ^= State << 15;
		}

		return State * 0x2545F491;
	}

	/* This generate is 32 bit number, with use 2^32 Bit Possibilities, The 32Bit** version */

	std::uint32_t XorShift32StarStar::NumberGeneration32Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			State ^= State << 13;
			State ^= State >> 17;
			State ^= State << 15;
			State *= 0x2545F491;
		}

		return State * 0x2545F491;
	}

	/* This generate is 64 bit number, with use 2^64 Bit Possibilities, The 64Bit version */

	std::uint64_t XorShift64::NumberGeneration64Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			//a, b, c = 16, 5, 17
			auto TransformedState = State[0] ^= State[0] << 16; //a
			State[0] = State[1];
			State[1] = (State[1] ^ (State[1] >> 17)) ^ (TransformedState ^ (TransformedState >> 5)); //c, b
		}

		std::uint64_t Result = static_cast<std::uint64_t>(State[1]) << 32 | static_cast<std::uint64_t>(State[0]);

		return Result;
	}

	/* This generate is 64 bit number, with use 2^64 Bit Possibilities, The 64Bit+ version */

	std::uint64_t XorShift64Plus::NumberGeneration64Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			//a, b, c = 16, 5, 17
			auto TransformedState = State[0] ^= State[0] << 16; //a
			State[0] = State[1];
			State[1] = (State[1] ^ (State[1] >> 17)) ^ (TransformedState ^ (TransformedState >> 5)); //c, b
		}

		std::uint64_t Result = static_cast<std::uint64_t>(State[1]) << 32 | static_cast<std::uint64_t>(State[0]);

		return Result + 0x9E3779B97F4A7C15;
	}

	/* This generate is 64 bit number, with use 2^64 Bit Possibilities, The 64Bit++ version */

	std::uint64_t XorShift64PlusPlus::NumberGeneration64Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			//a, b, c = 16, 5, 17
			auto TransformedState = State[0] ^= State[0] << 16; //a
			State[0] = State[1];
			State[1] = (State[1] ^ (State[1] >> 17)) ^ (TransformedState ^ (TransformedState >> 5)); //c, b
		}

		std::uint64_t Result = static_cast<std::uint64_t>(State[1]) << 32 | static_cast<std::uint64_t>(State[0]);

		return Result + (static_cast<std::uint64_t>(State[1]) + static_cast<std::uint64_t>(State[0])) + 0x9E3779B97F4A7C15;
	}

	/* This generate is 64 bit number, with use 2^64 Bit Possibilities, The 64Bit* version */

	std::uint64_t XorShift64Star::NumberGeneration64Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			//a, b, c = 16, 5, 17
			auto TransformedState = State[0] ^= State[0] << 16; //a
			State[0] = State[1];
			State[1] = (State[1] ^ (State[1] >> 17)) ^ (TransformedState ^ (TransformedState >> 5)); //c, b
		}

		std::uint64_t Result = static_cast<std::uint64_t>(State[1]) << 32 | static_cast<std::uint64_t>(State[0]);

		return Result * 0x2545F4914F6CDD1D;
	}

	/* This generate is 64 bit number, with use 2^64 Bit Possibilities, The 64Bit** version */

	std::uint64_t XorShift64StarStar::NumberGeneration64Bit(std::uint64_t Iterations)
	{
		for (std::size_t Round = 0; Round < Iterations; Round++)
		{
			//a, b, c = 16, 5, 17
			auto TransformedState = State[0] ^= State[0] << 16; //a
			State[0] = State[1];
			State[1] = (State[1] ^ (State[1] >> 17)) ^ (TransformedState ^ (TransformedState >> 5)); //c, b
		}

		std::uint64_t Result = static_cast<std::uint64_t>(State[1]) << 32 | static_cast<std::uint64_t>(State[0]);

		return Result * (static_cast<std::uint64_t>(State[1]) * static_cast<std::uint64_t>(State[0])) * 0x2545F4914F6CDD1D;;
	}

	/*
		https://groups.google.com/g/prng/c/hZRl7OIDgdE
		This generate is 32/64 bit number, with use (2^128/2^256/2^512/2^1024) Bit Possibilities.
		The (Xorshift/Xorshift*) version
	*/

	void XorshiftWithExtraState(std::span<std::uint64_t> State, std::uint64_t& Result)
	{
		if (State.empty())
			return;

		//Use this shift triple
		//a, b, c == 31, 11, 10

		std::uint64_t TransformedState = (State[0] ^ (State[0] << 31));

		//Loop to move the position of the array elements (to the left)
		std::ranges::rotate(State.begin(), State.begin() + 1, State.end());

		State[State.size() - 1] = (State[State.size() - 1] ^ State[State.size() - 1] >> 10)
			^ (TransformedState ^ (TransformedState >> 11)) * static_cast<std::uint64_t>(0x106689D45497FDB5);

		Result = State[State.size() - 1];
	}

	void XorshiftStarWithExtraState(std::span<std::uint64_t> State, std::uint32_t& StateIndex, std::uint64_t& Result)
	{
		if (State.empty())
			return;

		//Use this shift triple
		//a, b, c == 31, 11, 10

		std::uint64_t A = State[StateIndex];
		StateIndex = (StateIndex + 1) % State.size();
		std::uint64_t B = State[StateIndex];

		B ^= B << 31;
		B ^= B >> 11;
		A ^= A >> 10;

		State[StateIndex] = A ^ B;

		Result = State[StateIndex] * static_cast<std::uint64_t>(0x106689D45497FDB5);
	}
}