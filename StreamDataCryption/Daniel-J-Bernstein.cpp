#include "Daniel-J-Bernstein.h"

namespace StreamDataCryption::DJB
{
	/*
		Security Analysis of ChaCha20-Poly1305 AEAD
		https://www.cryptrec.go.jp/exreport/cryptrec-ex-2601-2016.pdf
	*/

	void Salsa20::InitializeState(std::span<std::uint8_t> Keys, std::span<std::uint8_t> NumberOnces)
	{
		std::array<std::uint32_t, 8> KeyPositionLookupTable { 1, 2, 3, 4, 11, 12, 13, 14 };

		std::array<std::uint32_t, 2> NumberOncePositionLookupTable { 6, 7 };

		std::uint32_t Number = 0;
		std::uint32_t ByteIndex = 0;
		for ( std::uint32_t Index = 0; Index < KeyPositionLookupTable.size(); ++Index )
		{
			if ( ByteIndex + 3 < Keys.size() )
			{
				Number = static_cast<std::uint32_t>( Keys[ ByteIndex ] ) << 24
					| static_cast<std::uint32_t>( Keys[ ByteIndex + 1 ] ) << 16
					| static_cast<std::uint32_t>( Keys[ ByteIndex + 2 ] ) << 8
					| static_cast<std::uint32_t>( Keys[ ByteIndex + 3 ] );
				ByteIndex += 4;
			}

			this->State[ KeyPositionLookupTable[ Index ] ] = Number;
			Number = 0;
		}

		ByteIndex = 0;
		for ( std::uint32_t Index = 0; Index < NumberOncePositionLookupTable.size(); ++Index )
		{
			if ( ByteIndex + 3 < NumberOnces.size() )
			{
				Number = static_cast<std::uint32_t>( NumberOnces[ ByteIndex ] ) << 24
					| static_cast<std::uint32_t>( NumberOnces[ ByteIndex + 1 ] ) << 16
					| static_cast<std::uint32_t>( NumberOnces[ ByteIndex + 2 ] ) << 8
					| static_cast<std::uint32_t>( NumberOnces[ ByteIndex + 3 ] );
				ByteIndex += 4;
			}

			this->State[ NumberOncePositionLookupTable[ Index ] ] = Number;
			Number = 0;
		}
	}

	void Salsa20::QuarterRound(std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d)
	{
		//Twilight-Dream's superficial understanding of the role of the basic operations needed for the QuarterRound function of the Salsa algorithm.
		//Step1: Nonlinear function (Modulo Addition)
		//Step2: Bit diffusion (Bitwise left rotation)
		//Step3: Bit confusion (Bitwise XOR)

		/*
			https://cr.yp.to/snuffle/spec.pdf
			Salsa20 rotation distances: 7, 9, 13, 18
			
			https://cr.yp.to/snuffle/salsafamily-20071225.pdf
			Should there be other rotation distances?
			I chose the Salsa20 rotation distances 7, 11, 13, 18 as doing a good job of spreading every low-weight change across bit positions within a few rounds. 
			The exactchoice of distances doesn¡¯t seem very important.
			My software uses SIMD vector operations for the Pentium 4, the Core 2, etal. 
			These operations rely on the fact that each column uses the same sequence of distances.
		*/

		b ^= std::rotl( (a + d), 7);
		c ^= std::rotl( ( b + a ), 9);
		d ^= std::rotl( ( c + b ), 13);
		a ^= std::rotl( ( d + c ), 18);
	}

