#pragma once

#ifndef DEFINE_COMMON_TOOLKIT
#define DEFINE_COMMON_TOOLKIT

#if __cplusplus >= 201103L && __cplusplus <= 201703L
inline std::wstring cpp2017_string2wstring(const std::string &_string)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.from_bytes(_string);
}

inline std::string cpp2017_wstring2string(const std::wstring &_wstring)
{
	using convert_typeX = std::codecvt_utf8<wchar_t>;
	std::wstring_convert<convert_typeX, wchar_t> converterX;

	return converterX.to_bytes(_wstring);
}
#endif

inline std::wstring string2wstring(const std::string& _string)
{
	::setlocale(LC_ALL, "");
	std::vector<wchar_t> wide_character_buffer;
	std::size_t source_string_count = 1;
	std::size_t found_not_ascii_count = 0;
	for(auto begin = _string.begin(), end = _string.end(); begin != end; begin++)
	{
		if(static_cast<const long long>(*begin) > 0)
		{
			++source_string_count;
		}
		else if (static_cast<const long long>(*begin) < 0)
		{
			++found_not_ascii_count;
		}
	}

	std::size_t target_wstring_count = source_string_count + (found_not_ascii_count / 2);

	wide_character_buffer.resize(target_wstring_count);

	#if defined(_MSC_VER)
	std::size_t _converted_count = 0;
	::mbstowcs_s(&_converted_count, &wide_character_buffer[0], target_wstring_count, _string.c_str(), ((size_t)-1));
	#else
	::mbstowcs(&wide_character_buffer[0], _string.c_str(), target_wstring_count);
	#endif

	std::size_t _target_wstring_size = 0;
	for(auto begin = wide_character_buffer.begin(), end = wide_character_buffer.end(); begin != end && *begin != L'\0'; begin++)
	{
		++_target_wstring_size;
	}
	std::wstring _wstring{ wide_character_buffer.data(),  _target_wstring_size };

	#if defined(_MSC_VER)
	if(_converted_count == 0)
	{
		throw std::runtime_error("The function string2wstring is not work !");
	}
	#endif

	if(found_not_ascii_count > 0)
	{
		//Need Contains character('\0') then check size
		if(((_target_wstring_size + 1) - source_string_count) != (found_not_ascii_count / 2))
		{
			throw std::runtime_error("The function string2wstring, An error occurs during conversion !");
		}
		else
		{
			return _wstring;
		}
	}
	else
	{
		//Need Contains character('\0') then check size
		if((_target_wstring_size + 1) != source_string_count)
		{
			 throw std::runtime_error("The function string2wstring, An error occurs during conversion !");
		}
		else
		{
			return _wstring;
		}
	}

}

inline std::string wstring2string(const std::wstring& _wstring)
{
	::setlocale(LC_ALL, "");
	std::vector<char> character_buffer;
	std::size_t source_wstring_count = 1;
	std::size_t found_not_ascii_count = 0;
	for(auto begin = _wstring.begin(), end = _wstring.end(); begin != end; begin++)
	{
		if(static_cast<const long long>(*begin) < 256)
		{
			++source_wstring_count;
		}
		else if (static_cast<const long long>(*begin) >= 256)
		{
			++found_not_ascii_count;
		}
	}
	std::size_t target_string_count = source_wstring_count + found_not_ascii_count * 2;

	character_buffer.resize(target_string_count);

	#if defined(_MSC_VER)
	std::size_t _converted_count = 0;
	::wcstombs_s(&_converted_count, &character_buffer[0], target_string_count, _wstring.c_str(), ((size_t)-1));
	#else
	::wcstombs(&character_buffer[0], _wstring.c_str(), target_string_count);
	#endif

	std::size_t _target_string_size = 0;
	for(auto begin = character_buffer.begin(), end = character_buffer.end(); begin != end && *begin != '\0'; begin++)
	{
		++_target_string_size;
	}
	std::string _string{ character_buffer.data(),  _target_string_size };

	#if defined(_MSC_VER)
	if(_converted_count == 0)
	{
		throw std::runtime_error("The function wstring2string is not work !");
	}
	#endif

	if(found_not_ascii_count > 0)
	{
		if(((_target_string_size + 1) - source_wstring_count) != (found_not_ascii_count * 2))
		{
			throw std::runtime_error("The function wstring2string, An error occurs during conversion !");
		}
		else
		{
			return _string;
		}
	}
	else
	{
		if((_target_string_size + 1) != source_wstring_count)
		{
			throw std::runtime_error("The function wstring2string, An error occurs during conversion !");
		}
		else
		{
			return _string;
		}
	}
}

#if __cplusplus >= 202002L

namespace CommonToolkit
{
	namespace CPP2020_Concepts
	{
		template < typename Other, template < typename... > class Self >
		struct is_specialization_class_of : std::false_type
		{
		};

		template < typename... Args, template < typename... > class Self >
		struct is_specialization_class_of< Self< Args... >, Self > : std::true_type
		{
		};

		template < typename Other, template < typename... > class Self >
		inline constexpr bool is_specialization_class_of_v = is_specialization_class_of< Other, Self >::value;

		template < typename Other, template < typename... > class Self >
		concept IsSpecializesClassType = is_specialization_class_of_v< Other, Self >;

		template <typename>
		inline constexpr bool is_array_class_type = false;
		template <typename Type, std::size_t Size>
		inline constexpr bool is_array_class_type<std::array<Type, Size>> = true;

		namespace IsIterable
		{
			template < typename IterableType, typename = void >
			struct implementation : std::false_type
			{
			};

			// this gets used only when we can call std::begin() and std::end() on that type
			template < typename IterableType >
			struct implementation< IterableType, std::void_t< decltype( std::begin( std::declval< IterableType >() ) ), decltype( std::end( std::declval< IterableType >() ) ) > > : std::true_type
			{
			};

			//std::cout << std::boolalpha;
			//std::cout << is_iterable_v<std::vector<double>> << '\n';
			//std::cout << is_iterable_v<std::map<int, double>> << '\n';
			//std::cout << is_iterable_v<double> << '\n';
			//struct A;
			//std::cout << is_iterable_v<A> << '\n';
			template < typename IterableType >
			inline constexpr bool is_iterable_v = implementation< IterableType >::value;

			// To allow ADL with custom begin/end
			using std::begin;
			using std::end;

			template < typename Type >
			concept IsIterable = requires
			{
				std::begin( std::declval< Type& >() ) == std::end( std::declval< Type& >() );  // begin/end and operator ==
				std::begin( std::declval< Type& >() ) != std::end( std::declval< Type& >() );  // begin/end and operator !=
				++std::declval< decltype( std::begin( std::declval< Type& >() ) )& >();		   // operator ++
				--std::declval< decltype( std::begin( std::declval< Type& >() ) )& >();
				*std::begin( std::declval< Type& >() );	 // operator*
			};
		}  // namespace IsIterable

		namespace WithRanges
		{
			template < typename IterableType >
			concept IsElementIterableLevel1Type = std::ranges::range< IterableType >;

			template < typename IterableType >
			concept IsElementIterableLevel2Type = std::ranges::input_range< IterableType > || std::ranges::output_range< IterableType, std::ranges::range_value_t< IterableType > >;

			template < typename IterableType >
			concept IsElementIterableWithRangeType = std::ranges::contiguous_range< IterableType > && requires( std::ranges::range_value_t< IterableType > container )
			{
				std::ranges::begin( container );
				std::ranges::end( container );
			};

			template < typename IterableType >
			concept IsElementIterableWithRangeType2 = std::ranges::random_access_range< IterableType > && requires( std::ranges::range_value_t< IterableType > container )
			{
				std::ranges::begin( container );
				std::ranges::end( container );
			};
		}  // namespace WithRanges

		template < typename LeftType, typename RightType >
		concept IsTwoSameType = std::same_as< LeftType, RightType >;

		template < typename Type >
		concept IsArithmeticDifferencesComparableType = std::totally_ordered< Type >;

		template < typename Type >
		concept IsEqualityComparableType = std::equality_comparable< Type >;

		template < typename Type >
		concept IsClassType = std::is_class_v< Type >;

		template < typename BaseClassType, typename DerivedClassType >
		concept IsTypeFromBaseClass = std::is_base_of_v< BaseClassType, DerivedClassType >;

		template < typename DataType >
		concept IsSimpleType = std::is_arithmetic_v< DataType > || std::is_enum_v< DataType > || std::is_class_v< DataType > || std::is_union_v< DataType > || std::is_reference_v< DataType > && ( !std::is_null_pointer_v< DataType > ) && ( !std::is_array_v< DataType > );

		template < typename DataType, typename ReferenceType, typename PointerType >
		concept IsCustomIteratorType = IsSimpleType< DataType > && std::is_reference_v< ReferenceType > &&( std::is_pointer_v< PointerType > || std::is_member_pointer_v< PointerType > && ( !std::is_null_pointer_v< PointerType > ) );

		template<class KeyValueMapType>
		concept IsKeyValueMapType = std::same_as<typename KeyValueMapType::value_type, std::pair<const typename KeyValueMapType::key_type, typename KeyValueMapType::mapped_type>>;

	}  // namespace CPP2020_Concepts
}  // namespace EODF_Reborn_CommonToolkit

namespace CommonToolkit
{
	using namespace CPP2020_Concepts;

	namespace MakeArrayImplement
	{
		template<typename Type, std::size_t N, std::size_t... I>
		constexpr auto make_array(std::index_sequence<I...>)
		{
			return std::array<Type, N>{ {I...} };
		}

		template<typename Type, typename FunctionType, std::size_t... Is>
		requires std::invocable<FunctionType>
		constexpr auto generate_array(FunctionType& function, std::index_sequence<Is...>) -> std::array<Type, sizeof...(Is)>
		{
			return {{ function(std::integral_constant<std::size_t, Is>{})... }};
		}
	}

	template<typename Type, std::size_t N>
	constexpr auto make_array()
	{
		static_assert(N >= Type{}, "no negative sizes");
		return MakeArrayImplement::make_array<Type, N>(std::make_index_sequence<N>{});
	}

	template<typename Type, std::size_t N, typename FunctionType>
	requires std::invocable<FunctionType>
	constexpr auto generate_array(FunctionType function)
	{
		return MakeArrayImplement::generate_array<Type>(function, std::make_index_sequence<N>{});
	}

