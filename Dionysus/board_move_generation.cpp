#include "board.h"

std::vector<Move> Board::get_valid_moves(int player) {

	std::vector<Move> moves = get_valid_captures(player);

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {

			int code = squares[r * 8 + c];
			if (code == EMPTY_SQUARE || code / 6 != player) continue;
			int type = code % 6;

			std::vector<Move> tmp;

			switch (type) {
			case PAWN:
				tmp = get_pawn_moves(player, r, c);
				break;
			case KNIGHT:
				tmp = get_knight_moves(player, r, c);
				break;
			case BISHOP:
				tmp = get_bishop_moves(player, r, c);
				break;
			case ROOK:
				tmp = get_rook_moves(player, r, c);
				break;
			case QUEEN:
				tmp = get_queen_moves(player, r, c);
				break;
			case KING:
				tmp = get_king_moves(player, r, c);
				break;
			}

			moves.insert(moves.end(), tmp.begin(), tmp.end());
		}
	}

	return moves;
}

std::vector<Move> Board::get_pawn_moves(int player, int r, int c) {
	std::vector<Move> moves;

	int dir = player == WHITE ? -1 : 1;

	std::vector<int> promotion_types = { KNIGHT, BISHOP, ROOK, QUEEN };

	int target_code = squares[(r + dir) * 8 + c];
	if (target_code == EMPTY_SQUARE) {

		// can promote if reach 8th rank
		if (r + dir == 0 || r + dir == 7) {
			for (auto& type : promotion_types) {
				Move m = { player, r * 8 + c, (r + dir) * 8 + c, PAWN, type, target_code };
				moves.push_back(m);
			}
		}
		else {
			Move m = { player, r * 8 + c, (r + dir) * 8 + c, PAWN, PAWN, target_code };
			moves.push_back(m);
		}

		// can move 2 on first go
		if ((player == WHITE && r == 6) || (player == BLACK && r == 1)) {
			target_code = squares[(r + (dir * 2)) * 8 + c];
			if (target_code == EMPTY_SQUARE) {
				Move m = { player, r * 8 + c, (r + (dir * 2)) * 8 + c, PAWN, PAWN, target_code };
				moves.push_back(m);
			}
		}
	}

	return moves;
}

std::vector<Move> Board::get_knight_moves(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(1, 2), std::make_pair(2, 1),
												   std::make_pair(-1, 2), std::make_pair(2, -1),
												   std::make_pair(1, -2), std::make_pair(-2, 1),
												   std::make_pair(-1, -2), std::make_pair(-2, -1) };

	for (int i = 0; i < 8; i++) {
		std::pair<int, int> dir = directions[i];

		// make sure we're not trying to move off the board
		if (r + dir.first < 0 || r + dir.first > 7 || c + dir.second < 0 || c + dir.second > 7) continue;

		int target_square = (r + dir.first) * 8 + c + dir.second;
		int target_code = squares[target_square];
		if (target_code == EMPTY_SQUARE) {
			Move m = { player, r * 8 + c, target_square, KNIGHT, KNIGHT, target_code };
			moves.push_back(m);
		}
	}

	return moves;
}

std::vector<Move> Board::get_bishop_moves(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(1, 1), std::make_pair(-1, -1),
												   std::make_pair(-1, 1), std::make_pair(1, -1) };

	for (int i = 0; i < 4; i++) {
		std::pair<int, int> dir = directions[i];
		for (int j = 1; j < 8; j++) {

			// make sure we're not trying to move off the board
			if (r + dir.first * j < 0 || r + dir.first * j > 7 || c + dir.second * j < 0 || c + dir.second * j > 7) break;

			int target_square = (r + dir.first * j) * 8 + c + dir.second * j;
			int target_code = squares[target_square];

			if (target_code == EMPTY_SQUARE) {
				Move m = { player, r * 8 + c, target_square, BISHOP, BISHOP, target_code };
				moves.push_back(m);
			}
			else break;
		}
	}

	return moves;
}

std::vector<Move> Board::get_rook_moves(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(0, 1), std::make_pair(0, -1),
												   std::make_pair(1, 0), std::make_pair(-1, 0) };

	for (int i = 0; i < 4; i++) {
		std::pair<int, int> dir = directions[i];
		for (int j = 1; j < 8; j++) {

			// make sure we're not trying to move off the board
			if (r + dir.first * j < 0 || r + dir.first * j > 7 || c + dir.second * j < 0 || c + dir.second * j > 7) break;

			int target_square = (r + dir.first * j) * 8 + c + dir.second * j;
			int target_code = squares[target_square];

			if (target_code == EMPTY_SQUARE) {
				Move m = { player, r * 8 + c, target_square, ROOK, ROOK, target_code };
				moves.push_back(m);
			}
			else break;
		}
	}

	return moves;
}

