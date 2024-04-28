#pragma once

#include "../MainProgram/Support+Library/Support-Library.hpp"

namespace BlockDataCryption::TinyEncryptionAlgorithmGroup
{
	class TEA
	{
	private:
		// Constant used for the encryption and decryption process
		// A key schedule constant
		const uint32_t DELTA = 0x9e3779b9;

	public:
		// Encrypts the given data using the TEA algorithm
		void encrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] );

		// Decrypts the given data using the TEA algorithm
		void decrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] );
	};

	class XTEA
	{
	private:
		// Constant used for the encryption and decryption process
		const uint32_t DELTA = 0x9e3779b9;

	public:
		// Encrypts the given data using the XTEA algorithm
		void encrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] );

		// Decrypts the given data using the XTEA algorithm
		void decrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] );
	};

	class XXTEA
	{
	private:
		// Constant used for the encryption and decryption process
		const uint32_t DELTA = 0x9e3779b9;

		// Helper function to compute the mixed result based on the current state
		inline uint32_t computeMX(uint32_t previousBlock, uint32_t currentBlock, uint32_t sum, const uint32_t* key, unsigned blockIndex, uint32_t keySelector);

	public:
		// Encrypts the given data using the XXTEA algorithm
		void encrypt( uint32_t* data, int length, uint32_t const key[ 4 ] );

		// Decrypts the given data using the XXTEA algorithm
		void decrypt( uint32_t* data, int length, uint32_t const key[ 4 ] );
	};

	inline void UnitTest()
	{
		TEA	  tea;
		XTEA  xtea;
		XXTEA xxtea;

		uint32_t data[ 2 ] = { 0x12345678, 0x9ABCDEF0 };						 // Sample data for all algorithms
		uint32_t key[ 4 ] = { 0x01234567, 0x89ABCDEF, 0xFEDCBA98, 0x76543210 };	 // Sample key for all algorithms

		// TEA Test
		std::cout << "Original Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl;
		std::cout << "Key Data: " << key[ 0 ] << ", " << key[ 1 ] << key[ 2 ] << ", " << key[ 3 ] << std::endl;
		tea.encrypt( data, key );
		std::cout << "TEA Encrypted Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl;
		tea.decrypt( data, key );
		std::cout << "TEA Decrypted Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl << std::endl;

		// Reset data for XTEA test
		data[ 0 ] = 0x12345678;
		data[ 1 ] = 0x9ABCDEF0;

		// XTEA Test
		std::cout << "Original Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl;
		std::cout << "Key Data: " << key[ 0 ] << ", " << key[ 1 ] << key[ 2 ] << ", " << key[ 3 ] << std::endl;
		xtea.encrypt( data, key );
		std::cout << "XTEA Encrypted Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl;
		xtea.decrypt( data, key );
		std::cout << "XTEA Decrypted Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl << std::endl;

		// Reset data for XXTEA test
		data[ 0 ] = 0x12345678;
		data[ 1 ] = 0x9ABCDEF0;

		// XXTEA Test
		std::cout << "Original Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl;
		std::cout << "Key Data: " << key[ 0 ] << ", " << key[ 1 ] << key[ 2 ] << ", " << key[ 3 ] << std::endl;
		xxtea.encrypt( data, 2, key );
		std::cout << "XXTEA Encrypted Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl;
		xxtea.decrypt( data, 2, key );
		std::cout << "XXTEA Decrypted Data: " << data[ 0 ] << ", " << data[ 1 ] << std::endl;
	}
}