	namespace MakeVectorImplement
	{
		template <typename Type, Type... VALUES>
		constexpr std::vector<Type> make_vector()
		{
			return std::vector<Type> { VALUES... };
		}
	}

	template <typename Type, Type... VALUES>
	constexpr std::vector<Type> make_vector( std::integer_sequence<Type, VALUES...> )
	{
		return MakeVectorImplement::make_vector<Type, VALUES...>();
	}

	//https://vladris.com/blog/2018/10/13/arithmetic-overflow-and-underflow.html
	//https://zh.cppreference.com/w/cpp/algorithm/iota
	template<bool is_increment_or_decrement, std::input_or_output_iterator IteratorType, typename IteratorSentinelType, typename NumericalType>
	requires std::sentinel_for<IteratorSentinelType, IteratorType>
	&& std::signed_integral<NumericalType>
	|| std::unsigned_integral<NumericalType>
	void numbers_sequence_generator(IteratorType first, IteratorSentinelType last, NumericalType value)
	{
		while (first != last)
		{
			*first++ = value;

			if constexpr(is_increment_or_decrement)
			{
				if(value + 1 == std::numeric_limits<NumericalType>::min())
					break;
				++value;
			}
			else if constexpr(is_increment_or_decrement)
			{
				if(value - 1 == std::numeric_limits<NumericalType>::max())
					break;
				--value;
			}
		}
	}

	template<bool is_increment_or_decrement, std::bidirectional_iterator IteratorType, typename IteratorSentinelType, typename NumericalType>
	requires std::integral<NumericalType>
	&& std::sentinel_for<IteratorSentinelType, IteratorType>
	void numbers_sequence_generator(IteratorType first, IteratorSentinelType last, NumericalType value, NumericalType other_value)
	{
		std::iter_difference_t<IteratorType> ranges_size = std::ranges::distance(first, last);

		if(ranges_size > 0)
		{
			while (first != last)
			{
				/*
					Equivalence Code:

					*first = value;
					first++;

				*/
				*first++ = value;

				if constexpr(is_increment_or_decrement)
				{
					//AdditionOverflows
					if( (other_value >= 0) && (value > std::numeric_limits<NumericalType>::max() - other_value) )
						break;
					//AdditionUnderflows
					else if( (other_value < 0) && (value < std::numeric_limits<NumericalType>::min() - other_value) )
						break;
					value += other_value;
				}
				else if constexpr(!is_increment_or_decrement)
				{
					//SubtractionOverflows
					if( (other_value < 0) && (value > std::numeric_limits<NumericalType>::max() + other_value) )
						break;
					//SubtractionOverflows
					else if( (other_value >= 0) && (value < std::numeric_limits<NumericalType>::min() + other_value) )
						break;
					value -= other_value;
				}
			}
		}
		else if (ranges_size < 0)
		{
			while (last != first)
			{
				/*
					Equivalence Code:

					*first = value;
					first--;

				*/
				*first-- = value;

				if constexpr(is_increment_or_decrement)
				{
					//AdditionOverflows
					if( (other_value >= 0) && (value > std::numeric_limits<NumericalType>::max() - other_value) )
						break;
					//AdditionUnderflows
					else if( (other_value < 0) && (value < std::numeric_limits<NumericalType>::min() - other_value) )
						break;
					value += other_value;
				}
				else if constexpr(!is_increment_or_decrement)
				{
					//SubtractionOverflows
					if( (other_value < 0) && (value > std::numeric_limits<NumericalType>::max() + other_value) )
						break;
					//SubtractionOverflows
					else if( (other_value >= 0) && (value < std::numeric_limits<NumericalType>::min() + other_value) )
						break;
					value -= other_value;
				}
			}
		}
		else
		{
			return;
		}
	}

	namespace ClassObjectComparer
	{
		template <typename Type>
		requires IsArithmeticDifferencesComparableType<Type>
		class LessThanOrder
		{
		public:
			bool operator()( Type leftObject, Type rightObject )
			{
				return leftObject > rightObject;
			}
		};

		template <typename Type>
		requires IsArithmeticDifferencesComparableType<Type>
		class GreaterThanOrder
		{
		public:
			bool operator()( Type leftObject, Type rightObject )
			{
				return leftObject < rightObject;
			}
		};

		template <typename Type>
		requires IsEqualityComparableType<Type>
		class EqualComparer
		{
		public:
			bool operator()( Type leftObject, Type rightObject )
			{
				return leftObject == rightObject;
			}
		};

		template <typename Type>
		requires IsEqualityComparableType<Type>
		class NotEqualComparer
		{
		public:
			bool operator()( Type leftObject, Type rightObject )
			{
				return leftObject != rightObject;
			}
		};
	}  // namespace ClassObjectComparer

	template <typename OtherType, typename DataObjectType>
	concept _Half_part_of_class_object_comparer = IsTwoSameType<CommonToolkit::ClassObjectComparer::template LessThanOrder<DataObjectType>, OtherType> || IsTwoSameType<CommonToolkit::ClassObjectComparer::template GreaterThanOrder<DataObjectType>, OtherType>;

	template <typename OtherType, typename DataObjectType>
	concept IsClassObjectComparerType = _Half_part_of_class_object_comparer<OtherType, DataObjectType> && IsArithmeticDifferencesComparableType<DataObjectType>;

	#if defined(__cpp_lib_char8_t)

	inline std::string from_u8string(const char8_t* utf8_string_data, std::size_t size)
	{
		std::u8string value = std::u8string(utf8_string_data, size);

		#if __cplusplus >= 202002L
		return std::string(std::bit_cast<char*>(&value), size);
		#else
		return std::string(reinterpret_cast<char*>(&value), size);
		#endif
	}

	inline std::string from_u8string(const std::u8string& utf8_string_data)
	{
		std::string string_data;

		for(auto& utf8_character : utf8_string_data)
		{
			const char const_utf8_character = static_cast<const char>(utf8_character);
			string_data.push_back( const_utf8_character );
		}

		return string_data;
	}

	inline std::string from_u8string(std::u8string&& utf8_string_data)
	{
		return std::move(std::string(utf8_string_data.begin(), utf8_string_data.end()));
	}

	#endif

	inline std::string from_wstring(const wchar_t* wstring_data)
	{
		std::wstring value = std::wstring(wstring_data, wcslen(wstring_data));

		return std::string(value.begin(), value.end());
	}

	inline std::string from_wstring(const std::wstring& wstring_data)
	{
		return wstring2string(wstring_data);
	}

	inline std::string from_wstring(std::wstring&& wstring_data)
	{
		return std::move(std::string(wstring_data.begin(), wstring_data.end()));
	}

	template <typename IteratorType>
	requires std::input_or_output_iterator<IteratorType>
	std::size_t IteratorOffsetDistance( IteratorType iteratorA, IteratorType iteratorB, std::size_t needOffsetCount )
	{
		//这里是指迭代器（泛型指针）偏移量，是否还有可以移动的距离
		//Here is the iterator (generic pointer) offset, whether there is still a distance that can be moved

		std::size_t iteratorMoveOffset = 0;
		if ( iteratorA != iteratorB )
		{
			std::size_t dataBlockDistanceDiffercnce = static_cast<std::size_t>( std::ranges::distance( iteratorA, iteratorB ) );
			iteratorMoveOffset = std::min( needOffsetCount, dataBlockDistanceDiffercnce );
		}
		return iteratorMoveOffset;
	}

	template <typename RangeType, typename IteratorType>
	requires std::input_or_output_iterator<IteratorType>
	RangeType ImplementationMakeSubrangeContent(IteratorType& iterator, std::size_t dataBlockDistanceDiffercnce, std::size_t& needOffsetCount, bool needUpdateaIterator)
	{
		RecheckRangeSize:

		if (needOffsetCount <= dataBlockDistanceDiffercnce)
		{
			// Sub-ranges are in of range
			//子范围是在范围内的
			RangeType subrange {iterator, iterator + needOffsetCount};

			if (needUpdateaIterator)
			{
				std::ranges::advance(iterator, needOffsetCount);
			}
			return subrange;
		}
		else
		{
			// Sub-ranges are out of range
			//子范围不在范围内
			while (needOffsetCount > dataBlockDistanceDiffercnce)
			{
				--needOffsetCount;
			}

			goto RecheckRangeSize;
		}
	}

	template <typename RangeType, typename IteratorType>
	requires std::input_or_output_iterator<IteratorType>
	RangeType MakeSubrangeContent( IteratorType& iteratorA, IteratorType iteratorB, std::size_t& needOffsetCount, bool needUpdateaIteratorA )
	{
		using RangeValueType = std::ranges::range_value_t<std::remove_cvref_t<RangeType>>;

		if constexpr(std::ranges::range<RangeType>)
		{
			// Sub-range has size
			//子范围有大小
			if (iteratorA != iteratorB)
			{
				std::size_t dataBlockDistanceDiffercnce = static_cast<std::size_t>(std::ranges::distance(iteratorA, iteratorB));
				return ImplementationMakeSubrangeContent<RangeType, IteratorType>(iteratorA, dataBlockDistanceDiffercnce, needOffsetCount, needUpdateaIteratorA);
			}
			else
			{
				return RangeType();
			}
		}
		else
		{
			static_assert(Dependent_Always_Failed<RangeType>, "RangeType is not a ranged container type!");
		}
	}

	//数据块的处理
	//Handling of data blocks
	namespace ProcessingDataBlock
	{
		template<typename Type>
		inline static constexpr bool IsArrayClassType()
		{
			return is_array_class_type<std::remove_cvref_t<Type>>;
		}

		template<typename Type, std::size_t N>
		inline static constexpr bool IsArrayClassType()
		{
			return is_array_class_type<std::remove_cvref_t<Type>>;
		}

		//文件数据拆分与结合
		//File data splitting and merging

		template <typename Range>
		concept DataBlockRangeBase = std::ranges::range<Range> && std::ranges::random_access_range<Range> || std::ranges::contiguous_range<Range>;

		template <typename Range>
		concept DataBlockRange = std::ranges::input_range<Range> || std::ranges::output_range<Range, std::ranges::range_value_t<Range>> || std::input_or_output_iterator<std::ranges::iterator_t<Range>>;

