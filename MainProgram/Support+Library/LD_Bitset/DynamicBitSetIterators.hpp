#pragma once

#include <vector>
#include <algorithm>

#include <iterator>
#include <stdexcept>
#include <utility>
#include <type_traits>

#include "BooleanBitWrapper.hpp"

namespace TwilightDream
{
	class AbstractBitIteratorBase
	{
	public:
		AbstractBitIteratorBase(size_t bit_position, size_t max_valid_bits)
			:
			bit_position(bit_position), max_valid_bits(max_valid_bits)
		{};
		virtual ~AbstractBitIteratorBase() = default;

		//Iterator increments by 1
		//迭代器自增
		virtual void offset_up() = 0;
		//Iterator decrements by 1
		//迭代器自减
		virtual void offset_down() = 0;
		virtual bool is_read_only_end_iterator() const = 0;
		virtual bool is_start_bit_position() const = 0;
		virtual bool is_end_bit_position() const = 0;
		//Iterator increments by n
		//迭代器自增n次
		virtual void advance( std::ptrdiff_t n ) = 0;
		//Iterator decrements by n
		//迭代器自减n次
		virtual void retreat( std::ptrdiff_t n ) = 0;

	protected:
		//The bit position currently pointed to (max_bit_position = max_valid_bits - 1)
		size_t bit_position = 0;
		//Current bit container data, and then determine the boundary size
		//Indicates the number of digits counted from the LSB position on the right to the MSB position on the left.
		const size_t max_valid_bits = 0;
		//This boolean flag, if true, means that the instantiated iterator is a read-only boundary flag, it cannot be moved bit_position, and its dereference data cannot be accessed.
		bool is_read_only = false;
	};

	// 正向迭代器基类
	class BitIteratorBaseData : public AbstractBitIteratorBase
	{
	public:
		BitIteratorBaseData(bool is_begin_or_end_iterator, std::vector<BooleanBitWrapper>* container, size_t MVB )
			: AbstractBitIteratorBase(is_begin_or_end_iterator ? 0 : MVB - 1, MVB), container_pointer(container)
		{
			if(container == nullptr)
				throw std::logic_error("The pointer to the pointer is an invalid pointer!");
			if(container->empty())
				throw std::logic_error("There is no data in the container and no iteration is allowed.");
			if(MVB == 0)
				throw std::logic_error("The maximum valid bit count is not set correctly.");
			
			if (is_begin_or_end_iterator)
			{
				// 开始位置的迭代器 
				data_pointer = &( (*container)[0] );
				is_read_only = false;
				is_after_end = false;
			}
			else
			{
				// 结束位置的迭代器
				data_pointer = &( (*container)[container->size() - 1] );
				is_read_only = true;
				is_after_end = true;
			}
		}

		virtual ~BitIteratorBaseData() = default;

		void offset_up() override;
		void offset_down() override;
		bool is_read_only_end_iterator() const override;
		bool is_start_bit_position() const override;
		bool is_end_bit_position() const override;
		void advance( std::ptrdiff_t n ) override;
		void retreat( std::ptrdiff_t n ) override;

	protected:
		BooleanBitWrapper*				data_pointer = nullptr;
		std::vector<BooleanBitWrapper>* container_pointer = nullptr;
		
		/*
			Regardless of whether the instantiated iterator is read-only or not.
			This boolean flag is used in the context of the current iterator's characteristics to indicate bit positions that are out of the container's range (e.g., before the start position or after the end position).
			But the current bit positions we record are not actually modified. 
			With this flag, we can perform an extra iteration to ensure that the iterator logic is correct.
		*/
		bool							is_after_end = false;
	};

