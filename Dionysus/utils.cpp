#include "utils.h"
#include <iostream>

Move create_move_from_lan(std::string lan, Board* board) {
	int player = board->is_white_to_move() ? WHITE : BLACK;

	int start_pos = get_square_index_from_notation(lan.substr(0, 2));
	int end_pos = get_square_index_from_notation(lan.substr(2, 2));

	std::vector<int> squares = board->get_squares();

	int start_type = squares[start_pos] % 6;
	int end_type = lan.size() == 4 ? start_type : get_piece_from_char(lan[4]);

	int prev_square = squares[end_pos];
	return { player, start_pos, end_pos, start_type, end_type, prev_square };
}

std::string create_lan_from_move(Move m) {
	std::string lan = get_notation_from_square_index(m.start) + get_notation_from_square_index(m.end);
	if (m.start_type != m.end_type) lan += std::tolower(get_char_from_piece(m.end_type));
	return lan;
}

int get_square_index_from_notation(std::string notation) {
	int c = notation[0] - 'a';
	int r = 7 - (notation[1] - '1');
	return r * 8 + c;
}

std::string get_notation_from_square_index(int square) {
	char c = square % 8 + 'a';
	char r = (7 - (square / 8)) + '1';
	std::string notation;
	notation += c;
	notation += r;
	return notation;
}

int get_piece_from_char(char c) {
	switch (c) {
	case 'p':
		return PAWN;
	case 'n':
		return KNIGHT;
	case 'b':
		return BISHOP;
	case 'r':
		return ROOK;
	case 'q':
		return QUEEN;
	case 'k':
		return KING;
	}
}

char get_char_from_piece(int piece) {
	switch (piece) {
	case PAWN:
		return 'P';
	case KNIGHT:
		return 'N';
	case BISHOP:
		return 'B';
	case ROOK:
		return 'R';
	case QUEEN:
		return 'Q';
	case KING:
		return 'K';
	}
}

unsigned short endian_swap_u16(unsigned short x) {
	x = (x >> 8) |
		(x << 8);
	return x;
}

unsigned int endian_swap_u32(unsigned int x) {
	x = (x >> 24) |
		((x << 8) & 0x00FF0000) |
		((x >> 8) & 0x0000FF00) |
		(x << 24);
	return x;
}

unsigned long long endian_swap_u64(unsigned long long x) {
	x = (x >> 56) |
		((x << 40) & 0x00FF000000000000) |
		((x << 24) & 0x0000FF0000000000) |
		((x << 8) & 0x000000FF00000000) |
		((x >> 8) & 0x00000000FF000000) |
		((x >> 24) & 0x0000000000FF0000) |
		((x >> 40) & 0x000000000000FF00) |
		(x << 56);
	return x;
}