		/*
			Input_Range = std::deque<std::vector<int>>
			InpuOutput_IteratorType = std::back_insert_iterator< std::vector<int> >
			std::ranges::range_value_t<Input_Range> = std::vector<int>
			Type = std::ranges::range_value_t<std::ranges::range_value_t<Input_Range>> = int
			std::output_iterator<Output_IteratorType, Type>
			 => *out++ = an_int
		*/
		template <typename Input_Range, typename Output_IteratorType>
		concept MergerRanges = DataBlockRangeBase<Input_Range> && std::input_iterator<std::ranges::iterator_t<Input_Range>> && std::output_iterator<Output_IteratorType, std::ranges::range_value_t<std::ranges::range_value_t<Input_Range>>>;


		/*
			Input_Range = std::vector<int>
			InpuOutput_IteratorType = std::back_insert_iterator< std::deque<std::vector<int>> >
			std::output_iterator<Output_IteratorType, Input_Range>
			  => *out++ = a_vector_int (Input_Range)
		*/
		template <typename Input_Range, typename Output_IteratorType>
		concept SplitterRanges = DataBlockRangeBase<Input_Range> && std::input_iterator<std::ranges::iterator_t<Input_Range>> && std::output_iterator<Output_IteratorType, Input_Range>;

		//数据块拆分器类
		//Data block splitter class
		struct Splitter
		{
			enum class WorkMode
			{
				Copy,
				Move,
			};

			template <typename Input_Range, typename Output_Range>
			requires DataBlockRange<Input_Range> && DataBlockRange<Output_Range>
			void operator()
			(
				Input_Range&& this_input_range,
				Output_Range&& this_output_range,
				const std::size_t& partition_size,
				WorkMode mode
			)
			{
				using input_range_t = std::remove_cvref_t<Input_Range>;
				using input_range_value_t = std::ranges::range_value_t<input_range_t>;
				using output_range_t = std::remove_cvref_t<Output_Range>;
				using output_subrange_value_t = std::ranges::range_value_t<output_range_t>;

				if ( partition_size <= 0 )
				{
					return;
				}

				auto range_beginIterator = std::ranges::begin( this_input_range );
				auto range_endIterator = std::ranges::end( this_input_range );

				constexpr bool is_key_value_range = std::same_as<std::set<input_range_value_t>, input_range_t> || CommonToolkit::CPP2020_Concepts::IsKeyValueMapType<input_range_t>;
				constexpr bool is_contiguous_range = std::ranges::contiguous_range<output_range_t>;
				constexpr bool is_random_access_range = std::ranges::random_access_range<output_range_t>;

				if constexpr( std::ranges::contiguous_range<input_range_t> && std::ranges::contiguous_range<output_range_t> )
				{
					// Both input and output ranges are contiguous, use memcpy for optimal performance

					auto beginIterator = std::ranges::begin( this_output_range );
					auto endIterator = std::ranges::end( this_output_range );

					while ( range_beginIterator != range_endIterator )
					{
						auto offsetCount = std::min( partition_size, static_cast<std::size_t>( std::ranges::distance( range_beginIterator, range_endIterator ) ) );
						std::vector<input_range_value_t> input_data_buffer(range_beginIterator, std::ranges::next( range_beginIterator, offsetCount ));
						output_subrange_value_t& output_data_buffer = *beginIterator;

						auto* byte_data_pointer = &(*input_data_buffer.begin());
						auto byte_data_size = input_data_buffer.size() * sizeof(input_range_value_t);
						auto* byte_data_pointer2 = &(*output_data_buffer.begin());
						::memcpy(byte_data_pointer2, byte_data_pointer, byte_data_size);

						/*for(std::size_t index = 0; index < input_data_buffer.size() && index < output_data_buffer.size(); ++index)
						{
							output_data_buffer[index] = input_data_buffer[index];
						}*/

						std::ranges::advance( range_beginIterator, offsetCount );

						if( beginIterator != endIterator )
							std::ranges::advance( beginIterator, 1 );
					}

					if ( mode == WorkMode::Move )
					{
						if constexpr(std::destructible<input_range_value_t>)
						{
							for ( auto&& sub_range_container : this_input_range )
							{
								std::destroy_at(std::addressof(sub_range_container));
							}
						}
						else if(std::integral<input_range_value_t> || std::is_pointer_v<input_range_value_t>)
						{
							const input_range_value_t value = 0;
							std::ranges::fill(this_input_range.begin(), this_input_range.end(), value);
						}
					}

					return;
				}
				else
				{
					 // Either input or output range is non-contiguous, use std::ranges::copy

					while ( range_beginIterator != range_endIterator )
					{
						auto offsetCount = std::min( partition_size, static_cast<std::size_t>( std::ranges::distance( range_beginIterator, range_endIterator ) ) );
						output_subrange_value_t sub_range_container( range_beginIterator, range_beginIterator + offsetCount );

						if constexpr ( is_key_value_range )
						{
							this_output_range.emplace_hint( this_output_range.end(), std::move( sub_range_container ) );
							sub_range_container.clear();

							while ( offsetCount != 0 )
							{
								range_beginIterator++;
								--offsetCount;
							}

							continue;
						}
						else if constexpr ( is_random_access_range )
						{
							this_output_range.emplace( this_output_range.end(), std::move( sub_range_container ) );

							sub_range_container.clear();
							range_beginIterator += offsetCount;
						}
						else if constexpr ( is_contiguous_range )
						{
							std::ranges::copy( std::make_move_iterator( sub_range_container.begin() ), std::make_move_iterator( sub_range_container.end() ), this_output_range.end() );
						}
					}

					if ( mode == WorkMode::Move )
					{
						this_input_range.clear();
					}

					return;
				}
			}

			/*
			Use:
			std::deque<std::vector<int>> target;
			std::vector<int> source;

			split(source, std::back_inserter(target), 24);

			*/

			template <typename Input_Range, typename Output_IteratorType>
			requires SplitterRanges<Input_Range, Output_IteratorType>
			void operator()
			(
				Input_Range&& one_input_range,
				Output_IteratorType many_output_range,
				const size_t partition_size
			)
			{
				if ( partition_size <= 0 )
				{
					return;
				}

				auto range_beginIterator = std::ranges::begin( one_input_range );
				auto range_endIterator = std::ranges::end( one_input_range );

				while ( range_beginIterator != range_endIterator )
				{
					auto offsetCount = std::min( partition_size, static_cast<std::size_t>( std::ranges::distance( range_beginIterator, range_endIterator ) ) );
					*many_output_range++ = { range_beginIterator, std::ranges::next( range_beginIterator, offsetCount ) };;
					std::ranges::advance( range_beginIterator, offsetCount );
				}
			}
		};

		inline Splitter splitter;

		//数据块结合器类
		//Data block merger class
		struct Merger
		{
			enum class WorkMode
			{
				Copy,
				Move,
			};

			template <typename Input_Range, typename Output_Range>
			requires DataBlockRange<Input_Range> && DataBlockRange<Output_Range>
			void operator()
			(
				Input_Range&& this_input_range,
				Output_Range&& this_output_range,
				WorkMode mode
			)
			{
				using input_range_t = std::remove_cvref_t<Input_Range>;
				using input_subrange_value_t = std::ranges::range_value_t<input_range_t>;
				using output_range_t = std::remove_cvref_t<Output_Range>;
				using output_range_value_t = std::ranges::range_value_t<output_range_t>;

				constexpr bool input_range_is_array_class_type = IsArrayClassType<input_range_t>();
				constexpr bool input_sub_range_is_array_class_type = IsArrayClassType<input_subrange_value_t>();

				constexpr bool output_range_is_array_class_type = IsArrayClassType<output_range_t>();
				//constexpr bool output_sub_range_is_array_class_type = IsArrayClassType<output_subrange_value_t>();

				if constexpr( std::ranges::contiguous_range<input_range_t> && std::ranges::contiguous_range<output_range_t> )
				{
					// Both input and output ranges are contiguous, use memcpy for optimal performance

					std::size_t byte_pointer_offset = 0;
					for ( auto&& sub_range_container : this_input_range )
					{
						auto* byte_data_pointer = &(*sub_range_container.begin());
						auto byte_data_size = sub_range_container.size() * sizeof(output_range_value_t);
						auto* byte_data_pointer2 = &(*this_output_range.begin());
						::memcpy(byte_data_pointer2 + byte_pointer_offset, byte_data_pointer, byte_data_size);
						byte_pointer_offset += byte_data_size;
					}
				}
				else
				{
					// Either input or output range is non-contiguous, use std::ranges::copy

					for ( auto&& sub_range_container : this_input_range )
					{
						std::ranges::copy(sub_range_container.begin(), sub_range_container.end(), std::back_inserter(this_output_range));
					}
				}

				if ( mode == WorkMode::Move )
				{
					if constexpr(std::destructible<input_subrange_value_t>)
					{
						for ( auto&& sub_range_container : this_input_range )
						{
							std::destroy_at(std::addressof(sub_range_container));
						}
					}
					else if(std::integral<input_subrange_value_t> || std::is_pointer_v<input_subrange_value_t>)
					{
						const input_subrange_value_t value = 0;
						std::ranges::fill(this_input_range.begin(), this_input_range.end(), value);
					}
				}
			}

			/*
			Use:
			std::deque<std::vector<int>> source;
			std::vector<int> target;

			merge(source, std::back_inserter(target));

			*/

			template <typename Input_Range, typename Output_IteratorType>
			requires MergerRanges<Input_Range, Output_IteratorType>
			void operator()( Input_Range&& many, Output_IteratorType one )
			{
				for ( auto&& sub_range : many )
				{
					for ( auto&& value : sub_range )
					{
						*one++ = value;
					}
				}
			}
		};

		inline Merger merger;

	}  // namespace ProcessingDataBlock

	#if defined( TEST_CPP2020_RANGE_MODIFIER )

	void TestCPP2020RangesModifier()
	{
		std::deque<std::vector<int>> source { { 1, 2, 3 }, { 4, 5, 6 } };
		std::vector<int>			 target1;

		ProcessingDataBlock::merger( source, std::back_inserter( target1 ) );
		for ( auto v : target1 )
			std::cout << v << ' ';

		std::cout << '\n';

		std::deque<std::vector<int>> target2;
		ProcessingDataBlock::splitter( target1, std::back_inserter( target2 ), 2 );

		for ( const auto& sub : target2 )
		{
			for ( auto v : sub )
			{
				std::cout << v << ' ';
			}
			std::cout << '\n';
		}
	}

	#endif

}  // namespace CommonToolkit

