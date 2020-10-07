#include <iostream>
#include <thread>

#include "board.h"
#include "move.h"
#include "utils.h"
#include "searcher.h"

Board board;
Searcher searcher;

void perform_search() {
	Move m = searcher.get_best_move(5000, &board);
	std::cout << "bestmove " << create_lan_from_move(m) << std::endl;
}

void process_UCI() {
	std::string instruction, command;
	std::thread searching_thread;

	bool first_search = true;

	while (std::getline(std::cin, instruction)) {
		command = instruction.substr(0, instruction.find(' '));
		int pos = command.size() + 1;
		if (command == "uci") {
			std::cout << "id name Dionysus \nid author Thomas Patterson\n";
			std::cout << "uciok" << std::endl;
		}
		else if (command == "isready") {
			std::cout << "readyok" << std::endl;
		}
		else if (command == "position") {
			std::string type = instruction.substr(pos, instruction.find(' ', pos) - pos);
			pos += type.size() + 1;
			if (type == "startpos") {
				board = Board();
			}
			else if (type == "fen") {
				std::string fen = "";
				for (int i = 0; i < 6; i++) {
					std::string tmp = instruction.substr(pos, instruction.find(' ', pos) - pos);
					pos += tmp.size() + 1;
					fen += tmp + " ";
				}
				board = Board(fen);
			}
			if (pos >= instruction.size()) continue;
			std::string move = instruction.substr(pos, instruction.find(' ', pos) - pos);
			pos += move.size() + 1;
			if (move == "moves") {
				while (pos < instruction.size()) {
					move = instruction.substr(pos, instruction.find(' ', pos) - pos);
					pos += move.size() + 1;
					Move m = create_move_from_lan(move, &board);
					board.make_move(m);
				}
			}
		}
		else if (command == "go") {
			if (first_search) first_search = false;
			else searching_thread.join();
			searching_thread = std::thread(perform_search);
		}
		else if (command == "stop") {
			searcher.stop();
		}
		else if (command == "quit") {
			break;
		}
		else {
			std::cout << "QUE??" << std::endl;
		}
	}
	searching_thread.detach();
}



int main() {
	/*//Board b("8/6Q1/4p3/7k/3QP1pp/8/5K1B/8 w - - 0 1");
	Board b("4k2r/8/8/8/8/8/8/7Q w k - 0 1");
	Move O_O = { BLACK, 4, 6, KING, KING, EMPTY_SQUARE };
	std::cout << b.make_move(O_O) << std::endl;
	b.print_board();
	/*std::vector<Move> m =  b.get_valid_moves(BLACK);
	std::cout << b.is_threatened(WHITE, 0) << std::endl;
	for (auto it : m) {
		std::cout << it.start % 8 << "," << it.start / 8 << " to " << it.end % 8 << "," << it.end / 8 << std::endl;
	}*/
	
	process_UCI(); 
}