#include "pch.h"
#include "gtest/gtest.h"
#include "../Dionysus/board.h"
#include "../Dionysus/board_move_generation.cpp"


//assumes we have already checked that the correct number of moves have been returned
bool expected_moves_are_seen(std::map<int, std::vector<int>> expected_moves, std::vector<Move> moves) {

	//removes each move from expected_moves when we find it in moves
	for (int i = 0; i < moves.size(); i++) {
		auto start_index = expected_moves.find(moves[i].start);
		if (start_index != expected_moves.end()) {
			auto end_index = std::find(start_index->second.begin(), start_index->second.end(), moves[i].end);
			if (end_index != start_index->second.end()) {
				start_index->second.erase(end_index);
			}
		}
	}

	//if expected_moves is empty, we have seen them all
	for (auto it : expected_moves) {
		if (it.second.size() != 0) return false;
	}

	return true;
}

TEST(BoardMoveGeneration, WhiteMovesFromStartPos) {
	Board b;
	std::vector<Move> moves = b.get_valid_moves(WHITE);

	ASSERT_EQ(moves.size(), 20);

	std::map<int, std::vector<int>> expected_moves;

	//pawn moves
	for (int i = 0; i < 8; i++) {
		std::vector<int> tmp = { 5 * 8 + i, 4 * 8 + i };
		expected_moves.emplace(6 * 8 + i, tmp);
	}

	//knight moves
	std::vector<int> tmp = { 5 * 8 + 0, 5 * 8 + 2 };
	expected_moves.emplace(7 * 8 + 1, tmp);
	std::vector<int> tmp2 = { 5 * 8 + 5, 5 * 8 + 7 };
	expected_moves.emplace(7 * 8 + 6, tmp2);

	EXPECT_TRUE(expected_moves_are_seen(expected_moves, moves));
}

TEST(BoardMoveGeneration, BlackMovesFromStartPos) {
	Board b;
	std::vector<Move> moves = b.get_valid_moves(BLACK);

	ASSERT_EQ(moves.size(), 20);

	std::map<int, std::vector<int>> expected_moves;

	//pawn moves
	for (int i = 0; i < 8; i++) {
		std::vector<int> tmp = { 2 * 8 + i, 3 * 8 + i };
		expected_moves.emplace(1 * 8 + i, tmp);
	}

	//knight moves
	std::vector<int> tmp = { 2 * 8 + 0, 2 * 8 + 2 };
	expected_moves.emplace(0 * 8 + 1, tmp);
	std::vector<int> tmp2 = { 2 * 8 + 5, 2 * 8 + 7 };
	expected_moves.emplace(0 * 8 + 6, tmp2);

	EXPECT_TRUE(expected_moves_are_seen(expected_moves, moves));
}

TEST(BoardMoveGeneration, PawnCanPromoteStraight) {
	Board b("8/3P4/8/8/8/8/8/8 w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);

	ASSERT_EQ(moves.size(), 4);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {11, {3, 3, 3, 3}} };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, moves));

	//check end_type of moves is correct
	std::vector<int> promotion_types = { KNIGHT, BISHOP, ROOK, QUEEN };
	for (int i = 0; i < moves.size(); i++) {
		auto it = std::find(promotion_types.begin(), promotion_types.end(), moves[i].end_type);
		if (it != promotion_types.end()) {
			promotion_types.erase(it);
		}
	}
	EXPECT_EQ(promotion_types.size(), 0);

	//check start_type and captured square is correct
	for (int i = 0; i < moves.size(); i++) {
		EXPECT_EQ(moves[i].prev_square, EMPTY_SQUARE);
		EXPECT_EQ(moves[i].start_type, PAWN);
	}
}

TEST(BoardMoveGeneration, PawnCanPromoteByCapturing) {
	Board b("8/8/8/8/8/8/3p4/3BR3 b - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(BLACK);

	ASSERT_EQ(moves.size(), 4);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {51, {60, 60, 60, 60}} };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, moves));

	//check end_type of moves is correct
	std::vector<int> promotion_types = { KNIGHT, BISHOP, ROOK, QUEEN };
	for (int i = 0; i < moves.size(); i++) {
		auto it = std::find(promotion_types.begin(), promotion_types.end(), moves[i].end_type);
		if (it != promotion_types.end()) {
			promotion_types.erase(it);
		}
	}
	EXPECT_EQ(promotion_types.size(), 0);

	//check start_type and captured square is correct
	for (int i = 0; i < moves.size(); i++) {
		EXPECT_EQ(moves[i].prev_square, WHITE * 6 + ROOK);
		EXPECT_EQ(moves[i].start_type, PAWN);
	}
}

TEST(BoardMoveGeneration, PawnCanMoveTwoOnFirstGo) {
	Board b("8/8/8/8/8/8/2P5/8 w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);

	ASSERT_EQ(moves.size(), 2);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {50, {42, 34}} };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, moves));

	//check captured square and start/end_type is correct
	for (int i = 0; i < moves.size(); i++) {
		EXPECT_EQ(moves[i].prev_square, EMPTY_SQUARE);
		EXPECT_EQ(moves[i].end_type, PAWN);
		EXPECT_EQ(moves[i].start_type, PAWN);
	}
}

