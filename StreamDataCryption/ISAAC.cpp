#include "ISAAC.h"

namespace StreamDataCryption::ISAAC
{
	//#define UPDATE_STATE_FUNCTION_COMPLEX

	std::atomic_bool sync_32bit_flag {false};
	std::atomic_bool sync_64bit_flag {false};

	void InitlaizeStateMix(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d, uint32_t& e, uint32_t& f, uint32_t& g, uint32_t& h)
	{
		//Make sure thread safe
		if(sync_32bit_flag.load(std::memory_order_seq_cst))
			sync_32bit_flag.wait(true, std::memory_order_seq_cst);

		sync_32bit_flag.store(true, std::memory_order_seq_cst);

		a ^= b << 11; d += a; b += c;
		
		b ^= c >> 2; e += b; c += d;
		
		c ^= d << 8; f += c; d += e;
		
		d ^= e >> 16; g += d; e += f;
		
		e ^= f << 10; h += e; f += g;
		
		f ^= g >> 4; a += f; g += h;
		
		g ^= h << 8; b += g; h += a;
		
		h ^= a >> 9; c += h; a += b;

		sync_32bit_flag.store(false, std::memory_order_seq_cst);
		sync_32bit_flag.notify_all();
	}

	void ISAAC_32Bit::Seed( std::string Seed, bool IsUseSeed )
	{
		/*
			Clear all register and counter
		*/
		a = b = 0;
		c = 0;
		
		for (uint32_t i = 0; i < STATE_SIZE; i++)
			StateArray[i] = ResultArray[i] = 0;

		/*
			Use seed data
		*/
		size_t SeedStringLength = Seed.length();
		for (uint32_t i = 0; i < STATE_SIZE; i++)
			// In case seed has less than 256 elements
			ResultArray[i] = i > SeedStringLength ? 0 : Seed[i];
		// Initialize ISAAC 32 Bit with seed
		InitializeState(IsUseSeed);
	}

	void ISAAC_32Bit::Seed( std::string Seed, std::uint32_t register_a, std::uint32_t register_b )
	{
		/*
			Set all register and reset counter
		*/
		a = register_a;
		b = register_b;
		c = 0;

		for (uint32_t i = 0; i < STATE_SIZE; i++)
			StateArray[i] = ResultArray[i] = 0;

		/*
			Use seed data
		*/
		size_t SeedStringLength = Seed.length();
		for (uint32_t i = 0; i < STATE_SIZE; i++)
			// In case seed has less than 256 elements
			ResultArray[i] = i > SeedStringLength ? 0 : Seed[i];
		// Initialize ISAAC 64 Bit with seed
		InitializeState(true);
	}

	void ISAAC_32Bit::Seed( std::uint32_t register_a, std::uint32_t register_b, std::span<const std::uint32_t> extra_seeds )
	{
		/*
			Set all register and reset counter
		*/
		a = register_a;
		b = register_b;
		c = 0;

		for (uint32_t i = 0; i < STATE_SIZE; i++)
			StateArray[i] = ResultArray[i] = 0;

		/*
			Use seed data
		*/
		size_t SeedStringLength = extra_seeds.size();
		for (uint32_t i = 0; i < STATE_SIZE; i++)
			// In case seed has less than 256 elements
			ResultArray[i] = i > SeedStringLength ? 0 : extra_seeds[i];
		// Initialize ISAAC 32 Bit with seed
		InitializeState(true);
	}

	std::uint32_t ISAAC_32Bit::Generate()
	{
		std::uint32_t ResultValue = ResultArray[RoundCounter];
		++RoundCounter;
		if(RoundCounter > ResultArray.size() - 1)
		{
			UpdateState();
			RoundCounter = 0;
		}
		return ResultValue;
	}

