#pragma once

#include "../MainProgram/Support+Library/Support-Library.hpp"

namespace CommonSecurity
{
	//随机数分布的功能库
	//Function library for random number distribution
	namespace RND
	{
		template <class _Elem, class _Traits>
		std::basic_ostream<_Elem, _Traits>&
		DataType_ValueWrite( std::basic_ostream<_Elem, _Traits>& Os, long double _DataValue_ )
		{
			constexpr long double _TwoPower31_ = 2147483648.0L;
			constexpr int _Nwords = 4;
			
			// write long double to stream
			int			_Ex;
			long double _Fraction_ = ::frexpl( _DataValue_, &_Ex );
			for ( int _Nw = 0; _Nw < _Nwords; ++_Nw )
			{  // break into 31-bit words
				_Fraction_ *= _TwoPower31_;
				long _Digits = static_cast<long>( _Fraction_ );
				_Fraction_ -= _Digits;
				Os << ' ' << _Digits;
			}
			Os << ' ' << _Ex;
			return Os;
		}

		template <class _Elem, class _Traits>
		std::basic_istream<_Elem, _Traits>&
		DataType_ValueRead( std::basic_istream<_Elem, _Traits>& Is, long double& _DataValue_ )
		{
			constexpr long double _TwoPower31_ = 2147483648.0L;
			constexpr int _Nwords = 4;
			
			// read long double from stream
			long double _Fraction_ = 0.0;
			long		_Digits_;
			for ( int _Nw = 0; _Nw < _Nwords; ++_Nw )
			{  // accumulate 31-bit words
				Is >> _Digits_;
				long double _TemporaryRealFloatingValue_ = _Digits_ / _TwoPower31_;
				for ( int Index = 0; Index < _Nw; ++Index )
				{
					_TemporaryRealFloatingValue_ /= _TwoPower31_;
				}

				_Fraction_ += _TemporaryRealFloatingValue_;
			}
			Is >> _Digits_;
			_DataValue_ = ::ldexpl( _Fraction_, _Digits_ );
			return Is;
		}

		template <class _Elem, class _Traits, class _DataType_>
		std::basic_istream<_Elem, _Traits>&
		DataType_ValueIn( std::basic_istream<_Elem, _Traits>& Is, _DataType_& _DataValue_ )
		{
			// read from stream
			long double _Value_;
			_DataType_	_Max = ( std::numeric_limits<_DataType_>::max )();
			DataType_ValueRead( Is, _Value_ );
			if ( ::fabsl( _Value_ ) <= _Max )
			{
				_DataValue_ = static_cast<_DataType_>( _Value_ );
			}
			else if ( _Value_ < 0 )
			{
				_DataValue_ = -_Max;
			}
			else
			{
				_DataValue_ = _Max;
			}

			return Is;
		}

		template <class _Elem, class _Traits, class _DataType_>
		std::basic_ostream<_Elem, _Traits>&
		DataType_ValueOut( std::basic_ostream<_Elem, _Traits>& Os, _DataType_ _DataValue_ )
		{
			// write to stream
			return DataType_ValueWrite( Os, _DataValue_ );
		}
		
		[[nodiscard]] constexpr int
		DoGenerateCanonicalIterations( const int Bits, const uint64_t RNG_NumberMin, const uint64_t RNG_NumberMax )
		{
			//For a URBG type `RNG_Type` with range == `(RNG_Type::max() - RNG_Type::min()) + 1`, returns the number of calls to generate at least Bits bits of entropy.
			//Specifically, max(1, ceil(_Bits / log2(range))).

			if ( Bits == 0 || ( RNG_NumberMax == std::numeric_limits<std::uint64_t>::max() && RNG_NumberMin == 0 ) )
			{
				return 1;
			}

			const auto RangeCount = ( RNG_NumberMax - RNG_NumberMin ) + 1;
			const auto Target = ~uint64_t { 0 } >> ( 64 - Bits );
			uint64_t   _Produce_ = 1;
			int		   _Ceil_ = 0;
			while ( _Produce_ <= Target )
			{
				++_Ceil_;
				if ( _Produce_ > std::numeric_limits<std::uint64_t>::max() / RangeCount )
				{
					break;
				}

				_Produce_ *= RangeCount;
			}

			return _Ceil_;
		}

		//从随机序列中建立一个浮点值
		//build a floating-point value from random sequence
		template <std::floating_point RealFloatingType, std::size_t Bits, class RNG_Type>
		[[nodiscard]] RealFloatingType
		GenerateCanonical( RNG_Type& RNG_Function )
		{
			constexpr auto Digits = static_cast<size_t>(std::numeric_limits<RealFloatingType>::digits );
			constexpr auto Minbits = static_cast<int>( Digits < Bits ? Digits : Bits );

			static_assert(0 <= Bits && Bits <= 64, "Number of invalid bits");

			constexpr auto RNG_NumberMin = static_cast<RealFloatingType>( ( RNG_Type::min )() );
			constexpr auto RNG_NumberMax = static_cast<RealFloatingType>( ( RNG_Type::max )() );
			constexpr auto RangeCount = ( RNG_NumberMax - RNG_NumberMin ) + RealFloatingType { 1.0 };

			constexpr int KTimes = DoGenerateCanonicalIterations( Minbits, ( RNG_Type::min )(), ( RNG_Type::max )() );

			RealFloatingType Answer { 0 };
			RealFloatingType Factor { 1 };

			for ( int Index = 0; Index < KTimes; ++Index )
			{
				// add in another set of bits
				Answer += ( static_cast<RealFloatingType>( RNG_Function() ) - RNG_NumberMin ) * Factor;
				Factor *= RangeCount;
			}

			return Answer / Factor;
		}

		template <class RNG_Type, class = void>
		struct HasStaticMinAndMax : std::false_type {};

		// This checks a requirement of N4901 [rand.req.urng] `concept uniform_random_bit_generator` but doesn't attempt
		// to implement the whole concept - we just need to distinguish Standard machinery from tr1 machinery.
		template <class RNG_Type>
		struct HasStaticMinAndMax<RNG_Type, std::void_t<decltype(std::bool_constant<(RNG_Type::min)() < (RNG_Type::max)()>::value)>> : std::true_type {};

		//从随机序列中建立一个浮点值
		// build a floating-point value from random sequence
		template <std::floating_point RealFloatingType, class RNG_Type>
		[[nodiscard]] RealFloatingType
		NRangeProbabilityEvaluation(RNG_Type& RNG_Function)
		{
			constexpr auto Digits  = static_cast<size_t>(std::numeric_limits<RealFloatingType>::digits);
			constexpr auto Bits    = ~size_t{0};
			constexpr auto Minbits = Digits < Bits ? Digits : Bits;

			if constexpr (HasStaticMinAndMax<RNG_Type>::value && Minbits <= 64)
			{
				return GenerateCanonical<RealFloatingType, Minbits>(RNG_Function);
			}
			else
			{
				// TRANSITION, for tr1 machinery only; Standard machinery can call generate_canonical directly
				constexpr auto RNG_NumberMin = static_cast<RealFloatingType>( ( RNG_Type::min )() );
				constexpr auto RNG_NumberMax = static_cast<RealFloatingType>( ( RNG_Type::max )() );
				constexpr auto RangeCount = ( RNG_NumberMax - RNG_NumberMin ) + RealFloatingType { 1.0 };

				const int _Ceil_ = static_cast<int>(::ceil(static_cast<RealFloatingType>(Minbits) / ::log2(RangeCount)));
				const int KTimes = _Ceil_ < 1 ? 1 : _Ceil_;

				RealFloatingType Answer { 0 };
				RealFloatingType Factor { 1 };

				for ( int Index = 0; Index < KTimes; ++Index )
				{
					// add in another set of bits
					Answer += ( static_cast<RealFloatingType>( RNG_Function() ) - RNG_NumberMin ) * Factor;
					Factor *= RangeCount;
				}

				return Answer / Factor;
			}
		}

		//将一个统一的随机数发生器包装成一个随机数发生器
		//Wrap a Uniform random number generator as an Random number generator
		template <class DifferenceType, class URNG_Type>
		requires std::uniform_random_bit_generator<std::remove_reference_t<URNG_Type>>
		class WARP_URNG_AS_AN_RNG
		{

		public:

			using Type0 = std::make_unsigned_t<DifferenceType>;
			using Type1 = typename URNG_Type::result_type;

			using UnsignedDifferenceType = std::conditional_t<sizeof( Type1 ) < sizeof( Type0 ), Type0, Type1>;

