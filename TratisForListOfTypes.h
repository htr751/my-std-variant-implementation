#pragma once
#include<cstddef>
#include<algorithm>
#include<stdexcept>
#include<utility>

template<typename... Types>
struct TratisForListOfTypes {};

template<>
struct TratisForListOfTypes<> {
	static constexpr std::size_t size = 0;
	static constexpr std::size_t align = 0;
	static constexpr unsigned int numOfTypes = 0;

	template<std::size_t index>
	static decltype(auto) getType()  {
		throw std::out_of_range("index is bigger than the number of types");
	}

	template<typename Type>
	static decltype(auto) typeIndex()  {
		throw std::out_of_range("type doesn't exist in collection");
	}
};

template<typename T, typename... Types>
struct TratisForListOfTypes<T, Types...> {
	static constexpr std::size_t size = std::max(sizeof(T), TratisForListOfTypes<Types...>::size);
	static constexpr std::size_t align = std::max(alignof(T), TratisForListOfTypes<Types...>::align);
	static constexpr unsigned int numOfTypes = TratisForListOfTypes<Types...>::numOfTypes + 1;

	template<std::size_t index>
	static constexpr decltype(auto) getType() {
		if constexpr (index == 0)
			return (T*)nullptr;
		else
			return TratisForListOfTypes<Types...>::template getType<index - 1>();
	}

	template<typename Type>
	static constexpr decltype(auto) typeIndex()  {
		if constexpr (std::is_same_v<Type, T>)
			return 0;
		else return TratisForListOfTypes<Types...>::template typeIndex<Type>() + 1;
	}
};
