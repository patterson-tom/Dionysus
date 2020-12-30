#include "board.h"
#include "zobrist_keys.h"
#include "utils.h"

#include <iostream>
#include <iomanip>
#include <map>

Board::Board() {
	generate_zobrist_keys();
	//starting position
	init_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

Board::Board(std::string fen) {
	generate_zobrist_keys();
	init_from_fen(fen);
}

Board::~Board() { }

void Board::init_from_fen(std::string fen) {
	squares.clear();
	can_castle.clear();
	half_move_clock.clear();
	en_passant_target.clear();

	king_positions.clear();
	king_positions.push_back({ -1, -1 });

	piece_counts.clear();
	piece_counts.push_back({ {0,0,0,0,0,0}, {0,0,0,0,0,0} });

	zobrist_hash.clear();
	zobrist_hash.push_back(0);

	//PLACEMENT OF PIECES
	int pos = 0;
	std::string piece_placement = fen.substr(pos, fen.find(' ', pos) - pos);
	
	std::map<char, int> piece_letters = { {'p', PAWN}, {'n', KNIGHT}, {'b', BISHOP}, {'r', ROOK}, {'q', QUEEN}, {'k', KING} };

	int index = 0;
	for (char c : piece_placement) {

		//if series of empty spaces
		if (c > '0' && c < '9') {
			int empty_squares = c - '0';
			for (int i = 0; i < empty_squares; i++) {
				squares.push_back(EMPTY_SQUARE);
				index++;
			}
		} //if an actual piece (but ignore all /s)
		else if (c != '/') {
			squares.push_back((c > 96 ? BLACK : WHITE) * 6 + piece_letters[std::tolower(c)]);
			piece_counts[0][c > 96 ? BLACK : WHITE][piece_letters[std::tolower(c)]]++;
			zobrist_hash[0] ^= zobrist_keys::piece_locations[index][c > 96 ? BLACK : WHITE][piece_letters[std::tolower(c)]];
			if (c == 'k') {
				king_positions[0][BLACK] = index;
			}
			else if (c == 'K') {
				king_positions[0][WHITE] = index;
			}
			index++;
		}
	}

	//ACTIVE COLOUR
	pos = piece_placement.size() + 1;
	white_to_move = fen[pos] == 'w';
	if (white_to_move) zobrist_hash[0] ^= zobrist_keys::white_to_move;

	//CASTLING AVAILABILITY
	pos += 2;
	std::string castling_availability = fen.substr(pos, fen.find(' ', pos) - pos);

	can_castle.push_back({ {false, false}, {false, false} });
	for (char c : castling_availability) {
		switch (c) {
		case 'K':
			can_castle[0][WHITE][RIGHT] = true;
			zobrist_hash[0] ^= zobrist_keys::can_castle[WHITE][RIGHT];
			break;
		case 'k':
			can_castle[0][BLACK][RIGHT] = true;
			zobrist_hash[0] ^= zobrist_keys::can_castle[BLACK][RIGHT];
			break;
		case 'Q':
			can_castle[0][WHITE][LEFT] = true;
			zobrist_hash[0] ^= zobrist_keys::can_castle[WHITE][LEFT];
			break;
		case 'q':
			can_castle[0][BLACK][LEFT] = true;
			zobrist_hash[0] ^= zobrist_keys::can_castle[BLACK][LEFT];
			break;
		}
	}

	//EN PASSANT TARGET
	pos += castling_availability.size() + 1;
	std::string ep_target = fen.substr(pos, fen.find(' ', pos) - pos);

	if (ep_target[0] == '-') {
		en_passant_target = { EMPTY_SQUARE };
	}
	else {
		int target = get_square_index_from_notation(ep_target);
		int pawn_square = target + white_to_move ? -8 : 8;
		int opp = white_to_move ? WHITE : BLACK;
		//if there is actually an enemy pawn threatening us
		if ((pawn_square % 8 < 7 && squares[pawn_square + 1] == (opp * 6 + PAWN)) || (pawn_square % 8 > 0 && squares[pawn_square - 1] == (opp * 6 + PAWN))) {
			en_passant_target = { target };
			zobrist_hash[0] ^= zobrist_keys::en_passant_target[ep_target[0] - 'a'];
		}
		else {
			en_passant_target = { EMPTY_SQUARE };
		}
		
	}

	//HALF MOVE CLOCK
	pos += ep_target.size() + 1;
	std::string hm_clock = fen.substr(pos, fen.find(' ', pos) - pos);

	half_move_clock = { std::stoi(hm_clock) };

	//FULLMOVE NUMBER
	//Not currently being used
}

//set the zobrist key for each possible board feature
//to create zobrist hash, all applicable keys are XORed together
void Board::generate_zobrist_keys() {

	//side to move key
	zobrist_keys::white_to_move = zobrist_keys::keys[780];

	//castling rights keys
	zobrist_keys::can_castle = { {0,0}, {0,0} };
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			zobrist_keys::can_castle[i][j] = zobrist_keys::keys[768 + i * 2 + (1 - j)];
		}
	}

	//en_passant_target keys
	for (int i = 0; i < 8; i++) {
		zobrist_keys::en_passant_target.push_back(zobrist_keys::keys[772+i]);
	}

	//piece locations keys
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			zobrist_keys::piece_locations.push_back({ {0,0,0,0,0,0}, {0,0,0,0,0,0} });
			for (int j = 0; j < 2; j++) {
				for (int k = 0; k < 6; k++) {
					zobrist_keys::piece_locations[r * 8 + c][j][k] = zobrist_keys::keys[(7 - r) * 8 + c + 64 * (k*2+(1-j))];
				}
			}
		}
	}
}

