#pragma once

#include "AbstractTuringMachine.h"

namespace trmch {

template<
        class State = int,
        class InputSymbol = char,
        class TapeSymbol = char,
        class Input = std::string,
        class Tape = std::string>
class TuringMachine : public AbstractTuringMachine<State, InputSymbol, TapeSymbol, Input, Tape>
{
    using NextStep = AbstractTuringMachine<>::NextStep;

public:
    struct Transition {
        State stateFrom;
        TapeSymbol symbolOriginal;
        NextStep nextStep;
    };

    TuringMachine(State q0, State qA, State qR, const std::vector<Transition>& deltaFunction)
         : AbstractTuringMachine<>(q0, qA, qR),
           m_deltaFunction(deltaFunction) {}

protected:
    void oneStep(State currentState, TapeSymbol currentSymbol, NextStep& nextStep) const override {

        for(const Transition& transition : m_deltaFunction)
        {
            if(transition.stateFrom == currentState && transition.symbolOriginal == currentSymbol)
            {
                nextStep = transition.nextStep;
            }
        }
    }

private:
    std::vector<Transition> m_deltaFunction;
};

}