std::vector<Move> Board::get_queen_moves(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(0, 1), std::make_pair(0, -1),
												   std::make_pair(1, 0), std::make_pair(-1, 0),
												   std::make_pair(1, 1), std::make_pair(-1, -1),
												   std::make_pair(-1, 1), std::make_pair(1, -1) };

	for (int i = 0; i < 8; i++) {
		std::pair<int, int> dir = directions[i];
		for (int j = 1; j < 8; j++) {

			// make sure we're not trying to move off the board
			if (r + dir.first * j < 0 || r + dir.first * j > 7 || c + dir.second * j < 0 || c + dir.second * j > 7) break;

			int target_square = (r + dir.first * j) * 8 + c + dir.second * j;
			int target_code = squares[target_square];

			if (target_code == EMPTY_SQUARE) {
				Move m = { player, r * 8 + c, target_square, QUEEN, QUEEN, target_code };
				moves.push_back(m);
			}
			else break;
		}
	}

	return moves;
}

std::vector<Move> Board::get_king_moves(int player, int r, int c) {

	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(0, 1), std::make_pair(0, -1),
												   std::make_pair(1, 0), std::make_pair(-1, 0),
												   std::make_pair(1, 1), std::make_pair(-1, -1),
												   std::make_pair(-1, 1), std::make_pair(1, -1) };

	for (int i = 0; i < 8; i++) {
		std::pair<int, int> dir = directions[i];

		// make sure we're not trying to move off the board
		if (r + dir.first < 0 || r + dir.first > 7 || c + dir.second < 0 || c + dir.second > 7) continue;

		int target_square = (r + dir.first) * 8 + c + dir.second;
		int target_code = squares[target_square];

		if (target_code == EMPTY_SQUARE) {
			Move m = { player, r * 8 + c, target_square, KING, KING, target_code };
			moves.push_back(m);
		}
	}

	//castle left
	if (can_castle.back()[player][LEFT]) {
		bool path_clear = true;
		for (int i = 1; i < c; i++) {
			if (squares[r * 8 + i] != EMPTY_SQUARE) {
				path_clear = false;
				break;
			}
		}
		if (path_clear) {
			Move m = { player, r * 8 + c, r * 8 + 2, KING, KING, EMPTY_SQUARE };
			moves.push_back(m);
		}
	}

	//castle right
	if (can_castle.back()[player][RIGHT]) {
		bool path_clear = true;
		for (int i = c + 1; i < 7; i++) {
			if (squares[r * 8 + i] != EMPTY_SQUARE) {
				path_clear = false;
				break;
			}
		}
		if (path_clear) {
			Move m = { player, r * 8 + c, r * 8 + 6, KING, KING, EMPTY_SQUARE };
			moves.push_back(m);
		}
	}

	return moves;
}

std::vector<Move> Board::get_valid_captures(int player) {

	std::vector<Move> moves;

	for (int r = 0; r < 8; r++) {
		for (int c = 0; c < 8; c++) {

			int code = squares[r * 8 + c];
			if (code == EMPTY_SQUARE || code / 6 != player) continue;
			int type = code % 6;

			std::vector<Move> tmp;

			switch (type) {
			case PAWN:
				tmp = get_pawn_captures(player, r, c);
				break;
			case KNIGHT:
				tmp = get_knight_captures(player, r, c);
				break;
			case BISHOP:
				tmp = get_bishop_captures(player, r, c);
				break;
			case ROOK:
				tmp = get_rook_captures(player, r, c);
				break;
			case QUEEN:
				tmp = get_queen_captures(player, r, c);
				break;
			case KING:
				tmp = get_king_captures(player, r, c);
				break;
			}

			moves.insert(moves.end(), tmp.begin(), tmp.end());
		}
	}

	return moves;
}

std::vector<Move> Board::get_pawn_captures(int player, int r, int c) {
	std::vector<Move> moves;

	int dir = player == WHITE ? -1 : 1;

	std::vector<int> promotion_types = { KNIGHT, BISHOP, ROOK, QUEEN };

	// attack towards 8th file
	if (c != 7) {
		int target_code = squares[(r + dir) * 8 + c + 1];
		if ((target_code != EMPTY_SQUARE && target_code / 6 != player) || (r + dir) * 8 + c + 1 == en_passant_target.back()) {

			// can promote if reach 8th rank
			if (r + dir == 0 || r + dir == 7) {
				for (auto& type : promotion_types) {
					Move m = { player, r * 8 + c, (r + dir) * 8 + c + 1, PAWN, type, target_code };
					moves.push_back(m);
				}
			}
			else {
				Move m = { player, r * 8 + c, (r + dir) * 8 + c + 1, PAWN, PAWN, target_code };
				moves.push_back(m);
			}
		}
	}

	// attack towards 1st file
	if (c != 0) {
		int target_code = squares[(r + dir) * 8 + c - 1];
		if ((target_code != EMPTY_SQUARE && target_code / 6 != player) || (r + dir) * 8 + c - 1 == en_passant_target.back()) {

			// can promote if reach 8th rank
			if (r + dir == 0 || r + dir == 7) {
				for (auto& type : promotion_types) {
					Move m = { player, r * 8 + c, (r + dir) * 8 + c - 1, PAWN, type, target_code };
					moves.push_back(m);
				}
			}
			else {
				Move m = { player, r * 8 + c, (r + dir) * 8 + c - 1, PAWN, PAWN, target_code };
				moves.push_back(m);
			}
		}
	}

	return moves;
}