			explicit WARP_URNG_AS_AN_RNG( URNG_Type& RNG_Function )
				: URNG_TypeReference( RNG_Function ), RandomBits( CHAR_BIT * sizeof( UnsignedDifferenceType ) ), RandomBitMask( UnsignedDifferenceType( -1 ) )
			{
				for ( ; ( URNG_Type::max )() - ( URNG_Type::min )() < RandomBitMask; RandomBitMask >>= 1 )
				{
					--RandomBits;
				}
			}

			// adapt URNG_Type closed range to [0, DifferenceTypeIndex)
			DifferenceType operator()( DifferenceType DifferenceTypeIndex )
			{
				// try a sample random value
				for ( ;; )
				{
					UnsignedDifferenceType ResultObject = 0; // random bits
					UnsignedDifferenceType MaskInRange = 0; // 2^N - 1, ResultObject is within [0, MaskInRange]

					while ( MaskInRange < UnsignedDifferenceType( DifferenceTypeIndex - 1 ) )
					{
						// need more random bits
						ResultObject <<= RandomBits - 1;  // avoid full shift
						ResultObject <<= 1;
						ResultObject |= FindBits();
						MaskInRange <<= RandomBits - 1;	 // avoid full shift
						MaskInRange <<= 1;
						MaskInRange |= RandomBitMask;
					}

					// ResultObject is [0, MaskInRange], DifferenceTypeIndex - 1 <= MaskInRange, return if unbiased
					if ( ResultObject / DifferenceTypeIndex < MaskInRange / DifferenceTypeIndex || MaskInRange % DifferenceTypeIndex == UnsignedDifferenceType( DifferenceTypeIndex - 1 ) )
					{
						return static_cast<DifferenceType>( ResultObject % DifferenceTypeIndex );
					}
				}
			}

			UnsignedDifferenceType FindAllBits()
			{
				UnsignedDifferenceType ResultObject = 0;

				for ( size_t NumberIndex = 0; NumberIndex < CHAR_BIT * sizeof( UnsignedDifferenceType ); NumberIndex += RandomBits )
				{
					// don't mask away any bits
					ResultObject <<= RandomBits - 1; // avoid full shift
					ResultObject <<= 1;
					ResultObject |= FindBits();
				}

				return ResultObject;
			}

			WARP_URNG_AS_AN_RNG( const WARP_URNG_AS_AN_RNG& ) = delete;
			WARP_URNG_AS_AN_RNG& operator=( const WARP_URNG_AS_AN_RNG& ) = delete;

		private:

			// return a random value within [0, RandomBitMask]
			UnsignedDifferenceType FindBits()
			{
				for ( ;; )
				{
					// repeat until random value is in range
					UnsignedDifferenceType RandomValue = URNG_TypeReference() - ( URNG_Type::min )();

					if ( RandomValue <= RandomBitMask )
					{
						return RandomValue;
					}
				}
			}

			URNG_Type&			   URNG_TypeReference;	// reference to URNG
			size_t				   RandomBits;			// number of random bits generated by _Get_bits()
			UnsignedDifferenceType RandomBitMask;		// 2^RandomBits - 1
		};

		// uniform integer distribution base
		template <std::integral IntegerType>
		class UniformInteger
		{
		public:
			using result_type = IntegerType;

			// parameter package
			struct param_type
			{
				using distribution_type = UniformInteger;

				param_type()
				{
					InitialParamType( 0, 9 );
				}

				explicit param_type( result_type MinimumValue0, result_type MaximumValue0 = 9 )
				{
					InitialParamType( MinimumValue0, MaximumValue0 );
				}

				[[nodiscard]] friend bool operator==( const param_type& Left, const param_type& Right )
				{
					return Left.MinimumValue == Right.MinimumValue && Left.MaximumValue == Right.MaximumValue;
				}

			#if __cplusplus < 202002L
				[[nodiscard]] friend bool operator!=( const param_type& Left, const param_type& Right )
				{
					return !( Left == Right );
				}
			#endif

				[[nodiscard]] result_type a() const
				{
					return MinimumValue;
				}

				[[nodiscard]] result_type b() const
				{
					return MaximumValue;
				}

				void InitialParamType( IntegerType MinimumValue0, IntegerType MaximumValue0 )
				{
					// set internal state

					my_cpp2020_assert( MinimumValue0 <= MaximumValue0, "invalid min and max arguments for uniform_int", std::source_location::current() );

					MinimumValue = MinimumValue0;
					MaximumValue = MaximumValue0;
				}

				result_type MinimumValue;
				result_type MaximumValue;
			};

			UniformInteger() : ParamPackageObject( 0, 9 ) {}

			explicit UniformInteger( IntegerType MinimumValue0, IntegerType MaximumValue0 = 9 ) : ParamPackageObject( MinimumValue0, MaximumValue0 ) {}

			explicit UniformInteger( const param_type& ParamObject0 ) : ParamPackageObject( ParamObject0 ) {}

			[[nodiscard]] result_type a() const
			{
				return ParamPackageObject.a();
			}

			[[nodiscard]] result_type b() const
			{
				return ParamPackageObject.b();
			}

			[[nodiscard]] param_type param() const
			{
				return ParamPackageObject;
			}

			void param( const param_type& ParamObject0 )
			{
				// set parameter package
				ParamPackageObject = ParamObject0;
			}

			[[nodiscard]] result_type( min )() const
			{
				return ParamPackageObject.MinimumValue;
			}

			[[nodiscard]] result_type( max )() const
			{
				return ParamPackageObject.MaximumValue;
			}

			void reset() {}	 // clear internal state

			template <class RandomNumberGenerator_EngineType>
			[[nodiscard]] result_type operator()( RandomNumberGenerator_EngineType& RNG_EngineObject ) const
			{
				return this->Evaluation( RNG_EngineObject, ParamPackageObject.MinimumValue, ParamPackageObject.MaximumValue );
			}

			template <class RandomNumberGenerator_EngineType>
			[[nodiscard]] result_type operator()( RandomNumberGenerator_EngineType& RNG_EngineObject, const param_type& ParamObject0 ) const
			{
				return this->Evaluation( RNG_EngineObject, ParamObject0.MinimumValue, ParamObject0.MaximumValue );
			}

			template <class RandomNumberGenerator_EngineType>
			[[nodiscard]] result_type operator()( RandomNumberGenerator_EngineType& RNG_EngineObject, result_type _Nx ) const
			{
				return this->Evaluation( RNG_EngineObject, 0, _Nx - 1 );
			}

			template <class _Elem, class _Traits>
			friend std::basic_istream<_Elem, _Traits>& operator>>( std::basic_istream<_Elem, _Traits>& Istr, UniformInteger& OperatedObject )
			{
				// read state from _Istr
				UniformInteger::result_type Min0;
				UniformInteger::result_type Max0;
				Istr >> Min0 >> Max0;
				OperatedObject.ParamPackageObject.InitialParamType( Min0, Max0 );
				return Istr;
			}

			template <class _Elem, class _Traits>
			friend std::basic_ostream<_Elem, _Traits>& operator<<( std::basic_ostream<_Elem, _Traits>& Ostr, const UniformInteger& OperatedObject )
			{
				// write state to _Ostr
				return Ostr << OperatedObject.ParamPackageObject.MinimumValue << ' ' << OperatedObject.ParamPackageObject.MaximumValue;
			}

		private:

			using UnsignedIntegerType = std::make_unsigned_t<IntegerType>;

			// compute next value in range [MinimumValue, MaximumValue]
			template <class RandomNumberGenerator_EngineType>
			result_type Evaluation( RandomNumberGenerator_EngineType& RNG_EngineObject, IntegerType MinimumValue, IntegerType MaximumValue ) const
			{
				WARP_URNG_AS_AN_RNG<UnsignedIntegerType, RandomNumberGenerator_EngineType> _Generator( RNG_EngineObject );

				const UnsignedIntegerType _UnsignedMinimunValue_ = AdjustNumber( static_cast<UnsignedIntegerType>( MinimumValue ) );
				const UnsignedIntegerType _UnsignedMaximunValue_ = AdjustNumber( static_cast<UnsignedIntegerType>( MaximumValue ) );

				UnsignedIntegerType UnsignedIntegerResult;

				if ( _UnsignedMaximunValue_ - _UnsignedMinimunValue_ == static_cast<UnsignedIntegerType>( -1 ) )
				{
					UnsignedIntegerResult = static_cast<UnsignedIntegerType>( _Generator.FindAllBits() );
				}
				else
				{
					UnsignedIntegerResult = static_cast<UnsignedIntegerType>( _Generator( static_cast<UnsignedIntegerType>( _UnsignedMaximunValue_ - _UnsignedMinimunValue_ + 1 ) ) );
				}

				return static_cast<IntegerType>( AdjustNumber( static_cast<UnsignedIntegerType>( UnsignedIntegerResult + _UnsignedMinimunValue_ ) ) );
			}

