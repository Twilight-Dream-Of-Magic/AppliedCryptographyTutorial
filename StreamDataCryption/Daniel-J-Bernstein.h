#pragma once

#include "../MainProgram/Support+Library/Support-Library.hpp"

namespace StreamDataCryption::DJB
{
	template <typename Type>
	requires std::unsigned_integral<Type>
	void IncrementCounter( std::span<Type> counter )
	{
		std::size_t index = counter.size() - 1;
		while ( index < counter.size() )
		{
			if ( counter[ index ] != std::numeric_limits<Type>::max() )
			{
				++counter[ index ];
				break;
			}
			else
			{
				counter[ index ] = std::numeric_limits<Type>::min();
				if ( index == 0 )
				{
					break;
				}
				--index;
			}
		}
	}

	template <typename Type>
	requires std::unsigned_integral<Type>
	void DecrementCounter( std::span<Type> counter )
	{
		std::size_t index = counter.size() - 1;
		while ( index < counter.size() )
		{
			if ( counter[ index ] != std::numeric_limits<Type>::min() )
			{
				--counter[ index ];
				break;
			}
			else
			{
				counter[ index ] = std::numeric_limits<Type>::max();
				if ( index == 0 )
				{
					break;
				}
				--index;
			}
		}
	}

	class Salsa20
	{
	private:
		/*
			Constant values: is "expand 32-byte k"
			State[0], State[5], State[10], State[15]
		*/
		std::array<std::uint32_t, 16> State
		{
			0x61707865, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x3320646E, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x79622D32, 0x00000000,
			0x00000000, 0x00000000, 0x00000000, 0x6B206574
		};

		std::array<std::uint32_t, 8> Keys;
		std::array<std::uint32_t, 2> NumberOnces;

		void QuarterRound( std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d );
		void UpdateState( std::array<std::uint32_t, 16>& WorkingState );

	public:
		void					  InitializeState( std::span<std::uint8_t> Keys, std::span<std::uint8_t> NumberOnces );
		std::vector<std::uint8_t> GenerateKeyStream( std::uint64_t Count );
		void WeakerTest()
		{
			std::array<std::uint32_t, 16> TestState
			{
				0x00000001, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000
			};

			std::array<std::uint32_t, 16> WorkingState
			{
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000
			};

			auto QuarterRoundTest = [](std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d) -> void
			{
				b = (a + d);
				c = (b + a);
				d = (c + b);
				a = (d + c);
			};

			auto PrintTestData = [&WorkingState]() ->void
			{
				// Print the state matrix in binary format after each 1/4 round function

				std::cout << "{\n";
				for ( size_t i = 0; i < 16; ++i )
				{
					std::cout << std::bitset<32>(WorkingState[ i ]).to_string();
					if ( i % 4 == 3 )
					{
						std::cout << ",\n";
					}
					else
					{
						std::cout << ", ";
					}
				}
				std::cout << "}\n";
			};

			//StateCopy = State
			std::ranges::copy(TestState.begin(), TestState.end(), WorkingState.begin());

			std::cout << "Weaker test with use Salsa20:" << std::endl;

			//Round function
			//State' = Function(StateCopy)
			for ( std::uint32_t Round = 0; Round < 20; Round += 2 )
			{
				// Odd round
				PrintTestData();
				QuarterRoundTest( WorkingState[ 0 ], WorkingState[ 4 ], WorkingState[ 8 ], WorkingState[ 12 ] );   // column 1
				PrintTestData();
				QuarterRoundTest( WorkingState[ 5 ], WorkingState[ 9 ], WorkingState[ 13 ], WorkingState[ 1 ] );   // column 2
				PrintTestData();
				QuarterRoundTest( WorkingState[ 10 ], WorkingState[ 14 ], WorkingState[ 2 ], WorkingState[ 6 ] );  // column 3
				PrintTestData();
				QuarterRoundTest( WorkingState[ 15 ], WorkingState[ 3 ], WorkingState[ 7 ], WorkingState[ 11 ] );  // column 4

				// Even round
				PrintTestData();
				QuarterRoundTest( WorkingState[ 0 ], WorkingState[ 1 ], WorkingState[ 2 ], WorkingState[ 3 ] );		 // row 1
				PrintTestData();
				QuarterRoundTest( WorkingState[ 5 ], WorkingState[ 6 ], WorkingState[ 7 ], WorkingState[ 4 ] );		 // row 2
				PrintTestData();
				QuarterRoundTest( WorkingState[ 10 ], WorkingState[ 11 ], WorkingState[ 8 ], WorkingState[ 9 ] );	 // row 3
				PrintTestData();
				QuarterRoundTest( WorkingState[ 15 ], WorkingState[ 12 ], WorkingState[ 13 ], WorkingState[ 14 ] );	 // row 4
				PrintTestData();
			}

			//Mixing the data of the working state with the data of the state is equivalent to a secure hash function
			//Hashed = Hash(State', State)
			//Hashed = State' + State
			//Hashed = Function(StateCopy) + State
			for ( std::uint32_t Index = 0; Index < 16; ++Index )
				WorkingState[ Index ] += TestState[ Index ];
		}

