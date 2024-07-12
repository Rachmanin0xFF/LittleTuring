# LittleTuring

A minimal header-only library for turing machines (contained in TuringMachine.h). Written as a little exercise for myself. It runs at 15ns per step on my mid-tier PC (BB(5) in <1s).

## Example Usage

```C++
// The class needs a <symbol type (defaults to int), and a state type (defaults to char)>
// for its tape symbols and state machine labels, respectively
TuringMachine<int, char> myMachine = from_std_format(std::string("1RB1LB_1LA0LC_1RH1LD_1RD0RA")); // 4-state 2-symbol busy beaver

while(myMachine.status == TuringStatus::Running) {
    myMachine.step();
    myMachine.print_tape(std::cout);
}

std::cout << "Machine stopped after " << myMachine.get_steps_completed() << " steps " << std::endl;
```