			// convert signed ranges to unsigned ranges and vice versa
			static UnsignedIntegerType AdjustNumber( UnsignedIntegerType UnsignedInegerValue )
			{
				if constexpr ( std::is_signed_v<IntegerType> )
				{
					const UnsignedIntegerType NumberAdjuster = ( static_cast<UnsignedIntegerType>( -1 ) >> 1 ) + 1;	 // 2^(N-1)

					if ( UnsignedInegerValue < NumberAdjuster )
					{
						return static_cast<UnsignedIntegerType>( UnsignedInegerValue + NumberAdjuster );
					}
					else
					{
						return static_cast<UnsignedIntegerType>( UnsignedInegerValue - NumberAdjuster );
					}
				}
				else
				{
					// IntegerType is already unsigned, do nothing
					return UnsignedInegerValue;
				}
			}

			param_type ParamPackageObject;
		};

		// uniform integer distribution
		template <class IntegerType>
		class UniformIntegerDistribution : public UniformInteger<IntegerType>
		{

		public:

			using _BaseType = UniformInteger<IntegerType>;
			using _ParamBaseType = typename _BaseType::param_type;
			using result_type = typename _BaseType::result_type;

			// parameter package
			struct param_type : _ParamBaseType
			{
				using distribution_type = UniformIntegerDistribution;

				param_type() : _ParamBaseType(0, (std::numeric_limits<IntegerType>::max)()) {}

				explicit param_type(result_type _Min0, result_type _Max0 = (std::numeric_limits<IntegerType>::max)()) : _ParamBaseType(_Min0, _Max0) {}

				param_type(const _ParamBaseType& OtherObject) : _ParamBaseType(OtherObject) {}
			};

			UniformIntegerDistribution() : _BaseType(0, (std::numeric_limits<IntegerType>::max)()) {}

			explicit UniformIntegerDistribution(IntegerType _Min0, IntegerType _Max0 = (std::numeric_limits<IntegerType>::max)()) : _BaseType(_Min0, _Max0) {}

			explicit UniformIntegerDistribution(const param_type& ParamObject) : _BaseType(ParamObject) {}

			[[nodiscard]] friend bool operator==(const UniformIntegerDistribution& Left, const UniformIntegerDistribution& Right)
			{
				return Left.param() == Right.param();
			}

		#if __cplusplus < 202002L
			[[nodiscard]] friend bool operator!=(const UniformIntegerDistribution& Left, const UniformIntegerDistribution& Right)
			{
				return !(Left == Right);
			}
		#endif

		};

		// uniform real number distribution base
		template <std::floating_point RealFloatingType>
		class UniformRealNumber
		{
		public:
			using result_type = RealFloatingType;

			// parameter package
			struct param_type
			{
				using distribution_type = UniformRealNumber;

				param_type()
				{
					InitialParamType( RealFloatingType{0.0}, RealFloatingType{1.0} );
				}

				explicit param_type( result_type MinimumValue0, result_type MaximumValue0 = 9 )
				{
					InitialParamType( MinimumValue0, MaximumValue0 );
				}

				[[nodiscard]] friend bool operator==( const param_type& Left, const param_type& Right )
				{
					return Left.MinimumValue == Right.MinimumValue && Left.MaximumValue == Right.MaximumValue;
				}

			#if __cplusplus < 202002L
				[[nodiscard]] friend bool operator!=( const param_type& Left, const param_type& Right )
				{
					return !( Left == Right );
				}
			#endif

				[[nodiscard]] result_type a() const
				{
					return MinimumValue;
				}

				[[nodiscard]] result_type b() const
				{
					return MaximumValue;
				}

				void InitialParamType( RealFloatingType MinimumValue0, RealFloatingType MaximumValue0 )
				{
					// set internal state

					my_cpp2020_assert
					( 
						MinimumValue0 <= MaximumValue0 && (0 <= MinimumValue0 || MaximumValue0 <= MinimumValue0 + (std::numeric_limits<RealFloatingType>::max)()),
						"invalid min and max arguments for uniform_real", 
						std::source_location::current() 
					);

					MinimumValue = MinimumValue0;
					MaximumValue = MaximumValue0;
				}

				result_type MinimumValue;
				result_type MaximumValue;
			};

			UniformRealNumber() : ParamPackageObject( RealFloatingType{0}, RealFloatingType{1} ) {}

			explicit UniformRealNumber( RealFloatingType MinimumValue0, RealFloatingType MaximumValue0 = 9 ) : ParamPackageObject( MinimumValue0, MaximumValue0 ) {}

			explicit UniformRealNumber( const param_type& ParamObject0 ) : ParamPackageObject( ParamObject0 ) {}

			[[nodiscard]] result_type a() const
			{
				return ParamPackageObject.a();
			}

			[[nodiscard]] result_type b() const
			{
				return ParamPackageObject.b();
			}

			[[nodiscard]] param_type param() const
			{
				return ParamPackageObject;
			}

			void param( const param_type& ParamObject0 )
			{
				// set parameter package
				ParamPackageObject = ParamObject0;
			}

			[[nodiscard]] result_type( min )() const
			{
				return ParamPackageObject.MinimumValue;
			}

			[[nodiscard]] result_type( max )() const
			{
				return ParamPackageObject.MaximumValue;
			}

			void reset() {}	 // clear internal state

			template <class RandomNumberGenerator_EngineType>
			[[nodiscard]] result_type operator()( RandomNumberGenerator_EngineType& RNG_EngineObject ) const
			{
				return this->Evaluation( RNG_EngineObject, this->ParamPackageObject );
			}

			template <class RandomNumberGenerator_EngineType>
			[[nodiscard]] result_type operator()( RandomNumberGenerator_EngineType& RNG_EngineObject, const param_type& ParamObject0 ) const
			{
				return this->Evaluation( RNG_EngineObject, ParamObject0 );
			}

			template <class _Elem, class _Traits>
			std::basic_istream<_Elem, _Traits>& Read( std::basic_istream<_Elem, _Traits>& Istr )
			{
				// read state from _Istr
				UniformRealNumber::result_type Min0;
				UniformRealNumber::result_type Max0;
				DataType_ValueIn( Istr, Min0 );
				DataType_ValueIn( Istr, Max0 );
				ParamPackageObject.InitialParamType( Min0, Max0 );
				return Istr;
			}

			template <class _Elem, class _Traits>
			std::basic_ostream<_Elem, _Traits>& Write( std::basic_ostream<_Elem, _Traits>& Ostr ) const
			{
				// write state to _Ostr
				DataType_ValueOut( Ostr, ParamPackageObject.MinimumValue );
				DataType_ValueOut( Ostr, ParamPackageObject.MaximumValue );
				return Ostr;
			}

			template <class _Elem, class _Traits>
			friend std::basic_istream<_Elem, _Traits>& operator>>( std::basic_istream<_Elem, _Traits>& Istr, UniformRealNumber& OperatedObject )
			{
				// read state from _Istr
				return OperatedObject.Read( Istr );
			}

			template <class _Elem, class _Traits>
			friend std::basic_ostream<_Elem, _Traits>& operator<<( std::basic_ostream<_Elem, _Traits>& Ostr, const UniformRealNumber& OperatedObject )
			{
				// write state to _Ostr
				return OperatedObject.Write( Ostr );
			}

		private:

			template <class RandomNumberGenerator_EngineType>
			result_type Evaluation( RandomNumberGenerator_EngineType& RNG_EngineObject, const param_type& ParamObject0 ) const
			{
				return NRangeProbabilityEvaluation<RealFloatingType>(RNG_EngineObject) * (ParamObject0.MaximumValue - ParamObject0.MinimumValue) + ParamObject0.MinimumValue;
			}

			param_type ParamPackageObject;
		};

		// uniform real number distribution
		template<std::floating_point RealFloatingType>
		class UniformRealNumberDistribution : public UniformRealNumber<RealFloatingType>
		{
		
		public:

			using _BaseType = UniformRealNumber<RealFloatingType>;
			using _ParamBaseType = typename _BaseType::param_type;
			using result_type = typename _BaseType::result_type;

			// parameter package
			struct param_type : _ParamBaseType
			{
				using distribution_type = UniformRealNumberDistribution;

				param_type() : _ParamBaseType(0, (std::numeric_limits<RealFloatingType>::max)()) {}

				explicit param_type(result_type _Min0, result_type _Max0 = (std::numeric_limits<RealFloatingType>::max)()) : _ParamBaseType(_Min0, _Max0) {}

