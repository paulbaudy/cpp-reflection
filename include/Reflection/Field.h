/*!
 *  @file Field.h
 *  @author Paul
 *  @date 2024-11-20
 *
 *  Declares a field class to reflect object members
 */
#pragma once

#include <type_traits>
#include <Core/StaticString.h>

#define WIDETEXT(x)  L ## x


using int32 = std::int32_t;

namespace Reflection
{
	namespace Details
	{
		/**
		* Empty string object
		* Mimics TStaticString interface, performing no operation
		*/
		class FLayoutNameEmptyString
		{
		public:
			constexpr FLayoutNameEmptyString() = default;
			constexpr FLayoutNameEmptyString(const FLayoutNameEmptyString&) = default;
			constexpr FLayoutNameEmptyString(FLayoutNameEmptyString&&) = default;
		};

		constexpr FLayoutNameEmptyString operator+(FLayoutNameEmptyString, FLayoutNameEmptyString)
		{
			return FLayoutNameEmptyString{};
		}
	}

	/**
	* Layout field
	* Provides the following :
	* Type : Type of the field
	* MemberOffset : Local field offset
	* Name() : Name of the field (if parsed)
	*/
	template<class T, int32 offset, int32 name_len = 0>
	struct TLayoutField : std::conditional<(name_len > 0), TStaticString<name_len>, Details::FLayoutNameEmptyString>::type
	{
		/**
		 * Type of the GetName() function
		 * Either a FLayoutNameEmpty or a TStaticString<n>
		 */
		using NameType = typename std::conditional< (name_len > 0), TStaticString<name_len>, Details::FLayoutNameEmptyString >::type;

	/** Type held by this field */
	using Type = T;
	/** Field memory offset */
	enum { MemberOffset = offset };

	/** Anonymous field type */
	using AnonymousType = TLayoutField<T, offset>;

	///** Tags held by this field */
	//using TTags = Tuple;
	///** Tag values */
	//TTags Tags;


	constexpr TLayoutField() = default;
	constexpr TLayoutField(const TLayoutField&) = default;
	constexpr TLayoutField(TLayoutField&&) = default;
	constexpr TLayoutField& operator=(const TLayoutField&) = default;
	constexpr TLayoutField& operator=(TLayoutField&&) = default;

	constexpr TLayoutField(const NameType& InName) : NameType(InName) {}
	constexpr TLayoutField(NameType&& InName) : NameType(std::move(InName)) {}

	/**
	* Get the offset of this field
	* @return Offset
	*/
	constexpr int32 GetOffset() const { return offset; }
	/**
	* Get the name of this field
	* @return Name
	*/
	constexpr const NameType& GetName() const { return static_cast<const NameType&>(*this); }
	};

	/**
	* Create an anonymous field
	* @return Field
	*/
	template<class T, int32 offset>
	constexpr TLayoutField<T, offset> MakeField()
	{
		return TLayoutField<T, offset>{};
	}
	/**
	* Create an anonymous field
	* @return Field
	*/
	template<class T, int32 offset>
	constexpr TLayoutField<T, offset> MakeField(Details::FLayoutNameEmptyString)
	{
		return TLayoutField<T, offset>{};
	}
	/**
	* Create a named field
	* @param InName Name of the field
	* @return Field
	*/
	template<class T, int32 offset, int32 n>
	constexpr TLayoutField<T, offset, n> MakeField(const TStaticString<n>& InName)
	{
		return TLayoutField<T, offset, n>{InName};
	}

	/**
	* Create a named field
	* @param InName Name of the field
	* @return Field
	*/
	template<class T, int32 offset, int32 n>
	constexpr TLayoutField<T, offset, n> MakeField(TStaticString<n>&& InName)
	{
		return TLayoutField<T, offset, n>{std::move(InName)};
	}

	/**
	* Create a named field
	* @param InName Name of the field
	* @return Field
	*/
	template<class T, int32 offset, int32 n>
	constexpr TLayoutField<T, offset, n> MakeField(const wchar_t(&InName)[n])
	{
		return TLayoutField<T, offset, n>{TStaticString<n>{InName}};
	}

	/**
	* Concatenate names ; empty base
	* @param InBase base name
	* @param InDerived Derived name
	* @return Base.Derived string
	*/
	template<int32 n1, int32 n2>
	constexpr TStaticString<n1 + n2> ConcatFieldName(const TStaticString<n1>& InBase, const TStaticString<n2>& InDerived)
	{
		return InBase + MakeStaticString(WIDETEXT(".")) + InDerived;
	}
	/**
	* Concatenate names ; empty base
	* @param InBase base name
	* @param InDerived Derived name
	* @return Derived string
	*/
	template<int32 n2>
	constexpr TStaticString<n2> ConcatFieldName(const TStaticString<1>& InBase, const TStaticString<n2>& InDerived)
	{
		return InDerived;
	}

	/**
	* Concatenate member names ; empty strings
	* @return Empty string
	*/
	constexpr Details::FLayoutNameEmptyString ConcatFieldName(Details::FLayoutNameEmptyString, Details::FLayoutNameEmptyString) { return Details::FLayoutNameEmptyString{}; }
	/**
	* Concatenate member names ; empty strings
	* @return Empty string
	*/
	template<int32 n>
	constexpr Details::FLayoutNameEmptyString ConcatFieldName(Details::FLayoutNameEmptyString, const TStaticString<n>&) { return Details::FLayoutNameEmptyString{}; }
}