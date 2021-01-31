#pragma once

#include "TuringMachine.h"
#include "TypeTraits.h"

namespace trmch {


// Public -----------------------


template<char... inputs>
struct MetaInput {
    static constexpr bool empty = (sizeof...(inputs) == 0);
    static constexpr int size = sizeof...(inputs);

    template<unsigned int i>
    static constexpr char char_at = extract_nth_arg<i, inputs...>;

    template<char c>
    using append_char = MetaInput<inputs..., c>;

    template<unsigned int n, char newChar>
    requires (!empty) && (n < size)
    struct ReplaceCharAt {

        template<unsigned int i, char... inConstruction>
        struct Type {
            using type = typename Type<
                i + 1,
                inConstruction...,
                (i == n ? newChar : extract_nth_arg<i, inputs...>)
            >::type;
        };

        template<char... inConstruction>
        struct Type<size, inConstruction...> {
            using type = MetaInput<inConstruction...>;
        };

        using type = typename Type<0>::type;
    };

    template<unsigned int n, char newChar>
    using replace_char_at = typename ReplaceCharAt<n, newChar>::type;

    template<unsigned int minCapacity, char fillChar>
    requires (minCapacity >= size)
    struct ExtendsIfNecessary {

        template<unsigned int i, char... inConstruction>
        struct Type {

            template<class AvoidCompileError, bool extending>
            struct Extending {
                using type = typename Type<i + 1, inConstruction..., extract_nth_arg<i, inputs...>>::type;
            };

            template<class AvoidCompileError>
            struct Extending<AvoidCompileError, true> {
                using type = typename Type<i + 1, inConstruction..., fillChar>::type;
            };


            using type = typename Extending<void, i >= size>::type;
        };

        template<char... inConstruction>
        struct Type<minCapacity, inConstruction...> {
            using type = MetaInput<inConstruction...>;
        };

        using type = typename Type<0>::type;
    };


    template<unsigned int minCapacity, char fillChar>
    using extends_if_necessary = typename ExtendsIfNecessary<minCapacity < size ? size : minCapacity, fillChar>::type;
};


template <typename T, T... chars>
constexpr MetaInput<chars...> operator""_input() { return { }; }


/// Variadic arguments to allow no arguments at all (empty input)
#define INPUT2() decltype(""_input)
#define INPUT2_NON_EMPTY(str) decltype(str##_input)
#define INPUT(...)  INPUT2##__VA_OPT__(_NON_EMPTY)(__VA_ARGS__)


template<int stateFrom_, char read_, int stateTo_, char written_, Move move_>
struct MetaTransition {
    static constexpr int stateFrom = stateFrom_;
    static constexpr char read = read_;
    static constexpr int stateTo = stateTo_;
    static constexpr char written = written_;
    static constexpr Move move = move_;
};


// Private ----------------------


/// Strategy to search the transition: iterate all of the transitions with templates


template<int nextState_, char writtenChar_, Move move_, bool exists_>
struct SearchTransitionResult {
    static constexpr int nextState = nextState_;
    static constexpr char writtenChar = writtenChar_;
    static constexpr Move move = move_;
    static constexpr bool exists = exists_;
};


template<int currentState, char currentChar, class... Transitions>
requires are_instances<MetaTransition, Transitions...>
struct SearchTransition;


template<int currentState, char currentChar, class FirstTransition, class... RestTransitions>
requires are_instances<MetaTransition, FirstTransition, RestTransitions...>
struct SearchTransition<currentState, currentChar, FirstTransition, RestTransitions...>
    : std::conditional_t<
        (currentState == FirstTransition::stateFrom && currentChar == FirstTransition::read),
        SearchTransitionResult<
            FirstTransition::stateTo,
            FirstTransition::written,
            FirstTransition::move,
            true
        >,
        SearchTransition<currentState, currentChar, RestTransitions...>
      > {};


template<int currentState, char currentChar>
struct SearchTransition<currentState, currentChar>
    : SearchTransitionResult<0, ' ', RIGHT, false> {};


/// Strategy to update the automata: store all configuration for each step


template<
    int stepIndex_,
    int currentState_,
    int currentPointer_,
    class currentInput_,
    class... Transitions
>
requires is_instance<MetaInput, currentInput_>
         && are_instances<MetaTransition, Transitions...>
struct Step {
    using currentInput = currentInput_;
    static constexpr int stepIndex = stepIndex_;
    static constexpr int currentState = currentState_;
    static constexpr int currentPointer = currentPointer_;
    static constexpr char currentSymbol = currentInput::template char_at<currentPointer>;