	void ISAAC_32Bit::InitializeState(bool IsUseSeed)
	{
		std::array<std::uint32_t, 8> MixArray {};

		auto& [a,b,c,d,e,f,g,h] = MixArray;
		a = b = c = d = e = f = g = h = 0x9e3779b9; //GOLDEN_RATIO binary

		//Scramble it
		for ( size_t i = 0; i < 4; i++ )
			InitlaizeStateMix(a,b,c,d,e,f,g,h);

		// Fill in state[] with messy stuff
		for (size_t i = 0; i < STATE_SIZE; i += 8 )
		{
			if(IsUseSeed) // Initialize using the contents of result[] as the seed
				for (size_t j = 0; j < 8; ++j)
					MixArray[j] += ResultArray[i + j];

			InitlaizeStateMix( a, b, c, d, e, f, g, h );

			for (uint32_t j = 0; j < 8; ++j)
				StateArray[i + j] = MixArray[j];
		}
		
		if(!IsUseSeed)
		{
			// Do a second pass to make all of the seed affect all of state[].
			for (size_t i = 0; i < STATE_SIZE; i += 8)
			{
				for (uint32_t j = 0; j < 8; ++j)
					MixArray[j] += StateArray[i + j];

				InitlaizeStateMix( a, b, c, d, e, f, g, h );

				for (uint32_t j = 0; j < 8; ++j)
					StateArray[i + j] = MixArray[j];
			}
		}

		UpdateState(); // Fill in the first set of results
		this->RoundCounter = 0; //Prepare to use the first set of results 
	}

