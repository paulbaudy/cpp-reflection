/*!
 *  @file LayoutIterator.h
 *  @author Paul
 *  @date 2024-11-20
 *
 * Class to iterate over a layout, re-entering child fields if desired
 * Tracks fields name
 *
 * Callable is expected to return a control flow value, defining whether or not to enter within child fields
 */

#pragma once

#include "Reflection/Layout.h"
#include "Core/TupleVisitor.h"

#define WIDETEXT(x)  L ## x

namespace Reflection
{
	namespace Details
	{
		// Base empty model
		template<class T, bool has_layout = HasLayout<typename T::Type>::Value>
		struct IterateNamedLayoutHelper;

		/**
		* Iterate layout helper
		* Specialization for types with no layout
		* Simply invoke the callable
		*/
		template<class T>
		struct IterateNamedLayoutHelper<T, false>
		{
			template<class field_t, class callable_t, class parent_field_t, class... args_t>
			constexpr void operator()(const parent_field_t& InParentField, const field_t& InField, callable_t&& InCallable, args_t&&... InArgs) const
			{
				(std::forward<callable_t>(InCallable))(InParentField, InField, std::forward<args_t>(InArgs)...);
			}
		};

		/**
		 * Iterate layout helper
		 * Specialization for types with a layout
		 * => If the callable returned EFieldIterator::Continue; iterate over child fields of the layout
		 */
		template<class T>
		struct IterateNamedLayoutHelper<T, true>
		{
			template<class field_t, class callable_t, class parent_field_t, class... args_t>
			constexpr void operator()(const parent_field_t& InParentField, const field_t& InField, callable_t&& InCallable, args_t&&... InArgs) const
			{
				EFieldIterator Chain = (std::forward<callable_t>(InCallable))(InParentField, InField, std::forward<args_t>(InArgs)...);

				if (Chain == EFieldIterator::Stop)
					return;

				IterateLayoutNamed
				(
					// Parent field
					// Type is the current type, offset is member::offset + parent::offset
					// Name as "x.y"; where x is the parent field name and y this field name (produces only "y" if parent is unnamed, i.e the fake void root field)
					InField
					, MakeNamedLayout<typename field_t::Type>()
					, std::forward<callable_t>(InCallable), std::forward<args_t>(InArgs)...
				);
			}
		};
	}

	/**
	* iterator over a layout field
	*/
	template<class field_t, class callable_t, class parent_field_t, class... args_t>
	constexpr void IterateLayoutNamed(const parent_field_t& InParentField, const field_t& InField, callable_t&& InCallable, args_t&&... InArgs)
	{
		constexpr int32 TotalOffset = static_cast<int32>(parent_field_t::MemberOffset) + static_cast<int32>(field_t::MemberOffset);
		const auto ChildField = MakeField<typename field_t::Type, TotalOffset>(ConcatFieldName(InParentField.GetName(), InField.GetName()));

		Details::IterateNamedLayoutHelper<field_t>{}(InParentField, ChildField, std::forward<callable_t>(InCallable), std::forward<args_t>(InArgs)...);
	}

	template<class... Ts, class callable_t, class parent_field_t, class... args_t>
	constexpr void IterateLayoutNamed(const parent_field_t& InParentField, const RTuple<Ts...>& InFields, callable_t&& InCallable, args_t&&... InArgs)
	{
		auto fn = [&](const auto& InField) { IterateLayoutNamed(InParentField, InField, std::forward<callable_t>(InCallable), std::forward<args_t>(InArgs)...); };
		VisitTupleElements(fn, InFields);
	}

	/**
	* Iterate over a layout (named)
	* @param InFields Layout fields
	*/
	template<class... Ts, class callable_t, class... args_t>
	constexpr void IterateLayoutNamed(const RTuple<Ts...>& InFields, callable_t&& InCallable, args_t&&... InArgs)
	{
		// Execute iterate layout(); producing a fake named parent of type void with name ""
		IterateLayoutNamed(TLayoutField<void, 0, 1>{WIDETEXT("")}, InFields, std::forward<callable_t>(InCallable), std::forward<args_t>(InArgs)...);
	}

	/**
	 * Iterate over the members of a layout
	 * @tparam T Type
	 * @param InCallable Callable to execute for each fields of T's layout
	 */
	template<class T, class callable_t, class... args_t>
	constexpr void IterateLayoutNamed(callable_t&& InCallable, args_t&&... InArgs)
	{
		IterateLayoutNamed(TLayout<T>::MakeLayout(), std::forward<callable_t>(InCallable), std::forward<args_t>(InArgs)...);
	}
}