    using TransitionResult = SearchTransition<currentState, currentSymbol, Transitions...>;

    static constexpr bool nextExists = TransitionResult::exists;
    static constexpr int nextState = TransitionResult::nextState;
    static constexpr char writtenChar = TransitionResult::writtenChar;
    static constexpr Move move = TransitionResult::move;

    static constexpr int nextPointer() {
        switch(move) {
            case RIGHT:
                return currentPointer + 1;
            case LEFT:
                if(currentPointer > 0) return currentPointer - 1;
                else return 0;
        }
    }

    static constexpr int nextInputSize() {
        int nextPointerMinSize = nextPointer() + 1;
        /* last position == 0 is reflected by size == 1, so +1 */

        if(nextPointerMinSize < currentInput::size) {
            return currentInput::size;
        }
        else {
            return nextPointerMinSize;
        }
    }

    using nextInput = typename currentInput
        ::template replace_char_at<currentPointer, writtenChar>
        ::template extends_if_necessary<nextInputSize(), ' '>;

    template<bool exists>
    struct NextStep {
        using type = Step<stepIndex + 1, nextState, nextPointer(), nextInput, Transitions...>;
    };

    using next_step = typename NextStep<nextExists>::type;
};


template<typename T>
constexpr bool is_step = false;

template<auto... Ts>
constexpr bool is_step<Step<Ts...>> = true;


/// These structures are to self-explain templated arguments instead of magic integer constants
struct StateBase {
    /// not explicit to allow directly integer parameter (conversion in template!)
    constexpr StateBase(int acceptState) : value(acceptState) {}
    constexpr operator int() const { return value; }
    int value;
};


// Public -----------------------


struct AcceptState : StateBase { using StateBase::StateBase; };
struct RejectState : StateBase { using StateBase::StateBase; };
struct InitialState : StateBase { using StateBase::StateBase; };


/*
template<char> void f() {}
template<int> void f() {}
template<class> void f() {}
template<class, class> void f() {}
template<class..., class...> void f() {}
*/

template<
    class Input,
    AcceptState acceptState,
    RejectState rejectState,
    InitialState initialState,
    class... Transitions>
requires is_instance<MetaInput, Input>
      && are_instances<MetaTransition, Transitions...>
      && (acceptState.value != rejectState.value)
struct MetaTuringMachine
{
    template<class T>
    //requires is_instance<Step, T>
    struct NextStep {

        template<bool exists, int nextState>
        struct Value {
            static constexpr bool result = NextStep<typename T::next_step>::result;
        };

        template<bool exists_alwaysTrue>
        struct Value<exists_alwaysTrue, rejectState> {
            static constexpr bool result = false;
        };

        template<bool exists_alwaysTrue>
        struct Value<exists_alwaysTrue, acceptState> {
            static constexpr bool result = true;
        };

        template<int nextState>
        struct Value<false, nextState> {
            static constexpr bool result = false;
        };

        static constexpr bool result = Value<T::nextExists, T::nextState>::result;
    };

    using first_input = std::conditional_t<
        Input::empty,
        MetaInput<' '>,
        Input
    >;
    using first_step = Step<1, initialState, 0, first_input, Transitions...>;

    static constexpr bool accept = NextStep<first_step>::result;
};

}