	void ISAAC_32Bit::UpdateState()
	{
		// Increment the counter
		++c;

		// Add the counter to 'b'
		b += c;
		
		// Default State Array Size Is 256
		std::uint64_t StateArraySize = STATE_SIZE;

		// Initialize working variables
		std::uint32_t x = 0, y = 0;

		#if !defined(UPDATE_STATE_FUNCTION_COMPLEX)
		std::uint32_t i;

		for (i=0; i < StateArraySize; i++)
		{
			x = StateArray[i];

			// barrel shift
			switch (i & 3)
			{
				case 0: a = a ^ (a << 13); break;
				case 1: a = a ^ (a >> 6); break;
				case 2: a = a ^ (a << 2); break;
				case 3: a = a ^ (a >> 16); break;
			}

			if(IsPlusVersion)
			{
				a = StateArray[(i+128) & 255] + a;
				y  = a ^ b + StateArray[std::rotr(x, 2) & 255];
				StateArray[i] = y;
				b = x + a ^ StateArray[std::rotr(y, 10) & 255]; 
				ResultArray[i]= b;
			}
			else
			{
				a = StateArray[(i+128) & 255] + a;
				y  = a + b + StateArray[(x >> 2) & 255];
				StateArray[i] = y;
				b = x + StateArray[(y >> 10) & 255]; 
				ResultArray[i]= b;
			}
		}

		#else
		/*
			In the UpdateState function of the ISAAC algorithm, the state array, StateArray, is divided into two halves, which are processed separately.
			The variables i and j serve as indices to iterate over these two halves.

			Here's what's happening:

			i iterates over the first half of StateArray. In the first loop, i ranges from 0 to StateArray.size() / 2 - 1.
			The variable j starts from StateArray.size() / 2 and moves towards the end of the array. Essentially, i is covering the first half of StateArray, while j is covering the second half. 
			This is the first "mixing" phase.

			In the second loop, the roles of i and j are reversed. i starts from StateArray.size() / 2 and moves towards the end of the array, while j ranges from 0 to StateArray.size() / 2 - 1.
			This time, i is covering the second half of StateArray, while j is covering the first half. This is the second "mixing" phase.

			The reason for this two-phase approach is to ensure that each element of the StateArray interacts with every other element during the update process.
			This provides a high degree of mixing and contributes to the cryptographic strength of the resulting random numbers.

			The "interval" here refers to the distinct ranges of i and j in each loop. The division of StateArray into two halves and the distinct intervals for i and j ensure that the entire StateArray is processed.
			This two-phase, or "double pass", approach also ensures that each update of the state array depends on both the previous and next states, providing a high degree of mixing and randomness.
		*/
		std::uint32_t i = 0, j = StateArraySize / 2;
		
		// First half of the state array: i （ [0, StateArraySize / 2 - 1], j （ [StateArraySize / 2, StateArraySize - 1]
		while(i < StateArraySize / 2)
		{
			x = StateArray[i];
			
			// barrel shift
			// Modify 'a' based on the value of 'i' mod 4
			switch (i & 3)
			{
				case 0: a = a ^ (a << 13); break;
				case 1: a = a ^ (a >> 6); break;
				case 2: a = a ^ (a << 2); break;
				case 3: a = a ^ (a >> 16); break;
			}

			// Process for Plus version
			if(IsPlusVersion)
			{
				a += StateArray[j++];
				y = a ^ b + StateArray[std::rotr(x, 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + a ^ StateArray[std::rotr(y, 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
			// Process for origin version
			else
			{
				a += StateArray[j++];
				y = a + b + StateArray[(x >> 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + StateArray[(y >> 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
		}
		
		// Reset 'j' for second half of the state array
		j = 0;

		// Second half of the state array: i （ [StateArraySize / 2, StateArraySize - 1], j （ [0, StateArraySize / 2 - 1]
		while(j < StateArraySize / 2)
		{
			x = StateArray[i];
			
			//a' = f(a, i)
			switch (i & 3)
			{
				case 0: a = a ^ (a << 13); break;
				case 1: a = a ^ (a >> 6); break;
				case 2: a = a ^ (a << 2); break;
				case 3: a = a ^ (a >> 16); break;
			}

			// Process for Plus version
			if(IsPlusVersion)
			{
				a += StateArray[j++];
				y = a ^ b + StateArray[std::rotr(x, 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + a ^ StateArray[std::rotr(y, 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
			// Process for origin version
			else
			{
				a += StateArray[j++];
				y = a + b + StateArray[(x >> 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + StateArray[(y >> 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
		}

		#endif
	}

	/****************************************************************************************************************************/

	void InitlaizeStateMix(uint64_t& a, uint64_t& b, uint64_t& c, uint64_t& d, uint64_t& e, uint64_t& f, uint64_t& g, uint64_t& h)
	{
		//Make sure thread safe
		if(sync_64bit_flag.load(std::memory_order_seq_cst))
			sync_64bit_flag.wait(true, std::memory_order_seq_cst);

		sync_64bit_flag.store(true, std::memory_order_seq_cst);

		a -= e; f ^= h >> 9; h += a;

		b -= f; g ^= a << 9; a += b;

		c -= g; h ^= b >> 23; b += c;

		d -= h; a ^= c << 15; c += d;

		e -= a; b ^= d >> 14; d += e;

		f -= b; c ^= e << 20; e += f;

		g -= c; d ^= f >> 17; f += g;

		h -= d; e ^= g << 14; g += h;

		sync_64bit_flag.store(false, std::memory_order_seq_cst);
		sync_64bit_flag.notify_all();
	}

	void ISAAC_64Bit::Seed(std::string Seed, bool IsUseSeed)
	{
		/*
			Clear all register and counter
		*/
		a = b = 0;
		c = 0;

		for (uint32_t i = 0; i < STATE_SIZE; i++)
			StateArray[i] = ResultArray[i] = 0;

		/*
			Use seed data
		*/
		size_t SeedStringLength = Seed.length();
		for (uint32_t i = 0; i < STATE_SIZE; i++)
			// In case seed has less than 256 elements
			ResultArray[i] = i > SeedStringLength ? 0 : Seed[i];
		// Initialize ISAAC 64 Bit with seed
		InitializeState(IsUseSeed);
	}

	void ISAAC_64Bit::Seed( std::string Seed, std::uint64_t register_a, std::uint64_t register_b )
	{
		/*
			Set all register and reset counter
		*/
		a = register_a;
		b = register_b;
		c = 0;

		for (uint32_t i = 0; i < STATE_SIZE; i++)
			StateArray[i] = ResultArray[i] = 0;

		/*
			Use seed data
		*/
		size_t SeedStringLength = Seed.length();
		for (uint32_t i = 0; i < STATE_SIZE; i++)
			// In case seed has less than 256 elements
			ResultArray[i] = i > SeedStringLength ? 0 : Seed[i];
		// Initialize ISAAC 64 Bit with seed
		InitializeState(true);
	}

	void ISAAC_64Bit::Seed( std::uint64_t register_a, std::uint64_t register_b, std::span<const std::uint64_t> extra_seeds )
	{
		/*
			Set all register and reset counter
		*/
		a = register_a;
		b = register_b;
		c = 0;

		for (uint32_t i = 0; i < STATE_SIZE; i++)
			StateArray[i] = ResultArray[i] = 0;

		/*
			Use seed data
		*/
		size_t SeedStringLength = extra_seeds.size();
		for (uint32_t i = 0; i < STATE_SIZE; i++)
			// In case seed has less than 256 elements
			ResultArray[i] = i > SeedStringLength ? 0 : extra_seeds[i];
		// Initialize ISAAC 32 Bit with seed
		InitializeState(true);
	}

	std::uint64_t ISAAC_64Bit::Generate()
	{
		std::uint64_t ResultValue = ResultArray[RoundCounter];
		++RoundCounter;
		if(RoundCounter > ResultArray.size() - 1)
		{
			UpdateState();
			RoundCounter = 0;
		}
		return ResultValue;
	}

	void ISAAC_64Bit::InitializeState(bool IsUseSeed)
	{
		std::array<std::uint64_t, 8> MixArray {};

		auto& [a,b,c,d,e,f,g,h] = MixArray;
		a = b = c = d = e = f = g = h = 0x9e3779b97f4a7c13; //GOLDEN_RATIO binary

		//Scramble it
		for ( size_t i = 0; i < 4; i++ )
			InitlaizeStateMix(a,b,c,d,e,f,g,h);

		// Fill in state[] with messy stuff
		for (size_t i = 0; i < STATE_SIZE; i += 8 )
		{
			if(IsUseSeed) // Initialize using the contents of result[] as the seed
				for (size_t j = 0; j < 8; ++j)
					MixArray[j] += ResultArray[i + j];

			InitlaizeStateMix( a, b, c, d, e, f, g, h );

			for (uint32_t j = 0; j < 8; ++j)
				StateArray[i + j] = MixArray[j];
		}
		
		if(!IsUseSeed)
		{
			// Do a second pass to make all of the seed affect all of state[].
			for (size_t i = 0; i < STATE_SIZE; i += 8)
			{
				for (uint32_t j = 0; j < 8; ++j)
					MixArray[j] += StateArray[i + j];

				InitlaizeStateMix( a, b, c, d, e, f, g, h );

				for (uint32_t j = 0; j < 8; ++j)
					StateArray[i + j] = MixArray[j];
			}
		}

		UpdateState(); // Fill in the first set of results
		this->RoundCounter = 0; //Prepare to use the first set of results 
	}

	void ISAAC_64Bit::UpdateState()
	{
		// Increment the counter
		++c;

		// Add the counter to 'b'
		b += c;
		
		// Default State Array Size Is 256
		std::uint64_t StateArraySize = STATE_SIZE;

		// Initialize working variables
		std::uint64_t x = 0, y = 0;

		#if !defined(UPDATE_STATE_FUNCTION_COMPLEX)
		std::uint32_t i;

		for (i=0; i < StateArraySize; i++)
		{
			x = StateArray[i];

			// barrel shift
			switch ( i & 3 )
			{
				case 0: a = ~(a ^ (a << 21)); break;
				case 1: a = a ^ (a >> 5); break;
				case 2: a = a ^ (a << 12); break;
				case 3: a = a ^ (a >> 33); break;
			}

			if(IsPlusVersion)
			{
				a = StateArray[(i+128) & 255] + a;
				y  = a ^ b + StateArray[std::rotr(x, 2) & 255];
				StateArray[i] = y;
				b = x + a ^ StateArray[std::rotr(y, 10) & 255]; 
				ResultArray[i]= b;
			}
			else
			{
				a = StateArray[(i+128) & 255] + a;
				y  = a + b + StateArray[(x >> 2) & 255];
				StateArray[i] = y;
				b = x + StateArray[(y >> 10) & 255]; 
				ResultArray[i]= b;
			}
		}

		#else
		/*
			In the UpdateState function of the ISAAC algorithm, the state array, StateArray, is divided into two halves, which are processed separately.
			The variables i and j serve as indices to iterate over these two halves.

			Here's what's happening:

			i iterates over the first half of StateArray. In the first loop, i ranges from 0 to StateArray.size() / 2 - 1.
			The variable j starts from StateArray.size() / 2 and moves towards the end of the array. Essentially, i is covering the first half of StateArray, while j is covering the second half. 
			This is the first "mixing" phase.

			In the second loop, the roles of i and j are reversed. i starts from StateArray.size() / 2 and moves towards the end of the array, while j ranges from 0 to StateArray.size() / 2 - 1.
			This time, i is covering the second half of StateArray, while j is covering the first half. This is the second "mixing" phase.

			The reason for this two-phase approach is to ensure that each element of the StateArray interacts with every other element during the update process.
			This provides a high degree of mixing and contributes to the cryptographic strength of the resulting random numbers.

			The "interval" here refers to the distinct ranges of i and j in each loop. The division of StateArray into two halves and the distinct intervals for i and j ensure that the entire StateArray is processed.
			This two-phase, or "double pass", approach also ensures that each update of the state array depends on both the previous and next states, providing a high degree of mixing and randomness.
		*/
		std::uint32_t i = 0, j = StateArraySize / 2;
		
		// First half of the state array: i （ [0, StateArraySize / 2 - 1], j （ [StateArraySize / 2, StateArraySize - 1]
		while(i < StateArraySize / 2)
		{
			x = StateArray[i];
			
			// barrel shift
			// Modify 'a' based on the value of 'i' mod 4
			switch ( i & 3 )
			{
				case 0: a = ~(a ^ (a << 21)); break;
				case 1: a = a ^ (a >> 5); break;
				case 2: a = a ^ (a << 12); break;
				case 3: a = a ^ (a >> 33); break;
			}

			// Process for Plus version
			if(IsPlusVersion)
			{
				a += StateArray[j++];
				y = a ^ b + StateArray[std::rotr(x, 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + a ^ StateArray[std::rotr(y, 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
			// Process for origin version
			else
			{
				a += StateArray[j++];
				y = a + b + StateArray[(x >> 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + StateArray[(y >> 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
		}
		
		// Reset 'j' for second half of the state array
		j = 0;

		// Second half of the state array: i （ [StateArraySize / 2, StateArraySize - 1], j （ [0, StateArraySize / 2 - 1]
		while(j < StateArraySize / 2)
		{
			x = StateArray[i];
			
			//a' = f(a, i)
			switch ( i & 3 )
			{
				case 0: a = ~(a ^ (a << 21)); break;
				case 1: a = a ^ (a >> 5); break;
				case 2: a = a ^ (a << 12); break;
				case 3: a = a ^ (a >> 33); break;
			}

			// Process for Plus version
			if(IsPlusVersion)
			{
				a += StateArray[j++];
				y = a ^ b + StateArray[std::rotr(x, 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + a ^ StateArray[std::rotr(y, 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
			// Process for origin version
			else
			{
				a += StateArray[j++];
				y = a + b + StateArray[(x >> 2) & (StateArraySize - 1)];
				StateArray[i] = y;
				b = x + StateArray[(y >> 10) & (StateArraySize - 1)];
				ResultArray[i++] = b;
			}
		}

		#endif
	}

	#if defined(UPDATE_STATE_FUNCTION_COMPLEX)
	#undef UPDATE_STATE_FUNCTION_COMPLEX
	#endif
}