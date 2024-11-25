/*!
 *  @file TupleVisitor.h
 *  @author Paul
 *  @date 2024-11-20
 *
 */

#include <functional>
#include <stdint.h>
#include "Tuple.h"

using uint32 = std::uint32_t;

template <typename T, T... Indices>
struct TIntegerSequence
{
};


template <typename TupleType>
struct TCVTupleArity;

template <typename... Types>
struct TCVTupleArity<const volatile RTuple<Types...>>
{
	enum { Value = sizeof...(Types) };
};

template <typename TupleType>
struct TTupleArity : TCVTupleArity<const volatile TupleType>
{
};

#if defined(__GNUC__) || defined(__GNUG__)
template <typename T, T N>
using TMakeIntegerSequence = __integer_pack<TIntegerSequence, T, N>;
#else 
template <typename T, T N>
using TMakeIntegerSequence = __make_integer_seq<TIntegerSequence, T, N>;
#endif

template <typename IntegerSequence>
struct TVisitTupleElements_Impl;

template <uint32... Indices>
struct TVisitTupleElements_Impl<TIntegerSequence<uint32, Indices...>>
{
	// We need a second function to do the invocation for a particular index, to avoid the pack expansion being
	// attempted on the indices and tuples simultaneously.
	template <uint32 Index, typename FuncType, typename... TupleTypes>
	inline static void InvokeFunc(FuncType&& Func, TupleTypes&&... Tuples)
	{
		std::invoke(std::forward<FuncType>(Func), std::forward<TupleTypes>(Tuples).template Get<Index>()...);
	}

	template <typename FuncType, typename... TupleTypes>
	static void Do(FuncType&& Func, TupleTypes&&... Tuples)
	{
		// This should be implemented with a fold expression when our compilers support it
		int Temp[] = { 0, (InvokeFunc<Indices>(std::forward<FuncType>(Func), std::forward<TupleTypes>(Tuples)...), 0)... };
		(void)Temp;
	}
};

template <typename FuncType, typename FirstTupleType, typename... TupleTypes>
inline void VisitTupleElements(FuncType&& Func, FirstTupleType&& FirstTuple, TupleTypes&&... Tuples)
{
	TVisitTupleElements_Impl<TMakeIntegerSequence<uint32, TTupleArity<std::decay_t<FirstTupleType>>::Value>>::Do(std::forward<FuncType>(Func), std::forward<FirstTupleType>(FirstTuple), std::forward<TupleTypes>(Tuples)...);
}