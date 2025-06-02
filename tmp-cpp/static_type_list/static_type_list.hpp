#pragma once

#include <cstdint>
#include <cstddef>
#include <limits>

template <typename Type1, typename Type2>
struct is_same
{
	constexpr static bool value = false;
};

template <typename Type>
struct is_same<Type, Type>
{
	constexpr static bool value = true;
};

template <typename Type1, typename Type2>
constexpr static bool is_same_v = is_same<Type1, Type2>::value;

template <bool Flag, typename T1, typename T2>
struct conditional
{
	using type = T1;
};

template <typename T1, typename T2>
struct conditional<false, T1, T2>
{
	using type = T2;
};

template <bool Flag, typename T1, typename T2>
using conditional_t = typename conditional<Flag, T1, T2>::type;

namespace StaticTypes
{

	/* type_list: A container to store types. It does not support storage of values. */
	template <typename ...Args>
	struct type_list
	{

	};

	/* length: For a given type_list, return the total amount of types stored. */
	template <typename List>
	struct length;

	template<typename... Args>
	struct length<type_list<Args...>>
	{
		constexpr static std::size_t value = sizeof...(Args);
	};

	/* length tests. */
	namespace 
	{
		static_assert(length<type_list<>>::value == 0);
		static_assert(length<type_list<int>>::value == 1);
		static_assert(length<type_list<int, double>>::value == 2);
	}

	/* front: Returns the type of the front (first) element of a give type_list. */
	template <typename List>
	struct front;

	template <typename Arg, typename... Args>
	struct front<type_list<Arg, Args...>>
	{
		using type = Arg;
	};

	/* front tests. */
	namespace
	{
		static_assert(is_same_v<int, typename front<type_list<int>>::type>);
		static_assert(is_same_v<int, typename front<type_list<int, double>>::type>);
	}

	/* back: Returns the type of the back (last) element of a given type_list. */
	template <typename List>
	struct back;

	/* back impl */
	namespace
	{
		template<typename... Args>
		struct back_impl;

		template <typename Arg>
		struct back_impl<Arg>
		{
			using type = Arg;
		};

		template <typename Arg, typename... Args>
		struct back_impl<Arg, Args...>
		{
			using type = typename back_impl<Args...>::type;
		};
	}

	template <typename... Args>
	struct back<type_list<Args...>>
	{
		using type = typename back_impl<Args...>::type;
	};

	namespace 
	{
		static_assert(is_same_v<int, typename back<type_list<int>>::type>);
		static_assert(is_same_v<double, typename back<type_list<int, double>>::type>);
	}

	/* nth_element_impl */
	namespace 
	{
		template<std::size_t I, std::size_t N, typename Arg, typename... Args>
		struct nth_element_impl
		{
			using type = typename nth_element_impl<I + 1, N, Args...>::type;
		};

		template<std::size_t N, typename Arg, typename... Args>
		struct nth_element_impl<N, N, Arg, Args...>
		{
			using type = Arg;
		};
	}

	/* nth_element: Returns the type of the element at nth index of a given type_list. */
	template<std::size_t N, typename List>
	struct nth_element;

	template<std::size_t N, typename... Args>
	struct nth_element<N, type_list<Args...>>
	{
		static_assert(N < length<type_list<Args...>>::value, "N should be smaller than the length of given type_list");

		using type = typename nth_element_impl<0, N, Args...>::type;
	};

	/* nth_element tests. */
	namespace
	{
		static_assert(is_same_v<int, typename nth_element<0, type_list<int>>::type>);
		static_assert(is_same_v<double, typename nth_element<1, type_list<int, double>>::type>);
	}

	/*	push_front: 
		For a given type Arg and a type_list, push Arg at the front of the list
		and return the new list.
	*/
	template <typename Arg, typename List>
	struct push_front;

	template <typename Arg, typename... Args>
	struct push_front<Arg, type_list<Args...>>
	{
		using type = type_list<Arg, Args...>;
	};

	/* push_front tests. */
	namespace
	{
		using test_list = type_list<int, double, char>;
		static_assert(is_same_v<bool, typename front<typename push_front<bool, test_list>::type>::type>);
	}

	/*	push_back: 
		For a given type Arg and a type_list, push Arg at the back of the list and 
		return the new list.
	*/
	template <typename Arg, typename List>
	struct push_back;

	template <typename Arg, typename... Args>
	struct push_back<Arg, type_list<Args...>>
	{
		using type = type_list<Args..., Arg>;
	};

