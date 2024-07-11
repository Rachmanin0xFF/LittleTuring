
#include "TuringMachine.h"
#include <iostream>

int main()
{
    TuringMachine<int, char> beaver;
    // This is BB(5) -- ([Marxen & Buntrock, 1990])
    // https://bbchallenge.org/1RB1LC_1RC1RB_1RD0LE_1LA1LD_1RZ0LA
    beaver = from_std_format(std::string("1RB1LC_1RC1RB_1RD0LE_1LA1LD_1RZ0LA"));

    long count = -2;
    while (beaver.status == TuringStatus::Running) {
        beaver.step();
        //beaver.print_tape();
        count++;
    }
    std::cout << "Halted after " << count - 2 << " iterations." << std::endl;
}