std::vector<Move> Board::get_knight_captures(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(1, 2), std::make_pair(2, 1),
												   std::make_pair(-1, 2), std::make_pair(2, -1),
												   std::make_pair(1, -2), std::make_pair(-2, 1),
												   std::make_pair(-1, -2), std::make_pair(-2, -1) };

	for (int i = 0; i < 8; i++) {
		std::pair<int, int> dir = directions[i];

		// make sure we're not trying to move off the board
		if (r + dir.first < 0 || r + dir.first > 7 || c + dir.second < 0 || c + dir.second > 7) continue;

		int target_square = (r + dir.first) * 8 + c + dir.second;
		int target_code = squares[target_square];
		if (target_code != EMPTY_SQUARE && target_code / 6 != player) {
			Move m = { player, r * 8 + c, target_square, KNIGHT, KNIGHT, target_code };
			moves.push_back(m);
		}
	}

	return moves;
}

std::vector<Move> Board::get_bishop_captures(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(1, 1), std::make_pair(-1, -1),
												   std::make_pair(-1, 1), std::make_pair(1, -1) };

	for (int i = 0; i < 4; i++) {
		std::pair<int, int> dir = directions[i];
		for (int j = 1; j < 8; j++) {

			// make sure we're not trying to move off the board
			if (r + dir.first * j < 0 || r + dir.first * j > 7 || c + dir.second * j < 0 || c + dir.second * j > 7) break;

			int target_square = (r + dir.first * j) * 8 + c + dir.second * j;
			int target_code = squares[target_square];

			if (target_code != EMPTY_SQUARE && target_code / 6 != player) {
				Move m = { player, r * 8 + c, target_square, BISHOP, BISHOP, target_code };
				moves.push_back(m);

				// stop scanning after we hit an opponent
				break;
			}
			else if (target_code != EMPTY_SQUARE && target_code / 6 == player) break;
		}
	}

	return moves;
}

std::vector<Move> Board::get_rook_captures(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(0, 1), std::make_pair(0, -1),
												   std::make_pair(1, 0), std::make_pair(-1, 0) };

	for (int i = 0; i < 4; i++) {
		std::pair<int, int> dir = directions[i];
		for (int j = 1; j < 8; j++) {

			// make sure we're not trying to move off the board
			if (r + dir.first * j < 0 || r + dir.first * j > 7 || c + dir.second * j < 0 || c + dir.second * j > 7) break;

			int target_square = (r + dir.first * j) * 8 + c + dir.second * j;
			int target_code = squares[target_square];

			if (target_code != EMPTY_SQUARE && target_code / 6 != player) {
				Move m = { player, r * 8 + c, target_square, ROOK, ROOK, target_code };
				moves.push_back(m);

				// stop scanning after we hit an opponent
				break;
			}
			else if (target_code != EMPTY_SQUARE && target_code / 6 == player) break;
		}
	}

	return moves;
}

std::vector<Move> Board::get_queen_captures(int player, int r, int c) {
	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(0, 1), std::make_pair(0, -1),
												   std::make_pair(1, 0), std::make_pair(-1, 0),
												   std::make_pair(1, 1), std::make_pair(-1, -1),
												   std::make_pair(-1, 1), std::make_pair(1, -1) };

	for (int i = 0; i < 8; i++) {
		std::pair<int, int> dir = directions[i];
		for (int j = 1; j < 8; j++) {

			// make sure we're not trying to move off the board
			if (r + dir.first * j < 0 || r + dir.first * j > 7 || c + dir.second * j < 0 || c + dir.second * j > 7) break;

			int target_square = (r + dir.first * j) * 8 + c + dir.second * j;
			int target_code = squares[target_square];

			if (target_code != EMPTY_SQUARE && target_code / 6 != player) {
				Move m = { player, r * 8 + c, target_square, QUEEN, QUEEN, target_code };
				moves.push_back(m);

				// stop scanning after we hit an opponent
				break;
			}
			else if (target_code != EMPTY_SQUARE && target_code / 6 == player) break;
		}
	}

	return moves;
}

std::vector<Move> Board::get_king_captures(int player, int r, int c) {

	std::vector<Move> moves;
	std::vector<std::pair<int, int>> directions = { std::make_pair(0, 1), std::make_pair(0, -1),
												   std::make_pair(1, 0), std::make_pair(-1, 0),
												   std::make_pair(1, 1), std::make_pair(-1, -1),
												   std::make_pair(-1, 1), std::make_pair(1, -1) };

	for (int i = 0; i < 8; i++) {
		std::pair<int, int> dir = directions[i];

		// make sure we're not trying to move off the board
		if (r + dir.first < 0 || r + dir.first > 7 || c + dir.second < 0 || c + dir.second > 7) continue;

		int target_square = (r + dir.first) * 8 + c + dir.second;
		int target_code = squares[target_square];

		if (target_code != EMPTY_SQUARE && target_code / 6 != player) {
			Move m = { player, r * 8 + c, target_square, KING, KING, target_code };
			moves.push_back(m);
		}
	}

	return moves;
}