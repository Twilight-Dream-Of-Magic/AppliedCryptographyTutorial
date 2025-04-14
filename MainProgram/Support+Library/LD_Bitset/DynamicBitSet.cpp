#include "DynamicBitSet.hpp"

namespace TwilightDream
{
	/* DynamicBitSet Iterators */

	DynamicBitSet::iterator DynamicBitSet::begin()
	{
		return iterator(true, &this->bitset, this->valid_number_of_bits());
	}

	DynamicBitSet::iterator DynamicBitSet::end()
	{
		return iterator(false, &this->bitset, this->valid_number_of_bits());
	}

	DynamicBitSet::const_iterator DynamicBitSet::cbegin() const
	{
		std::cerr << "Warning: Using a const DynamicBitSet with iterator is discouraged." << std::endl;
		return const_iterator(true, &this->bitset, this->valid_number_of_bits());
	}

	DynamicBitSet::const_iterator DynamicBitSet::cend() const
	{
		std::cerr << "Warning: Using a const DynamicBitSet with iterator is discouraged." << std::endl;
		return const_iterator(false, &this->bitset, this->valid_number_of_bits());
	}

	DynamicBitSet::reverse_iterator DynamicBitSet::rbegin()
	{
		return reverse_iterator(true, &this->bitset, this->valid_number_of_bits());
	}

	DynamicBitSet::reverse_iterator DynamicBitSet::rend()
	{
		return reverse_iterator(false, &this->bitset, this->valid_number_of_bits());
	}

	DynamicBitSet::const_reverse_iterator DynamicBitSet::crbegin() const
	{
		std::cerr << "Warning: Using a const DynamicBitSet with iterator is discouraged." << std::endl;
		return const_reverse_iterator(true, &this->bitset, this->valid_number_of_bits());
	}

	DynamicBitSet::const_reverse_iterator DynamicBitSet::crend() const
	{
		std::cerr << "Warning: Using a const DynamicBitSet with iterator is discouraged." << std::endl;
		return const_reverse_iterator(false, &this->bitset, this->valid_number_of_bits());
	}

	// Subscript Operator for non-const DynamicBitSet
	BitReference DynamicBitSet::operator[]( size_t index )
	{
		if ( index >= this->data_size )
			throw std::out_of_range( "Index out of range" );

		return BitReference( &(this->bitset[index / 32]), uint32_t(1) << index % 32 );
	}

	// Subscript Operator for non-const DynamicBitSet
	bool DynamicBitSet::operator[]( size_t index ) const
	{
		if ( index >= this->data_size )
			throw std::out_of_range( "Index out of range" );

		return this->bitset[ index / 32 ].bit_get( index % 32 );
	}

	// TODO: Is there a more efficient way to do this than converting to a binary string?
	// Bitwise left rotation (<<<=)
	void DynamicBitSet::rotate_left( size_t shift )
	{
		if ( shift == 0 || bitset.size() * 32 == shift || bitset.empty())
		{
			return;
		}

		if( shift >= 256 )
		{
			std::vector<bool> buffer = this->bit_vector_data();
			std::rotate(buffer.begin(), buffer.begin() + shift % buffer.size(), buffer.end());
			*this = DynamicBitSet(buffer);
			return;
		}

		DynamicBitSet left = *this;
		DynamicBitSet right = *this;
		left <<= shift;
		right >>= (bitset.size() * 32 - shift);
		*this = left | right;

		this->clear_leading_bit_zeros(false);

		data_size = this->valid_number_of_bits();
		data_capacity = this->bitset.size() * 32;
		data_chunk_count = this->bitset.size();
	}

	// TODO: Is there a more efficient way to do this than converting to a binary string?
	// Bitwise right rotation (>>>=)
	void DynamicBitSet::rotate_right( size_t shift )
	{
		if ( shift == 0 || bitset.size() * 32 == shift || bitset.empty() )
		{
			return;
		}

		if( shift >= 256 )
		{
			std::vector<bool> buffer = this->bit_vector_data();
			std::rotate(buffer.rbegin(), buffer.rbegin() + shift % buffer.size(), buffer.rend());
			*this = DynamicBitSet(buffer);
			return;
		}

		DynamicBitSet left = *this;
		DynamicBitSet right = *this;
		left >>= shift;
		right <<= (bitset.size() * 32 - shift);
		*this = left | right;

		this->clear_leading_bit_zeros(false);

		data_size = this->valid_number_of_bits();
		data_capacity = this->bitset.size() * 32;
		data_chunk_count = this->bitset.size();
	}

}  // namespace TwilightDream