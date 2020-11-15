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

	//read each instruction
	while (std::getline(std::cin, instruction)) {

		//command = first word in each instruction
		command = instruction.substr(0, instruction.find(' '));

		//pos keeps track of current position while parsing each instruction
		int pos = command.size() + 1;

		//respond to uci with name and author
		if (command == "uci") {
			std::cout << "id name Dionysus \nid author Thomas Patterson\n";
			std::cout << "uciok" << std::endl;
		}
		//respond to isready with readyok, as per spec
		else if (command == "isready") {
			std::cout << "readyok" << std::endl;
		}
		//position specifies current board position
		else if (command == "position") {
			std::string type = instruction.substr(pos, instruction.find(' ', pos) - pos);
			pos += type.size() + 1;

			//if startpos, create default board
			if (type == "startpos") {
				board = Board();
			}

			//else if fen, read in the fen, and create board with that fen
			else if (type == "fen") {
				std::string fen = "";
				for (int i = 0; i < 6; i++) {
					std::string tmp = instruction.substr(pos, instruction.find(' ', pos) - pos);
					pos += tmp.size() + 1;
					fen += tmp + " ";
				}
				board = Board(fen);
			}

			//if no more moves are specified, move to next instruction
			if (pos >= instruction.size()) continue;

			//otherwise read in and make the specified moves
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

		//when recieve go, start searching on currently loaded position
		else if (command == "go") {

			//need to join thread from previous search before we can start this one
			if (first_search) first_search = false;
			else searching_thread.join();

			searching_thread = std::thread(perform_search);
		}

		//stop indicates we should stop searching
		else if (command == "stop") {
			searcher.stop();
		}

		//quit indicates we should end program
		else if (command == "quit") {
			break;
		}

		//if unrecognised command
		else {
			std::cout << "*Unrecognised command" << std::endl;
		}
	}
	
	//when exiting program
	searching_thread.detach();
}



int main() {	
	process_UCI(); 
}