				param_type(const _ParamBaseType& OtherObject) : _ParamBaseType(OtherObject) {}
			};

			UniformRealNumberDistribution() : _BaseType(RealFloatingType{0}, RealFloatingType{1}) {}

			explicit UniformRealNumberDistribution(RealFloatingType _Min0, RealFloatingType _Max0 = RealFloatingType{1}) : _BaseType(_Min0, _Max0) {}

			explicit UniformRealNumberDistribution(const param_type& ParamObject) : _BaseType(ParamObject) {}

			[[nodiscard]] friend bool operator==(const UniformRealNumberDistribution& Left, const UniformRealNumberDistribution& Right)
			{
				return Left.param() == Right.param();
			}

		#if __cplusplus < 202002L
			[[nodiscard]] friend bool operator!=(const UniformRealNumberDistribution& Left, const UniformRealNumberDistribution& Right)
			{
				return !(Left == Right);
			}
		#endif
		};

		class BernoulliDistribution
		{

		public:
			using result_type = bool;

			struct param_type
			{
				using distribution_type = BernoulliDistribution;

				param_type()
				{
					InitialParamType( 0.5 );
				}

				explicit param_type( double _Px0 )
				{
					InitialParamType( _Px0 );
				}

				[[nodiscard]] friend bool operator==( const param_type& Left, const param_type& Right )
				{
					return Left._RememberProbability_ == Right._RememberProbability_;
				}

			#if  __cplusplus < 202002L
				[[nodiscard]] friend bool operator!=( const param_type& Left, const param_type& Right )
				{
					return !( Left == Right );
				}
			#endif

				[[nodiscard]] double p() const
				{
					return _RememberProbability_;
				}

				void InitialParamType( double _Px0 )
				{
					// set internal state
					my_cpp2020_assert( 0.0 <= _Px0 && _Px0 <= 1.0, "invalid probability argument for bernoulli_distribution", std::source_location::current() );

					_RememberProbability_ = _Px0;
				}

				double _RememberProbability_;
			};

			BernoulliDistribution() : ParamPackageObject( 0.5 ) {}

			explicit BernoulliDistribution( double _Px0 ) : ParamPackageObject( _Px0 ) {}

			explicit BernoulliDistribution( const param_type& ParamObject0 ) : ParamPackageObject( ParamObject0 ) {}

			[[nodiscard]] double p() const
			{
				return ParamPackageObject.p();
			}

			[[nodiscard]] param_type param() const
			{
				return ParamPackageObject;
			}

			void param( const param_type& _Par0 )
			{
				// set parameter package
				ParamPackageObject = _Par0;
			}

			[[nodiscard]] result_type( min )() const
			{
				// get smallest possible result
				return false;
			}

			[[nodiscard]] result_type( max )() const
			{
				// get largest possible result
				return true;
			}

			void reset() {}	 // clear internal state

			template <class RandomNumberGenerator_EngineType>
			[[nodiscard]] result_type operator()( RandomNumberGenerator_EngineType& RNG_EngineObject ) const
			{
				return this->Evaluation( RNG_EngineObject, ParamPackageObject );
			}

			template <class RandomNumberGenerator_EngineType>
			[[nodiscard]] result_type operator()( RandomNumberGenerator_EngineType& RNG_EngineObject, const param_type& ParamObject0 ) const
			{
				return this->Evaluation( RNG_EngineObject, ParamObject0 );
			}

			[[nodiscard]] friend bool operator==( const BernoulliDistribution& Left, const BernoulliDistribution& Right )
			{
				return Left.param() == Right.param();
			}

		#if  __cplusplus < 202002L 
			[[nodiscard]] friend bool operator!=( const BernoulliDistribution& Left, const BernoulliDistribution& Right )
			{
				return !( Left == Right );
			}
		#endif

			template <class _Elem, class _Traits>
			friend std::basic_istream<_Elem, _Traits>& operator>>( std::basic_istream<_Elem, _Traits>& Istr, BernoulliDistribution& OperatedObject )
			{
				// read state from Istr
				double RememberProbability0;
				DataType_ValueIn( Istr, RememberProbability0 );
				OperatedObject.ParamPackageObject.InitialParamType( RememberProbability0 );
				return Istr;
			}

			template <class _Elem, class _Traits>
			friend std::basic_ostream<_Elem, _Traits>& operator<<( std::basic_ostream<_Elem, _Traits>& Ostr, const BernoulliDistribution& OperatedObject )
			{
				// write state to Ostr
				DataType_ValueOut( Ostr, OperatedObject.ParamPackageObject._RememberProbability_ );
				return Ostr;
			}

		private:
			template <class RandomNumberGenerator_EngineType>
			result_type Evaluation( RandomNumberGenerator_EngineType& RNG_EngineObject, const param_type& ParamObject0 ) const
			{
				return NRangeProbabilityEvaluation<double>(RNG_EngineObject) < ParamObject0._RememberProbability_;
			}

			param_type ParamPackageObject;
		};

		template<typename RNG_Type>
		requires std::uniform_random_bit_generator<std::remove_reference_t<RNG_Type>>
		struct PseudoRandomNumberEngine
		{
			//Whether the pseudo-random is initialized by seed
			static inline bool PseudoRandomIsInitialBySeed = false;
			static inline RNG_Type random_generator;

			//C++ 初始化伪随机数的种子
			//C++ Initialize the seed of the pseudo-random number
			template <std::integral IntegerType>
			void InitialBySeed( IntegerType seedNumber, bool ResetFlag = false )
			{
				if ( ResetFlag == true )
					PseudoRandomIsInitialBySeed = false;

				if ( PseudoRandomIsInitialBySeed == false )
				{
					random_generator.seed( seedNumber );
					PseudoRandomIsInitialBySeed = true;
				}
			}

			template<std::integral IntegerType, typename IteratorType>
			void InitialBySeed( IteratorType begin, IteratorType end, bool ResetFlag = false )
			{
				static_assert(std::convertible_to<std::iter_value_t<IteratorType>, IntegerType>, "");

				if ( ResetFlag == true )
					PseudoRandomIsInitialBySeed = false;

				if ( PseudoRandomIsInitialBySeed == false )
				{
					random_generator.seed( begin, end );
					PseudoRandomIsInitialBySeed = true;
				}
			}

			template<std::integral IntegerType, typename SeedSeq>
			void InitialBySeed( SeedSeq seedNumberSequence, bool ResetFlag = false )
			{
				static_assert(not std::convertible_to<SeedSeq, IntegerType>, "");

				if ( ResetFlag == true )
					PseudoRandomIsInitialBySeed = false;

				if ( PseudoRandomIsInitialBySeed == false )
				{
					random_generator.seed( seedNumberSequence );
					PseudoRandomIsInitialBySeed = true;
				}
			}

			//C++ 生成伪随机数
			//C++ generates random numbers
			template <typename IntegerType>
			requires std::integral<IntegerType>
			IntegerType GenerateNumber(IntegerType minimum, IntegerType maximum, bool is_nonlinear_mode)
			{
				if (PseudoRandomIsInitialBySeed == true)
				{
					if (minimum > 0)
						minimum = std::numeric_limits<IntegerType>::min();
					if (maximum < 0)
						maximum = std::numeric_limits<IntegerType>::max();

					if (!is_nonlinear_mode)
					{
						static RND::UniformIntegerDistribution<IntegerType> number_distribution(minimum, maximum);

						if constexpr (std::signed_integral<IntegerType>)
						{
							auto random_unsigned_number = number_distribution(random_generator);
							auto random_unsigned_number2 = number_distribution(random_generator);

							if (minimum < 0)
							{
								auto can_be_subtracted_count = minimum;
								~can_be_subtracted_count;

							RegenerateNumber:

								while (random_unsigned_number > can_be_subtracted_count - 1 || random_unsigned_number == 0)
									random_unsigned_number = number_distribution(random_generator);

								while (random_unsigned_number2 > can_be_subtracted_count - 1 || random_unsigned_number2 == 0)
									random_unsigned_number2 = number_distribution(random_generator);

								if (random_unsigned_number == random_unsigned_number2)
									goto RegenerateNumber;

								if (random_unsigned_number > random_unsigned_number2)
									return 0 - random_unsigned_number;
								else if (random_unsigned_number < random_unsigned_number2)
									return 0 - random_unsigned_number2;
							}

							return number_distribution(random_generator);
						}
						else
							return number_distribution(random_generator);
					}
					else
					{
						IntegerType random_number = 0, random_number2 = 0;

						if (maximum == std::numeric_limits<IntegerType>::max())
							maximum -= 1;

						auto lambda_GenerateNumberAtIntervals = [&random_number, &random_number2, &minimum](const IntegerType middle_number)
						{
							for (random_number = random_generator(); random_number < minimum || random_number > middle_number; )
							{
								random_number = random_generator();
							}

							for (random_number2 = random_generator(); random_number2 < minimum || random_number2 > middle_number + 1; )
							{
								random_number2 = random_generator();
							}
						};

						if ((maximum & 1) == 1)
						{
							auto middle_number = (maximum + 1) >> 1;

							lambda_GenerateNumberAtIntervals(middle_number);

							auto range_count = random_number + random_number2;

							if (range_count == maximum)
								return middle_number - 1;
							else if (range_count < middle_number)
								return middle_number - range_count - 1;
							else
								return maximum - range_count + middle_number - 1;
						}
						else
						{
							auto middle_number = maximum >> 1;

							lambda_GenerateNumberAtIntervals(middle_number);

							auto range_count = random_number + random_number2;

							if (range_count < middle_number)
								return middle_number - range_count - 1;
							else
								return maximum - range_count + middle_number - 1;
						}
					}
				}
			}

