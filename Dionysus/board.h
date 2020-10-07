#pragma once

#include <vector>
#include <string>

#include "defs.h"
#include "move.h"
#include "search_result.h"

#include "transposition_table.h"

class Board {

private:
	std::vector<int> squares;

	bool white_to_move;
	bool searching;

	//these are stacks so we can easily restore previous board states after trialling board states during minimax
	std::vector<std::vector<std::vector<bool>>> can_castle; // e.g. can_castle[last_element][player][dir] = can player castle in direction dir. right(1) is kingside
	std::vector<int> half_move_clock;
	std::vector<int> en_passant_target;
	std::vector<std::vector<int>> king_positions;
	std::vector<std::vector<std::vector<int>>> piece_counts;
	std::vector<unsigned long long> zobrist_hash;

	void init_from_fen(std::string);

	void generate_zobrist_keys();
	
	std::vector<Move> get_pawn_moves(int, int, int);
	std::vector<Move> get_knight_moves(int, int, int);
	std::vector<Move> get_bishop_moves(int, int, int);
	std::vector<Move> get_rook_moves(int, int, int);
	std::vector<Move> get_queen_moves(int, int, int);
	std::vector<Move> get_king_moves(int, int, int);

	std::vector<Move> get_pawn_captures(int, int, int);
	std::vector<Move> get_knight_captures(int, int, int);
	std::vector<Move> get_bishop_captures(int, int, int);
	std::vector<Move> get_rook_captures(int, int, int);
	std::vector<Move> get_queen_captures(int, int, int);
	std::vector<Move> get_king_captures(int, int, int);

public:
	Board();
	Board(std::string);
	~Board();

	bool make_move(Move);
	void undo_move(Move);

	bool in_check(int);
	bool is_threatened(int, int);
	bool is_threatened(int, int, std::vector<Move>);

	std::vector<int> get_squares();
	bool is_white_to_move();
	std::vector<std::vector<bool>> get_castling_rights();
	int get_half_move_clock();
	int get_en_passant_target();
	unsigned long long get_zobrist_hash();

	bool is_three_move_rep();

	double evaluate_position();

	std::vector<Move> get_valid_moves(int);
	std::vector<Move> get_valid_captures(int);
	void print_board();

};