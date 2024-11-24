/*!
 *  @file Layout.h
 *  @author Paul
 *  @date 2024-11-20
 *
 *  Declares the main reflection class for objects.
 */

#pragma once

#include <stdint.h>

#include "Field.h"
#include <Core/StaticString.h>
#include <Core/Tuple.h>
#include <Core/Name.h>

using int32 = std::int32_t;

#define WIDETEXT(x)  L ## x

/**
 * Determines if T is constructible from a set of arguments.
 */
template <typename T, typename... Args>
struct TIsConstructible
{
	enum { Value = __is_constructible(T, Args...) };
};



namespace Reflection
{
	/**
	 * void_t implementation (todo move to TypeTraits/VoidT)
	 */
	template<class... Ts>
	using VoidT = void;

	enum class EFieldIterator
	{
		/** Enter nested structures */
		Enter,
		/** Stop */
		Stop
	};

	namespace Details
	{
		/**
		* Helper to implement a state layout
		*/

		template<class T, class = void>
		class TDefaultLayoutImpl
		{
		public:
			TDefaultLayoutImpl() = delete;
		};

		template<class T>
		class TDefaultLayoutImpl<T, VoidT<typename T::Layout>> : public T::Layout
		{
		};
	}

	/**
	* Rig layout accessor
	* @tparam T Type to get the layout from
	*/
	template<class T>
	class TLayout : public Details::TDefaultLayoutImpl<T>
	{
	};

	/**
	* Check whether a type has a layout
	* Provides HasLayout<T>::Value 1 if so, 0 otherwise
	*/
	template<class T>
	struct HasLayout : TIsConstructible<TLayout<T>> {};

	/**
	 * Construct a tuple of each layout elements for a given type
	 * @return Tuple of TLayoutField<> with its corresponding member name
	 */
	template<class T>
	constexpr decltype(auto) MakeNamedLayout()
	{
		return TLayout<T>::MakeLayout();
	}


	/**
	* Type of a layout (named)
	*/
	template<class T>
	using TNamedLayoutType = typename std::decay<decltype(TLayout<T>::MakeLayout())>::Type;

	namespace Details
	{
		template<class T>
		struct TLayoutTupleImpl;

		template<class... Ts>
		struct TLayoutTupleImpl<RTuple<Ts...>>
		{
			using Type = RTuple<typename Ts::AnonymousType...>;
		};
	}

	template<class T>
	using TLayoutType = typename Details::TLayoutTupleImpl<TNamedLayoutType<T>>::Type;

	/**
	* Construct a tuple of each layout elements for a given type (anonymous)
	* @return Tuple of TLayoutField<>
	*/
	template<class T>
	constexpr decltype(auto) MakeLayout()
	{
		return TLayoutType<T>{};
	}
}

#define RF_BEGIN_LAYOUT(T)\
namespace Reflection{\
template<>\
class TLayout<T>\
{\
public:\
	using Type = T;\
	static constexpr decltype(auto) GetName() { return ::Reflection::MakeStaticString(WIDETEXT(#T));}\
	static FName GetFName() { static FName Result{WIDETEXT(#T)}; return Result; }\
	static constexpr decltype(auto) MakeLayout() { return MakeTuple(

#define RF_END_LAYOUT()\
);}\
};}

#define RF_ENTRY(N) ::Reflection::MakeField<typename std::decay<decltype(Type::N)>::type, offsetof(Type, N)>(WIDETEXT(#N))