//assumes that the move is pseudo-legal
bool Board::make_move(Move m) {
	can_castle.push_back(can_castle.back());
	king_positions.push_back(king_positions.back());
	piece_counts.push_back(piece_counts.back());
	zobrist_hash.push_back(zobrist_hash.back());
	
	//if rook killed, need to disallow castling on that side
	if (m.prev_square % 6 == ROOK) {
		//if rook killed on left
		if (m.end % 8 == 0) {
			can_castle.back()[m.player == WHITE ? BLACK : WHITE][LEFT] = false;
			zobrist_hash.back() ^= zobrist_keys::can_castle[m.player == WHITE ? BLACK : WHITE][LEFT];
		} //if rook killed in right
		else if (m.end % 8 == 7) {
			can_castle.back()[m.player == WHITE ? BLACK : WHITE][RIGHT] = false;
			zobrist_hash.back() ^= zobrist_keys::can_castle[m.player == WHITE ? BLACK : WHITE][RIGHT];
		}
	}

	//if move rook, may need to disallow castling on a side
	if (m.start_type == ROOK) {
		//if rook started on left
		if (m.start % 8 == 0) {
			can_castle.back()[m.player][LEFT] = false;
			zobrist_hash.back() ^= zobrist_keys::can_castle[m.player][LEFT];
		} //if rook started in right
		else if (m.start % 8 == 7) {
			can_castle.back()[m.player][RIGHT] = false;
			zobrist_hash.back() ^= zobrist_keys::can_castle[m.player][RIGHT];
		}
	}//if king moved, update king pos and remove castle rights
	else if (m.start_type == KING) {
		can_castle.back()[m.player][LEFT] = false;
		can_castle.back()[m.player][RIGHT] = false;

		zobrist_hash.back() ^= zobrist_keys::can_castle[m.player][LEFT];
		zobrist_hash.back() ^= zobrist_keys::can_castle[m.player][RIGHT];

		//update king_positions
		king_positions.back()[m.player] = m.end;
	}

	//increment the half_move_clock
	half_move_clock.push_back(half_move_clock.back());
	half_move_clock.back()++;
	//if pawn move or capture made, reset half_move_clock
	if (m.start_type == PAWN || m.prev_square != EMPTY_SQUARE) {
		half_move_clock.back() = 0;
	}

	//update en_passant_target
	if (en_passant_target.back() != EMPTY_SQUARE) {
		zobrist_hash.back() ^= zobrist_keys::en_passant_target[en_passant_target.back() % 8];
	}
	en_passant_target.push_back(EMPTY_SQUARE);
	if (m.start_type == PAWN && abs(m.start - m.end) == 16) {
		int opp = m.player == WHITE ? BLACK : WHITE;
		//if there is actually an enemy pawn threatening us
		if ((m.end % 8 < 7 && squares[m.end + 1] == (opp * 6 + PAWN)) || (m.end % 8 > 0 && squares[m.end - 1] == (opp * 6 + PAWN))) {
			en_passant_target.back() = (m.start + m.end) / 2;
			zobrist_hash.back() ^= zobrist_keys::en_passant_target[m.start % 8];
		}
	}

	//if en passant
	if (m.start_type == PAWN && m.prev_square == EMPTY_SQUARE && abs(m.start - m.end) % 8 != 0) {
		if (m.player == WHITE) {
			zobrist_hash.back() ^= zobrist_keys::piece_locations[m.end + 8][BLACK][PAWN];
			squares[m.end + 8] = EMPTY_SQUARE;
		}
		else {
			zobrist_hash.back() ^= zobrist_keys::piece_locations[m.end - 8][WHITE][PAWN];
			squares[m.end - 8] = EMPTY_SQUARE;
		}
		piece_counts.back()[m.player == WHITE ? BLACK : WHITE][PAWN]--;
	}

	//if castle
	bool legal_castle = true;
	if (m.start_type == KING && abs(m.start - m.end) == 2) {

		if (is_threatened(m.player, m.start)) legal_castle = false;
		bool right = m.end > m.start;
		int rook_pos = (m.start + m.end) / 2;

		zobrist_hash.back() ^= zobrist_keys::piece_locations[rook_pos][m.player][ROOK];
		squares[rook_pos] = m.player * 6 + ROOK;

		if (is_threatened(m.player, rook_pos)) legal_castle = false;

		zobrist_hash.back() ^= zobrist_keys::piece_locations[right ? (m.start + 3) : (m.start - 4)][m.player][ROOK];
		squares[right ? (m.start + 3) : (m.start - 4)] = EMPTY_SQUARE;
	}

	//update piece counts if there is a capture
	if (squares[m.end] != EMPTY_SQUARE) {
		piece_counts.back()[m.player == WHITE ? BLACK : WHITE][squares[m.end] % 6]--;
		zobrist_hash.back() ^= zobrist_keys::piece_locations[m.end][m.player == WHITE ? BLACK : WHITE][squares[m.end] % 6];
	}

	//if promoting, need to update piece counts
	if (m.start_type == PAWN && m.end_type != PAWN) {
		piece_counts.back()[m.player][PAWN]--;
		piece_counts.back()[m.player][m.end_type]++;
	}

	//update the zobrist hash for the board change
	zobrist_hash.back() ^= zobrist_keys::piece_locations[m.end][m.player][m.end_type];
	zobrist_hash.back() ^= zobrist_keys::piece_locations[m.start][m.player][m.start_type];

	//update the actual board
	squares[m.end] = m.player * 6 + m.end_type;
	squares[m.start] = EMPTY_SQUARE;

	//flip who is to play
	white_to_move = !white_to_move;
	zobrist_hash.back() ^= zobrist_keys::white_to_move;

	//if king is in check, undo move and return false
	if (in_check(m.player) || !legal_castle) {
		undo_move(m);
		return false;
	}

	//king is not in check, move is legal
	return true;
}

