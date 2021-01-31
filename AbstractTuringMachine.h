#pragma once

#include <iostream>
#include <sstream>
#include <optional>
#include <cassert>
#include <algorithm>
#include <exception>
#include "StringStream.h"

namespace trmch {

enum Move {
    LEFT, RIGHT
};

template<
        class State = int,
        class InputSymbol = char,
        class TapeSymbol = char,
        class Input = std::string,
        class Tape = std::string>
class AbstractTuringMachine {
public:
    struct NextStep {
        State nextState;
        TapeSymbol writeSymbol;
        Move whereToMove;
    };

    virtual ~AbstractTuringMachine() = default;

    AbstractTuringMachine(State q0, State qA, State qR)
            : initialState(q0),
              acceptState(qA),
              rejectState(qR) {}

    void checkAccept(std::initializer_list<Input> shouldAccept, std::initializer_list<Input> shouldReject) {
        using std::cerr;
        using std::endl;

        for (auto &in : shouldAccept)
            if (!accept(in)) {
                cerr << "Error" << endl;
            }

        for (auto &in : shouldReject)
            if (!reject(in)) {
                cerr << "Error" << endl;
            }
    }

    void debug(const Input &input) const {
        Tape finalTape;

        bool accepted = accept(input, &finalTape);

        std::cout << "Execution of " << quote(input) << ":" << std::endl;
        std::cout << (accepted ? "Accepted" : "Rejected") << ", final tape: " << quote(finalTape) << std::endl;
    }

    [[nodiscard]] bool reject(const Input &input, Tape *finalTape = nullptr) const {
        return !accept(input, finalTape);
    }

    [[nodiscard]] bool accept(const Input &input, Tape *finalTape = nullptr) const {
        std::optional<bool> ret;
        Tape tape;
        State currentState = initialState;
        std::size_t currentSymbol = 0;

        // First copy input to tape
        tape = {input.begin(), input.end()};

        while (!ret.has_value()) {
            if (currentSymbol >= tape.size()) {
                // Expand tape

                tape.resize(currentSymbol + 1, blankSymbol);
            }

            NextStep nextStep;
            nextStep.whereToMove = Move::RIGHT; /* Arbitrarly advance to right by default */
            nextStep.writeSymbol = tape[currentSymbol]; /* By default don't overwrite anything */
            nextStep.nextState = rejectState; /* If no transition found, reject */

            oneStep(currentState, tape[currentSymbol], nextStep);

            tape[currentSymbol] = nextStep.writeSymbol;

            switch (nextStep.whereToMove) {
                case Move::LEFT:
                    if (currentSymbol != 0) --currentSymbol;
                    break;

                case Move::RIGHT:
                    ++currentSymbol;
                    break;
            }

            currentState = nextStep.nextState;

            if (currentState == acceptState) {
                ret = true;
            } else if (currentState == rejectState) {
                ret = false;
            }
        }

        if (finalTape) {
            *finalTape = tape;
        }

        return ret.value();
    }

protected:
    virtual void oneStep(State currentState, TapeSymbol currentSymbol, NextStep &nextStep) const = 0;

private:
    State acceptState;
    State rejectState;
    State initialState;
    TapeSymbol blankSymbol = ' ';
};

}