		Salsa20() = default;
		explicit Salsa20( std::span<std::uint8_t> Keys, std::span<std::uint8_t> NumberOnces )
		{
			this->InitializeState( Keys, NumberOnces );
		}
	};

	class Chacha20
	{

	private:
		/*
			Constant values: is "expand 32-byte k"
			State[0], State[1], State[2], State[3]
		*/
		std::array<std::uint32_t, 16> State
		{
			0x61707865, 0x3320646E, 0x79622D32, 0x6B206574,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x00000000, 0x00000000,
			0x00000000, 0x00000000, 0x00000000, 0x00000000
		};

		std::array<std::uint32_t, 8> Keys;
		std::array<std::uint32_t, 2> NumberOnces;

		void QuarterRound( std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d );
		void UpdateState( std::array<std::uint32_t, 16>& WorkingState );

	public:
		void					  InitializeState( std::span<std::uint8_t> Keys, std::span<std::uint8_t> NumberOnces );
		std::vector<std::uint8_t> GenerateKeyStream( std::uint64_t Count );
		void WeakerTest()
		{
			std::array<std::uint32_t, 16> TestState
			{
				0x00000001, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000
			};

			std::array<std::uint32_t, 16> WorkingState
			{
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000,
				0x00000000, 0x00000000, 0x00000000, 0x00000000
			};

			auto QuarterRoundTest = [](std::uint32_t& a, std::uint32_t& b, std::uint32_t& c, std::uint32_t& d) -> void
			{
				a += b; d = a;
				c += d; b = c;
		
				a += b; d = a;
				c += d; b = c;
			};

			auto PrintTestData = [&WorkingState]() ->void
			{
				// Print the state matrix in binary format after each 1/4 round function

				std::cout << "{\n";
				for ( size_t i = 0; i < 16; ++i )
				{
					std::cout << std::bitset<32>(WorkingState[ i ]).to_string();
					if ( i % 4 == 3 )
					{
						std::cout << ",\n";
					}
					else
					{
						std::cout << ", ";
					}
				}
				std::cout << "}\n";
			};

			//StateCopy = State
			std::ranges::copy(TestState.begin(), TestState.end(), WorkingState.begin());

			std::cout << "Weaker test with use Chacha20:" << std::endl;

			//Round function
			//State' = Function(StateCopy)
			for ( std::uint32_t Round = 0; Round < 20; Round += 2 )
			{
				// Odd round
				PrintTestData();
				QuarterRoundTest( WorkingState[ 0 ], WorkingState[ 4 ], WorkingState[ 8 ], WorkingState[ 12 ] );   // column 0
				PrintTestData();
				QuarterRoundTest( WorkingState[ 1 ], WorkingState[ 5 ], WorkingState[ 9 ], WorkingState[ 13 ] );   // column 1
				PrintTestData();
				QuarterRoundTest( WorkingState[ 2 ], WorkingState[ 6 ], WorkingState[ 10 ], WorkingState[ 14 ] );  // column 2
				PrintTestData();
				QuarterRoundTest( WorkingState[ 3 ], WorkingState[ 7 ], WorkingState[ 11 ], WorkingState[ 15 ] );  // column 3
				PrintTestData();

				// Even round
				PrintTestData();
				QuarterRoundTest( WorkingState[ 0 ], WorkingState[ 5 ], WorkingState[ 10 ], WorkingState[ 15 ] );  // diagonal 1 (main diagonal)
				PrintTestData();
				QuarterRoundTest( WorkingState[ 1 ], WorkingState[ 6 ], WorkingState[ 11 ], WorkingState[ 12 ] );  // diagonal 2
				PrintTestData();
				QuarterRoundTest( WorkingState[ 2 ], WorkingState[ 7 ], WorkingState[ 8 ], WorkingState[ 13 ] );   // diagonal 3
				PrintTestData();
				QuarterRoundTest( WorkingState[ 3 ], WorkingState[ 4 ], WorkingState[ 9 ], WorkingState[ 14 ] );   // diagonal 4
				PrintTestData();
			}

			//Mixing the data of the working state with the data of the state is equivalent to a secure hash function
			//Hashed = Hash(State', State)
			//Hashed = State' + State
			//Hashed = Function(StateCopy) + State
			for ( std::uint32_t Index = 0; Index < 16; ++Index )
				WorkingState[ Index ] += TestState[ Index ];
		}

		Chacha20() = default;
		explicit Chacha20( std::span<std::uint8_t> Keys, std::span<std::uint8_t> NumberOnces )
		{
			this->InitializeState( Keys, NumberOnces );
		}
	};

}  // namespace StreamDataCryption::DJB
