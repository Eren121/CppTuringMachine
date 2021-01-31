#include <catch2/catch.hpp>

#include "MetaTuringMachine.h"

using namespace trmch;
using namespace std;

TEST_CASE("MetaTuringMachine") {

    SECTION("No transition found should reject") {

        static_assert(!MetaTuringMachine<
            MetaInput<>,
            -1, -2, 0
        >::accept, "[empty transitions][empty input][no transition used]");

        static_assert(!MetaTuringMachine<
            MetaInput<'a', 'b', 'c'>,
            -1, -2, 0
        >::accept, "[empty transitions][non-empty input][no transition used]");

        static_assert(!MetaTuringMachine<
            MetaInput<>,
            -1, -2, 0,
            MetaTransition<100, 'x', 101, 'y', RIGHT>
        >::accept, "[non-empty transitions][empty input][no transition used]");

        static_assert(!MetaTuringMachine<
            MetaInput<'a', 'b', 'c'>,
            -1, -2, 0,
            MetaTransition<100, 'x', 101, 'y', RIGHT>
        >::accept, "[non-empty transitions][non-empty input][no transition used]");

        static_assert(!MetaTuringMachine<
            MetaInput<>,
            -1, -2, 0,
            MetaTransition<0, ' ', 1, 'x', RIGHT>
        >::accept, "[non-empty transitions][empty input (replaced by blank)][some transitions used]");

        static_assert(!MetaTuringMachine<
            MetaInput<'a', 'b', 'c'>,
            -1, -2, 0,
            MetaTransition<0, 'a', 1, 'x', RIGHT>
        >::accept, "[non-empty transitions][non-empty input][some transitions used]");

        static_assert(!MetaTuringMachine<
            MetaInput<'0'>,
            -1, -2, 100,
            MetaTransition<0, '0', 1, 'x', RIGHT>,
            MetaTransition<1, ' ', 2, 'y', RIGHT>,
            MetaTransition<2, ' ', 3, 'z', RIGHT>
        >::accept, "There are some transitions, none are used");

        static_assert(!MetaTuringMachine<
            MetaInput<'0'>,
            -1, -2, 0,
            MetaTransition<0, '0', 1, 'x', RIGHT>,
            MetaTransition<1, ' ', 2, 'y', RIGHT>,
            MetaTransition<2, ' ', 3, 'z', RIGHT>
        >::accept, "Some transitions are used");
    }

    SECTION("Simple machines that should accept the input") {

        static_assert(MetaTuringMachine<
            MetaInput<>,
            -1, -2, 0,
            MetaTransition<0, ' ', -1, 'x', RIGHT>
        >::accept);

        static_assert(MetaTuringMachine<
            MetaInput<'a'>,
            5, -2, 0,
            MetaTransition<0, 'a', 1, 'x', RIGHT>,
            MetaTransition<0, 'x', 100, 'x', RIGHT>, // unused
            MetaTransition<1, ' ', 2, 'x', LEFT>,
            MetaTransition<2, ' ', 100, 'x', LEFT>, // unused
            MetaTransition<2, 'x', 3, 'y', LEFT>, // Do not move
            MetaTransition<3, 'y', 4, 'z', LEFT>, // Do not move
            MetaTransition<4, 'z', 5, 'z', RIGHT>
        >::accept);

        static_assert(MetaTuringMachine<
            MetaInput<'0', '0'>,
            1, -1, 0,
            MetaTransition<0, '0', 1, '#', RIGHT>,
            MetaTransition<0, '1', 2, '_', RIGHT>,
            MetaTransition<2, ' ', 1, '~', RIGHT>
        >::accept);
    }

    SECTION("Internal implementation") {

        SECTION("MetaInput as string litteral") {

            static_assert(std::is_same_v<
                INPUT(),
                MetaInput<>
            >, "Empty inputs should be the same (no argument version)");

            static_assert(std::is_same_v<
                INPUT(""),
                MetaInput<>
            >, "Empty inputs should be the same (empty quotes version)");

            static_assert(std::is_same_v<
               INPUT("abc"),
                MetaInput<'a', 'b', 'c'>
            >, "Inputs should be the same");
        }
        SECTION("SearchTransition") {

            static_assert(SearchTransition<
                0, 'a',
                MetaTransition<0, 'a', 1, 'z', RIGHT>,
                MetaTransition<0, 'a', 1, 'z', RIGHT>,
                MetaTransition<0, 'a', 1, 'z', RIGHT>
            >::nextState == 1, "Valid transition should be found");

            static_assert(SearchTransition<
                2, 'a',
                MetaTransition<0, 'a', 1, 'z', RIGHT>,
                MetaTransition<0, 'a', 1, 'z', RIGHT>,
                MetaTransition<0, 'a', 1, 'z', RIGHT>
            >::exists == false, "Transition not found should not exists");
        }

        SECTION("extract_nth_arg") {

            static_assert(extract_nth_arg<2, 'a', 'b', 'c', 'd'> == 'c', "Third argument should be extracted");
            static_assert(extract_nth_arg<0, 'z', 'x'> == 'z', "First argument should be extracted");
        }

        SECTION("MetaInput utilities methods") {

            SECTION("MetaInput::replace_char_at") {

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>::template replace_char_at<1, 'x'>,
                    MetaInput<'a', 'x', 'c'>
                >, "Character should be replaced");

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>::template replace_char_at<0, 'x'>,
                    MetaInput<'x', 'b', 'c'>
                >, "Character should be replaced");
            }

            SECTION("MetaInput::extends_if_necessary") {

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>::template extends_if_necessary<3, ' '>,
                    MetaInput<'a', 'b', 'c'>
                >, "Input should not be changed");

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>::template extends_if_necessary<1, ' '>,
                    MetaInput<'a', 'b', 'c'>
                >, "Input should not be changed");

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>::template extends_if_necessary<0, ' '>,
                    MetaInput<'a', 'b', 'c'>
                >, "Input should not be changed");

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>::template extends_if_necessary<10, ' '>,
                    MetaInput<'a', 'b', 'c', ' ', ' ', ' ', ' ', ' ', ' ', ' '>
                >, "Input should be resized to fit the capacity");

                static_assert(is_same_v<
                    MetaInput<>::template extends_if_necessary<1, ' '>,
                    MetaInput<' '>
                >, "Empty input should be extended to one blank character");

                static_assert(is_same_v<
                    MetaInput<>::template extends_if_necessary<0, ' '>,
                    MetaInput<>
                >, "Empty input should be empty again (extending to size 0 = not extending at all)");

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>::template extends_if_necessary<4, 'x'>,
                    MetaInput<'a', 'b', 'c', 'x'>
                >, "Input should be extended with desired character");
            }


            SECTION("Chaining MetaInput::replace_char_at | MetaInput::extends_if_necessary") {

                static_assert(is_same_v<
                    MetaInput<'a', 'b', 'c'>
                    ::template extends_if_necessary<5, 'd'>
                    ::template replace_char_at<4, 'e'>,
                    MetaInput<'a', 'b', 'c', 'd', 'e'>
                >, "Chain methods should be the right result");
            }
        }
    }
}