	// 反向迭代器基类
	class ReverseBitIteratorBaseData : public AbstractBitIteratorBase
	{
	public:
		ReverseBitIteratorBaseData(bool is_begin_or_end_iterator, std::vector<BooleanBitWrapper>* container, size_t MVB )
			: AbstractBitIteratorBase(is_begin_or_end_iterator ? MVB - 1 : 0, MVB), container_pointer(container)
		{
			if(container == nullptr)
				throw std::logic_error("The pointer to the pointer is an invalid pointer!");
			if(container->empty())
				throw std::logic_error("There is no data in the container and no iteration is allowed.");
			if(MVB == 0)
				throw std::logic_error("The maximum valid bit count is not set correctly.");

			if (is_begin_or_end_iterator)
			{
				// 开始位置的迭代器
				data_pointer = &( (*container)[container->size() - 1] );
				is_read_only = false;
				is_before_begin = false;
			}
			else
			{
				// 结束位置的迭代器
				data_pointer = &( (*container)[0] );
				is_read_only = true;
				is_before_begin = true;
			}
		}

		virtual ~ReverseBitIteratorBaseData() = default;

		void offset_up() override;
		void offset_down() override;
		bool is_read_only_end_iterator() const override;
		bool is_start_bit_position() const override;
		bool is_end_bit_position() const override;
		void advance( std::ptrdiff_t n ) override;
		void retreat( std::ptrdiff_t n ) override;

	protected:
		BooleanBitWrapper*				data_pointer = nullptr;
		std::vector<BooleanBitWrapper>* container_pointer = nullptr;
		
		/*
			Regardless of whether the instantiated iterator is read-only or not.
			This boolean flag is used in the context of the current iterator's characteristics to indicate bit positions that are out of the container's range (e.g., before the start position or after the end position).
			But the current bit positions we record are not actually modified. 
			With this flag, we can perform an extra iteration to ensure that the iterator logic is correct.
		*/
		bool							is_before_begin = false;
	};

	// 常量(不可以修改迭代器说指向的数据,但是这个迭代器可以移动自己的位置)正向迭代器基类
	class ConstantBitIteratorBaseData : public AbstractBitIteratorBase
	{
	public:
		ConstantBitIteratorBaseData(bool is_begin_or_end_iterator, const std::vector<BooleanBitWrapper>* container, size_t MVB )
			: AbstractBitIteratorBase(is_begin_or_end_iterator ? 0 : MVB - 1, MVB), container_pointer(container)
		{
			if(container == nullptr)
				throw std::logic_error("The pointer to the pointer is an invalid pointer!");
			if(container->empty())
				throw std::logic_error("There is no data in the container and no iteration is allowed.");
			if(MVB == 0)
				throw std::logic_error("The maximum valid bit count is not set correctly.");
			
			if (is_begin_or_end_iterator)
			{
				// 开始位置的迭代器 
				data_pointer = &( (*container)[0] );
				is_read_only = false;
				is_after_end = false;
			}
			else
			{
				// 结束位置的迭代器
				data_pointer = &( (*container)[container->size() - 1] );
				is_read_only = true;
				is_after_end = true;
			}
		}

		virtual ~ConstantBitIteratorBaseData() = default;

		void offset_up() override;
		void offset_down() override;
		bool is_read_only_end_iterator() const override;
		bool is_start_bit_position() const override;
		bool is_end_bit_position() const override;
		void advance( std::ptrdiff_t n ) override;
		void retreat( std::ptrdiff_t n ) override;

	protected:
		const BooleanBitWrapper*			  data_pointer = nullptr;
		const std::vector<BooleanBitWrapper>* container_pointer = nullptr;
		
		/*
			Regardless of whether the instantiated iterator is read-only or not.
			This boolean flag is used in the context of the current iterator's characteristics to indicate bit positions that are out of the container's range (e.g., before the start position or after the end position).
			But the current bit positions we record are not actually modified. 
			With this flag, we can perform an extra iteration to ensure that the iterator logic is correct.
		*/
		bool								  is_after_end = false;
	};