TEST(BoardMoveGeneration, PawnCantMoveTwoOnRankThree) {
	Board b("8/8/8/8/8/2P5/8/8 w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);

	ASSERT_EQ(moves.size(), 1);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {42, {34}} };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, moves));

	//check captured square and start/end_type is correct
	for (int i = 0; i < moves.size(); i++) {
		EXPECT_EQ(moves[i].prev_square, EMPTY_SQUARE);
		EXPECT_EQ(moves[i].end_type, PAWN);
		EXPECT_EQ(moves[i].start_type, PAWN);
	}
}

TEST(BoardMoveGeneration, PawnCantMoveStraightThroughPiece) {
	Board b("8/8/8/8/2R1b3/2P1P3/8/8 w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);
	std::vector<Move> pawn_moves;
	std::copy_if(moves.begin(), moves.end(), std::back_inserter(pawn_moves), [](Move m) { return m.start_type == PAWN; });

	ASSERT_EQ(pawn_moves.size(), 0);
}



TEST(BoardMoveGeneration, PawnCanCaptureDiagonally) {
	Board b("8/p3p2p/RQ1rrrQR/8/8/8/8/8 b - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(BLACK);
	std::vector<Move> pawn_moves;
	std::copy_if(moves.begin(), moves.end(), std::back_inserter(pawn_moves), [](Move m) { return m.start_type == PAWN; });
	ASSERT_EQ(pawn_moves.size(), 2);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {8, {17} }, {15, {22} } };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, pawn_moves));

	//check captured square and start/end_type is correct
	for (int i = 0; i < pawn_moves.size(); i++) {
		EXPECT_EQ(pawn_moves[i].prev_square, WHITE * 6 + QUEEN);
		EXPECT_EQ(pawn_moves[i].end_type, PAWN);
		EXPECT_EQ(moves[i].start_type, PAWN);
	}
}

TEST(BoardMoveGeneration, KnightMovesCorrectlyInCentre) {
	Board b("8/8/7P/4r3/6N1/8/8/8 w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);
	std::vector<Move> knight_moves;
	std::copy_if(moves.begin(), moves.end(), std::back_inserter(knight_moves), [](Move m) { return m.start_type == KNIGHT; });
	ASSERT_EQ(knight_moves.size(), 5);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {38, {21, 28, 44, 53, 55} } };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, knight_moves));

	//check captured square of moves is correct
	std::vector<int> expected_captures = { EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, BLACK * 6 + ROOK };
	for (int i = 0; i < knight_moves.size(); i++) {
		auto it = std::find(expected_captures.begin(), expected_captures.end(), knight_moves[i].prev_square);
		if (it != expected_captures.end()) {
			expected_captures.erase(it);
		}
	}
	EXPECT_EQ(expected_captures.size(), 0);

	//check start/end_type is correct
	for (int i = 0; i < knight_moves.size(); i++) {
		EXPECT_EQ(knight_moves[i].end_type, KNIGHT);
		EXPECT_EQ(knight_moves[i].start_type, KNIGHT);
	}
}

TEST(BoardMoveGeneration, BishopMovesCorrectlyInCentre) {
	Board b("5n2/6b1/8/8/8/2R5/8/8 b - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(BLACK);
	std::vector<Move> bishop_moves;
	std::copy_if(moves.begin(), moves.end(), std::back_inserter(bishop_moves), [](Move m) { return m.start_type == BISHOP; });
	ASSERT_EQ(bishop_moves.size(), 6);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {14, {7, 21, 28, 35, 42, 23} } };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, bishop_moves));

	//check captured square of moves is correct
	std::vector<int> expected_captures = { EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, WHITE * 6 + ROOK };
	for (int i = 0; i < bishop_moves.size(); i++) {
		auto it = std::find(expected_captures.begin(), expected_captures.end(), bishop_moves[i].prev_square);
		if (it != expected_captures.end()) {
			expected_captures.erase(it);
		}
	}
	EXPECT_EQ(expected_captures.size(), 0);

	//check start/end_type is correct
	for (int i = 0; i < bishop_moves.size(); i++) {
		EXPECT_EQ(bishop_moves[i].end_type, BISHOP);
		EXPECT_EQ(bishop_moves[i].start_type, BISHOP);
	}
}

TEST(BoardMoveGeneration, RookMovesCorrectlyInCentre) {
	Board b("2B5/8/2r2P2/2b5/8/8/8/8 b - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(BLACK);
	std::vector<Move> rook_moves;
	std::copy_if(moves.begin(), moves.end(), std::back_inserter(rook_moves), [](Move m) { return m.start_type == ROOK; });
	ASSERT_EQ(rook_moves.size(), 7);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {18, {10, 2, 17, 16, 19, 20, 21} } };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, rook_moves));

	//check captured square of moves is correct
	std::vector<int> expected_captures = { EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, WHITE * 6 + PAWN, WHITE * 6 + BISHOP };
	for (int i = 0; i < rook_moves.size(); i++) {
		auto it = std::find(expected_captures.begin(), expected_captures.end(), rook_moves[i].prev_square);
		if (it != expected_captures.end()) {
			expected_captures.erase(it);
		}
	}
	EXPECT_EQ(expected_captures.size(), 0);

	//check start/end_type is correct
	for (int i = 0; i < rook_moves.size(); i++) {
		EXPECT_EQ(rook_moves[i].end_type, ROOK);
		EXPECT_EQ(rook_moves[i].start_type, ROOK);
	}
}

