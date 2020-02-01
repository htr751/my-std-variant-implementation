#pragma once
#include<type_traits>
#include"TratisForListOfTypes.h"
#include<memory>

template<typename T, typename... Types>
class variant {
	typename std::aligned_storage<TratisForListOfTypes<T, Types...>::size, TratisForListOfTypes<T, Types...>::align>::type storage;
	bool is_valid;
	unsigned int curType;

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
			unsigned int index = TratisForListOfTypes<T, Types...>::template typeIndex<std::remove_reference_t<U>>();
			this->curType = index;
		}
		catch (std::out_of_range & e) {
			std::cout << e.what() << std::endl;
			throw e;
		}
		new (&storage) std::remove_reference_t<U>(std::forward<U>(value));
		this->is_valid = true;
	}

	~variant() {
		this->destructObjects(std::make_integer_sequence<int, TratisForListOfTypes<T, Types...>::numOfTypes>{});
	}

private:
	template<int... Seq>
	void destructObjects(std::integer_sequence<int, Seq...> seq) {
		(destructObjectsHelper<Seq, std::remove_pointer_t<decltype(TratisForListOfTypes<T, Types...>::template getType<Seq>())>>(), ...);
	}

	template<int n, typename T>
	bool destructObjectsHelper() {
		if (n != this->curType)
			return false;
		std::destroy_at(std::launder(reinterpret_cast<T*>(&this->storage._Pad)));
		return true;
	}

};

template<typename T>
void destruct(T& t) {
	((T&)t).~T();
}