	// 常量(不可以修改迭代器说指向的数据,但是这个迭代器可以移动自己的位置)反向迭代器基类
	class ConstantReverseBitIteratorBaseData : public AbstractBitIteratorBase
	{
	public:
		ConstantReverseBitIteratorBaseData(bool is_begin_or_end_iterator, const std::vector<BooleanBitWrapper>* container, size_t MVB )
			: AbstractBitIteratorBase(is_begin_or_end_iterator ? MVB - 1 : 0, MVB), container_pointer(container)
		{
			if(container == nullptr)
				throw std::logic_error("The pointer to the pointer is an invalid pointer!");
			if(container->empty())
				throw std::logic_error("There is no data in the container and no iteration is allowed.");
			if(MVB == 0)
				throw std::logic_error("The maximum valid bit count is not set correctly.");

			if (is_begin_or_end_iterator)
			{
				// 开始位置的迭代器
				data_pointer = &( (*container)[container->size() - 1] );
				is_read_only = false;
				is_before_begin = false;
			}
			else
			{
				// 结束位置的迭代器
				data_pointer = &( (*container)[0] );
				is_read_only = true;
				is_before_begin = true;
			}
		}

		virtual ~ConstantReverseBitIteratorBaseData() = default;

		void offset_up() override;
		void offset_down() override;
		bool is_read_only_end_iterator() const override;
		bool is_start_bit_position() const override;
		bool is_end_bit_position() const override;
		void advance( std::ptrdiff_t n ) override;
		void retreat( std::ptrdiff_t n ) override;

	protected:
		const BooleanBitWrapper*			  data_pointer = nullptr;
		const std::vector<BooleanBitWrapper>* container_pointer = nullptr;
		
		/*
			Regardless of whether the instantiated iterator is read-only or not.
			This boolean flag is used in the context of the current iterator's characteristics to indicate bit positions that are out of the container's range (e.g., before the start position or after the end position).
			But the current bit positions we record are not actually modified. 
			With this flag, we can perform an extra iteration to ensure that the iterator logic is correct.
		*/
		bool								  is_before_begin = false;
	};



	struct BitReference
	{
		uint32_t* data_pointer;
		uint32_t bits_mask;

		BitReference() noexcept;
		BitReference(BooleanBitWrapper* wrapper_pointer, uint32_t bits_mask);
		BitReference(const BitReference& other) = default;

		operator bool() const noexcept;
		BitReference& operator=(bool value) noexcept;
		BitReference& operator=(const BitReference& other) noexcept;
		BitReference& operator^=(const BitReference& other) noexcept;
		BitReference& operator&=(const BitReference& other) noexcept;
		BitReference& operator|=(const BitReference& other) noexcept;
		bool operator~() noexcept;
		bool operator==(const BitReference& other) const;
		bool operator<(const BitReference& other) const;
		bool operator>(const BitReference& other) const;
		bool operator<=(const BitReference& other) const;
		bool operator>=(const BitReference& other) const;
		void flip() noexcept;
	};

	struct BitIterator : BitIteratorBaseData
	{
		using BitIteratorBaseData::BitIteratorBaseData;

		using iterator_category = std::random_access_iterator_tag;
		using iterator = BitIterator;
		using value_type = bool;
		using difference_type = std::ptrdiff_t;
		using pointer = BitReference*;
		using reference = BitReference;

		BitReference operator*();
		BitReference operator[](std::ptrdiff_t offset);
		BitIterator operator+(std::ptrdiff_t offset) const;
		BitIterator operator-(std::ptrdiff_t offset) const;
		BitIterator& operator+=(std::ptrdiff_t offset);
		BitIterator& operator-=(std::ptrdiff_t offset);
		BitIterator& operator++();
		BitIterator operator++(int);
		BitIterator& operator--();
		BitIterator operator--(int);
		BitIterator& operator=(const BitIterator& other);
		bool operator==(const BitIterator& other) const;
		bool operator!=(const BitIterator& other) const;
		bool operator<(const BitIterator& other) const;
		bool operator>(const BitIterator& other) const;
		bool operator<=(const BitIterator& other) const;
		bool operator>=(const BitIterator& other) const;

		friend std::ptrdiff_t operator-(const BitIterator& left, const BitIterator& right);
	};