void Board::undo_move(Move m) {
	//pop last move off of stacks
	can_castle.pop_back();
	half_move_clock.pop_back();
	en_passant_target.pop_back();
	king_positions.pop_back();
	piece_counts.pop_back();
	zobrist_hash.pop_back();

	//restore board pos
	squares[m.start] = m.player * 6 + m.start_type;
	squares[m.end] = m.prev_square;

	//if en passant
	if (m.start_type == PAWN && m.prev_square == EMPTY_SQUARE && abs(m.start - m.end) % 8 != 0) {
		if (m.player == WHITE) squares[m.end + 8] = BLACK * 6 + PAWN;
		else squares[m.end - 8] = WHITE * 6 + PAWN;
	}

	//if castle
	if (m.start_type == KING && abs(m.start - m.end) == 2) {
		bool right = m.end > m.start;
		squares[(m.start + m.end) / 2] = EMPTY_SQUARE;
		squares[right ? (m.start + 3) : (m.start - 4)] = m.player * 6 + ROOK;
	}

	white_to_move = !white_to_move;
}

bool Board::in_check(int player) {
	return is_threatened(player, king_positions.back()[player]);
}

bool Board::is_threatened(int player, int pos) {
	std::vector<Move> opponent_moves = get_valid_moves(player == WHITE ? BLACK : WHITE);
	return is_threatened(player, pos, opponent_moves);
}