	/* push_back tests. */
	namespace
	{
		using test_list = type_list<int, double, char>;
		static_assert(is_same_v<bool, typename back<typename push_back<bool, test_list>::type>::type>);
	}

	/*	pop_front: 
		Removes the front (first) element from the given type_list and returns the new list.
	*/
	template <typename List>
	struct pop_front;

	template <typename Arg, typename... Args>
	struct pop_front<type_list<Arg, Args...>>
	{
		using type = type_list<Args...>;
	};

	/* pop_front tests */
	namespace
	{
		using test_list = type_list<int, double, char>;
		static_assert(is_same_v<double, typename front<typename pop_front<test_list>::type>::type>);
	}

	/*	pop_back:
		Removes the back(last) element from the given type_list and returns the new list.
	*/
	template <typename List>
	struct pop_back;
	
	template <typename Arg, typename... Args>
	struct pop_back<type_list<Arg, Args...>>
	{
		using type = typename push_front<Arg, typename pop_back<type_list<Args...>>::type>::type;
	};

	template <typename Arg>
	struct pop_back<type_list<Arg>>
	{
		using type = type_list<>;
	};

	/* pop_back tests */
	namespace
	{
		using test_list = type_list<int, double, char>;
		using new_test_list = typename pop_back<test_list>::type;

		static_assert(is_same_v<type_list<int, double>, new_test_list>);
		static_assert(is_same_v<type_list<>, typename pop_back<type_list<int>>::type>);
	}

	/*	index_of:
	*	For a given type Arg and a type_list returns the index of the first found Arg. 
		If it is not found, a negative number is returned.
	*/
	template <typename Arg, typename List>
	struct index_of;

	/* index_of_impl */
	namespace
	{
		template <typename Arg, typename... Args>
		struct index_of_impl;

		template <typename Arg, typename OtherArg, typename... Args>
		struct index_of_impl<Arg, OtherArg, Args...>
		{
			constexpr static int value = 1 + index_of_impl<Arg, Args...>::value;
		};

		template <typename Arg, typename... Args>
		struct index_of_impl<Arg, Arg, Args...>
		{
			constexpr static int value = 0;
		};

		template <typename Arg>
		struct index_of_impl<Arg>
		{
			constexpr static int value = std::numeric_limits<int>::min();
		};

	}

	template <typename Arg, typename... Args>
	struct index_of<Arg, type_list<Args...>>
	{
		constexpr static int value = index_of_impl<Arg, Args...>::value;
	};

	/* index_of tests. */
	namespace
	{
		static_assert(index_of<int, type_list<>>::value < 0);
		static_assert(index_of<int, type_list<int, double>>::value == 0);
		static_assert(index_of<double, type_list<int, double>>::value == 1);
		static_assert(index_of<char, type_list<int, double>>::value < 0);
	}

	/*	contains:
		For a given type Arg and type_list returns true if Arg is found within the type_list. 
		Otherwise returns false.
	*/
	template <typename Arg, typename List>
	struct contains;

	/* contians impl. */
	namespace
	{
		template <typename Arg, typename... Args>
		struct contains_impl;

		template <typename Arg, typename... Args>
		struct contains_impl<Arg, Arg, Args...>
		{
			constexpr static bool value = true;
		};

		template <typename Arg, typename OtherArg, typename... Args>
		struct contains_impl<Arg, OtherArg, Args...>
		{
			constexpr static bool value = contains_impl<Arg, Args...>::value;
		};

		template <typename Arg>
		struct contains_impl<Arg>
		{
			constexpr static bool value = false;
		};
	}

	template <typename Arg, typename... Args>
	struct contains<Arg, type_list<Args...>>
	{
		constexpr static bool value = contains_impl<Arg, Args...>::value;
	};

	/* contains tests. */
	namespace
	{
		static_assert(!contains<int, type_list<>>::value);
		static_assert(contains<int, type_list<int>>::value);
		static_assert(!contains<int, type_list<double>>::value);
		static_assert(contains<int, type_list<double, int>>::value);
		static_assert(!contains<char, type_list<double, int>>::value);
	}
	
	/*	count:
		For a given type Arg and a type_list returns the number of occurrences of Arg within the type_list.
	*/
	template <typename Arg, typename List>
	struct count;

	/* count impl */
	namespace 
	{
		template <typename Arg, typename... Args>
		struct count_impl;