inline void AnalysisErrorCode( const std::error_code& error_code_object )
{
	const int error_code_number = error_code_object.value();

	#if 0

		if(error_code_number != 0)
		{
			const std::string& error_message = error_code_object.message();
			std::cout << CommonToolkit::from_u8string(u8"发生错误，已获得标准系统错误代码，代码为：") << error_code_number << ", 中止..." << std::endl;
			std::cout << "Error occurred, Standard system error codes have been obtained, code is: " << error_code_number << ", aborting..." << std::endl;
			std::cout << CommonToolkit::from_u8string(u8"The error message is(错误消息是): ") << error_message << std::endl;

			throw std::system_error(error_code_object);
		}

	#else

		if(error_code_number != 0)
		{
			const std::string& error_message = error_code_object.message();
			std::cout << CommonToolkit::from_wstring(L"发生错误，已获得标准系统错误代码，代码为：") << error_code_number << CommonToolkit::from_wstring(L", 中止...") << std::endl;
			std::cout << "Error occurred, Standard system error codes have been obtained, code is: " << error_code_number << ", aborting..." << std::endl;
			std::cout << CommonToolkit::from_wstring(L"The error message is(错误消息是): ") << error_message << std::endl;

			throw std::system_error(error_code_object);
		}

	#endif
}

#endif	// __cplusplus

#if !defined(BYTE_SWAP_FUNCTON) && __cplusplus >= 202002L
#define BYTE_SWAP_FUNCTON
#endif // !BYTE_SWAP_FUNCTON

#if !defined(MEMORY_DATA_TYPE_PACKER_AND_UNPACKER) && __cplusplus >= 202002L
#define MEMORY_DATA_TYPE_PACKER_AND_UNPACKER
#endif // !MEMORY_DATA_TYPE_PACKER_AND_UNPACKER

#if !defined(INTEGER_PACKCATION_OLD) && __cplusplus < 202002L
#define INTEGER_PACKCATION_OLD
#endif // !INTEGER_PACKCATION_OLD

#if !defined(INTEGER_UNPACKCATION_OLD) && __cplusplus < 202002L
#define INTEGER_UNPACKCATION_OLD
#endif // !INTEGER_UNPACKCATION_OLD

namespace CommonToolkit
{
	inline namespace MemoryBits
	{
		struct BitOperations
		{

		public:
			static void ReverseByteArray(const void *Source, void * Destination, std::size_t size)
			{
				std::uint8_t *source_pointer = const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(Source));
				std::uint8_t *destination_pointer = reinterpret_cast<std::uint8_t*>(Destination);

				destination_pointer = destination_pointer + (size - 1);
				for (std::size_t counter = 0; counter < size; ++counter)
				{
					*destination_pointer = *source_pointer;
					source_pointer += 1;
					destination_pointer -= 1;
				} // end for
			} // end function ReverseByteArray

			static std::int32_t ReverseBytesInt32(const std::int32_t value)
			{
				auto integer_a = value & 0xFF;
				auto integer_b = Asr32(value, 8) & 0xFF;
				auto integer_c = Asr32(value, 16) & 0xFF;
				auto integer_d = Asr32(value, 24) & 0xFF;

				return (integer_a << 24) | (integer_b << 16) | (integer_c << 8) | (integer_d << 0);
			} // end function ReverseBytesInt32

			static std::uint8_t ReverseBitsUInt8(const std::uint8_t value)
			{
				std::uint8_t result = ((value >> 1) & 0x55) | ((value << 1) & 0xAA);
				result = ((result >> 2) & 0x33) | ((result << 2) & 0xCC);
				return ((result >> 4) & 0x0F) | ((result << 4) & 0xF0);
			} // end function ReverseBitsUInt8

			static std::uint16_t ReverseBytesUInt16(const std::uint16_t value)
			{
				return ((value & std::uint32_t(0xFF)) << 8 | (value & std::uint32_t(0xFF00)) >> 8);
			} // end function ReverseBytesUInt16

			static std::uint32_t ReverseBytesUInt32(const std::uint32_t value)
			{
				return (value & std::uint32_t(0x000000FF)) << 24 |
					(value & std::uint32_t(0x0000FF00)) << 8 |
					(value & std::uint32_t(0x00FF0000)) >> 8 |
					(value & std::uint32_t(0xFF000000)) >> 24;
			} // end function ReverseBytesUInt32

			static std::uint64_t ReverseBytesUInt64(const std::uint64_t value)
			{
				return (value & std::uint64_t(0x00000000000000FF)) << 56 |
					(value & std::uint64_t(0x000000000000FF00)) << 40 |
					(value & std::uint64_t(0x0000000000FF0000)) << 24 |
					(value & std::uint64_t(0x00000000FF000000)) << 8 |
					(value & std::uint64_t(0x000000FF00000000)) >> 8 |
					(value & std::uint64_t(0x0000FF0000000000)) >> 24 |
					(value & std::uint64_t(0x00FF000000000000)) >> 40 |
					(value & std::uint64_t(0xFF00000000000000)) >> 56;
			} // end function ReverseBytesUInt64

			//Arithmetic bit-shift 32
			static std::int32_t Asr32(const std::int32_t value, const std::int32_t ShiftBits)
			{
				return std::int32_t(std::uint32_t(std::uint32_t(std::uint32_t(value) >> (ShiftBits & 31)) |
					(std::uint32_t(std::int32_t(std::uint32_t(0 - std::uint32_t(std::uint32_t(value) >> 31)) &
						std::uint32_t(std::int32_t(0 - (bool((ShiftBits & 31) != 0)))))) << (32 - (ShiftBits & 31)))));
			} // end function Asr32

			//Arithmetic bit-shift 64
			static std::int64_t Asr64(const std::int64_t value, const std::int32_t ShiftBits)
			{
				return std::int64_t(std::uint64_t(std::uint64_t(std::uint64_t(value) >> (ShiftBits & 63)) |
					(std::uint64_t(std::int64_t(std::uint64_t(0 - std::uint64_t(std::uint64_t(value) >> 63)) &
						std::uint64_t(std::int64_t(size_t(0) - (bool((ShiftBits & 63) != 0)))))) << (64 - (ShiftBits & 63)))));
			} // end function Asr64

		}; // end class Bits

		struct BitConverters
		{

		public:
			static void swap_copy_to_u32
			(
				const void* source_pointer,
				const std::int32_t source_pointer_index,
				void* destination_pointer,
				std::int32_t destination_pointer_index,
				const std::int32_t data_size
			)
			{
				std::uint32_t* lbegin, *ldestination, *lend;
				std::uint8_t* lsource_block;
				std::int32_t ldata_size;

				// if all pointers and length are 32-bits aligned
				if
				(
					((std::int32_t((std::uint8_t*)(destination_pointer)-(std::uint8_t*)(0))
					| ((std::uint8_t*)(source_pointer)-(std::uint8_t*)(0))
					| source_pointer_index
					| destination_pointer_index | data_size) & 3) == 0
				)
				{
					// copy memory as 32-bit words
					lbegin = (std::uint32_t*)const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index);
					lend = (std::uint32_t*)const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index);
					ldestination = (std::uint32_t*)reinterpret_cast<std::uint8_t*>(destination_pointer) + destination_pointer_index;
					while (lbegin < lend)
					{
						*ldestination = BitOperations::ReverseBytesUInt32(*lbegin);
						ldestination += 1;
						lbegin += 1;
					} // end while
				} // end if
				else
				{
					lsource_block = const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index);