	void Salsa20::UpdateState(std::array<std::uint32_t, 16>& WorkingState)
	{
		//StateCopy = State
		std::ranges::copy(this->State.begin(), this->State.end(), WorkingState.begin());

		//Round function
		//State' = Function(StateCopy)
		for (std::uint32_t Round = 0; Round < 20; Round += 2)
		{
			// Odd round
			this->QuarterRound(WorkingState[ 0], WorkingState[ 4], WorkingState[ 8], WorkingState[12]);	// column 1
			this->QuarterRound(WorkingState[ 5], WorkingState[ 9], WorkingState[13], WorkingState[ 1]);	// column 2
			this->QuarterRound(WorkingState[10], WorkingState[14], WorkingState[ 2], WorkingState[ 6]);	// column 3
			this->QuarterRound(WorkingState[15], WorkingState[ 3], WorkingState[ 7], WorkingState[11]);	// column 4
			
			// Even round
			this->QuarterRound(WorkingState[ 0], WorkingState[ 1], WorkingState[ 2], WorkingState[ 3]);	// row 1
			this->QuarterRound(WorkingState[ 5], WorkingState[ 6], WorkingState[ 7], WorkingState[ 4]);	// row 2
			this->QuarterRound(WorkingState[10], WorkingState[11], WorkingState[ 8], WorkingState[ 9]);	// row 3
			this->QuarterRound(WorkingState[15], WorkingState[12], WorkingState[13], WorkingState[14]);	// row 4
		}

		//Mixing the data of the working state with the data of the state is equivalent to a secure hash function
		//Hashed = Hash(State', State)
		//Hashed = State' + State
		//Hashed = Function(StateCopy) + State
		for (std::uint32_t Index = 0; Index < 16; ++Index)
			WorkingState[Index] += this->State[Index];
		
		//Update Counter
		std::span<std::uint32_t> Counters {this->State.begin() + 9, this->State.begin() + 10};
		IncrementCounter(Counters);
	}

	std::vector<std::uint8_t> Salsa20::GenerateKeyStream(std::uint64_t Count)
	{
		std::vector<std::uint8_t> KeyStream(Count, 0x00);
		std::array<std::uint32_t, 16> WorkingState {};

		for ( std::uint64_t CurrentCount = 0; CurrentCount < Count; CurrentCount += 64 )
		{
			this->UpdateState( WorkingState );

			// Convert working state to key stream bytes
			for ( std::uint32_t WorkingStateIndex = 0; WorkingStateIndex < 16; ++WorkingStateIndex )
			{
				std::uint32_t Value = WorkingState[ WorkingStateIndex ];
				for ( std::uint32_t ByteIndex = 0; ByteIndex < 4 && ( CurrentCount + WorkingStateIndex * 4 + ByteIndex ) < Count; ++ByteIndex )
				{
					KeyStream[ CurrentCount + WorkingStateIndex * 4 + ByteIndex ] = static_cast<std::uint8_t>( Value & 0xFF );
					Value >>= 8;
				}
			}
		}

		return KeyStream;
	}

	void Chacha20::InitializeState( std::span<std::uint8_t> Keys, std::span<std::uint8_t> NumberOnces )
	{
		std::array<std::uint32_t, 8> KeyPositionLookupTable { 4, 5, 6, 7, 8, 9, 10, 11 };

		std::array<std::uint32_t, 2> NumberOncePositionLookupTable { 14, 15 };

		std::uint32_t Number = 0;
		std::uint32_t ByteIndex = 0;
		for ( std::uint32_t Index = 0; Index < KeyPositionLookupTable.size(); ++Index )
		{
			if ( ByteIndex + 3 < Keys.size() )
			{
				Number = static_cast<std::uint32_t>( Keys[ ByteIndex ] ) << 24
					| static_cast<std::uint32_t>( Keys[ ByteIndex + 1 ] ) << 16
					| static_cast<std::uint32_t>( Keys[ ByteIndex + 2 ] ) << 8
					| static_cast<std::uint32_t>( Keys[ ByteIndex + 3 ] );
				ByteIndex += 4;
			}

			this->State[ KeyPositionLookupTable[ Index ] ] = Number;
			Number = 0;
		}

		ByteIndex = 0;
		for ( std::uint32_t Index = 0; Index < NumberOncePositionLookupTable.size(); ++Index )
		{
			if ( ByteIndex + 3 < NumberOnces.size() )
			{
				Number = static_cast<std::uint32_t>( NumberOnces[ ByteIndex ] ) << 24
					| static_cast<std::uint32_t>( NumberOnces[ ByteIndex + 1 ] ) << 16
					| static_cast<std::uint32_t>( NumberOnces[ ByteIndex + 2 ] ) << 8
					| static_cast<std::uint32_t>( NumberOnces[ ByteIndex + 3 ] );
				ByteIndex += 4;
			}

			this->State[ NumberOncePositionLookupTable[ Index ] ] = Number;
			Number = 0;
		}
	}

