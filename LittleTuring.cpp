
#include "TuringMachine.h"
#include <iostream>

int main()
{
    TinyTuring::TuringMachine<int, char> beaver;
    // This is BB(5) -- ([Marxen & Buntrock, 1990])
    // https://bbchallenge.org/1RB1LC_1RC1RB_1RD0LE_1LA1LD_1RZ0LA
    beaver = TinyTuring::from_std_format(std::string("1RB1LC_1RC1RB_1RD0LE_1LA1LD_1RZ0LA"));

    while (beaver.status == TinyTuring::TuringStatus::Running) {
        beaver.step();
        //beaver.print_tape();
    }

    auto status = beaver.step();
    std::cout << (status == TinyTuring::TuringStatus::Halted ? "Halted" : "Errored");
    std::cout << " after " << beaver.get_steps_completed() << " iterations." << std::endl;
}