//if the piece in position pos owned by player is under attack from any of the given moves
bool Board::is_threatened(int player, int pos, std::vector<Move> moves) {
	bool pawn = squares[pos] % 6 == PAWN;
	for (Move m : moves) {
		if (m.end == pos) return true;
		if (pawn && m.start_type == PAWN && m.end == en_passant_target.back() && abs(m.end - pos) == 8) return true;
	}
	return false;
}

//has the current position been seen twice before
bool Board::is_three_move_rep() {
	//if pawn move or capture in last 6, impossible for three fold rep
	if (half_move_clock.back() < 4) return false;

	//iterate backwards and count occurences of current hash, until last irreversible move
	int seen = 1;
	int ind = zobrist_hash.size() - 5;
	while (zobrist_hash.size() - 1 - ind <= half_move_clock.back()) {
		if (zobrist_hash[ind] == zobrist_hash.back()) {
			seen++;
			if (seen >= 3) return true;
		} 
		ind -= 2;
	}

	return false;
}

//currently only based on piece values and mobility (number of available moves)
double Board::evaluate_position() {

	double val = 0;

	val += piece_counts.back()[WHITE][PAWN];
	val += piece_counts.back()[WHITE][KNIGHT] * 3;
	val += piece_counts.back()[WHITE][BISHOP] * 3;
	val += piece_counts.back()[WHITE][ROOK] * 5;
	val += piece_counts.back()[WHITE][QUEEN] * 9;
	val += piece_counts.back()[WHITE][KING] * 50;

	val -= piece_counts.back()[BLACK][PAWN];
	val -= piece_counts.back()[BLACK][KNIGHT] * 3;
	val -= piece_counts.back()[BLACK][BISHOP] * 3;
	val -= piece_counts.back()[BLACK][ROOK] * 5;
	val -= piece_counts.back()[BLACK][QUEEN] * 9;
	val -= piece_counts.back()[BLACK][KING] * 50;

	val += get_valid_moves(WHITE).size() * 0.1;
	val -= get_valid_moves(BLACK).size() * 0.1;

	return val;
}

void Board::print_board() {
	std::cout << std::endl;
	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {
			char code = squares[r * 8 + c];
			if (code == EMPTY_SQUARE) {
				std::cout << "- ";
			} else if (code / 6 == BLACK) {
				std::cout << RED << get_char_from_piece(code % 6) << RESET << " ";
			} else {
				std::cout << GREEN << get_char_from_piece(code % 6) << RESET << " ";
			}
		}
		std::cout << std::endl;
	}
}

std::vector<int> Board::get_squares() {
	return squares;
}

int Board::get_square(int ind) {
	return squares[ind];
}

bool Board::is_white_to_move() {
	return white_to_move;
}

std::vector<std::vector<bool>> Board::get_castling_rights() {
	return can_castle.back();
}

int Board::get_half_move_clock() {
	return half_move_clock.back();
}

int Board::get_en_passant_target() {
	return en_passant_target.back();
}

unsigned long long Board::get_zobrist_hash() {
	return zobrist_hash.back();
}