					ldata_size = data_size + destination_pointer_index;
					while (destination_pointer_index < ldata_size)
					{
						reinterpret_cast<std::uint8_t*>(destination_pointer)[destination_pointer_index ^ 3] = *lsource_block;

						lsource_block += 1;
						destination_pointer_index += 1;
					} // end while
				} // end else

			} // end function swap_copy_to_u32

			static void swap_copy_to_u64
			(
				const void* source_pointer,
				const std::int32_t source_pointer_index,
				void* destination_pointer,
				std::int32_t destination_pointer_index,
				const std::int32_t data_size
			)
			{
				std::uint64_t* lbegin, * ldestination, * lend;
				std::uint8_t* lsource_block;
				std::int32_t ldata_size;

				// if all pointers and length are 64-bits aligned
				if
				(
					((std::int32_t((std::uint8_t*)(destination_pointer)-(std::uint8_t*)(0))
					| ((std::uint8_t*)(source_pointer)-(std::uint8_t*)(0))
					| source_pointer_index
					| destination_pointer_index
					| data_size) & 7) == 0
				)
				{
					// copy aligned memory block as 64-bit integers
					lbegin = (std::uint64_t*)const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index);
					lend = (std::uint64_t*)const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index) + data_size;
					ldestination = (std::uint64_t*)reinterpret_cast<std::uint8_t*>(destination_pointer) + destination_pointer_index;
					while (lbegin < lend)
					{
						*ldestination = BitOperations::ReverseBytesUInt64(*lbegin);
						ldestination += 1;
						lbegin += 1;
					} // end while
				} // end if
				else
				{
					lsource_block = const_cast<std::uint8_t*>(reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index);

					ldata_size = data_size + destination_pointer_index;
					while (destination_pointer_index < ldata_size)
					{
						reinterpret_cast<std::uint8_t*>(destination_pointer)[destination_pointer_index ^ 7] = *lsource_block;

						lsource_block += 1;
						destination_pointer_index += 1;
					} // end while
				} // end else
			} // end function swap_copy_to_u64

			static std::uint32_t be2me_32(const std::uint32_t number)
			{
				if constexpr(std::endian::native == std::endian::little)
				{
					return BitOperations::ReverseBytesUInt32(number);
				} // end if

				return number;
			} // end function be2me_32

			static std::uint64_t be2me_64(const std::uint64_t number)
			{
				if constexpr(std::endian::native == std::endian::little)
				{
					return BitOperations::ReverseBytesUInt64(number);
				} // end if

				return number;
			} // end function be2me_64

			static std::uint32_t le2me_32(const std::uint32_t number)
			{
				if constexpr(std::endian::native != std::endian::little)
				{
					return BitOperations::ReverseBytesUInt32(number);
				} // end if

				return number;
			} // end function le2me_32

			static std::uint64_t le2me_64(const std::uint64_t number)
			{
				if constexpr(std::endian::native != std::endian::little)
				{
					return BitOperations::ReverseBytesUInt64(number);
				} // end if

				return number;
			} // end function le2me_64

			//Big endian 32bit(copy memory)
			static void be32_copy
			(
				const void* source_pointer,
				const std::int32_t source_pointer_index,
				void* destination_pointer,
				const std::int32_t destination_pointer_index,
				const std::int32_t data_size
			)
			{
				if constexpr(std::endian::native != std::endian::big)
				{
					BitConverters::swap_copy_to_u32
					(
						source_pointer,
						source_pointer_index,
						destination_pointer,
						destination_pointer_index,
						data_size
					);
				} // end if
				else
				{
					if(destination_pointer == nullptr || source_pointer == nullptr)
						my_cpp2020_assert(false, "The memory address pointer cannot be a null pointer!", std::source_location::current());

					if(data_size == 0)
						return;

					::memmove
					(
						reinterpret_cast<std::uint8_t*>(destination_pointer) + destination_pointer_index,
						reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index,
						data_size
					);
				} // end else
			} // end function be32_copy

			//Big endian 64bit(copy memory)
			static void be64_copy
			(
				const void* source_pointer,
				const std::int32_t source_pointer_index,
				void* destination_pointer,
				const std::int32_t destination_pointer_index,
				const std::int32_t data_size
			)
			{
				if constexpr(std::endian::native != std::endian::big)
				{
					BitConverters::swap_copy_to_u64
					(
						source_pointer,
						source_pointer_index,
						destination_pointer,
						destination_pointer_index,
						data_size
					);
				} // end if
				else
				{

					if(destination_pointer == nullptr || source_pointer == nullptr)
						my_cpp2020_assert(false, "The memory address pointer cannot be a null pointer!", std::source_location::current());

					if(data_size == 0)
						return;

					::memmove
					(
						reinterpret_cast<std::uint8_t*>(destination_pointer) + destination_pointer_index,
						reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index,
						data_size
					);
				} // end else
			} // end function be64_copy

			//Liitle endian 32bit(copy memory)
			static void le32_copy
			(
				const void* source_pointer,
				const std::int32_t source_pointer_index,
				void* destination_pointer,
				const std::int32_t destination_pointer_index,
				const std::int32_t data_size
			)
			{
				if constexpr(std::endian::native != std::endian::little)
				{
					BitConverters::swap_copy_to_u32
					(
						source_pointer,
						source_pointer_index,
						destination_pointer,
						destination_pointer_index,
						data_size
					);
				} // end if
				else
				{
					if(destination_pointer == nullptr || source_pointer == nullptr)
						my_cpp2020_assert(false, "The memory address pointer cannot be a null pointer!", std::source_location::current());

					if(data_size == 0)
						return;

					::memmove
					(
						reinterpret_cast<std::uint8_t*>(destination_pointer) + destination_pointer_index,
						reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index,
						data_size
					);
				} // end else
			} // end function be64_copy

			//Liitle endian 64bit(copy memory)
			static void le64_copy
			(
				const void* source_pointer,
				const std::int32_t source_pointer_index,
				void* destination_pointer,
				const std::int32_t destination_pointer_index,
				const std::int32_t data_size
			)
			{
				if constexpr(std::endian::native != std::endian::little)
				{
					BitConverters::swap_copy_to_u64
					(
						source_pointer,
						source_pointer_index,
						destination_pointer,
						destination_pointer_index,
						data_size
					);
				} // end if
				else
				{
					if(destination_pointer == nullptr || source_pointer == nullptr)
						my_cpp2020_assert(false, "The memory address pointer cannot be a null pointer!", std::source_location::current());

					if(data_size == 0)
						return;

					::memmove
					(
						reinterpret_cast<std::uint8_t*>(destination_pointer) + destination_pointer_index,
						reinterpret_cast<const std::uint8_t*>(source_pointer) + source_pointer_index,
						data_size
					);
				} // end else
			} // end function be64_copy

		};
	}

	inline namespace IntegerExchangeBytes
	{
		// unpackInteger convert unsigned long long int to array<byte, 8>
		// packInteger convert array of byte to specific integer type
		template <typename IntegerType>
		requires std::is_integral_v<IntegerType>
		constexpr auto unpackInteger( IntegerType data )
		{
			constexpr auto					 byteCount = std::numeric_limits<IntegerType>::digits / 8;
			std::array<std::byte, byteCount> answer;
			for ( int index = byteCount - 1; index >= 0; --index )
			{
				answer[ index ] = static_cast<std::byte>( data & 0xFF );
				data >>= 8;
			}
			return answer;
		}
		inline constexpr TwoByte packInteger( SpanTwoByte data )
		{
			return ( static_cast<TwoByte>( data[ 0 ] ) << 8 ) | ( static_cast<TwoByte>( data[ 1 ] ) );
		}
		inline constexpr FourByte packInteger( SpanFourByte data )
		{
			return ( static_cast<FourByte>( data[ 0 ] ) << 24 ) | ( static_cast<FourByte>( data[ 1 ] ) << 16 ) | ( static_cast<FourByte>( data[ 2 ] ) << 8 ) | ( static_cast<FourByte>( data[ 3 ] ) );
		}
		inline constexpr EightByte packInteger( SpanEightByte data )
		{
			return ( static_cast<EightByte>( packInteger( SpanFourByte{ data.begin(), 4u } ) ) << 32 ) | static_cast<EightByte>( packInteger( SpanFourByte{ data.begin() + 4, 4u } ) );
		}

		#if defined( BYTE_SWAP_FUNCTON )

		/*
			Reference source code: https://gist.github.com/raidoz/4163b8ec6672aabb0656b96692af5e33
			cross-platform / cross-compiler standalone endianness conversion
		*/
		namespace ByteSwap
		{
			namespace Implementation
			{
				inline std::uint16_t _builtin_byteswap_uint16(const std::uint16_t& value)
				{
					unsigned short other_value = 0;
					other_value =  (value << 8);
					other_value += (value >> 8);
					return other_value;
				}

				inline std::uint32_t _builtin_byteswap_uint32(const std::uint32_t& value)
				{
					unsigned int other_value = 0;
					other_value =  (value << 24);
					other_value += (value <<  8) & 0x00FF0000;
					other_value += (value >>  8) & 0x0000FF00;
					other_value += (value >> 24);
					return other_value;
				}

				inline std::uint64_t _builtin_byteswap_uint64(const std::uint64_t& value)
				{
					unsigned long long other_value = 0;
					other_value =  (value << 56);
					other_value += (value << 40) & 0x00FF000000000000;
					other_value += (value << 24) & 0x0000FF0000000000;
					other_value += (value <<  8) & 0x000000FF00000000;
					other_value += (value >>  8) & 0x00000000FF000000;
					other_value += (value >> 24) & 0x0000000000FF0000;
					other_value += (value >> 40) & 0x000000000000FF00;
					other_value += (value >> 56);
					return other_value;
				}

				//! C++ Byte-swap 16-bit unsigned short
				[[nodiscard]] static inline constexpr std::uint16_t Byteswap(const std::uint16_t& ByteValue) noexcept
				{
					if (std::is_constant_evaluated())
					{
						#if defined(_MSC_VER)

						return static_cast<unsigned short>((ByteValue << 8) | (ByteValue >> 8));

						#else

						return ((( ByteValue  >> 8 ) & 0xffu ) | (( ByteValue  & 0xffu ) << 8 ));

						#endif
					}
					else
					{
						return _builtin_byteswap_uint16(ByteValue);
					}
				}
 
				//Type unsigned long equal to type unsigned int
				//! C++ Byte-swap 32-bit unsigned int
				[[nodiscard]] static inline constexpr std::uint32_t Byteswap(const std::uint32_t& ByteValue) noexcept
				{
					if (std::is_constant_evaluated())
					{
						#if defined(_MSC_VER)

						return (ByteValue << 24) | ((ByteValue << 8) & 0x00FF'0000) | ((ByteValue >> 8) & 0x0000'FF00) | (ByteValue >> 24);

						#else

						return ((( ByteValue & 0xff000000u ) >> 24 ) |
								(( ByteValue & 0x00ff0000u ) >> 8  ) |
								(( ByteValue & 0x0000ff00u ) << 8  ) |
								(( ByteValue & 0x000000ffu ) << 24 ));

						#endif
					}
					else
					{
						return _builtin_byteswap_uint32(ByteValue);
					}
				}

				//! C++ Byte-swap 64-bit unsigned long long
				[[nodiscard]] static inline constexpr std::uint64_t Byteswap(const std::uint64_t& ByteValue) noexcept
				{
					if (std::is_constant_evaluated())
					{
						#if defined(_MSC_VER)

						return (ByteValue << 56) | ((ByteValue << 40) & 0x00FF'0000'0000'0000) | ((ByteValue << 24) & 0x0000'FF00'0000'0000) |
							   ((ByteValue << 8) & 0x0000'00FF'0000'0000) | ((ByteValue >> 8) & 0x0000'0000'FF00'0000) |
							   ((ByteValue >> 24) & 0x0000'0000'00FF'0000) | ((ByteValue >> 40) & 0x0000'0000'0000'FF00) | (ByteValue >> 56);

						#else

						return ((( ByteValue & 0xff00000000000000ull ) >> 56 ) |
								(( ByteValue & 0x00ff000000000000ull ) >> 40 ) |
								(( ByteValue & 0x0000ff0000000000ull ) >> 24 ) |
								(( ByteValue & 0x000000ff00000000ull ) >> 8  ) |
								(( ByteValue & 0x00000000ff000000ull ) << 8  ) |
								(( ByteValue & 0x0000000000ff0000ull ) << 24 ) |
								(( ByteValue & 0x000000000000ff00ull ) << 40 ) |
								(( ByteValue & 0x00000000000000ffull ) << 56 ));

						#endif
					}
					else
					{
						return _builtin_byteswap_uint64(ByteValue);
					}
				}

				//! C++ Byte-swap 32-bit float
				static inline float Byteswap(float ByteValue)
				{
					#ifdef __cplusplus
						static_assert(sizeof(float) == sizeof(uint32_t), "Unexpected float format");
						/* Problem: de-referencing float pointer as uint32_t breaks strict-aliasing rules for C++ and C, even if it normally works
						 *   uint32_t val = bswap32(*(reinterpret_cast<const uint32_t *>(&f)));
						 *   return *(reinterpret_cast<float *>(&val));
						 */
						// memcpy approach is guaranteed to work in C & C++ and fn calls should be optimized out:
						uint32_t asInt;
						::memcpy(&asInt, reinterpret_cast<const void *>(&ByteValue), sizeof(uint32_t));
						asInt = Byteswap(asInt);
						::memcpy(&ByteValue, reinterpret_cast<void *>(&asInt), sizeof(float));
						return ByteValue;
					#else
						_Static_assert(sizeof(float) == sizeof(uint32_t), "Unexpected float format");
						// union approach is guaranteed to work in C99 and later (but not in C++, though in practice it normally will):
						union { uint32_t asInt; float asFloat; } conversion_union;
						conversion_union.asFloat = ByteValue;
						conversion_union.asInt = Byteswap(conversion_union.asInt);
						return conversion_union.asFloat;
					#endif
				}

				//! C++ Byte-swap 64-bit double
				static inline double Byteswap(double ByteValue)
				{
					#ifdef __cplusplus
						static_assert(sizeof(double) == sizeof(uint64_t), "Unexpected double format");
						uint64_t asInt;
						::memcpy(&asInt, reinterpret_cast<const void *>(&ByteValue), sizeof(uint64_t));
						asInt = Byteswap(asInt);
						::memcpy(&ByteValue, reinterpret_cast<void *>(&asInt), sizeof(double));
						return ByteValue;
					#else
						_Static_assert(sizeof(double) == sizeof(uint64_t), "Unexpected double format");
						union { uint64_t asInt; double asDouble; } conversion_union;
						conversion_union.asDouble = ByteValue;
						conversion_union.asInt = Byteswap(conversion_union.asInt);
						return conversion_union.asDouble;
					#endif
				}
			}

			template <class Type> requires std::is_integral_v<Type>
			[[nodiscard]] constexpr Type byteswap(const Type ByteValue) noexcept
			{
				using ThisType = std::remove_cvref_t<Type>;

				if constexpr (sizeof(ThisType) == 1)
				{
					return ByteValue;
				}
				else if constexpr (sizeof(ThisType) == 2)
				{
					return static_cast<ThisType>(Implementation::Byteswap(static_cast<std::uint16_t>(ByteValue)));
				}
				else if constexpr (sizeof(ThisType) == 4)
				{
					return static_cast<Type>(Implementation::Byteswap(static_cast<std::uint32_t>(ByteValue)));
				}
				else if constexpr (sizeof(ThisType) == 8)
				{
					return static_cast<ThisType>(Implementation::Byteswap(static_cast<std::uint64_t>(ByteValue)));
				}
				else if constexpr (std::same_as<ThisType, float>)
				{
					return static_cast<Type>(Implementation::Byteswap(static_cast<float>(ByteValue)));
				}
				else if constexpr (std::same_as<ThisType, double>)
				{
					return static_cast<Type>(Implementation::Byteswap(static_cast<double>(ByteValue)));
				}
				else
				{
					static_assert(CommonToolkit::Dependent_Always_Failed<ThisType>, "Unexpected integer size");
				}
			}
		}

		#endif

		#if defined(MEMORY_DATA_TYPE_PACKER_AND_UNPACKER)

		class MemoryDataFormatExchange
		{

		private:
			std::array<std::uint8_t, 2> twobyte_array { 0, 0 };
			std::array<std::uint8_t, 4> fourbyte_array { 0, 0, 0, 0 };
			std::array<std::uint8_t, 8> eightbyte_array { 0, 0, 0, 0, 0, 0, 0, 0 };

		public:
			std::uint16_t Packer_2Byte(std::span<const std::uint8_t> bytes)
			{
				my_cpp2020_assert(bytes.size() == 2, "The required byte array size is 2", std::source_location::current());

				#if 0

				auto ValueA = bytes.operator[](0);
				auto ValueB = bytes.operator[](1);

				std::uint16_t integer = ValueA & 0xFF;
				integer |= ((static_cast<std::uint16_t>(ValueB) << 8) & 0xFF00);

				#else

				std::uint16_t integer = 0;
				::memcpy(&integer, bytes.data(), bytes.size_bytes());

				#endif

				if constexpr(std::endian::native == std::endian::big)
				{
					#if __cpp_lib_byteswap

					integer = std::byteswap(integer);

					#else

					integer = ByteSwap::byteswap(integer);

					#endif
				}

				return integer;
			}

			std::span<std::uint8_t> Unpacker_2Byte(std::uint16_t integer)
			{
				if constexpr(std::endian::native == std::endian::big)
				{
					#if __cpp_lib_byteswap

					integer = std::byteswap(integer);

					#else

					integer = ByteSwap::byteswap(integer);

					#endif
				}

				#if 0

				twobyte_array.fill(0);
				std::span<std::uint8_t> bytes { twobyte_array };
				bytes.operator[](0) = (integer & 0x000000FF);
				bytes.operator[](1) = (integer & 0x0000FF00) >> 8;

				#else

				std::span<std::uint8_t> bytes { twobyte_array };
				::memcpy(bytes.data(), &integer, bytes.size_bytes());

				#endif

				return bytes;
			}

			std::uint32_t Packer_4Byte(std::span<const std::uint8_t> bytes)
			{
				my_cpp2020_assert(bytes.size() == 4, "The required byte array size is 4", std::source_location::current());

				#if 0

				auto ValueA = bytes.operator[](0);
				auto ValueB = bytes.operator[](1);
				auto ValueC = bytes.operator[](2);
				auto ValueD = bytes.operator[](3);

				std::uint32_t integer = ValueA & 0xFF;
				integer |= ((static_cast<std::uint32_t>(ValueB) << 8) & 0xFF00);
				integer |= ((static_cast<std::uint32_t>(ValueC) << 16) & 0xFF0000);
				integer |= ((static_cast<std::uint32_t>(ValueD) << 24) & 0xFF000000);

				#else

				std::uint32_t integer = 0;
				::memcpy(&integer, bytes.data(), bytes.size_bytes());

				#endif

				if constexpr(std::endian::native == std::endian::big)
				{
					#if __cpp_lib_byteswap

					integer = std::byteswap(integer);

					#else

					integer = ByteSwap::byteswap(integer);

					#endif
				}

				return integer;
			}

			std::span<std::uint8_t> Unpacker_4Byte(std::uint32_t integer)
			{
				if constexpr(std::endian::native == std::endian::big)
				{
					#if __cpp_lib_byteswap

					integer = std::byteswap(integer);

					#else

					integer = ByteSwap::byteswap(integer);

					#endif
				}

				#if 0

				fourbyte_array.fill(0);
				std::span<std::uint8_t> bytes { fourbyte_array };
				bytes.operator[](0) = (integer & 0x000000FF);
				bytes.operator[](1) = (integer & 0x0000FF00) >> 8;
				bytes.operator[](2) = (integer & 0x00FF0000) >> 16;
				bytes.operator[](3) = (integer & 0xFF000000) >> 24;

				#else

				std::span<std::uint8_t> bytes { fourbyte_array };
				::memcpy(bytes.data(), &integer, bytes.size_bytes());

				#endif

				return bytes;
			}

			std::uint64_t Packer_8Byte(std::span<const std::uint8_t> bytes)
			{
				my_cpp2020_assert(bytes.size() == 8, "The required byte array size is 8", std::source_location::current());

				#if 0

				auto ValueA = bytes.operator[](0);
				auto ValueB = bytes.operator[](1);
				auto ValueC = bytes.operator[](2);
				auto ValueD = bytes.operator[](3);
				auto ValueE = bytes.operator[](4);
				auto ValueF = bytes.operator[](5);
				auto ValueG = bytes.operator[](6);
				auto ValueH = bytes.operator[](7);

				std::uint64_t integer = ValueA & 0xFF;
				integer |= ((static_cast<std::uint64_t>(ValueB) << 8) & 0xFF00);
				integer |= ((static_cast<std::uint64_t>(ValueC) << 16) & 0xFF0000);
				integer |= ((static_cast<std::uint64_t>(ValueD) << 24) & 0xFF000000);
				integer |= ((static_cast<std::uint64_t>(ValueE) << 32) & 0xFF00000000);
				integer |= ((static_cast<std::uint64_t>(ValueF) << 40) & 0xFF0000000000);
				integer |= ((static_cast<std::uint64_t>(ValueG) << 48) & 0xFF000000000000);
				integer |= ((static_cast<std::uint64_t>(ValueH) << 56) & 0xFF00000000000000);

				#else

				std::uint64_t integer = 0;
				::memcpy(&integer, bytes.data(), bytes.size_bytes());

				#endif

				if constexpr(std::endian::native == std::endian::big)
				{
					#if __cpp_lib_byteswap

					integer = std::byteswap(integer);

					#else

					integer = ByteSwap::byteswap(integer);

					#endif
				}

				return integer;
			}

			std::span<std::uint8_t> Unpacker_8Byte(std::uint64_t integer)
			{
				if constexpr(std::endian::native == std::endian::big)
				{
					#if __cpp_lib_byteswap

					integer = std::byteswap(integer);

					#else

					integer = ByteSwap::byteswap(integer);

					#endif
				}

				#if 0

				eightbyte_array.fill(0);
				std::span<std::uint8_t> bytes { eightbyte_array };
				bytes.operator[](0) = (integer & 0x00000000000000FF);
				bytes.operator[](1) = (integer & 0x000000000000FF00) >> 8;
				bytes.operator[](2) = (integer & 0x0000000000FF0000) >> 16;
				bytes.operator[](3) = (integer & 0x00000000FF000000) >> 24;
				bytes.operator[](4) = (integer & 0x000000FF00000000) >> 32;
				bytes.operator[](5) = (integer & 0x0000FF0000000000) >> 40;
				bytes.operator[](6) = (integer & 0x00FF000000000000) >> 48;
				bytes.operator[](7) = (integer & 0xFF00000000000000) >> 56;

				#else

				std::span<std::uint8_t> bytes { eightbyte_array };
				::memcpy(bytes.data(), &integer, bytes.size_bytes());

				#endif

				return bytes;
			}

			MemoryDataFormatExchange() = default;
			~MemoryDataFormatExchange() = default;

		};

		template<typename IntegerType, typename ByteType>
		concept BytesExchangeIntegersConecpt = std::is_integral_v<std::remove_cvref_t<IntegerType>> && std::is_same_v<std::remove_cvref_t<ByteType>, unsigned char> || std::is_same_v<std::remove_cvref_t<ByteType>, std::byte>;

		/*

			//Example Code:

			std::deque<unsigned char> Word;

			unsigned int InputWord = 0;
			unsigned int OutputWord = 0;
			std::vector<std::byte> bytes
			{
				static_cast<std::byte>(Word.operator[](0)),
				static_cast<std::byte>(Word.operator[](1)),
				static_cast<std::byte>(Word.operator[](2)),
				static_cast<std::byte>(Word.operator[](3))
			};

			std::span<std::byte> byteSpan{ bytes.begin(), bytes.end() };
			CommonToolkit::MessagePacking<unsigned int>(byteSpan, &InputWord);

			OutputWord = (InputWord << 8) | (InputWord >> 24);

			std::vector<unsigned int> words
			{
				OutputWord
			};
			std::span<unsigned int> wordSpan{ words };
			CommonToolkit::MessageUnpacking<unsigned int>(wordSpan, bytes.data());

			Word.operator[](0) = static_cast<unsigned char>(bytes.operator[](0));
			Word.operator[](1) = static_cast<unsigned char>(bytes.operator[](1));
			Word.operator[](2) = static_cast<unsigned char>(bytes.operator[](2));
			Word.operator[](3) = static_cast<unsigned char>(bytes.operator[](3));

			bytes.clear();
			words.clear();

		*/

		template<typename IntegerType, typename ByteType>
		requires BytesExchangeIntegersConecpt<IntegerType, ByteType>
		void MessagePacking(const std::span<const ByteType>& input, IntegerType* output)
		{
			if constexpr((std::endian::native != std::endian::big) && (std::endian::native != std::endian::little))
			{
				throw std::invalid_argument("The byte order of your system's devices is unknown!");
			}
			
			if(input.size() % sizeof(IntegerType) != 0)
			{
				throw std::length_error("The size of the data must be aligned with the size of the type!");
			}

			if(output == nullptr)
			{
				throw std::logic_error("The target of the copied byte must not be a null pointer!");
			}

			constexpr bool whether_not_need_byteswap = (std::endian::native == std::endian::little);

			if constexpr (whether_not_need_byteswap)
			{
				::memcpy(output, input.data(), input.size());
			}
			else
			{
				auto begin = input.data();
				auto end = input.data() + input.size();
				for (auto iterator = begin; iterator != end; iterator += sizeof(IntegerType))
				{
					IntegerType value;
					::memcpy(&value, iterator, sizeof(IntegerType));

					#if __cpp_lib_byteswap

					*output++ = std::byteswap(value);

					#else

					*output++ = ByteSwap::byteswap(value);

					#endif
				}
			}
		}

		template<typename IntegerType, typename ByteType>
		requires BytesExchangeIntegersConecpt<IntegerType, ByteType>
		std::vector<IntegerType> MessagePacking(const ByteType* input_pointer, std::size_t input_size)
		{
			if constexpr((std::endian::native != std::endian::big) && (std::endian::native != std::endian::little))
			{
				std::cout << "The byte order of your system's devices is unknown!" << std::endl;
				throw std::invalid_argument("");
			}

			if(input_pointer == nullptr)
				throw std::logic_error("The source of the copied byte must not be a null pointer!");

			if(input_size == 0)
				throw std::logic_error("The source size of the copied bytes cannot be 0!");
			else if (input_size % sizeof(IntegerType) != 0)
				throw std::length_error("The size of the data must be aligned with the size of the type!");
			else
			{
				std::vector<IntegerType> output_vector(input_size / sizeof(IntegerType), 0);

				::memcpy(output_vector.data(), input_pointer, input_size);

				constexpr bool whether_need_byteswap = (std::endian::native == std::endian::big);

				if constexpr(whether_need_byteswap)
				{
					std::span<IntegerType> temporary_span { output_vector.data(), output_vector.size() };

					for(auto& temporary_value : temporary_span )
					{
						#if __cpp_lib_byteswap

						input_value = std::byteswap(value);

						#else

						temporary_value = ByteSwap::byteswap(temporary_value);

						#endif
					}
				}

				return output_vector;
			}
		}

		/*

			//Example Code:

			std::deque<unsigned char> Word;

			unsigned int InputWord = 0;
			unsigned int OutputWord = 0;
			std::vector<std::byte> bytes
			{
				static_cast<std::byte>(Word.operator[](0)),
				static_cast<std::byte>(Word.operator[](1)),
				static_cast<std::byte>(Word.operator[](2)),
				static_cast<std::byte>(Word.operator[](3))
			};

			std::span<std::byte> byteSpan{ bytes.begin(), bytes.end() };
			CommonToolkit::MessagePacking<unsigned int>(byteSpan, &InputWord);

			OutputWord = (InputWord << 8) | (InputWord >> 24);

			std::vector<unsigned int> words
			{
				OutputWord
			};
			std::span<unsigned int> wordSpan{ words };
			CommonToolkit::MessageUnpacking<unsigned int>(wordSpan, bytes.data());

			Word.operator[](0) = static_cast<unsigned char>(bytes.operator[](0));
			Word.operator[](1) = static_cast<unsigned char>(bytes.operator[](1));
			Word.operator[](2) = static_cast<unsigned char>(bytes.operator[](2));
			Word.operator[](3) = static_cast<unsigned char>(bytes.operator[](3));

			bytes.clear();
			words.clear();

		*/

		template<typename IntegerType, typename ByteType>
		requires BytesExchangeIntegersConecpt<IntegerType, ByteType>
		void MessageUnpacking(const std::span<const IntegerType>& input, ByteType* output)
		{
			if constexpr((std::endian::native != std::endian::big) && (std::endian::native != std::endian::little))
			{
				std::cout << "The byte order of your system's devices is unknown!" << std::endl;
				throw std::invalid_argument("");
			}
			
			if(output == nullptr)
			{
				throw std::logic_error("The target of the copied byte must not be a null pointer!");
			}

			constexpr bool whether_not_need_byteswap = (std::endian::native == std::endian::little);

			if constexpr (whether_not_need_byteswap)
			{
				::memcpy(output, input.data(), input.size() * sizeof(IntegerType));
			}
			else
			{
				// intentional copy
				for (IntegerType value : input)
				{
					#if __cpp_lib_byteswap

					value = std::byteswap(value);

					#else

					value = ByteSwap::byteswap(value);

					#endif

					::memcpy(output, &value, sizeof(IntegerType));
					output += sizeof(IntegerType);
				}
			}
		}

		template<typename IntegerType, typename ByteType>
		requires BytesExchangeIntegersConecpt<IntegerType, ByteType>
		std::vector<ByteType> MessageUnpacking(const IntegerType* input_pointer, std::size_t input_size)
		{
			if constexpr((std::endian::native != std::endian::big) && (std::endian::native != std::endian::little))
			{
				std::cout << "The byte order of your system's devices is unknown!" << std::endl;
				throw std::invalid_argument("");
			}

			if(input_pointer == nullptr)
				throw std::logic_error("The source of the copied byte must not be a null pointer!");

			if(input_size == 0)
				throw std::logic_error("The source size of the copied bytes cannot be 0!");
			else
			{
				std::vector<IntegerType> temporary_vector(input_pointer, input_pointer + input_size);

				constexpr bool whether_need_byteswap = (std::endian::native == std::endian::big);

				if constexpr(whether_need_byteswap)
				{
					std::span<IntegerType> temporary_span { temporary_vector.begin(), temporary_vector.end() };

					for(auto& temporary_value : temporary_span )
					{
						#if __cpp_lib_byteswap

						input_value = std::byteswap(value);

						#else

						temporary_value = ByteSwap::byteswap(temporary_value);

						#endif
					}

					std::vector<ByteType> output_vector(input_size * sizeof(IntegerType), 0);

					::memcpy(output_vector.data(), temporary_vector.data(), output_vector.size());

					return output_vector;
				}
				else
				{
					std::vector<ByteType> output_vector(input_size * sizeof(IntegerType), 0);

					::memcpy(output_vector.data(), input_pointer, output_vector.size());

					return output_vector;
				}
			}
		}

		template <typename ByteType, std::size_t Size>
		requires std::is_same_v<std::remove_cvref_t<ByteType>, unsigned char> || std::is_same_v<std::remove_cvref_t<ByteType>, std::byte>
		auto bytes_order_fixup(std::span<const ByteType> bytes)
		{
			auto buffer_bytes = std::array<ByteType, Size>{};
			if constexpr (std::endian::native == std::endian::little)
				std::copy(bytes.data(), bytes.data() + Size, buffer_bytes.data());
			else
				std::reverse_copy(bytes.data(), bytes.data() + Size, buffer_bytes.data());
			return buffer_bytes;
		}

		template <typename IntegerType, typename ByteType>
		requires BytesExchangeIntegersConecpt<IntegerType, ByteType>
		auto value_to_bytes(const IntegerType& value)
		{
			auto bytes = std::array<ByteType, sizeof(IntegerType)>{};
			::memcpy(bytes.data(), &value, sizeof(IntegerType));
			return bytes_order_fixup<ByteType, sizeof(IntegerType)>(bytes);
		}

		template <typename IntegerType, typename ByteType>
		requires BytesExchangeIntegersConecpt<IntegerType, ByteType>
		auto value_from_bytes(std::span<const ByteType> bytes)
		{
			my_cpp2020_assert(bytes.size() == sizeof(IntegerType), "", std::source_location::current());
			auto buffer_bytes = bytes_order_fixup<ByteType, sizeof(IntegerType)>(bytes);
			auto value = IntegerType{};
			::memcpy(&value, buffer_bytes.data(), sizeof(IntegerType));
			return value;
		}

		#endif

		#if defined( INTEGER_PACKCATION_OLD )

		inline int32_t ByteArrayToInteger32Bit( const std::vector<unsigned char>& temporaryBytes )
		{
			auto& ValueA = temporaryBytes.operator[](0);
			auto& ValueB = temporaryBytes.operator[](1);
			auto& ValueC = temporaryBytes.operator[](2);
			auto& ValueD = temporaryBytes.operator[](3);

			int32_t number = ValueA & 0xFF;
			number |= ((static_cast<int32_t>(ValueB) << 8) & 0xFF00);
			number |= ((static_cast<int32_t>(ValueC) << 16) & 0xFF0000);
			number |= ((static_cast<int32_t>(ValueD) << 24) & 0xFF000000);
			return number;
		}

		inline int64_t ByteArrayToInteger64Bit( const std::vector<unsigned char>& temporaryBytes )
		{
			auto& ValueA = temporaryBytes.operator[](0);
			auto& ValueB = temporaryBytes.operator[](1);
			auto& ValueC = temporaryBytes.operator[](2);
			auto& ValueD = temporaryBytes.operator[](3);
			auto& ValueE = temporaryBytes.operator[](4);
			auto& ValueF = temporaryBytes.operator[](5);
			auto& ValueG = temporaryBytes.operator[](6);
			auto& ValueH = temporaryBytes.operator[](7);

			int64_t number = ValueA & 0xFF;
			number |= ((static_cast<int64_t>(ValueB) << 8) & 0xFF00);
			number |= ((static_cast<int64_t>(ValueC) << 16) & 0xFF0000);
			number |= ((static_cast<int64_t>(ValueD) << 24) & 0xFF000000);
			number |= ((static_cast<int64_t>(ValueE) << 32) & 0xFF00000000);
			number |= ((static_cast<int64_t>(ValueF) << 40) & 0xFF0000000000);
			number |= ((static_cast<int64_t>(ValueG) << 48) & 0xFF000000000000);
			number |= ((static_cast<int64_t>(ValueH) << 56) & 0xFF00000000000000);
			return number;
		}

		//Turn byte 8bit array to integer 32bit
		inline void MessagePacking32Bit( const std::vector<uint8_t>& input, std::vector<uint32_t>& output )
		{
			std::vector<unsigned char> temporaryBytes = std::vector<unsigned char>();

			auto begin = input.begin(), end = input.end();
			while(begin != end)
			{
				std::size_t iteratorMoveOffset = 0;
				std::size_t dataBlockDistanceDiffercnce = static_cast<std::size_t>( std::ranges::distance( begin, end ) );
				iteratorMoveOffset = std::min( static_cast<std::size_t>(4), dataBlockDistanceDiffercnce );

				temporaryBytes.insert(temporaryBytes.begin(), begin, begin + iteratorMoveOffset);
				int32_t value = ByteArrayToInteger32Bit(temporaryBytes);
				output.push_back(static_cast<uint32_t>(value));

				temporaryBytes.clear();

				begin += iteratorMoveOffset;
			}
		}

		//Turn byte 8bit array to integer 64bit
		inline void MessagePacking64Bit( const std::vector<uint8_t>& input, std::vector<uint64_t>& output )
		{
			std::vector<unsigned char> temporaryBytes = std::vector<unsigned char>();

			auto begin = input.begin(), end = input.end();
			while(begin != end)
			{
				std::size_t iteratorMoveOffset = 0;
				std::size_t dataBlockDistanceDiffercnce = static_cast<std::size_t>( std::ranges::distance( begin, end ) );
				iteratorMoveOffset = std::min( static_cast<std::size_t>(8), dataBlockDistanceDiffercnce );

				temporaryBytes.insert(temporaryBytes.begin(), begin, begin + iteratorMoveOffset);
				int64_t value = ByteArrayToInteger64Bit(temporaryBytes);
				output.push_back(static_cast<uint64_t>(value));

				temporaryBytes.clear();

				begin += iteratorMoveOffset;
			}
		}

		#endif

		#if defined( INTEGER_UNPACKCATION_OLD )

		inline std::vector<unsigned char> ByteArrayFromInteger32Bit( const int32_t& number, std::vector<unsigned char>& temporaryBytes )
		{
			temporaryBytes.operator[](0) = (number & 0x000000FF);
			temporaryBytes.operator[](1) = (number & 0x0000FF00) >> 8;
			temporaryBytes.operator[](2) = (number & 0x00FF0000) >> 16;
			temporaryBytes.operator[](3) = (number & 0xFF000000) >> 24;

			return temporaryBytes;
		}

		inline std::vector<unsigned char> ByteArrayFromInteger64Bit( const int64_t& number, std::vector<unsigned char>& temporaryBytes )
		{
			temporaryBytes.operator[](0) = (number & 0x00000000000000FF);
			temporaryBytes.operator[](1) = (number & 0x000000000000FF00) >> 8;
			temporaryBytes.operator[](2) = (number & 0x0000000000FF0000) >> 16;
			temporaryBytes.operator[](3) = (number & 0x00000000FF000000) >> 24;
			temporaryBytes.operator[](4) = (number & 0x000000FF00000000) >> 32;
			temporaryBytes.operator[](5) = (number & 0x0000FF0000000000) >> 40;
			temporaryBytes.operator[](6) = (number & 0x00FF000000000000) >> 48;
			temporaryBytes.operator[](7) = (number & 0xFF00000000000000) >> 56;

			return temporaryBytes;
		}

		//Turn integer 32bit to byte 8bit array
		inline void MessageUnpacking32Bit( std::vector<uint32_t>& input, std::vector<uint8_t>& output )
		{
			std::vector<unsigned char> temporaryBytes = std::vector<unsigned char>();

			for(auto begin = input.begin(), end = input.end(); begin != end; ++begin)
			{
				int32_t number = static_cast<int32_t>(*begin);

				temporaryBytes.resize(4);
				std::vector<unsigned char> input = ByteArrayFromInteger32Bit(number, temporaryBytes);

				for(auto& value : input)
				{
					output.push_back(value);
				}

				temporaryBytes.clear();
			}
		}

		//Turn integer 64bit to byte 8bit array
		inline void MessageUnpacking64Bit( std::vector<uint64_t>& input, std::vector<uint8_t>& output )
		{
			std::vector<unsigned char> temporaryBytes = std::vector<unsigned char>();

			for(auto begin = input.begin(), end = input.end(); begin != end; ++begin)
			{
				int64_t number = static_cast<int64_t>(*begin);

				temporaryBytes.resize(8);
				std::vector<unsigned char> input = ByteArrayFromInteger64Bit(number, temporaryBytes);

				for(auto& value : input)
				{
					output.push_back(value);
				}

				temporaryBytes.clear();
			}
		}

		#endif
	}

	#if defined(BYTE_SWAP_FUNCTON)
	#undef BYTE_SWAP_FUNCTON
	#endif // !BYTE_SWAP_FUNCTON

	#if defined(MEMORY_DATA_TYPE_PACKER_AND_UNPACKER)
	#undef MEMORY_DATA_TYPE_PACKER_AND_UNPACKER
	#endif // !MEMORY_DATA_TYPE_PACKER_AND_UNPACKER

	#if defined(INTEGER_PACKCATION_OLD)
	#undef INTEGER_PACKCATION_OLD
	#endif // !INTEGER_PACKCATION_OLD

	#if defined(INTEGER_UNPACKCATION_OLD)
	#undef INTEGER_UNPACKCATION_OLD
	#endif // !INTEGER_UNPACKCATION_OLD
}