			PseudoRandomNumberEngine() = default;
			~PseudoRandomNumberEngine() = default;
		};
	}

	/*
		Shuffle And Sample
	*/

	//针对容器内容进行洗牌
	//Shuffling against container content
	struct UniformShuffleRangeImplement
	{
		//RNG is random number generator
		template<std::random_access_iterator RandomAccessIteratorType, std::sentinel_for<RandomAccessIteratorType> SentinelIteratorType, typename RNG_Type>
		requires std::permutable<RandomAccessIteratorType> && std::uniform_random_bit_generator<std::remove_reference_t<RNG_Type>>
		RandomAccessIteratorType
		operator()(RandomAccessIteratorType first, SentinelIteratorType last, RNG_Type&& functionRNG)
		{
			using iterator_difference_t = std::iter_difference_t<RandomAccessIteratorType>;
			using number_distribution_t = RND::UniformIntegerDistribution<iterator_difference_t>;
			using number_distribution_param_t = typename number_distribution_t::param_type;

			number_distribution_t number_distribution_object;
			const auto distance { last - first };

			for(iterator_difference_t index{1}; index < distance; ++index)
			{
				std::ranges::iter_swap(first + index, first + number_distribution_object(functionRNG, number_distribution_param_t(0, index)));
			}
			return std::ranges::next(first, last);
		}

		template <std::ranges::random_access_range RandomAccessRangeType, typename RNG_Type>
		requires std::permutable<std::ranges::iterator_t<RandomAccessRangeType>> && std::uniform_random_bit_generator<std::remove_reference_t<RNG_Type>>
		std::ranges::borrowed_iterator_t<RandomAccessRangeType>
		operator()( RandomAccessRangeType&& range, RNG_Type&& functionRNG )
		{
			return this->operator()( std::ranges::begin( range ), std::ranges::end( range ), std::forward<RNG_Type>( functionRNG ) );
		}

		template<std::random_access_iterator RandomAccessIteratorType, typename RNG_Type>
		requires std::permutable<RandomAccessIteratorType> && std::uniform_random_bit_generator<std::remove_reference_t<RNG_Type>>
		void KnuthShuffle(RandomAccessIteratorType begin, RandomAccessIteratorType end, RNG_Type&& functionRNG)
		{
			for ( std::iter_difference_t<RandomAccessIteratorType> difference_value = end - begin - 1; difference_value >= 1; --difference_value )
			{
				std::size_t iterator_offset = functionRNG() % ( difference_value + 1 );
				if ( iterator_offset != difference_value )
				{
					std::iter_swap( begin + iterator_offset, begin + difference_value );
				}
			}
		}

		template<std::ranges::random_access_range RandomAccessRangeType, typename RNG_Type>
		requires std::permutable<std::ranges::iterator_t<RandomAccessRangeType>> && std::uniform_random_bit_generator<std::remove_reference_t<RNG_Type>>
		void KnuthShuffle(RandomAccessRangeType&& range, RNG_Type&& functionRNG)
		{
			return (*this).KnuthShuffle(std::ranges::begin(range), std::ranges::end( range ), std::forward<RNG_Type>( functionRNG ));
		}
	};

	inline UniformShuffleRangeImplement ShuffleRangeData{};

	//针对容器内容进行采样
	//Sampling against container contents
	struct UniformSampleRangeImplement
	{
		template<std::input_iterator InputIteratorType, std::sentinel_for<InputIteratorType> SentinalIteratorType, std::weakly_incrementable OutputIteratorType, typename RNG_Type>
		requires (std::forward_iterator<InputIteratorType> || std::random_access_iterator<OutputIteratorType>)
		&& std::indirectly_copyable<InputIteratorType, OutputIteratorType>
		&& std::uniform_random_bit_generator<std::remove_reference_t<RNG_Type>>
		OutputIteratorType operator()( InputIteratorType read_first, SentinalIteratorType read_last, OutputIteratorType write_first, std::iter_difference_t<InputIteratorType> size, RNG_Type&& functionRNG ) const
		{
			using iterator_difference_t = std::iter_difference_t<InputIteratorType>;
			using number_distribution_t = RND::UniformIntegerDistribution<iterator_difference_t>;
			using number_distribution_param_t = typename number_distribution_t::param_type;
			number_distribution_t number_distribution{};

			if constexpr (std::forward_iterator<InputIteratorType>)
			{
				// this branch preserves "stability" of the sample elements
				auto remaining_size {std::ranges::distance(read_first, read_last)};
				for ( size = std::ranges::min( size, remaining_size ); size != 0; ++read_first )
				{
					if ( number_distribution( functionRNG, number_distribution_param_t( 0, --remaining_size ) ) < size )
					{
						*write_first++ = *read_first;
						--size;
					}
				}

				return write_first;
			}
			else
			{
				// OutputIteratorType is a random_access_iterator
				iterator_difference_t sample_size{0};

				// copy [read_first, read_first + M) elements to "random access" output
				while (read_first != read_last && sample_size != size)
				{
					write_first[sample_size++] = *read_first;
					++read_first;
				}

				// overwrite some of the copied elements with randomly selected ones
				for ( auto pop_size { sample_size }; read_first != read_last; ++read_first, ++pop_size )
				{
					const auto random_index = number_distribution( functionRNG, number_distribution_param_t{ 0, pop_size } );
					if ( random_index < size )
						write_first[ random_index ] = *read_first;
				}

				return write_first + sample_size;
			}
		}

		template <std::ranges::input_range InputRangeType, std::weakly_incrementable OutputIteratorType, typename RNG_Type>
		requires( std::ranges::forward_range<InputRangeType> || std::random_access_iterator<OutputIteratorType> )
		&& std::indirectly_copyable<std::ranges::iterator_t<InputRangeType>, OutputIteratorType> 
		&& std::uniform_random_bit_generator<std::remove_reference_t<RNG_Type>>
		OutputIteratorType operator()( InputRangeType&& read_range, OutputIteratorType write_iterator, std::ranges::range_difference_t<InputRangeType> size, RNG_Type&& functionRNG ) const
		{
			return this->operator()( std::ranges::begin( read_range ), std::ranges::end( read_range ), std::move( write_iterator ), size, std::forward<RNG_Type>( functionRNG ) );
		}
	};

	inline UniformSampleRangeImplement SampleRangeData{};
}

namespace CommonSecurity
{
	namespace Seeds
	{
		#ifndef RANDUTILS_HPP
		#define RANDUTILS_HPP 1

		// Ugly platform-specific code for auto_seeded

		#if !defined(RANDOM_UTILS_CPU_ENTROPY) && defined(__has_builtin)
			#if __has_builtin(__builtin_readcyclecounter) && !defined(__aarch64__)
				#define RANDOM_UTILS_CPU_ENTROPY __builtin_readcyclecounter()
			#endif
		#endif
		#if !defined(RANDOM_UTILS_CPU_ENTROPY)
			#if __i386__
				#if __GNUC__
					#define RANDOM_UTILS_CPU_ENTROPY __builtin_ia32_rdtsc()
				#else
					#include <immintrin.h>
					#define RANDOM_UTILS_CPU_ENTROPY __rdtsc()
				#endif
			#else
				#define RANDOM_UTILS_CPU_ENTROPY 0
			#endif
		#endif

		#if defined(RANDOM_UTILS_GETPID)
			// Already defined externally
		#elif defined(_WIN64) || defined(_WIN32)
			#include <process.h>
			#define RANDOM_UTILS_GETPID _getpid()
		#elif defined(__unix__) || defined(__unix) \
			  || (defined(__APPLE__) && defined(__MACH__))
			#include <unistd.h>
			#define RANDOM_UTILS_GETPID getpid()
		#else
			#define RANDOM_UTILS_GETPID 0
		#endif

