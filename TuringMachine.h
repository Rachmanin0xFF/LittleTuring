//@author Adam Lastowka

#pragma once
#include <vector>
#include <map>
#include <memory>
#include <iostream>
#include <array>

namespace TinyTuring {

	enum TuringStatus {
		Halted,
		Running,
		Error
	};

	// Rules are stored in a map of RuleKeys (as the keys, of course) and RuleInstructions (values).
	// The "keys" are pairs made from (the currently read symbol, and the machine state).
	// To find out what the machine should be doing, we make a new RuleKey with the current symbol and state,
	// then we look at the associated instruction in the map.
	struct RuleKey {
		RuleKey() {};
		RuleKey(int symb, int state) :
			symbol{ symb },
			state{ state }
		{};
		int symbol{ -1 };
		int state{ -1 };
	};
	struct RuleInstruction {
		RuleInstruction() {};
		RuleInstruction(int print_symbol, char tape_motion, int final_state) :
			print_symbol{ print_symbol },
			tape_motion{ tape_motion },
			final_state{ final_state } {}
		int print_symbol{ -1 };
		int final_state{ -1 };
		char tape_motion{ -1 };
	};
	// We need this for std::map to work
	inline bool operator<(const RuleKey& a, const RuleKey& b) {
		if (a.state != b.state) {
			return a.state < b.state;
		}
		else {
			return a.symbol < b.symbol;
		}
		// This is probably more concise, but I like seeing the above explicitly.
		// return std::tie(a.symbol, a.state) < std::tie(b.symbol, b.state)
	};

	// T is the tape symbol data type
	// U is the state machine's state data type
	// these types are only for convenience -- internally, the machine uses numbers (and a map for the rules)
	template <typename T = int, typename U = int> class TuringMachine {
	public:
		TuringMachine() {};

		TuringMachine(const T& default_symbol, const U& default_state) {
			tape_right.push_back(0);
			add_symbol(default_symbol);
			add_state(default_state);
			this->default_state = 0;
			this->default_symbol = 0;
		};

		// we don't want any copies, so we keep these operations 'safe'
		// maybe I should've used a set. TODO
		void add_symbol(const T& symb) {
			if (symbol_map.count(symb) == 0) {
				symbol_map.insert({ symb, symbol_index.size() });
				symbol_index.push_back(symb);
			}
		};
		void add_state(const U& state) {
			if (state_map.count(state) == 0) {
				state_map.insert({ state, state_index.size() });
				state_index.push_back(state);
			}
		};

		// you can use this, or just tell the machine to go to some silly state
		// and let it error (lol)
		void set_halt_state(const U& state) {
			add_state(state);
			halt_state = state_map[state];
		}

		void add_rule(const U& state_in, const T& symbol_in, const char& move_to, const U& state_out, const T& symbol_out) {
			add_state(state_in);
			add_state(state_out);
			add_symbol(symbol_in);
			add_symbol(symbol_out);
			RuleKey rk(symbol_map[symbol_in], state_map[state_in]);
			RuleInstruction ri(symbol_map[symbol_out], move_to, state_map[state_out]);
			rule_table.insert({ rk, ri });
		};
		// if you don't want to write anything
		void add_rule(const U& state_in, const T& symbol_in, const char& move_to, const U& state_out) {
			add_rule(state_in, symbol_in, move_to, state_out, symbol_in);
		};

		// The tape extends both ways. I wanted to keep it contiguous, so we're using a two-sided std::vector (i.e. two vectors)
		inline long tapeR_index() { return position; };
		inline long tapeL_index() { return -(position + 1); }; // no redundancy -- position 0 belongs to the right side

		// A single "step"..."iteration"...? What is this usually called?
		TuringStatus step() {
			if (state == halt_state) {
				status = TuringStatus::Halted;
				return status;
			}
			if (status == TuringStatus::Error) {
				return status;
			}

			// figure out where we are
			auto active_tape_half = (position >= 0) ? &tape_right : &tape_left;
			int tape_memory_index = (position >= 0) ? tapeR_index() : tapeL_index();
			if (tape_memory_index == active_tape_half->size()) {
				active_tape_half->push_back(default_symbol); // expand the tape if necessary
			}

			// get state machine status & current symbol
			RuleKey temp_key = { (*active_tape_half)[tape_memory_index], state };

			// try to get the associated instruction
			RuleInstruction* temp_instruction = &(rule_table[temp_key]);
			if (rule_table[temp_key].final_state == -1) {
				status = TuringStatus::Error;
				return status;
			}

			// write the new symbol
			(*active_tape_half)[tape_memory_index] = temp_instruction->print_symbol;

			position += temp_instruction->tape_motion;
			state = temp_instruction->final_state;

			steps_completed++;

			return status;
		};

		void reset_tape() {
			tape_right.clear();
			tape_left.clear();
			position = 0;
			state = default_state;
			steps_completed = 0;
			status = TuringStatus::Running;
		};

		// returns the tape, patched together from the left and right halves
		std::vector<T> get_tape() {
			std::vector<T> output;
			for (int i = tape_left.size() - 1; i >= 0; i--) {
				output.push_back(symbol_index[tape_left[i]]);
			}
			for (auto x : tape_right) {
				output.push_back(symbol_index[x]);
			}
			return output;
		};

		// shows the tape TODO: allow specification of output stream
		void print_tape(std::ostream &output_stream = std::cout) {
			std::vector<T> tape_array = get_tape();
			for (auto s : tape_array) {
				output_stream << s;
			}
			output_stream << std::endl;
		};

		// shows state & head position
		void print_status(std::ostream& output_stream = std::cout) {
			output_stream << "state=" << state_index[state] << " | position=" << position << std::endl;
		}

		const int& const get_steps_completed() {
			return steps_completed;
		};

		TuringStatus status{ TuringStatus::Running }; // ok, I guess the user can forcefully halt the machine
	private:
		std::vector<int> tape_right;
		std::vector<int> tape_left;
		int default_symbol{ -1 }; // important! the tape gets filled with this!!!
		int default_state{ -1 };
		int halt_state{ -1 };
		RuleKey temp_key;
		long position{ 0 };
		int state{ 0 };
		long steps_completed{ 0 };

		std::map<RuleKey, RuleInstruction> rule_table;

		// the vectors are for quickly translating symbol / state indices into their specified types
		// the maps are for going the other way
		// these are only used when making the machine / exporting data -- everything internal is just numbers
		std::vector<T> symbol_index;
		std::map<T, int> symbol_map;
		std::vector<U> state_index;
		std::map<T, int> state_map;
	};

	// parse the format used on https://bbchallenge.org/
	// it's pretty nice! (the format, not this code)
	TuringMachine<int, char> from_std_format(const std::string& rules) {
		TuringMachine<int, char> machine(0, 'A');
		char state_in = 'A';
		int symbol_in = 0;
		for (std::string::size_type i = 0; i < rules.size();) {
			int symbol = rules[i] - '0';
			if (symbol + '0' != '-') {
				char direction = rules[i + 1]; int movement = 0;
				switch (direction) {
				case 'R':
					movement = 1;
					break;
				case 'L':
					movement = -1;
					break;
				}
				char state = rules[i + 2];

				machine.add_rule(state_in, symbol_in, movement, state, symbol);
			}
			else machine.add_rule(state_in, symbol_in, 0, -1);

			i += 3;
			if (rules[i] == '_') {
				i++;
				state_in++;
				symbol_in = 0;
			}
			else symbol_in++;
		}

		return machine;
	}
}