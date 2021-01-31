#include "TuringMachine.h"

/// Accept { 0^n1^n | n > 0 }
class AnBn : public trmch::AbstractTuringMachine<>
{
public:
    AnBn() : AbstractTuringMachine(0, 4, -1) {}

protected:
    void oneStep(int currentState, char currentSymbol, NextStep &nextStep) const override
    {
        switch(currentState)
        {
            case 0:
                break;

            case 1:
                break;

            case 2:
                break;

            case 3:

                break;
        }
    }
};

class Test : public trmch::AbstractTuringMachine<>
{
public:
    Test() : AbstractTuringMachine(0, 5, -1) {}

protected:
    void oneStep(int currentState, char currentSymbol, NextStep &nextStep) const override
    {
        nextStep.nextState = currentState + 1;
        nextStep.whereToMove = trmch::RIGHT;
        nextStep.writeSymbol = '#';
    }
};

int main()
{
    using namespace trmch;

    TuringMachine<> m(0, 2, -1,
        {{0, 'a', 1, 'b', RIGHT}}
    );

    m.debug("a");

    return 0;
}
