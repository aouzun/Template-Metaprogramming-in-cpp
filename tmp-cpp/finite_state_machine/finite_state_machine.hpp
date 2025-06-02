#pragma once

#include <cstddef>
#include "../static_type_list/static_type_list.hpp"


namespace FSM_
{
	/* Transition function:
		Representing a single transition, moving FromState to ToState with the given Input.
	*/
	template <typename FromState, typename ToState, typename Input>
	struct TransitionFunction
	{
		using from_state = FromState;
		using to_state = ToState;
		using input = Input;
	};


	/*
	* Finite State Machine:
	* One Single Initial State
	* N Transition Functions. FromState - Input - TargetState
	* M Final States
	*/
	template <typename InitialState, typename TransitionFunctionList, typename FinalStatesList>
	struct FiniteStateMachine
	{
		using initial_state = InitialState;
		using transition_function_list = TransitionFunctionList;
		using final_states_list = FinalStatesList;
	};

	/*	For a given State, Input and Transition function, return true if function matches the input.
		Otherwise return false.
	*/

	template <typename State, typename Input, typename TransitionFunction>
	struct CheckTransition
	{
		constexpr static bool value =
			is_same_v<typename TransitionFunction::from_state, State>&&
			is_same_v<typename TransitionFunction::input, Input>;
	};

	/* For a given State and Input and the transition list, find the next state */
	template <typename FromState, typename Input, typename TransitionFunctionList>
	struct FindNextState
	{
		template <typename TransitionFunction>
		using CheckTransitionFunction = CheckTransition<FromState, Input, TransitionFunction>;

		using matched_transition = typename StaticTypes::search<CheckTransitionFunction, TransitionFunctionList>::type;
		using type = typename matched_transition::to_state;
	};

	template <typename FSM, typename CurrentState, typename InputList>
	struct ParseInput;

	template <typename FSM, typename CurrentState, typename Arg, typename... Args>
	struct ParseInput<FSM, CurrentState, StaticTypes::type_list<Arg, Args...>>
	{
		using next_state = typename FindNextState<CurrentState, Arg, typename FSM::transition_function_list>::type;

		/* Reached final state. */
		using type = typename ParseInput<FSM, next_state, StaticTypes::type_list<Args...>>::type;

		/* true if reached state is a final state, otherwise false. */
		constexpr static bool value = ParseInput<FSM, next_state, StaticTypes::type_list<Args...>>::value;
	};

	/* If void is reached, than the parser failed. */
	template <typename FSM, typename List>
	struct ParseInput<FSM, void, List>
	{
		constexpr static bool value = false;
		using type = void;
	};

	template <typename FSM, typename CurrentState>
	struct ParseInput<FSM, CurrentState, StaticTypes::type_list<>>
	{
		constexpr static bool value = StaticTypes::contains<CurrentState, FSM::final_states_list>::value;
		using type = CurrentState;
	};


	namespace FSMUsage
	{
		#include "../static_type_list/static_type_list.hpp"

		/*
			Check whether a string has even number of zeroes.
			Alphabet: 0-1
			2 States N1 and N2
			Final	State	= {N1}
			Initial State	= {N1}
			Transition Functions = {{N1, '0', N2}, {N1, '1', N1}, {N2, '0', N1}, {N2, '1', N2}};

		*/

		namespace StringWithEvenNumberOfZeroes
		{
			struct State1{};
			struct State2{};

			struct Input0;
			struct Input1;

			using FSM1 = FiniteStateMachine<
				State1,
				StaticTypes::type_list<
				TransitionFunction<State1, State2, Input0>,
				TransitionFunction<State1, State1, Input1>,
				TransitionFunction<State2, State1, Input0>,
				TransitionFunction<State2, State2, Input1>>,
				StaticTypes::type_list<State1>>;

			/* Empty input */
			using emptyInputResult = ParseInput<FSM1, FSM1::initial_state, StaticTypes::type_list<>>;

			static_assert(is_same_v<State1, emptyInputResult::type>);
			static_assert(emptyInputResult::value);

			/* Input: 00 */

			using twoZeroesResult = ParseInput<FSM1, FSM1::initial_state, StaticTypes::type_list<Input0, Input0>>;

			static_assert(is_same_v<State1, twoZeroesResult::type>);
			static_assert(twoZeroesResult::value);

			/* Input: 0 */

			using singleZeroResult = ParseInput<FSM1, FSM1::initial_state, StaticTypes::type_list<Input0>>;

			static_assert(is_same_v<State2, singleZeroResult::type>);
			static_assert(false == singleZeroResult::value);

			/* Input: 1 */

			using singleOneResult = ParseInput<FSM1, FSM1::initial_state, StaticTypes::type_list<Input1>>;

			static_assert(is_same_v<State1, singleOneResult::type>);
			static_assert(singleOneResult::value);

			/* Input: 10 */

			using oneZeroResult = ParseInput<FSM1, FSM1::initial_state, StaticTypes::type_list<Input0, Input1>>;

			static_assert(is_same_v<State2, oneZeroResult::type>);
			static_assert(false == oneZeroResult::value);

			/* Random 50 length input, 24 zeroes */

			using randomResult = ParseInput<FSM1, FSM1::initial_state, StaticTypes::type_list<Input0, Input0, Input1, Input0, Input0, Input1, Input0, Input1, Input1, Input1, Input1, Input0, Input1, Input1, Input0, Input0, Input0, Input1, Input1, Input0, Input1, Input0, Input1, Input1, Input0, Input1, Input0, Input0, Input1, Input0, Input1, Input0, Input0, Input0, Input1, Input0, Input0, Input1, Input0, Input1, Input1, Input1, Input1, Input0, Input0, Input1, Input1, Input1, Input0, Input1>>;

			static_assert(is_same_v<State1, randomResult::type>);
			static_assert(randomResult::value);
		}
	}
}