/*!
 *  @file LayoutView.h
 *  @author Paul
 *  @date 2024-11-20
 *
 *  Declares the layout view.
 */

#pragma once

#include <span>
#include <cstring>
#include <Core/Name.h>

using int32 = std::int32_t;
using uint8 = std::uint8_t;

namespace Rf
{
	// Rebindable reference
	template<class T>
	class TReferenceWrapper
	{
	public:
		TReferenceWrapper(T& InRef)
			: Ref(&InRef)
		{
		}
		TReferenceWrapper(const TReferenceWrapper&) = default;
		TReferenceWrapper(TReferenceWrapper&&) = default;
		TReferenceWrapper& operator=(const TReferenceWrapper&) = default;
		TReferenceWrapper& operator=(TReferenceWrapper&&) = default;

		T& Get() const { return *Ref; }

		operator T& () const { return Get(); }

		/**
		 * Invokes this reference wrapper as if by calling Get()(args...)
		 * @param InArgs Invoke arguments
		 * @return Get()(InArgs...)
		 */
		template<class... args_t>
		auto operator()(args_t&&... InArgs) const -> decltype(Invoke(DeclVal<T&>(), DeclVal<args_t>()...))
		{
			return (*Ref)(Forward<args_t>(InArgs)...);
		}
		/**
		 * Invokes this reference wrapper as if by calling Get()(args...)
		 * @param InArgs Invoke arguments
		 * @return Get()(InArgs...)
		 */
		template<class... args_t>
		auto operator()(args_t&&... InArgs) -> decltype(Invoke(DeclVal<T&>(), DeclVal<args_t>()...))
		{
			return (*Ref)(Forward<args_t>(InArgs)...);
		}

	private:
		T* Ref;
	};

	template<class T>
	TReferenceWrapper<T> Ref(T& InRef)
	{
		return TReferenceWrapper<T>(InRef);
	}

	template<class T>
	TReferenceWrapper<const T> CRef(const T& InRef)
	{
		return TReferenceWrapper<const T>(InRef);
	}

	/**
	 * View to a reflectable
	 */
	template<bool is_const>
	class TLayoutFieldView
	{
	public:
		template<class T>
		using RefType = typename std::conditional<is_const, const typename std::remove_reference<T>::type&, typename std::remove_reference<T>::type&>::type;
		/** Reference wrapper type */
		template<class T>
		using RefWrapType = typename std::conditional<is_const, TReferenceWrapper<const T>, TReferenceWrapper<T>>::type;
		/** Value type */
		template<class T>
		using ValueType = typename std::conditional<is_const, const T, T>::type;
		/** Internal type */
		using ByteType = typename std::conditional<is_const, const uint8, uint8>::type;

		TLayoutFieldView() = default;

		/**
		 * Construct from a reference to a state
		 * @param InState State to refer to
		 */
		template<class T>
		explicit TLayoutFieldView(TReferenceWrapper<T> InState)
			: Data(reinterpret_cast<ByteType*>(&(InState.Get())))
			, Size(sizeof(T))
#if CHECK_STATE_TYPE
			, Type(TTypeInfo<std::remove_const_t<T>>::GetFName())
#endif
		{
		}

		TLayoutFieldView(FName InType, std::span<ByteType> InData)
			: Data(InData.GetData())
			, Size(InData.Num())
#if CHECK_STATE_TYPE
			, Type(InType)
#endif
		{
		}

		TLayoutFieldView(const TLayoutFieldView&) = default;
		TLayoutFieldView(TLayoutFieldView&&) = default;
		TLayoutFieldView& operator=(const TLayoutFieldView&) = default;
		TLayoutFieldView& operator=(TLayoutFieldView&&) = default;


		/**
		 * Cast this state
		 * @tparam T Type to cast to
		 * @return Reference to the casted type
		 */
		template<class T>
		RefType<T> Cast() const
		{
#if CHECK_STATE_TYPE
			// Check types if enabled
			check(Type == TTypeInfo<T>::GetFName());
			check(Size >= static_cast<int32>(sizeof(T)));
#endif
			return *reinterpret_cast<ValueType<T>*>(Data);
		}

		/**
		 * Copy to another state
		 * @param OutState State to copy to
		 */
		void CopyTo(TLayoutFieldView<false> OutState) const
		{
#if CHECK_STATE_TYPE
			check(Type == OutState.Type && Size >= OutState.Size);
#endif
			std::memcpy(OutState.Data, Data, Size);
		}

		/**
		 * Check whether this state is valid
		 * @return True if valid
		 */
		bool IsValid() const
		{
			return Data != nullptr;
		}

		/**
		 * Extract a field value
		 * @param InField Field to extract
		 * @return Reference to the member
		 */
		template<class T, int32 offset, int32 n>
		RefType<T> Get(const Reflection::TLayoutField<T, offset, n>& InField) const
		{
			(void)InField;	// compilation warning
			return (*reinterpret_cast<ValueType<T>*>(Data + offset));
		}

		/**
		 * Get a view to the data
		 * @return View to data
		 */
		std::span<ByteType> GetData() const { return TArrayView<ByteType>(Data, Size); }

#if CHECK_STATE_TYPE
		FName GetType() const { return Type; }
#endif

	private:
		ByteType* Data = nullptr;
		int32 Size = 0;
#if CHECK_STATE_TYPE
		FName Type;
#endif

		friend class TLayoutFieldView<!is_const>;
	};

	class FLayoutFieldView : public TLayoutFieldView<false>
	{
	public:
		using TLayoutFieldView<false>::TLayoutFieldView;
	};

	class FLayoutFieldConstView : public TLayoutFieldView<true>
	{
	public:
		using TLayoutFieldView<true>::TLayoutFieldView;

		FLayoutFieldConstView() = default;
		FLayoutFieldConstView(const FLayoutFieldConstView&) = default;
		FLayoutFieldConstView(FLayoutFieldConstView&&) = default;
		FLayoutFieldConstView& operator=(const FLayoutFieldConstView&) = default;
		FLayoutFieldConstView& operator=(FLayoutFieldConstView&&) = default;
		/**
		 * Construct from a non-const state
		 * @param InState Non const state
		 */
		FLayoutFieldConstView(const FLayoutFieldView& InState)
			: TLayoutFieldView<true>(reinterpret_cast<const TLayoutFieldView<true>&>(InState))
		{
		}
	};
};