		#if __cpp_constexpr >= 201304L
			#define RANDOM_UTILS_GENERALIZED_CONSTEXPR constexpr
		#else
			#define RANDOM_UTILS_GENERALIZED_CONSTEXPR
		#endif

		//////////////////////////////////////////////////////////////////////////////
		//
		// seed_sequence_fe
		//
		//////////////////////////////////////////////////////////////////////////////

		/*
		* seed_sequence_fe implements a fixed-entropy seed sequence; it conforms to all
		* the requirements of a Seed Sequence concept.
		*
		* seed_sequence_fe<N> implements a seed sequence which seeds based on a store of
		* N * 32 bits of entropy.	Typically, it would be initialized with N or more
		* integers.
		*
		* seed_sequence_fe128 and seed_sequence_fe256 are provided as convenience typedefs for
		* 128- and 256-bit entropy stores respectively.  These variants outperform
		* std::seed_seq, while being better mixing the bits it is provided as entropy.
		* In almost all common use cases, they serve as better drop-in replacements
		* for seed_seq.
		*
		* Technical details
		*
		* Assuming it constructed with M seed integers as input, it exhibits the
		* following properties
		*
		* * Diffusion/Avalanche:  A single-bit change in any of the M inputs has a
		*	 50% chance of flipping every bit in the bitstream produced by generate.
		*	 Initializing the N-word entropy store with M words requires O(N * M)
		*	 time precisely because of the avalanche requirements.	Once constructed,
		*	 calls to generate are linear in the number of words generated.
		*
		* * Bias freedom/Bijection: If M == N, the state of the entropy store is a
		*	 bijection from the M inputs (i.e., no states occur twice, none are
		*	 omitted). If M > N the number of times each state can occur is the same
		*	 (each state occurs 2**(32*(M-N)) times, where ** is the power function).
		*	 If M < N, some states cannot occur (bias) but no state occurs more
		*	 than once (it's impossible to avoid bias if M < N; ideally N should not
		*	 be chosen so that it is more than M).
		*
		*	 Likewise, the generate function has similar properties (with the entropy
		*	 store as the input data).	If more outputs are requested than there is
		*	 entropy, some outputs cannot occur.  For example, the Mersenne Twister
		*	 will request 624 outputs, to initialize it's 19937-bit state, which is
		*	 much larger than a 128-bit or 256-bit entropy pool.  But in practice,
		*	 limiting the Mersenne Twister to 2**128 possible initializations gives
		*	 us enough initializations to give a unique initialization to trillions
		*	 of computers for billions of years.  If you really have 624 words of
		*	 *real* high-quality entropy you want to use, you probably don't need
		*	 an entropy mixer like this class at all.  But if you *really* want to,
		*	 nothing is stopping you from creating a randutils::seed_sequence_fe<624>.
		*
		* * As a consequence of the above properties, if all parts of the provided
		*	 seed data are kept constant except one, and the remaining part is varied
		*	 through K different states, K different output sequences will be produced.
		*
		* * Also, because the amount of entropy stored is fixed, this class never
		*	 performs dynamic allocation and is free of the possibility of generating
		*	 an exception.
		*
		* Ideas used to implement this code include hashing, a simple PCG generator
		* based on an MCG base with an XorShift output function and permutation
		* functions on tuples.
		*
		* More detail at
		*	   http://www.pcg-random.org/posts/developing-a-seed_seq-alternative.html
		*/

		template
		<
			size_t count = 4, typename IntRep = uint32_t,
			size_t mix_rounds = 1 + (count <= 2)
		>
		struct seed_sequence_fe
		{

		public:
			// types
			typedef IntRep result_type;

		private:
			static constexpr uint32_t INIT_A = 0x43b0d7e5;
			static constexpr uint32_t MULT_A = 0x931e8875;

			static constexpr uint32_t INIT_B = 0x8b51f9dd;
			static constexpr uint32_t MULT_B = 0x58f38ded;

			static constexpr uint32_t MIX_MULT_L = 0xca01f9dd;
			static constexpr uint32_t MIX_MULT_R = 0x4973f715;
			static constexpr uint32_t XSHIFT = sizeof(IntRep)*8/2;

			RANDOM_UTILS_GENERALIZED_CONSTEXPR
			static IntRep fast_exponential(IntRep x, IntRep power)
			{
				IntRep result = IntRep(1);
				IntRep multiplier = x;
				while (power != IntRep(0))
				{
					IntRep thismult = power & IntRep(1) ? multiplier : IntRep(1);
					result *= thismult;
					power >>= 1;
					multiplier *= multiplier;
				}
				return result;
			}

			std::array<IntRep, count> _hash_mixer_;

			template <typename InputIter>
			void mix_entropy(InputIter begin, InputIter end);

		public:
			seed_sequence_fe(const seed_sequence_fe&)	  = delete;
			void operator=(const seed_sequence_fe&)	 = delete;

			template <typename T>
			seed_sequence_fe(std::initializer_list<T> init)
			{
				seed(init.begin(), init.end());
			}

			template <typename InputIter>
			seed_sequence_fe(InputIter begin, InputIter end)
			{
				seed(begin, end);
			}

			// generating functions
			template <typename RandomAccessIterator>
			void generate(RandomAccessIterator first, RandomAccessIterator last) const;

			static constexpr size_t size()
			{
				return count;
			}

			template <typename OutputIterator>
			void param(OutputIterator dest) const;

			template <typename InputIter>
			void seed(InputIter begin, InputIter end)
			{
				mix_entropy(begin, end);
				// For very small sizes, we do some additional mixing.
				// For normal sizes, this loop never performs any iterations.
				for (size_t i = 1; i < mix_rounds; ++i)
					stir();
			}

			seed_sequence_fe& stir()
			{
				mix_entropy(_hash_mixer_.begin(), _hash_mixer_.end());
				return *this;
			}

		};

		template <size_t count, typename IntRep, size_t r>
		template <typename InputIter>
		void seed_sequence_fe<count, IntRep, r>::mix_entropy(InputIter begin, InputIter end)
		{
			auto hash_const = INIT_A;
			auto hash = [&](IntRep value)
			{
				value ^= hash_const;
				hash_const *= MULT_A;
				value *= hash_const;
				value ^= value >> XSHIFT;
				return value;
			};

			auto mix = [](IntRep x, IntRep y)
			{
				IntRep result = MIX_MULT_L*x - MIX_MULT_R*y;
				result ^= result >> XSHIFT;
				return result;
			};

			InputIter current = begin;
			for (auto& elem : _hash_mixer_)
			{
				if (current != end)
					elem = hash(*current++);
				else
					elem = hash(0U);
			}

			for (auto& src : _hash_mixer_)
				for (auto& dest : _hash_mixer_)
					if (&src != &dest)
						dest = mix(dest,hash(src));

			for (; current != end; ++current)
				for (auto& dest : _hash_mixer_)
					dest = mix(dest,hash(*current));
		}

		template <size_t count, typename IntRep, size_t mix_rounds>
		template <typename OutputIterator>
		void seed_sequence_fe<count,IntRep,mix_rounds>::param(OutputIterator dest) const
		{
			const IntRep INV_A = fast_exponential(MULT_A, IntRep(-1));
			const IntRep MIX_INV_L = fast_exponential(MIX_MULT_L, IntRep(-1));

			auto mixer_copy = _hash_mixer_;
			for (size_t round = 0; round < mix_rounds; ++round) {
				// Advance to the final value.	We'll backtrack from that.
				auto hash_const = INIT_A*fast_exponential(MULT_A, IntRep(count * count));

				for (auto src = mixer_copy.rbegin(); src != mixer_copy.rend(); ++src)
					for (auto dest = mixer_copy.rbegin(); dest != mixer_copy.rend(); ++dest)
						if (src != dest)
						{
							IntRep revhashed = *src;
							auto mult_const = hash_const;
							hash_const *= INV_A;
							revhashed ^= hash_const;
							revhashed *= mult_const;
							revhashed ^= revhashed >> XSHIFT;
							IntRep unmixed = *dest;
							unmixed ^= unmixed >> XSHIFT;
							unmixed += MIX_MULT_R*revhashed;
							unmixed *= MIX_INV_L;
							*dest = unmixed;
						}

				for (auto r_iterator = mixer_copy.rbegin(); r_iterator != mixer_copy.rend(); ++r_iterator)
				{
					IntRep unhashed = *r_iterator;
					unhashed ^= unhashed >> XSHIFT;
					unhashed *= fast_exponential(hash_const, IntRep(-1));
					hash_const *= INV_A;
					unhashed ^= hash_const;
					*r_iterator = unhashed;
				}
			}
			std::copy(mixer_copy.begin(), mixer_copy.end(), dest);
		}