	void Chacha20::QuarterRound(std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d)
	{
		//Twilight-Dream's superficial understanding of the role of the basic operations needed for the QuarterRound function of the Chacha algorithm.
		//Step1: Nonlinear function (Modulo Addition)
		//Step2: Bit confusion (Bitwise XOR)
		//Step3: Bit diffusion (Bitwise left rotation)

		/*
			The reason for choosing these four rotation distances (16, 12, 8, 7) is the same as this Salsa20 algorithm. 
			
			https://cr.yp.to/chacha/chacha-20080128.pdf
			The above code also shows a much less important difference between ChaCha20 and Salsa20:
			I changed the rotation distances 7, 9, 13, 18 to 16, 12, 8, 7. 
			The difference in security appears to be negligible: 7, 9, 13, 18 appears marginally better in some diffusion measures, and 16, 12, 8, 7 appears marginally better in others, but the margins are tiny, far smaller than the presumed inaccuracy of the diffusion measures as predictors of security. 
			The change boosts speed slightly onsome platforms while making no difference on other platforms.
		*/

		a += b; d ^= a; d = std::rotl(d, 16);
		c += d; b ^= c; b = std::rotl(b, 12);
		
		a += b; d ^= a; d = std::rotl(d, 8);
		c += d; b ^= c; b = std::rotl(b, 7);
	}

	void Chacha20::UpdateState(std::array<std::uint32_t, 16>& WorkingState)
	{
		//StateCopy = State
		std::ranges::copy(this->State.begin(), this->State.end(), WorkingState.begin());

		//Round function
		//State' = Function(StateCopy)
		for (std::uint32_t Round = 0; Round < 20; Round += 2)
		{
			// Odd round
			this->QuarterRound(WorkingState[0], WorkingState[4], WorkingState[ 8], WorkingState[12]); // column 0
			this->QuarterRound(WorkingState[1], WorkingState[5], WorkingState[ 9], WorkingState[13]); // column 1
			this->QuarterRound(WorkingState[2], WorkingState[6], WorkingState[10], WorkingState[14]); // column 2
			this->QuarterRound(WorkingState[3], WorkingState[7], WorkingState[11], WorkingState[15]); // column 3
			
			// Even round
			this->QuarterRound(WorkingState[0], WorkingState[5], WorkingState[10], WorkingState[15]); // diagonal 1 (main diagonal)
			this->QuarterRound(WorkingState[1], WorkingState[6], WorkingState[11], WorkingState[12]); // diagonal 2
			this->QuarterRound(WorkingState[2], WorkingState[7], WorkingState[ 8], WorkingState[13]); // diagonal 3
			this->QuarterRound(WorkingState[3], WorkingState[4], WorkingState[ 9], WorkingState[14]); // diagonal 4
		}

		//Mixing the data of the working state with the data of the state is equivalent to a secure hash function
		//Hashed = Hash(State', State)
		//Hashed = State' + State
		//Hashed = Function(StateCopy) + State
		for (std::uint32_t Index = 0; Index < 16; ++Index)
			WorkingState[Index] += this->State[Index];

		//Update Counter
		std::span<std::uint32_t> Counters {this->State.begin() + 12, this->State.begin() + 13};
		IncrementCounter(Counters);
	}

	std::vector<std::uint8_t> Chacha20::GenerateKeyStream(std::uint64_t Count)
	{
		std::vector<std::uint8_t> KeyStream(Count, 0x00);
		std::array<std::uint32_t, 16> WorkingState {};
		
		for ( std::uint64_t CurrentCount = 0; CurrentCount < Count; CurrentCount += 64 )
		{
			this->UpdateState( WorkingState );

			// Convert working state to key stream bytes
			for ( std::uint32_t WorkingStateIndex = 0; WorkingStateIndex < 16; ++WorkingStateIndex )
			{
				std::uint32_t Value = WorkingState[ WorkingStateIndex ];
				for ( std::uint32_t ByteIndex = 0; ByteIndex < 4 && ( CurrentCount + WorkingStateIndex * 4 + ByteIndex ) < Count; ++ByteIndex )
				{
					KeyStream[ CurrentCount + WorkingStateIndex * 4 + ByteIndex ] = static_cast<std::uint8_t>( Value & 0xFF );
					Value >>= 8;
				}
			}
		}

		return KeyStream;
	}
}