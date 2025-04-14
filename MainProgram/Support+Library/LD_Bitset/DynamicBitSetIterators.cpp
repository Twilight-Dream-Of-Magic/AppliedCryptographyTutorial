#include "DynamicBitSetIterators.hpp"

namespace TwilightDream
{
	void BitIteratorBaseData::offset_up()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot increment a read-only end iterator.");
		}

		if (is_after_end)
		{
			throw std::logic_error("It is not possible to increment the bit position after the end position of the forward iterator to cause an overflow.");
		}

		if (is_end_bit_position())
		{
			is_after_end = true;
			return;
		}

		if((bit_position + 1) < max_valid_bits)
			bit_position++;
		else
			bit_position = max_valid_bits - 1;

		
		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void BitIteratorBaseData::offset_down()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot decrement a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to decrement the bit position to before the start of the forward iterator to cause an underflow.");
		}

		if (is_after_end)
		{
			is_after_end = false;
			bit_position = max_valid_bits - 1;
			return;
		}

		if((bit_position - 1) >= 0)
			bit_position--;
		else
			bit_position = 0;

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	bool BitIteratorBaseData::is_read_only_end_iterator() const
	{
		return is_read_only && is_after_end;
	}

	//bit_position is 0?
	bool BitIteratorBaseData::is_start_bit_position() const
	{
		return bit_position == 0 && data_pointer == &((*container_pointer)[0]);
	}

	//bit_position is max_valid_bits - 1?
	bool BitIteratorBaseData::is_end_bit_position() const
	{
		return bit_position == max_valid_bits - 1 && data_pointer == &((*container_pointer)[container_pointer->size() - 1]);
	}

	void BitIteratorBaseData::advance(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			retreat(-n);
			return;
		}
		
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot advance a read-only end iterator.");
		}

		if(is_after_end && is_end_bit_position())
		{
			throw std::logic_error("It is not possible to advance the bit position after the end position of the forward iterator to cause an overflow.");
		}

		if((bit_position + n) == max_valid_bits)
		{
			bit_position = max_valid_bits - 1;
			is_after_end = true;
		}
		else if((bit_position + n) < max_valid_bits)
		{
			bit_position += n;
			is_after_end = false;
		}
		else
		{
			bit_position = max_valid_bits - 1;
			is_after_end = true;
			throw std::logic_error("It is not possible to advance the bit position after the end position of the forward iterator to cause an overflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void BitIteratorBaseData::retreat(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			advance(-n);
			return;
		}
		
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot retreat a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to retreat the bit position to before the start of the forward iterator to cause an underflow.");
		}

		if(is_after_end && is_end_bit_position() && n < max_valid_bits)
		{
			bit_position -= (n - 1);
			is_after_end = false;
			data_pointer = &((*container_pointer)[bit_position / 32]);
			return;
		}

		if(bit_position > n && ((bit_position - n) == 0))
		{
			bit_position = 0;
			is_after_end = false;
		}
		else if(bit_position >= n && ((bit_position - n) >= 0))
		{
			bit_position -= n;
			is_after_end = false;
		}
		else
		{
			bit_position = 0;
			is_after_end = false;
			throw std::logic_error("It is not possible to retreat the bit position to before the start of the forward iterator to cause an underflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	/******************************/

	void ReverseBitIteratorBaseData::offset_up()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot increment a read-only end iterator.");
		}

		if (is_before_begin)
		{
			throw std::logic_error("It is not possible to increment the bit position before the start position of the reverse iterator to cause an overflow.");
		}

		if (is_end_bit_position())
		{
			is_before_begin = true;
			return;
		}

		if((bit_position - 1) >= 0)
			bit_position--;
		else
			bit_position = 0;

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void ReverseBitIteratorBaseData::offset_down()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot decrement a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to decrement the bit position after the end position of the reverse iterator to cause an underflow.");
		}

		if (is_before_begin)
		{
			is_before_begin = false;
			bit_position = 0;
			return;
		}

		if((bit_position + 1) < max_valid_bits)
			bit_position++;
		else
			bit_position = max_valid_bits - 1;

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	bool ReverseBitIteratorBaseData::is_read_only_end_iterator() const
	{
		return is_read_only && is_before_begin;
	}

	//bit_position is max_valid_bits - 1?
	bool ReverseBitIteratorBaseData::is_start_bit_position() const
	{
		return bit_position == max_valid_bits - 1 && data_pointer == &((*container_pointer)[container_pointer->size() - 1]);
	}

	//bit_position is 0?
	bool ReverseBitIteratorBaseData::is_end_bit_position() const
	{
		return bit_position == 0 && data_pointer == &((*container_pointer)[0]);
	}

	void ReverseBitIteratorBaseData::advance(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			retreat(-n);
			return;
		}

		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot advance a read-only end iterator.");
		}

		if (is_before_begin && is_end_bit_position())
		{
			throw std::logic_error("It is not possible to advance the bit position before the start position of the reverse iterator to cause an overflow.");
		}

		if(bit_position < n && ((bit_position - n) == static_cast<size_t>(-1)))
		{
			bit_position = 0;
			is_before_begin = true;
		}
		else if(bit_position >= n && ((bit_position - n) >= 0))
		{
			bit_position -= n;
			is_before_begin = false;
		}
		else
		{
			bit_position = 0;
			is_before_begin = true;
			throw std::logic_error("It is not possible to advance the bit position before the start position of the reverse iterator to cause an overflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void ReverseBitIteratorBaseData::retreat(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			advance(-n);
			return;
		}

		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot retreat a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to retreat the bit position after the end position of the reverse iterator to cause an underflow.");
		}

		if (is_before_begin && is_end_bit_position() && n < max_valid_bits)
		{
			bit_position += (n - 1);
			is_before_begin = false;
			data_pointer = &((*container_pointer)[bit_position / 32]);
			return;
		}

		if((bit_position + n) == max_valid_bits - 1)
		{
			bit_position = max_valid_bits - 1;
			is_before_begin = false;
		}
		else if((bit_position + n) < max_valid_bits)
		{
			bit_position += n;
			is_before_begin = false;
		}
		else
		{
			bit_position = max_valid_bits - 1;
			is_before_begin = false;
			throw std::logic_error("It is not possible to retreat the bit position after the end position of the reverse iterator to cause an underflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	/******************************/

	void ConstantBitIteratorBaseData::offset_up()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot increment a read-only end iterator.");
		}

		if (is_after_end)
		{
			throw std::logic_error("It is not possible to increment the bit position after the end position of the forward iterator to cause an overflow.");
		}

		if (is_end_bit_position())
		{
			is_after_end = true;
			return;
		}

		if((bit_position + 1) < max_valid_bits)
			bit_position++;
		else
			bit_position = max_valid_bits - 1;

		
		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void ConstantBitIteratorBaseData::offset_down()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot decrement a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to decrement the bit position to before the start of the forward iterator to cause an underflow.");
		}

		if (is_after_end)
		{
			is_after_end = false;
			bit_position = max_valid_bits - 1;
			return;
		}

		if((bit_position - 1) >= 0)
			bit_position--;
		else
			bit_position = 0;

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	bool ConstantBitIteratorBaseData::is_read_only_end_iterator() const
	{
		return is_read_only && is_after_end;
	}

	//bit_position is 0?
	bool ConstantBitIteratorBaseData::is_start_bit_position() const
	{
		return bit_position == 0 && data_pointer == &((*container_pointer)[0]);
	}

	//bit_position is max_valid_bits - 1?
	bool ConstantBitIteratorBaseData::is_end_bit_position() const
	{
		return bit_position == max_valid_bits - 1 && data_pointer == &((*container_pointer)[container_pointer->size() - 1]);
	}

	void ConstantBitIteratorBaseData::advance(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			retreat(-n);
			return;
		}
		
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot advance a read-only end iterator.");
		}

		if(is_after_end && is_end_bit_position())
		{
			throw std::logic_error("It is not possible to advance the bit position after the end position of the forward iterator to cause an overflow.");
		}

		if((bit_position + n) == max_valid_bits)
		{
			bit_position = max_valid_bits - 1;
			is_after_end = true;
		}
		else if((bit_position + n) < max_valid_bits)
		{
			bit_position += n;
			is_after_end = false;
		}
		else
		{
			bit_position = max_valid_bits - 1;
			is_after_end = true;
			throw std::logic_error("It is not possible to advance the bit position after the end position of the forward iterator to cause an overflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void ConstantBitIteratorBaseData::retreat(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			advance(-n);
			return;
		}
		
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot retreat a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to retreat the bit position to before the start of the forward iterator to cause an underflow.");
		}

		if(is_after_end && is_end_bit_position() && n < max_valid_bits)
		{
			bit_position -= (n - 1);
			is_after_end = false;
			data_pointer = &((*container_pointer)[bit_position / 32]);
			return;
		}

		if(bit_position > n && ((bit_position - n) == 0))
		{
			bit_position = 0;
			is_after_end = false;
		}
		else if(bit_position >= n && ((bit_position - n) >= 0))
		{
			bit_position -= n;
			is_after_end = false;
		}
		else
		{
			bit_position = 0;
			is_after_end = false;
			throw std::logic_error("It is not possible to retreat the bit position to before the start of the forward iterator to cause an underflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	/******************************/

	void ConstantReverseBitIteratorBaseData::offset_up()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot increment a read-only end iterator.");
		}

		if (is_before_begin)
		{
			throw std::logic_error("It is not possible to increment the bit position before the start position of the reverse iterator to cause an overflow.");
		}

		if (is_end_bit_position())
		{
			is_before_begin = true;
			return;
		}

		if((bit_position - 1) >= 0)
			bit_position--;
		else
			bit_position = 0;

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void ConstantReverseBitIteratorBaseData::offset_down()
	{
		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot decrement a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to decrement the bit position after the end position of the reverse iterator to cause an underflow.");
		}

		if (is_before_begin)
		{
			is_before_begin = false;
			bit_position = 0;
			return;
		}

		if((bit_position + 1) < max_valid_bits)
			bit_position++;
		else
			bit_position = max_valid_bits - 1;

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	bool ConstantReverseBitIteratorBaseData::is_read_only_end_iterator() const
	{
		return is_read_only && is_before_begin;
	}

	//bit_position is max_valid_bits - 1?
	bool ConstantReverseBitIteratorBaseData::is_start_bit_position() const
	{
		return bit_position == max_valid_bits - 1 && data_pointer == &((*container_pointer)[container_pointer->size() - 1]);
	}

	//bit_position is 0?
	bool ConstantReverseBitIteratorBaseData::is_end_bit_position() const
	{
		return bit_position == 0 && data_pointer == &((*container_pointer)[0]);
	}

	void ConstantReverseBitIteratorBaseData::advance(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			retreat(-n);
			return;
		}

		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot advance a read-only end iterator.");
		}

		if (is_before_begin && is_end_bit_position())
		{
			throw std::logic_error("It is not possible to advance the bit position before the start position of the reverse iterator to cause an overflow.");
		}

		if(bit_position < n && ((bit_position - n) == static_cast<size_t>(-1)))
		{
			bit_position = 0;
			is_before_begin = true;
		}
		else if(bit_position >= n && ((bit_position - n) >= 0))
		{
			bit_position -= n;
			is_before_begin = false;
		}
		else
		{
			bit_position = 0;
			is_before_begin = true;
			throw std::logic_error("It is not possible to advance the bit position before the start position of the reverse iterator to cause an overflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}

	void ConstantReverseBitIteratorBaseData::retreat(std::ptrdiff_t n)
	{
		if (n < 0)
		{
			advance(-n);
			return;
		}

		if (is_read_only_end_iterator())
		{
			throw std::logic_error("Cannot retreat a read-only end iterator.");
		}

		if (is_start_bit_position())
		{
			throw std::logic_error("It is not possible to retreat the bit position after the end position of the reverse iterator to cause an underflow.");
		}

		if (is_before_begin && is_end_bit_position() && n < max_valid_bits)
		{
			bit_position += (n - 1);
			is_before_begin = false;
			data_pointer = &((*container_pointer)[bit_position / 32]);
			return;
		}

		if((bit_position + n) == max_valid_bits - 1)
		{
			bit_position = max_valid_bits - 1;
			is_before_begin = false;
		}
		else if((bit_position + n) < max_valid_bits)
		{
			bit_position += n;
			is_before_begin = false;
		}
		else
		{
			bit_position = max_valid_bits - 1;
			is_before_begin = false;
			throw std::logic_error("It is not possible to retreat the bit position after the end position of the reverse iterator to cause an underflow.");
		}

		data_pointer = &((*container_pointer)[bit_position / 32]);
	}



	BitReference::BitReference() noexcept
		: data_pointer( nullptr ), bits_mask( 0 ) {}

	BitReference::BitReference(BooleanBitWrapper* wrapper_pointer, uint32_t bits_mask) 
		: data_pointer(&(wrapper_pointer->bits)), bits_mask(bits_mask)
	{
		if(wrapper_pointer == nullptr)
			throw std::runtime_error("BitReference::BitReference(): The pointer of the bit iterator points to the data that needs to be referenced and cannot be empty!");
	}

	BitReference::operator bool() const noexcept
	{
		return (*data_pointer & bits_mask) != 0;
	}

	BitReference& BitReference::operator=(bool value) noexcept
	{
		if (value)
			*data_pointer |= bits_mask;
		else
			*data_pointer &= ~bits_mask;
		return *this;
	}

	BitReference& BitReference::operator=(const BitReference& other) noexcept
	{
		return *this = static_cast<bool>(other);
	}

	BitReference& BitReference::operator^=(const BitReference& other) noexcept
	{
		if (static_cast<bool>(*this) != static_cast<bool>(other))
			*data_pointer |= bits_mask;
		else
			*data_pointer &= ~bits_mask;
		return *this;
	}

	BitReference& BitReference::operator&=(const BitReference& other) noexcept
	{
		if (static_cast<bool>(*this) && static_cast<bool>(other))
			*data_pointer |= bits_mask;
		else
			*data_pointer &= ~bits_mask;
		return *this;
	}

	BitReference& BitReference::operator|=(const BitReference& other) noexcept
	{
		if (static_cast<bool>(*this) || static_cast<bool>(other))
			*data_pointer |= bits_mask;
		else
			*data_pointer &= ~bits_mask;
		return *this;
	}

	bool BitReference::operator~() noexcept
	{
		return !static_cast<bool>(*this);
	}

	bool BitReference::operator==(const BitReference& other) const
	{
		return static_cast<bool>(*this) == static_cast<bool>(other);
	}

	bool BitReference::operator<(const BitReference& other) const
	{
		return !static_cast<bool>(*this) && static_cast<bool>(other);
	}

	bool BitReference::operator>(const BitReference& other) const
	{
		return static_cast<bool>(*this) && !static_cast<bool>(other);
	}

	bool BitReference::operator<=(const BitReference& other) const
	{
		return !(static_cast<bool>(*this) > static_cast<bool>(other));
	}

	bool BitReference::operator>=(const BitReference& other) const
	{
		return !(static_cast<bool>(*this) < static_cast<bool>(other));
	}

	void BitReference::flip() noexcept
	{
		*data_pointer ^= bits_mask;
	}

	/******************************/

	BitReference BitIterator::operator*()
	{
		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot dereference a read-only end iterator." );
		}
		return BitReference( data_pointer, uint32_t(1) << bit_position % 32 );
	}

	BitReference BitIterator::operator[]( std::ptrdiff_t offset )
	{
		BitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return *temp;
	}

	BitIterator BitIterator::operator+( std::ptrdiff_t offset ) const
	{
		BitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return temp;
	}

	BitIterator BitIterator::operator-( std::ptrdiff_t offset ) const
	{
		BitIterator temp = *this;
		temp.retreat( offset );	 // retreat内部已处理异常
		return temp;
	}

	BitIterator& BitIterator::operator+=( std::ptrdiff_t offset )
	{
		this->advance( offset );  // advance内部已处理异常
		return *this;
	}

	BitIterator& BitIterator::operator-=( std::ptrdiff_t offset )
	{
		this->retreat( offset );  // retreat内部已处理异常
		return *this;
	}

	BitIterator& BitIterator::operator++()
	{
		this->offset_up();	// offset_up内部已处理异常
		return *this;
	}

	BitIterator BitIterator::operator++( int )
	{
		BitIterator temp = *this;
		this->offset_up();	// offset_up内部已处理异常
		return temp;
	}

	BitIterator& BitIterator::operator--()
	{
		this->offset_down();  // offset_down内部已处理异常
		return *this;
	}

	BitIterator BitIterator::operator--( int )
	{
		BitIterator temp = *this;
		this->offset_down();  // offset_down内部已处理异常
		return temp;
	}

	bool BitIterator::operator==( const BitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		return bit_position == other.bit_position && is_after_end == other.is_after_end;
	}

	bool BitIterator::operator!=( const BitIterator& other ) const
	{
		return !( *this == other );
	}

	bool BitIterator::operator<( const BitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return !is_after_end && other.is_after_end;
		}
		return bit_position < other.bit_position;
	}

	bool BitIterator::operator>( const BitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return is_after_end && !other.is_after_end;
		}
		return bit_position > other.bit_position;
	}

	bool BitIterator::operator<=( const BitIterator& other ) const
	{
		return !( *this > other );
	}

	bool BitIterator::operator>=( const BitIterator& other ) const
	{
		return !( *this < other );
	}

	BitIterator& BitIterator::operator=( const BitIterator& other )
	{
		if ( this == &other )
		{
			return *this;  // Return *this to deal with self-assignment
		}

		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot assign to a read-only end iterator." );
		}

		if(container_pointer == nullptr)
			container_pointer == other.container_pointer;

		if ( container_pointer == other.container_pointer && max_valid_bits == other.max_valid_bits )
		{
			bit_position = other.bit_position;
			data_pointer = other.data_pointer;
			is_read_only = other.is_read_only;
			is_after_end = other.is_after_end;
		}
		else
		{
			throw std::logic_error( "The two containers are different and cannot be assigned." );
		}

		return *this;
	}

	std::ptrdiff_t operator-( const BitIterator& left, const BitIterator& right )
	{
		if ( left.container_pointer != right.container_pointer || left.max_valid_bits != right.max_valid_bits )
		{
			throw std::logic_error( "Computes the distance between iterators from different containers." );
		}

		if (left.bit_position == right.bit_position && left.is_after_end == right.is_after_end)
		{
			return 0;
		}

		size_t left_position = left.bit_position;
		size_t right_position = right.bit_position;

		if (left.is_after_end) {
			left_position = left.max_valid_bits;
		}

		if (right.is_after_end) {
			right_position = right.max_valid_bits;
		}

		size_t result = left_position > right_position ? left_position - right_position : right_position - left_position;
		return left_position > right_position ? static_cast<std::ptrdiff_t>(result) : -static_cast<std::ptrdiff_t>(result);
	}

	/******************************/

	BitReference ReverseBitIterator::operator*()
	{
		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot dereference a read-only end iterator." );
		}
		return BitReference( data_pointer, uint32_t(1) << bit_position % 32 );
	}

	BitReference ReverseBitIterator::operator[]( std::ptrdiff_t offset )
	{
		ReverseBitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return *temp;
	}

	ReverseBitIterator ReverseBitIterator::operator+( std::ptrdiff_t offset ) const
	{
		ReverseBitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return temp;
	}

	ReverseBitIterator ReverseBitIterator::operator-( std::ptrdiff_t offset ) const
	{
		ReverseBitIterator temp = *this;
		temp.retreat( offset );	 // retreat内部已处理异常
		return temp;
	}

	ReverseBitIterator& ReverseBitIterator::operator+=( std::ptrdiff_t offset )
	{
		this->advance( offset );  // advance内部已处理异常
		return *this;
	}

	ReverseBitIterator& ReverseBitIterator::operator-=( std::ptrdiff_t offset )
	{
		this->retreat( offset );  // retreat内部已处理异常
		return *this;
	}

	ReverseBitIterator& ReverseBitIterator::operator++()
	{
		this->offset_up();	// offset_up内部已处理异常
		return *this;
	}

	ReverseBitIterator ReverseBitIterator::operator++( int )
	{
		ReverseBitIterator temp = *this;
		this->offset_up();	// offset_up内部已处理异常
		return temp;
	}

	ReverseBitIterator& ReverseBitIterator::operator--()
	{
		this->offset_down();  // offset_down内部已处理异常
		return *this;
	}

	ReverseBitIterator ReverseBitIterator::operator--( int )
	{
		ReverseBitIterator temp = *this;
		this->offset_down();  // offset_down内部已处理异常
		return temp;
	}

	bool ReverseBitIterator::operator==( const ReverseBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		return bit_position == other.bit_position && is_before_begin == other.is_before_begin;
	}

	bool ReverseBitIterator::operator!=( const ReverseBitIterator& other ) const
	{
		return !( *this == other );
	}

	bool ReverseBitIterator::operator<( const ReverseBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return is_before_begin && !other.is_before_begin;
		}
		return bit_position > other.bit_position;  // Note the '>' because it's a reverse iterator
	}

	bool ReverseBitIterator::operator>( const ReverseBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return !is_before_begin && other.is_before_begin;
		}
		return bit_position < other.bit_position;  // Note the '<' because it's a reverse iterator
	}

	bool ReverseBitIterator::operator<=( const ReverseBitIterator& other ) const
	{
		return !( *this > other );;
	}

	bool ReverseBitIterator::operator>=( const ReverseBitIterator& other ) const
	{
		return !( *this < other );;
	}

	ReverseBitIterator& ReverseBitIterator::operator=( const ReverseBitIterator& other )
	{
		if ( this == &other )
		{
			return *this;  // Return *this to deal with self-assignment
		}

		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot assign to a read-only end iterator." );
		}

		if(container_pointer == nullptr)
			container_pointer == other.container_pointer;

		if ( container_pointer == other.container_pointer && max_valid_bits == other.max_valid_bits )
		{
			bit_position = other.bit_position;
			data_pointer = other.data_pointer;
			is_read_only = other.is_read_only;
			is_before_begin = other.is_before_begin;
		}
		else
		{
			throw std::logic_error( "The two containers are different and cannot be assigned." );
		}

		return *this;
	}

	std::ptrdiff_t operator-( const ReverseBitIterator& left, const ReverseBitIterator& right )
	{
		if ( left.container_pointer != right.container_pointer || left.max_valid_bits != right.max_valid_bits )
		{
			throw std::logic_error( "Computes the distance between iterators from different containers." );
		}

		if (left.bit_position == right.bit_position && left.is_before_begin == right.is_before_begin)
		{
			return 0;
		}

		size_t left_position = left.bit_position;
		size_t right_position = right.bit_position;

		if (left.is_before_begin) {
			left_position = static_cast<size_t>(-1);
		}

		if (right.is_before_begin) {
			right_position = static_cast<size_t>(-1);
		}

		size_t result = left_position > right_position ? left_position - right_position : right_position - left_position;
		return left_position > right_position ? static_cast<std::ptrdiff_t>(result) : -static_cast<std::ptrdiff_t>(result);

	}

	/******************************/

	bool ConstantBitIterator::operator*() const
	{
		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot dereference a read-only end iterator." );
		}
		return (data_pointer->bits & (1 << (bit_position % 32))) != 0;
	}

	bool ConstantBitIterator::operator[]( std::ptrdiff_t offset ) const
	{
		ConstantBitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return *temp;
	}

	ConstantBitIterator ConstantBitIterator::operator+( std::ptrdiff_t offset ) const
	{
		ConstantBitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return temp;
	}

	ConstantBitIterator ConstantBitIterator::operator-( std::ptrdiff_t offset ) const
	{
		ConstantBitIterator temp = *this;
		temp.retreat( offset );	 // retreat内部已处理异常
		return temp;
	}

	ConstantBitIterator& ConstantBitIterator::operator++()
	{
		this->offset_up();	// offset_up内部已处理异常
		return *this;
	}

	ConstantBitIterator ConstantBitIterator::operator++( int )
	{
		ConstantBitIterator temp = *this;
		this->offset_up();	// offset_up内部已处理异常
		return temp;
	}

	ConstantBitIterator& ConstantBitIterator::operator--()
	{
		this->offset_down();  // offset_down内部已处理异常
		return *this;
	}

	ConstantBitIterator ConstantBitIterator::operator--( int )
	{
		ConstantBitIterator temp = *this;
		this->offset_down();  // offset_down内部已处理异常
		return temp;
	}

	bool ConstantBitIterator::operator==( const ConstantBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		return bit_position == other.bit_position && is_after_end == other.is_after_end;
	}

	bool ConstantBitIterator::operator!=( const ConstantBitIterator& other ) const
	{
		return !( *this == other );
	}

	bool ConstantBitIterator::operator<( const ConstantBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return !is_after_end && other.is_after_end;
		}
		return bit_position < other.bit_position;
	}

	bool ConstantBitIterator::operator>( const ConstantBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return is_after_end && !other.is_after_end;
		}
		return bit_position > other.bit_position;
	}

	bool ConstantBitIterator::operator<=( const ConstantBitIterator& other ) const
	{
		return !( *this > other );
	}

	bool ConstantBitIterator::operator>=( const ConstantBitIterator& other ) const
	{
		return !( *this < other );
	}

	ConstantBitIterator& ConstantBitIterator::operator=( const ConstantBitIterator& other )
	{
		if ( this == &other )
		{
			return *this;  // Return *this to deal with self-assignment
		}

		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot assign to a read-only end iterator." );
		}

		if(container_pointer == nullptr)
			container_pointer == other.container_pointer;

		if ( container_pointer == other.container_pointer && max_valid_bits == other.max_valid_bits )
		{
			bit_position = other.bit_position;
			data_pointer = other.data_pointer;
			is_read_only = other.is_read_only;
			is_after_end = other.is_after_end;
		}
		else
		{
			throw std::logic_error( "The two containers are different and cannot be assigned." );
		}

		return *this;
	}

	std::ptrdiff_t operator-( const ConstantBitIterator& left, const ConstantBitIterator& right )
	{
		if ( left.container_pointer != right.container_pointer || left.max_valid_bits != right.max_valid_bits )
		{
			throw std::logic_error( "Computes the distance between iterators from different containers." );
		}

		if (left.bit_position == right.bit_position && left.is_after_end == right.is_after_end)
		{
			return 0;
		}

		size_t left_position = left.bit_position;
		size_t right_position = right.bit_position;

		if (left.is_after_end) {
			left_position = left.max_valid_bits;
		}

		if (right.is_after_end) {
			right_position = right.max_valid_bits;
		}

		size_t result = left_position > right_position ? left_position - right_position : right_position - left_position;
		return left_position > right_position ? static_cast<std::ptrdiff_t>(result) : -static_cast<std::ptrdiff_t>(result);
	}

	/******************************/

	bool ConstantReverseBitIterator::operator*() const
	{
		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot dereference a read-only end iterator." );
		}
		return (data_pointer->bits & (1 << (bit_position % 32))) != 0;
	}

	bool ConstantReverseBitIterator::operator[]( std::ptrdiff_t offset ) const
	{
		ConstantReverseBitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return *temp;
	}

	ConstantReverseBitIterator ConstantReverseBitIterator::operator+( std::ptrdiff_t offset ) const
	{
		ConstantReverseBitIterator temp = *this;
		temp.advance( offset );	 // advance内部已处理异常
		return temp;
	}

	ConstantReverseBitIterator ConstantReverseBitIterator::operator-( std::ptrdiff_t offset ) const
	{
		ConstantReverseBitIterator temp = *this;
		temp.retreat( offset );	 // retreat内部已处理异常
		return temp;
	}

	ConstantReverseBitIterator& ConstantReverseBitIterator::operator++()
	{
		this->offset_up();	// offset_up内部已处理异常
		return *this;
	}

	ConstantReverseBitIterator ConstantReverseBitIterator::operator++( int )
	{
		ConstantReverseBitIterator temp = *this;
		this->offset_up();	// offset_up内部已处理异常
		return temp;
	}

	ConstantReverseBitIterator& ConstantReverseBitIterator::operator--()
	{
		this->offset_down();  // offset_down内部已处理异常
		return *this;
	}

	ConstantReverseBitIterator ConstantReverseBitIterator::operator--( int )
	{
		ConstantReverseBitIterator temp = *this;
		this->offset_down();  // offset_down内部已处理异常
		return temp;
	}

	bool ConstantReverseBitIterator::operator==( const ConstantReverseBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		return bit_position == other.bit_position && is_before_begin == other.is_before_begin;
	}

	bool ConstantReverseBitIterator::operator!=( const ConstantReverseBitIterator& other ) const
	{
		return !( *this == other );
	}

	bool ConstantReverseBitIterator::operator<( const ConstantReverseBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return is_before_begin && !other.is_before_begin;
		}
		return bit_position > other.bit_position;  // Note the '>' because it's a reverse iterator
	}

	bool ConstantReverseBitIterator::operator>( const ConstantReverseBitIterator& other ) const
	{
		if ( container_pointer != other.container_pointer || max_valid_bits != other.max_valid_bits )
		{
			throw std::logic_error( "Comparing iterators from different containers." );
		}
		if ( bit_position == other.bit_position )
		{
			return !is_before_begin && other.is_before_begin;
		}
		return bit_position < other.bit_position;  // Note the '<' because it's a reverse iterator
	}

	bool ConstantReverseBitIterator::operator<=( const ConstantReverseBitIterator& other ) const
	{
		return !( *this > other );;
	}

	bool ConstantReverseBitIterator::operator>=( const ConstantReverseBitIterator& other ) const
	{
		return !( *this < other );;
	}

	ConstantReverseBitIterator& ConstantReverseBitIterator::operator=( const ConstantReverseBitIterator& other )
	{
		if ( this == &other )
		{
			return *this;  // Return *this to deal with self-assignment
		}

		if ( is_read_only_end_iterator() )
		{
			throw std::logic_error( "Cannot assign to a read-only end iterator." );
		}

		if(container_pointer == nullptr)
			container_pointer == other.container_pointer;

		if ( container_pointer == other.container_pointer && max_valid_bits == other.max_valid_bits )
		{
			bit_position = other.bit_position;
			data_pointer = other.data_pointer;
			is_read_only = other.is_read_only;
			is_before_begin = other.is_before_begin;
		}
		else
		{
			throw std::logic_error( "The two containers are different and cannot be assigned." );
		}

		return *this;
	}

	std::ptrdiff_t operator-( const ConstantReverseBitIterator& left, const ConstantReverseBitIterator& right )
	{
		if ( left.container_pointer != right.container_pointer || left.max_valid_bits != right.max_valid_bits )
		{
			throw std::logic_error( "Computes the distance between iterators from different containers." );
		}

		if (left.bit_position == right.bit_position && left.is_before_begin == right.is_before_begin)
		{
			return 0;
		}

		size_t left_position = left.bit_position;
		size_t right_position = right.bit_position;

		if (left.is_before_begin) {
			left_position = static_cast<size_t>(-1);
		}

		if (right.is_before_begin) {
			right_position = static_cast<size_t>(-1);
		}

		size_t result = left_position > right_position ? left_position - right_position : right_position - left_position;
		return left_position > right_position ? static_cast<std::ptrdiff_t>(result) : -static_cast<std::ptrdiff_t>(result);

	}

}  // namespace TwilightDream