TEST(BoardMoveGeneration, QueenMovesCorrectlyInCentre) {
	Board b("8/2N5/4qq2/4Q3/8/2R1b1P1/8/8 w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);
	std::vector<Move> queen_moves;
	std::copy_if(moves.begin(), moves.end(), std::back_inserter(queen_moves), [](Move m) { return m.start_type == QUEEN; });
	ASSERT_EQ(queen_moves.size(), 14);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {28, {19, 20, 21, 24, 25, 26, 27, 29, 30, 31, 35, 36, 37, 44} } };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, queen_moves));

	//check captured square of moves is correct
	std::vector<int> expected_captures = { EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, BLACK * 6 + QUEEN, BLACK * 6 + QUEEN, BLACK * 6 + BISHOP };
	for (int i = 0; i < queen_moves.size(); i++) {
		auto it = std::find(expected_captures.begin(), expected_captures.end(), queen_moves[i].prev_square);
		if (it != expected_captures.end()) {
			expected_captures.erase(it);
		}
	}
	EXPECT_EQ(expected_captures.size(), 0);

	//check start/end_type is correct
	for (int i = 0; i < queen_moves.size(); i++) {
		EXPECT_EQ(queen_moves[i].end_type, QUEEN);
		EXPECT_EQ(queen_moves[i].start_type, QUEEN);
	}
}

TEST(BoardMoveGeneration, KingMovesCorrectlyInCentre) {
	Board b("8/8/2n5/3KP3/3P4/8/8/8 w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);
	std::vector<Move> king_moves;
	std::copy_if(moves.begin(), moves.end(), std::back_inserter(king_moves), [](Move m) { return m.start_type == KING; });
	ASSERT_EQ(king_moves.size(), 6);

	//check location of moves are correct
	std::map<int, std::vector<int>> expected_moves = { {27, {18, 19, 20, 26, 34, 36} } };
	EXPECT_TRUE(expected_moves_are_seen(expected_moves, king_moves));

	//check captured square of moves is correct
	std::vector<int> expected_captures = { EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, EMPTY_SQUARE, BLACK * 6 + KNIGHT };
	for (int i = 0; i < king_moves.size(); i++) {
		auto it = std::find(expected_captures.begin(), expected_captures.end(), king_moves[i].prev_square);
		if (it != expected_captures.end()) {
			expected_captures.erase(it);
		}
	}
	EXPECT_EQ(expected_captures.size(), 0);

	//check start/end_type is correct
	for (int i = 0; i < king_moves.size(); i++) {
		EXPECT_EQ(king_moves[i].end_type, KING);
		EXPECT_EQ(king_moves[i].start_type, KING);
	}
}

TEST(BoardMoveGeneration, EnPassantIsFound) {
	Board b("8/8/8/3pP3/8/8/8/8 b - e4 0 1");
	std::vector<Move> moves = b.get_valid_moves(BLACK);
	ASSERT_EQ(moves.size(), 2);

	//check one move is diagonal to square e4
	int en_passant_moves = 0;
	for (Move m : moves) {
		if (m.end == 36) en_passant_moves++;
	}

	EXPECT_EQ(en_passant_moves, 1);
}

TEST(BoardMoveGeneration, EnPassantIsNotFoundWhenNotThere) {
	Board b("8/8/8/3pP3/8/8/8/8 b - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(BLACK);
	ASSERT_EQ(moves.size(), 1);

	//check no moves are diagonal to square e4
	int en_passant_moves = 0;
	for (Move m : moves) {
		if (m.end == 36) en_passant_moves++;
	}

	EXPECT_EQ(en_passant_moves, 0);
}

TEST(BoardMoveGeneration, CastlingIsFound) {
	Board b("8/8/8/8/8/8/8/R3K2R w KQ - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);

	//check two moves are king castling
	int castling_moves = 0;
	for (Move m : moves) {
		if (m.start_type == KING && abs(m.end - m.start) == 2) castling_moves++;
	}

	EXPECT_EQ(castling_moves, 2);
}

TEST(BoardMoveGeneration, CastlingIsNotFoundWhenNotThere) {
	Board b("8/8/8/8/8/8/8/R3K2R w - - 0 1");
	std::vector<Move> moves = b.get_valid_moves(WHITE);

	//check no moves are king castling
	int castling_moves = 0;
	for (Move m : moves) {
		if (m.start_type == KING && abs(m.end - m.start) == 2) castling_moves++;
	}

	EXPECT_EQ(castling_moves, 0);
}