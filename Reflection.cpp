﻿/*!
 *  @file Reflection.cpp
 *  @author Paul
 *  @date 2024-11-21
 *
 *  main entry point
 */

#include "Reflection/LayoutIterator.h"
#include "Reflection/LayoutView.h"
#include "Reflection/Layout.h"

#include <iostream>

struct FooStruct
{
	double X = 0.f;
	double Y = 0.f;
	double Z = 0.f;
	std::uint32_t W = 0;
};

RF_BEGIN_LAYOUT(FooStruct)
	RF_ENTRY(X),
	RF_ENTRY(Y),
	RF_ENTRY(Z),
	RF_ENTRY(W)
RF_END_LAYOUT()

// Should be replaced by expansion statements in future versions of C++

namespace Reflection
{
	template<class T>
	class TReflectionTraits
	{
		TReflectionTraits() = delete;
	};

	template<>
	class TReflectionTraits<double>
	{
	public:
		static void Foo(const double& InValue)
		{
			std::cout << "Double : " << InValue << std::endl;
		}
	};

	template<>
	class TReflectionTraits<std::uint32_t>
	{
	public:
		static void Foo(const std::uint32_t& InValue)
		{
			std::cout << "Integer32 : " << InValue << std::endl;
		}
	};

	template<class T, bool bHasTraits = TIsConstructible<TReflectionTraits<T>>::Value>
	class FReflectionStateHelper;

	template<class T>
	class FReflectionStateHelper<T, false>
	{
	public:
		template<class parent_t, class field_t> static Reflection::EFieldIterator FooIterator(const parent_t&, const field_t&, const Rf::FLayoutFieldView&) { return EFieldIterator::Enter; }
	};

	template<class T>
	class FReflectionStateHelper<T, true>
	{
	public:
		template<class field_t, class parent_t>
		static Reflection::EFieldIterator FooIterator(const parent_t&, const field_t& InField, const Rf::FLayoutFieldView& InViewer)
		{
			const auto& Value = InViewer.Get(InField);
			TReflectionTraits<T>::Foo(Value);
			return EFieldIterator::Stop;
		}
	};

	template<class field_t, class parent_field_t>
	Reflection::EFieldIterator FooIterator(const parent_field_t& InParentField, const field_t& InField, const Rf::FLayoutFieldView& InViewer)
	{
		using T = typename field_t::Type;
		return FReflectionStateHelper<T>::FooIterator(InParentField, InField, InViewer);
	}


	void Test()
	{
		FooStruct Foo{ 1.f, 2.f, 3.f, 4};
		Rf::FLayoutFieldView FooViewer(Rf::Ref(Foo));

		auto Callable = [&FooViewer](const auto& ParentField, const auto& Field) { return FooIterator(ParentField, Field, FooViewer); };
		Reflection::IterateLayoutNamed<FooStruct>(Callable);

	}
}

int main()
{
	Reflection::Test();
	return 0;
}