		template <typename Arg, typename... Args>
		struct count_impl<Arg, Arg, Args...>
		{
			constexpr static std::size_t value = 1 + count_impl<Arg, Args...>::value;
		};

		template <typename Arg, typename OtherArg, typename... Args>
		struct count_impl<Arg, OtherArg, Args...>
		{
			constexpr static std::size_t value = count_impl<Arg, Args...>::value;
		};

		template <typename Arg>
		struct count_impl<Arg>
		{
			constexpr static std::size_t value = 0;
		};
	}

	template <typename Arg, typename... Args>
	struct count<Arg, type_list<Args...>>
	{
		constexpr static std::size_t value = count_impl<Arg, Args...>::value;
	};

	/* count tests */
	namespace
	{
		static_assert(0 == count<int, type_list<>>::value);
		static_assert(1 == count<int, type_list<int>>::value);
		static_assert(3 == count<int, type_list<int, int, int>>::value);
		static_assert(0 == count<char, type_list<int, int, int>>::value);
		static_assert(1 == count<char, type_list<int, int, int, char>>::value);
	}

	/*	concat: 
		For given two type_list, concatante them into one type_list and return it.
	*/
	template <typename List1, typename List2>
	struct concat;

	template <typename List1, typename Arg, typename... Args>
	struct concat<List1, type_list<Arg, Args...>>
	{
		using type = typename concat<typename push_back<Arg, List1>::type, type_list<Args...>>::type;
	};

	template <typename List1>
	struct concat<List1, type_list<>>
	{
		using type = List1;
	};
	
	/* concat tests. */
	namespace
	{
		static_assert(is_same_v<type_list<int>, typename concat<type_list<>, type_list<int>>::type>);
		static_assert(is_same_v<type_list<int>, typename concat<type_list<int>, type_list<>>::type>);

		static_assert(is_same_v<type_list<int, double>, typename concat<type_list<int>, type_list<double>>::type>);
	}

	/*	remove:
	*	Remove the first instance of given Arg from the given type_list, if it exists.
	*	No-op if Arg does not exist within type_list.
	*/

	template <typename Arg, typename List>
	struct remove;

	template <typename Arg, typename... Args>
	struct remove<Arg, type_list<Arg, Args...>>
	{
		using type = type_list<Args...>;
	};

	template <typename Arg, typename OtherArg, typename... Args>
	struct remove< Arg, type_list<OtherArg, Args...>>
	{
		using type = typename concat<type_list<OtherArg>, typename remove<Arg, type_list<Args...>>::type>::type;
	};

	template <typename Arg>
	struct remove<Arg, type_list<>>
	{
		using type = type_list<>;
	};

	/* remove tests. */
	namespace
	{
		static_assert(is_same_v<typename remove<int, type_list<>>::type, type_list< >> );
		static_assert(is_same_v<typename remove<int, type_list<int>>::type, type_list< >>);
		static_assert(is_same_v<typename remove<double, type_list<int>>::type, type_list<int>>);

		static_assert(is_same_v<typename remove<double, type_list<double, int, double>>::type, type_list<int,double>>);
	}

	/*	remove_all: 
		Remove all instances of given Arg from the given type_list, if it exists.
		No-op if Arg does not exist within type_list.
	*/
	template <typename Arg, typename List>
	struct remove_all;

	template <typename Arg, typename OtherArg, typename... Args>
	struct remove_all<Arg, type_list<OtherArg, Args...>>
	{
		using type = typename concat<type_list<OtherArg>, typename remove_all<Arg, type_list<Args...>>::type>::type;
	};

	template <typename Arg, typename... Args>
	struct remove_all<Arg, type_list<Arg, Args...>>
	{
		using type = typename remove_all<Arg, type_list<Args...>>::type;
	};

	template <typename Arg>
	struct remove_all<Arg, type_list<>>
	{
		using type = typename type_list<>;
	};

	/* remove_all tests. */
	namespace
	{
		static_assert(is_same_v<typename remove_all<int, type_list<>>::type, type_list< >> );
		static_assert(is_same_v<typename remove_all<int, type_list<int>>::type, type_list< >>);
		static_assert(is_same_v<typename remove_all<int, type_list<int, int>>::type, type_list< >>);
		static_assert(is_same_v<typename remove_all<int, type_list<int, double, int>>::type, type_list<double>>);
		static_assert(is_same_v<typename remove_all<char, type_list<int, double, int>>::type, type_list<int, double, int>>);

	}
}