	struct ReverseBitIterator : ReverseBitIteratorBaseData
	{
		using ReverseBitIteratorBaseData::ReverseBitIteratorBaseData;

		BitReference operator*();
		BitReference operator[](std::ptrdiff_t offset);
		ReverseBitIterator operator+(std::ptrdiff_t offset) const;
		ReverseBitIterator operator-(std::ptrdiff_t offset) const;
		ReverseBitIterator& operator-=(std::ptrdiff_t n);
		ReverseBitIterator& operator+=(std::ptrdiff_t n);
		ReverseBitIterator& operator++();
		ReverseBitIterator operator++(int);
		ReverseBitIterator& operator--();
		ReverseBitIterator operator--(int);
		ReverseBitIterator& operator=(const ReverseBitIterator& other);
		bool operator==(const ReverseBitIterator& other) const;
		bool operator!=(const ReverseBitIterator& other) const;
		bool operator<(const ReverseBitIterator& other) const;
		bool operator>(const ReverseBitIterator& other) const;
		bool operator<=(const ReverseBitIterator& other) const;
		bool operator>=(const ReverseBitIterator& other) const;

		friend std::ptrdiff_t operator-(const ReverseBitIterator& left, const ReverseBitIterator& right);
	};

	struct ConstantBitIterator : ConstantBitIteratorBaseData
	{
		using ConstantBitIteratorBaseData::ConstantBitIteratorBaseData;

		using iterator_category = std::random_access_iterator_tag;
		using iterator = ConstantBitIterator;
		using value_type = bool;
		using difference_type = std::ptrdiff_t;
		using pointer = bool*;
		using reference = bool;

		bool operator*() const;
		bool operator[](std::ptrdiff_t offset) const;
		ConstantBitIterator operator+(std::ptrdiff_t offset) const;
		ConstantBitIterator operator-(std::ptrdiff_t offset) const;
		std::ptrdiff_t operator-(const ConstantBitIterator& other) const;
		ConstantBitIterator& operator++();
		ConstantBitIterator operator++(int);
		ConstantBitIterator& operator--();
		ConstantBitIterator operator--(int);
		ConstantBitIterator& operator=(const ConstantBitIterator& other);
		bool operator==(const ConstantBitIterator& other) const;
		bool operator!=(const ConstantBitIterator& other) const;
		bool operator<(const ConstantBitIterator& other) const;
		bool operator>(const ConstantBitIterator& other) const;
		bool operator<=(const ConstantBitIterator& other) const;
		bool operator>=(const ConstantBitIterator& other) const;

		friend std::ptrdiff_t operator-(const ConstantBitIterator& left, const ConstantBitIterator& right);
	};

	struct ConstantReverseBitIterator : ConstantReverseBitIteratorBaseData
	{
		using ConstantReverseBitIteratorBaseData::ConstantReverseBitIteratorBaseData;

		bool operator*() const;
		bool operator[](std::ptrdiff_t offset) const;
		ConstantReverseBitIterator operator+(std::ptrdiff_t offset) const;
		ConstantReverseBitIterator operator-(std::ptrdiff_t offset) const;
		std::ptrdiff_t operator-(const ConstantReverseBitIterator& other) const;
		ConstantReverseBitIterator& operator++();
		ConstantReverseBitIterator operator++(int);
		ConstantReverseBitIterator& operator--();
		ConstantReverseBitIterator operator--(int);
		ConstantReverseBitIterator& operator=(const ConstantReverseBitIterator& other);
		bool operator==(const ConstantReverseBitIterator& other) const;
		bool operator!=(const ConstantReverseBitIterator& other) const;
		bool operator<(const ConstantReverseBitIterator& other) const;
		bool operator>(const ConstantReverseBitIterator& other) const;
		bool operator<=(const ConstantReverseBitIterator& other) const;
		bool operator>=(const ConstantReverseBitIterator& other) const;

		friend std::ptrdiff_t operator-(const ConstantReverseBitIterator& left, const ConstantReverseBitIterator& right);
	};

}  // namespace TwilightDream