		template <size_t count, typename IntRep, size_t mix_rounds>
		template <typename RandomAccessIterator>
		void seed_sequence_fe<count,IntRep,mix_rounds>::generate
		(
				RandomAccessIterator dest_begin,
				RandomAccessIterator dest_end
		) const
		{
			auto src_begin = _hash_mixer_.begin();
			auto src_end   = _hash_mixer_.end();
			auto src	   = src_begin;
			auto hash_const = INIT_B;
			for (auto dest = dest_begin; dest != dest_end; ++dest)
			{
				auto data_value = *src;
				if (++src == src_end)
					src = src_begin;
				data_value ^= hash_const;
				hash_const *= MULT_B;
				data_value *= hash_const;
				data_value ^= data_value >> XSHIFT;
				*dest = data_value;
			}
		}

		using seed_sequence_fe128 = seed_sequence_fe<4, uint32_t>;
		using seed_sequence_fe256 = seed_sequence_fe<8, uint32_t>;


		//////////////////////////////////////////////////////////////////////////////
		//
		// auto_seeded
		//
		//////////////////////////////////////////////////////////////////////////////

		/*
		 * randutils::auto_seeded
		 *
		 *	 Extends a seed sequence class with a nondeterministic default constructor.
		 *	 Uses a variety of local sources of entropy to portably initialize any
		 *	 seed sequence to a good default state.
		 *
		 *	 In normal use, it's accessed via one of the following type aliases,
		 *   which use seed_sequence_fe128 and seed_sequence_fe256 above.
		 *
		 *		 randutils::auto_seed_128
		 *		 randutils::auto_seed_256
		 *
		 *	 It's discussed in detail at
		 *		 http://www.pcg-random.org/posts/simple-portable-cpp-seed-entropy.html
		 *	 and its motivation (why you can't just use std::random_device) here
		 *		 http://www.pcg-random.org/posts/cpps-random_device.html
		 */

		template <typename SeedSequence>
		class auto_seeded : public SeedSequence
		{
			using default_seeds = std::array<uint32_t, 13>;

			template <typename T>
			static uint32_t shift_to_32bit_integer(T value)
			{
				if (sizeof(T) <= 4)
					return uint32_t(value);
				else
				{
					uint64_t result = uint64_t(value);
					result *= 0xbc2ad017d719504d;
					return uint32_t(result ^ (result >> 32));
				}
			}

			template <typename T>
			static uint32_t hash(T&& value)
			{
				return shift_to_32bit_integer
				(
					std::hash<typename std::remove_reference<typename std::remove_cv<T>::type>::type>{}
					(
						std::forward<T>(value)
					)
				);
			}

			static constexpr uint32_t fnv(uint32_t hash, const char* pos)
			{
				return *pos == '\0' ? hash : fnv((hash * 16777619U) ^ *pos, pos+1);
			}

			default_seeds local_entropy()
			{
				// This is a constant that changes every time we compile the code
				constexpr uint32_t compile_stamp =
					fnv(2166136261U, __DATE__ __TIME__ __FILE__);

				// Some people think you shouldn't use the random device much because on some platforms it could be expensive to call or "use up" vital system-wide entropy,
				// so we just call it once.
				static uint32_t random_int = std::random_device{}();

				// The heap can vary from run to run as well.
				void* malloc_address = malloc(sizeof(int));
				free(malloc_address);
				auto heap  = hash(malloc_address);
				auto stack = hash(&malloc_address);

				// Every call, we increment our random int.
				// We don't care about race conditons.
				// The more, the merrier.
				random_int += 0xedf19156;

				// Classic seed, the time.
				// It ought to change, especially since this is (hopefully) nanosecond resolution time.
				auto hitime = std::chrono::high_resolution_clock::now().time_since_epoch().count();

				// Address of the thing being initialized.
				// That can mean that different seed sequences in different places in memory will be different.
				// Even for the same object, it may vary from run to run in systems with ASLR,
				// such as OS X, but on Linux it might not unless we compile with -fPIC -pic.
				auto self_data = hash(this);

				// The address of the time function.
				// It should hopefully be in  a system library that hopefully isn't always in the same place
				// (might not change until system is rebooted though)
				auto time_function_code = hash(&std::chrono::high_resolution_clock::now);

				// The address of the exit function.
				// It should hopefully be in a system library that hopefully isn't always in the same place
				// (might not change until system is rebooted though).
				// Hopefully it's in a different library from time_func.
				auto exit_function_code = hash(&_Exit);

				// The address of a local function.
				// That may be in a totally different part of memory.
				// On OS X it'll vary from run to run thanks to ASLR, on Linux it might not unless we compile with -fPIC -pic.
				// Need the cast because it's an overloaded function and we need to pick the right one.
				auto self_function_code = hash( static_cast<uint32_t (*)(uint64_t)>( &auto_seeded::shift_to_32bit_integer ) );

				// Hash our thread id.
				// It seems to vary from run to run on OS X, not so much on Linux.
				auto thread_id	= hash(std::this_thread::get_id());

				// Hash of the ID of a type.
				// May or may not vary, depending on implementation.
				#if __cpp_rtti || __GXX_RTTI
				auto type_id   = shift_to_32bit_integer(typeid(*this).hash_code());
				#else
				uint32_t type_id   = 0;
				#endif

				// Platform-specific entropy
				auto pid = shift_to_32bit_integer(RANDOM_UTILS_GETPID);
				auto cpu = shift_to_32bit_integer(RANDOM_UTILS_CPU_ENTROPY);

				return
				{
					{
						random_int, shift_to_32bit_integer(hitime), stack, heap, self_data,
						self_function_code, exit_function_code, time_function_code, thread_id, type_id, pid,
						cpu, compile_stamp
					}
				};
			}


		public:
			using SeedSequence::SeedSequence;

			using base_seed_sequence = SeedSequence;

			const base_seed_sequence& base() const
			{
				return *this;
			}

			base_seed_sequence& base()
			{
				return *this;
			}

			auto_seeded(default_seeds seeds)
				: SeedSequence(seeds.begin(), seeds.end())
			{
				// Nothing else to do
			}

			auto_seeded()
				: auto_seeded(local_entropy())
			{
				// Nothing else to do
			}
		};

		using auto_seed_128 = auto_seeded<seed_sequence_fe128>;
		using auto_seed_256 = auto_seeded<seed_sequence_fe256>;


		//////////////////////////////////////////////////////////////////////////////
		//
		// uniform_distribution
		//
		//////////////////////////////////////////////////////////////////////////////

		/*
		 * This template typedef provides either
		 *	  - uniform_int_distribution, or
		 *	  - uniform_real_distribution
		 * depending on the provided type
		 */

		template <typename Numeric>
		using UniformDistributionType = typename std::conditional
		<
			std::is_integral<Numeric>::value,
			CommonSecurity::RND::UniformIntegerDistribution<Numeric>,
			CommonSecurity::RND::UniformRealNumberDistribution<Numeric> 
		>::type;

		//////////////////////////////////////////////////////////////////////////////
		//
		// random_generator
		//
		//////////////////////////////////////////////////////////////////////////////

		/*
		 * randutils::random_generator
		 *
		 *	 An Easy-to-Use Random API
		 *
		 *	 Provides all the power of C++11's random number facility in an easy-to
		 *	 use wrapper.
		 *
		 *	 In normal use, it's accessed via one of the following type aliases, which
		 *	 also use auto_seed_256 by default
		 *
		 *		 randutils::default_rng
		 *		 randutils::mt19937_rng
		 *
		 *	 It's discussed in detail at
		 *		 http://www.pcg-random.org/posts/ease-of-use-without-loss-of-power.html
		 */

		template
		<
			typename RandomEngine = std::default_random_engine,
			typename DefaultSeedSequence = auto_seed_256
		>
		class random_generator
		{

		public:
			using engine_type		= RandomEngine;
			using default_seed_type = DefaultSeedSequence;

		private:
			engine_type _random_engine_;

			// This SFINAE(Substitution failure is not an error) evilness provides a mechanism to cast classes that aren't themselves (technically)
			// Seed Seqeuences but derive from a seed sequence to be passed to functions that require actual Seed Squences.
			// To do so, the class should provide a the type base_seed_sequence and a base() member function.

			template <typename T>
			static constexpr bool has_base_seed_sequence(typename T::base_seed_sequence*)
			{
				return true;
			}

			template <typename T>
			static constexpr bool has_base_seed_sequence(...)
			{
				return false;
			}

