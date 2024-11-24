/*!
 *  @file StaticString.h
 *  @author Paul
 *  @date 2024-11-20
 *
 *  Declares a static string class with stack allocation.
 */

#pragma once

#include <stdint.h>
#include <string>

using int32 = std::int32_t;

namespace Reflection
{
	template<int32 n>
	struct TStaticString
	{
		using CharType = wchar_t;
		CharType Data[n] = {};

		/**
		 * Construct from a literal string
		 * @param InData Literal string
		 */
		constexpr TStaticString(const CharType(&InData)[n])
		{
			for (std::int32_t i = 0; i < n; ++i) Data[i] = InData[i];
		}

		constexpr TStaticString() = default;
		constexpr TStaticString(const TStaticString&) = default;
		constexpr TStaticString(TStaticString&&) = default;
		constexpr TStaticString& operator=(const TStaticString&) = default;
		constexpr TStaticString& operator=(TStaticString&&) = default;

		constexpr const CharType* CStr() const { return (Data); }
		constexpr std::int32_t Num() const { return n - 1; }

		constexpr CharType& operator[](int32 i) { return Data[i]; }
		constexpr const CharType& operator[](int32 i) const { return Data[i]; }

		/**
		 * Convert to a standard C++ string
		 * @return std::string
		 */
		std::string ToStdString() const { return std::string(Num(), CStr()); }

	private:
		/**
		 * Construct which concatenates two strings
		 * @param LHS Prefix
		 * @param RHS Suffix
		 */
		template<int32 a, int32 b>
		constexpr TStaticString(const TStaticString<a>& LHS, const TStaticString<b>& RHS)
		{
			for (int32 i = 0; i < a - 1; ++i)
				Data[i] = LHS[i];

			for (int32 i = 0; i < b; ++i)
				Data[a - 1 + i] = RHS[i];
		}

		template<int32 n1, int32 n2>
		friend constexpr TStaticString<n1 + n2 - 1> operator+ (const TStaticString<n1>&, const TStaticString<n2>&);
	};

	/**
	* Concatenate two static strings
	* @param LHS Prefix
	* @param RHS Suffix
	* @return Concatenated string
	*/
	template<int32 n1, int32 n2>
	constexpr TStaticString<n1 + n2 - 1> operator+ (const TStaticString<n1>& LHS, const TStaticString<n2>& RHS)
	{
		return TStaticString<n1 + n2 - 1>{LHS, RHS};
	}

	/**
	 * Create a static string from a literal string, deducing its size from the input argument
	 * @return Static string
	 */
	template<int32 n>
	constexpr TStaticString<n> MakeStaticString(const wchar_t(&InData)[n])
	{
		return TStaticString<n>{InData};
	}
}