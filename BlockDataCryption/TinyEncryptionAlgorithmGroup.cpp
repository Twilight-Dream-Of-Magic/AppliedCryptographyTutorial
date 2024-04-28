#include "TinyEncryptionAlgorithmGroup.h"

namespace BlockDataCryption::TinyEncryptionAlgorithmGroup
{
	void TEA::encrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] )
	{
		uint32_t leftBlock = data[ 0 ], rightBlock = data[ 1 ], sum = 0;

		// Main encryption loop (32 rounds)
		for ( int round = 0; round < 32; round++ )
		{
			sum += DELTA;
			leftBlock += ( ( rightBlock << 4 ) + key[ 0 ] ) ^ ( rightBlock + sum ) ^ ( ( rightBlock >> 5 ) + key[ 1 ] );
			rightBlock += ( ( leftBlock << 4 ) + key[ 2 ] ) ^ ( leftBlock + sum ) ^ ( ( leftBlock >> 5 ) + key[ 3 ] );
		}

		data[ 0 ] = leftBlock;
		data[ 1 ] = rightBlock;
	}

	void TEA::decrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] )
	{
		//5 = log2(32): 2^{5} = 32
		//set up: sum is (delta << 5) & 0xFFFFFFFF
		uint32_t leftBlock = data[ 0 ], rightBlock = data[ 1 ], sum = 0xC6EF3720;

		// Main decryption loop (32 rounds)
		for ( int round = 0; round < 32; round++ )
		{
			rightBlock -= ( ( leftBlock << 4 ) + key[ 2 ] ) ^ ( leftBlock + sum ) ^ ( ( leftBlock >> 5 ) + key[ 3 ] );
			leftBlock -= ( ( rightBlock << 4 ) + key[ 0 ] ) ^ ( rightBlock + sum ) ^ ( ( rightBlock >> 5 ) + key[ 1 ] );
			sum -= DELTA;
		}

		data[ 0 ] = leftBlock;
		data[ 1 ] = rightBlock;
	}

	void XTEA::encrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] )
	{
		uint32_t leftBlock = data[ 0 ], rightBlock = data[ 1 ], sum = 0;

		// Main encryption loop (32 rounds)
		for ( int round = 0; round < 32; round++ )
		{
			leftBlock += ( ( ( rightBlock << 4 ) ^ ( rightBlock >> 5 ) ) + rightBlock ) ^ ( sum + key[ sum & 3 ] );
			sum += DELTA;
			rightBlock += ( ( ( leftBlock << 4 ) ^ ( leftBlock >> 5 ) ) + leftBlock ) ^ ( sum + key[ ( sum >> 11 ) & 3 ] );
		}

		data[ 0 ] = leftBlock;
		data[ 1 ] = rightBlock;
	}

	void XTEA::decrypt( uint32_t data[ 2 ], const uint32_t key[ 4 ] )
	{
		uint32_t leftBlock = data[ 0 ], rightBlock = data[ 1 ], sum = 0xC6EF3720;

		// Main decryption loop (32 rounds)
		for ( int round = 0; round < 32; round++ )
		{
			rightBlock -= ( ( ( leftBlock << 4 ) ^ ( leftBlock >> 5 ) ) + leftBlock ) ^ ( sum + key[ ( sum >> 11 ) & 3 ] );
			sum -= DELTA;
			leftBlock -= ( ( ( rightBlock << 4 ) ^ ( rightBlock >> 5 ) ) + rightBlock ) ^ ( sum + key[ sum & 3 ] );
		}

		data[ 0 ] = leftBlock;
		data[ 1 ] = rightBlock;
	}

	uint32_t XXTEA::computeMX( uint32_t RightBlock, uint32_t LeftBlock, uint32_t sum, const uint32_t* key, unsigned blockIndex, uint32_t keySelector )
	{
		// Splitting the formula for clarity:
		uint32_t part1 = (RightBlock >> 5) ^ (LeftBlock << 2);
		uint32_t part2 = (LeftBlock >> 3) ^ (RightBlock << 4);
		uint32_t part3 = sum ^ LeftBlock;
		uint32_t part4 = key[(blockIndex & 3) ^ keySelector] ^ RightBlock;

		return (part1 + part2) ^ (part3 + part4);
	}

	void XXTEA::encrypt(uint32_t* data, int length, uint32_t const key[4])
	{
		uint32_t RightBlock, LeftBlock, sum;
		uint32_t blockIndex, rounds, keySelector;

		//$[\text{round} \div X\omega] + 1$
		rounds = 6 + 52 / length;
		//$\Delta_{0} = 1 \times 0x9E3779B9$
		sum = 0;
		RightBlock = data[length - 1];  // This is the lastBlock for the first iteration

		do {
			//$\Delta_{i + 1} = \Delta_{i} + 0x9E3779B9$
			sum += DELTA;
			//$KeySelector_{i} = \Delta_{i} \gg 2$
			keySelector = (sum >> 2) & 3;

			for (blockIndex = 0; blockIndex < length - 1; blockIndex++) {
				//The `data[blockIndex]` is $X_{round}$
				//The `LeftBlock` is $X_{round + 1}$
				//The `RightBlock` is $X_{round - 1}$
				LeftBlock = data[blockIndex + 1];
				RightBlock = data[blockIndex] += computeMX(RightBlock, LeftBlock, sum, key, blockIndex, keySelector);
			}

			LeftBlock = data[0];  // This is the firstBlock for the next iteration
			RightBlock = data[length - 1] += computeMX(RightBlock, LeftBlock, sum, key, blockIndex, keySelector);

		} while (--rounds);
	}

	void XXTEA::decrypt(uint32_t* data, int length, uint32_t const key[4])
	{
		uint32_t LeftBlock, RightBlock, sum;
		uint32_t blockIndex, rounds, keySelector;

		//$[\text{round} \div X\omega] + 1$
		rounds = 6 + 52 / length;
		//$\Delta_{\text{round} - 1} = \text{round} \times 0x9E3779B9$
		sum = rounds * DELTA;
		LeftBlock = data[0];  // This is the firstBlock for the first iteration

		do {
			//$KeySelector_{i} = \Delta_{i} \gg 2$
			keySelector = (sum >> 2) & 3;

			for (blockIndex = length - 1; blockIndex > 0; blockIndex--) {
				//The `data[blockIndex]` is $X_{round}$
				//The `RightBlock` is $X_{round + 1}$
				//The `LeftBlock` is $X_{round - 1}$
				RightBlock = data[blockIndex - 1];
				LeftBlock = data[blockIndex] -= computeMX(RightBlock, LeftBlock, sum, key, blockIndex, keySelector);
			}

			RightBlock = data[length - 1];  // This is the lastBlock for the next iteration
			LeftBlock = data[0] -= computeMX(RightBlock, LeftBlock, sum, key, blockIndex, keySelector);
			
			//$\Delta_{i - 1} = \Delta_{i} - 0x9E3779B9$
			sum -= DELTA;

		} while (--rounds);
	}
} // namespace BlockDataCryption::TinyEncryptionAlgorithmGroup