			template <typename SeedSequenceBased>
			static auto seed_sequence_cast
			(
				SeedSequenceBased&& seed_sequence,
				typename std::enable_if<has_base_seed_sequence<SeedSequenceBased>(0)>::type* = 0
			) -> decltype(seed_sequence.base())
			{
				return seed_sequence.base();
			}

			template <typename SeedSequence>
			static SeedSequence seed_sequence_cast
			(
				SeedSequence&& seed_sequence,
				typename std::enable_if<!has_base_seed_sequence<SeedSequence>(0)>::type* = 0
			)
			{
				return seed_sequence;
			}

		public:
			template
			<
				typename Seeding = default_seed_type,
				typename... Params
			>
			random_generator(Seeding&& seeding = default_seed_type{})
				: 
				_random_engine_
				{
					seed_sequence_cast(std::forward<Seeding>(seeding))
				}
			{
				// Nothing (else) to do
			}

			// Work around Clang DR777 bug in Clang 3.6 and earlier by adding a
			// redundant overload rather than mixing parameter packs and default
			// arguments.
			//	   https://llvm.org/bugs/show_bug.cgi?id=23029
			template
			<
				typename Seeding,
				typename... Params
			>
			random_generator(Seeding&& seeding, Params&&... params)
				: 
				_random_engine_
				{
					seed_sequence_cast(std::forward<Seeding>(seeding)),
					std::forward<Params>(params)...
				}
			{
				// Nothing (else) to do
			}

			template
			<
				typename Seeding = default_seed_type,
				typename... Params
			>
			void seed(Seeding&& seeding = default_seed_type{})
			{
				_random_engine_.seed(seed_sequence_cast(seeding));
			}

			// Work around Clang DR777 bug in Clang 3.6 and earlier by adding a
			// redundant overload rather than mixing parameter packs and default
			// arguments.
			//	   https://llvm.org/bugs/show_bug.cgi?id=23029
			template
			<
				typename Seeding,
				typename... Params
			>
			void seed(Seeding&& seeding, Params&&... params)
			{
				_random_engine_.seed(seed_sequence_cast(seeding), std::forward<Params>(params)...);
			}


			RandomEngine& engine()
			{
				return _random_engine_;
			}

			template
			<
				typename ResultType,
				template <typename> class ND_Type = std::normal_distribution,
				typename... Params
			>
			ResultType variate(Params&&... params)
			{
				ND_Type<ResultType> dist(std::forward<Params>(params)...);

				return dist(_random_engine_);
			}

			template <typename Numeric>
			Numeric uniform(Numeric lower, Numeric upper)
			{
				return variate<Numeric,UniformDistributionType>(lower, upper);
			}

			template
			<
				typename Iter,
				typename... Params
			>
			void generate(Iter first, Iter last, Params&&... params)
			{
				using result_type = typename std::remove_reference<decltype(*(first))>::type;

				UniformDistributionType<result_type> dist(std::forward<Params>(params)...);

				std::generate(first, last, [&]{ return dist(_random_engine_); });
			}

			template
			<
				typename Range,
				typename... Params
			>
			void generate(Range&& range, Params&&... params)
			{
				generate<UniformDistributionType>
				(
					std::begin(range),
					std::end(range),
					std::forward<Params>(params)...
				);
			}

			template <typename Iter>
			void shuffle(Iter first, Iter last)
			{
				CommonSecurity::ShuffleRangeData(first, last, _random_engine_);
			}

			template <typename Range>
			void shuffle(Range&& range)
			{
				shuffle(std::begin(range), std::end(range));
			}

			template <typename Iter>
			Iter choose(Iter first, Iter last)
			{
				auto dist = std::distance(first, last);
				if (dist < 2)
					return first;
				using distance_type = decltype(dist);
				distance_type choice = uniform(distance_type(0), --dist);
				std::advance(first, choice);
				return first;
			}

			template <typename Range>
			auto choose(Range&& range) -> decltype(std::begin(range))
			{
				return choose(std::begin(range), std::end(range));
			}


			template <typename Range>
			auto pick(Range&& range) -> decltype(*std::begin(range))
			{
				return *choose(std::begin(range), std::end(range));
			}

			template <typename T>
			auto pick(std::initializer_list<T> range) -> decltype(*range.begin())
			{
				return *choose(range.begin(), range.end());
			}

			template <typename Size, typename Iter>
			Iter sample(Size to_go, Iter first, Iter last)
			{
				auto total = std::distance(first, last);
				using value_type = decltype(*first);

				return std::stable_partition
				(
					first, last,
					[&](const value_type&)
					{
						--total;
						using distance_type = decltype(total);
						distance_type zero{};
						if (uniform(zero, total) < to_go)
						{
							--to_go;
							return true;
						}
						else
						{
							return false;
						}
					 }
				);
			}

			template <typename Size, typename Range>
			auto sample(Size to_go, Range&& range) -> decltype(std::begin(range))
			{
				return sample(to_go, std::begin(range), std::end(range));
			}
		};

		using default_rng = random_generator<std::default_random_engine>;
		using mt19937_rng = random_generator<std::mt19937>;

		#undef RANDOM_UTILS_CPU_ENTROPY
		#undef RANDOM_UTILS_GENERALIZED_CONSTEXPR
		#undef RANDOM_UTILS_GETPID

		#endif // RANDUTILS_HPP


		//生成安全的随机数
		//Generate secure random number
		inline auto GenerateSecureRandomNumber(std::random_device& true_hardware_random_device)
		{
			//This is current timestamp
			//当前时间戳
			auto system_clock_current_timestamp = std::chrono::duration_cast<std::chrono::seconds>
			(
				std::chrono::system_clock::now().time_since_epoch()
			).count();
		
			auto high_resolution_clock_current_timestamp = std::chrono::duration_cast<std::chrono::microseconds>
			(
				std::chrono::high_resolution_clock::now().time_since_epoch()
			).count();


			/*return static_cast<std::uint64_t>(true_hardware_random_device())
				^ static_cast<std::uint64_t>(system_clock_current_timestamp)
				^ static_cast<std::uint64_t>(high_resolution_clock_current_timestamp);*/

			/*return static_cast<std::uint32_t>(true_hardware_random_device())
				^ static_cast<std::uint32_t>(system_clock_current_timestamp)
				^ static_cast<std::uint32_t>(high_resolution_clock_current_timestamp >> 32)
				^ static_cast<std::uint32_t>(high_resolution_clock_current_timestamp)
				^ static_cast<std::uint32_t>(system_clock_current_timestamp >> 32);*/

			return static_cast<std::uint64_t>(true_hardware_random_device())
				^ static_cast<std::uint64_t>(system_clock_current_timestamp)
				^ static_cast<std::uint64_t>(high_resolution_clock_current_timestamp >> 32)
				^ static_cast<std::uint64_t>(high_resolution_clock_current_timestamp)
				^ static_cast<std::uint64_t>(system_clock_current_timestamp >> 32);
		}

		//生成安全的随机数种子
		//Generate secure random number seeds
		template<typename RandomNumberSeedType> requires std::integral<RandomNumberSeedType>
		inline RandomNumberSeedType GenerateSecureRandomNumberSeed(std::random_device& true_hardware_random_device)
		{
			/*
				RandomNumberSeedType random_number_value = CURRENT_SYSTEM_BITS == 64
				? static_cast<RandomNumberSeedType>( GenerateSecureRandomNumber(true_hardware_random_device) ) ^ static_cast<RandomNumberSeedType>( GenerateSecureRandomNumber(true_hardware_random_device) >> 32)
				: static_cast<RandomNumberSeedType>( GenerateSecureRandomNumber(true_hardware_random_device) );
			*/
		
			RandomNumberSeedType random_number_value = static_cast<RandomNumberSeedType>( GenerateSecureRandomNumber(true_hardware_random_device) );

			return random_number_value;
		}

		//生成安全的随机数种子序列
		//Generate a secure sequence of random number seeds
		template<typename RandomNumberSeedType> requires std::integral<RandomNumberSeedType>
		inline std::vector<RandomNumberSeedType> GenerateSecureRandomNumberSeedSequence(std::size_t size)
		{
			using namespace CommonSecurity::Seeds;
			using seed_sequence_fe_type = seed_sequence_fe<64, RandomNumberSeedType>;
			using auto_seed_type = auto_seeded<seed_sequence_fe_type>;
		
			if(size == 0)
				size = 1;

			std::vector<RandomNumberSeedType> random_number_seed_sequence(size, 0);

			auto_seed_type seeder;
			seeder.generate(random_number_seed_sequence.begin(), random_number_seed_sequence.end());

			return random_number_seed_sequence;
		}
	}
}
