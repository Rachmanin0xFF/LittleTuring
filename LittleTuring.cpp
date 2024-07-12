
#include "TuringMachine.h"
#include <iostream>

int main()
{
    // Example Usage:
    using namespace TinyTuring;

    // This is BB(5) -- ([Marxen & Buntrock, 1990])
    // https://bbchallenge.org/1RB1LC_1RC1RB_1RD0LE_1LA1LD_1RZ0LA
    TuringMachine<int, char> beaver{ from_std_format(std::string("1RB1LC_1RC1RB_1RD0LE_1LA1LD_1RZ0LA")) };

    while (beaver.status == TuringStatus::Running) {
        beaver.step();
        //beaver.print_tape(std::cout);
    }

    // most bb.challenge.org machines halt via error (they go to state 'Z', which is never defined)
    auto status = beaver.step();
    std::cout << (status == TuringStatus::Halted ? "Halted" : "Errored");
    std::cout << " after " << beaver.get_steps_completed() << " iterations." << std::endl;
}