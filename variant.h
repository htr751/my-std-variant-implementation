#pragma once
#include<type_traits>
#include"TratisForListOfTypes.h"
#include<memory>
#include<variant>

template<typename T, typename... Types>
class variant {
	typename std::aligned_storage<TratisForListOfTypes<T, Types...>::size, TratisForListOfTypes<T, Types...>::align>::type storage;
	bool is_valid;
	std::size_t curType;

public:
	template<typename U = T, typename C = std::enable_if_t<std::is_default_constructible_v<U>, bool>>
	variant() {
		new(&storage) T();
		this->is_valid = true;
		this->curType = 0;
	}

	template<typename U>
	variant(U&& value) {
		try {
			std::size_t index = TratisForListOfTypes<T, Types...>::template typeIndex<std::remove_reference_t<U>>();
			this->curType = index;
		}
		catch (std::out_of_range & e) {
			throw e;
		}
		new (&storage) std::remove_reference_t<U>(std::forward<U>(value));
		this->is_valid = true;
	}

	constexpr std::size_t index() const noexcept {
		return this->curType;
	}

	template<typename U>
	void operator=(U&& value) {
		std::size_t index = 0;
		try {
			index = TratisForListOfTypes<T, Types...>::template typeIndex<std::remove_reference_t<U>>();
		}
		catch(std::out_of_range& e){
			throw e;
		}

		this->destructObject();
		new (&this->storage._Pad) std::remove_reference_t<U>(std::forward<U>(value));
		this->curType = index;
		this->is_valid = true;
	}

	~variant() {
		this->destructObject();
	}

private:
	void destructObject() {
		this->destructObjectHelper(std::make_integer_sequence<int, TratisForListOfTypes<T, Types...>::numOfTypes>{});
	}

	template<int... Seq>
	void destructObjectHelper(std::integer_sequence<int, Seq...> seq) {
		(DestructObjectIfIncluded<Seq, std::remove_pointer_t<decltype(TratisForListOfTypes<T, Types...>::template getType<Seq>())>>(), ...);
	}

	template<int n, typename T>
	bool DestructObjectIfIncluded() {
		if (n != this->curType)
			return false;
		std::destroy_at(std::launder(reinterpret_cast<T*>(&this->storage._Pad)));
		return true;
	}

	template<std::size_t Index, typename... Types>
	friend decltype(auto) get(const variant<Types...>& value);

};

template<std::size_t Index, typename... Types>
decltype(auto) get(const variant<Types...>& value) {
	if (value.curType != Index)
		throw std::bad_variant_access();

	return (std::add_lvalue_reference_t<std::remove_pointer_t<decltype(TratisForListOfTypes<Types...>::template getType<Index>())>>)value.storage._Pad;
}