namespace CommonToolkit
{
	namespace ByteRangesAdapters 
	{
		#if defined(__cpp_lib_byte) && !defined(__cpp_lib_span)

		inline void characterToByte(const std::vector<char>& input, std::vector<std::byte>& output )
		{
			if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& characterData : input)
				{
					output.push_back( static_cast<std::byte>(static_cast<std::uint8_t>(characterData)) );
				}
			}
		}

		inline void characterFromByte(const std::vector<std::byte>& input, std::vector<char>& output)
		{
			if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& byteData : input)
				{
					output.push_back( static_cast<char>(static_cast<std::uint8_t>(byteData)) );
				}
			}
		}

		inline void classicByteToByte(const std::vector<std::uint8_t>& input, std::vector<std::byte>& output )
		{
			if(output.size() < input.size())
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& characterData : input)
				{
					output.push_back( static_cast<std::byte>(characterData) );
				}
			}
			else if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
		}

		inline void classicByteFromByte(const std::vector<std::byte>& input, std::vector<std::uint8_t>& output)
		{
			if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& byteData : input)
				{
					output.push_back( static_cast<std::uint8_t>(byteData) );
				}
			}
		}

		#elif defined(__cpp_lib_byte) && defined(__cpp_lib_span)

		inline void characterToByte( std::span<const char> input, std::vector<std::byte>& output )
		{
			if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& characterData : input)
				{
					output.push_back( static_cast<std::byte>(static_cast<std::uint8_t>(characterData)) );
				}
			}
		}

		inline void characterFromByte( std::span<const std::byte> input, std::vector<char>& output )
		{
			if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& byteData : input)
				{
					output.push_back( static_cast<char>(static_cast<std::uint8_t>(byteData)) );
				}
			}
		}

		inline void classicByteToByte( std::span<const std::uint8_t> input, std::vector<std::byte>& output )
		{
			if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& characterData : input)
				{
					output.push_back( static_cast<std::byte>(characterData) );
				}
			}
		}

		inline void classicByteFromByte( std::span<const std::byte> input, std::vector<std::uint8_t>& output)
		{
			if(output.size() == input.size())
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& byteData : input)
				{
					output.push_back( static_cast<std::uint8_t>(byteData) );
				}
			}
		}

		#endif

		#if !defined(__cpp_lib_span)

		inline void characterToClassicByte( const std::vector<char>& input , std::vector<std::uint8_t>& output )
		{
			if ( output.size() == input.size() )
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& characterData : input)
				{
					output.push_back( static_cast<std::uint8_t>(characterData) );
				}
			}
		}

		inline void characterFromClassicByte( const std::vector<std::uint8_t>& input, std::vector<char>& output )
		{
			if ( output.size() == input.size() )
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& byteData : input)
				{
					output.push_back( static_cast<char>(byteData) );
				}
			}
		}

		#else

		inline void characterToClassicByte( std::span<const char> input , std::vector<std::uint8_t>& output )
		{
			if ( output.size() == input.size() )
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& characterData : input)
				{
					output.push_back( static_cast<std::uint8_t>(characterData) );
				}
			}
		}

		inline void characterFromClassicByte( std::span<const std::uint8_t> input, std::vector<char>& output )
		{
			if ( output.size() < input.size() )
			{
				::memcpy(output.data(), input.data(), input.size());
			}
			else
			{
				output.clear();
				output.reserve(input.size());
				for (const auto& byteData : input)
				{
					output.push_back( static_cast<char>(byteData) );
				}
			}
		}

		#endif
	}
}

#endif	// !DEFINE_COMMON_TOOLKIT