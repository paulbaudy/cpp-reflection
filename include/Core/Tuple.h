/*!
 *  @file Tuple.h
 *  @author Paul
 *  @date 2024-11-20
 *
 *  Declares the tuple helper class & MakeTuple method.
 */

#pragma once

#include <tuple>
#include <utility> 
#include <type_traits>

template <class _Ty>
struct _Unrefwrap_helper { // leave unchanged if not a reference_wrapper
	using type = _Ty;
};

template <class _Ty>
struct _Unrefwrap_helper<std::reference_wrapper<_Ty>> { // make a reference from a reference_wrapper
	using type = _Ty&;
};
 // decay, then unwrap a reference_wrapper
template <class _Ty>
using _Unrefwrap_t = typename _Unrefwrap_helper<std::decay_t<_Ty>>::type;

/*!
 * Tuple child class to add the templated Get method 
 */
template <typename... Ts>
class RTuple : public std::tuple<Ts...> 
{
public:
	using std::tuple<Ts...>::tuple;

	template <std::size_t N>
	decltype(auto) Get() const
	{
		return std::get<N>(*this);
	}
};

template <class... _Types>
[[nodiscard]] constexpr RTuple<_Unrefwrap_t<_Types>...> MakeTuple(_Types&&... _Args) 
{
	using _Ttype = RTuple<_Unrefwrap_t<_Types>...>;
	return _Ttype(_STD forward<_Types>(_Args)...);
}