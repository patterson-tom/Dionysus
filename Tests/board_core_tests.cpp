#include "pch.h"
#include "gtest/gtest.h"
#include "../Dionysus/board.h"
#include "../Dionysus/board_core.cpp"
#include "../Dionysus/utils.cpp"
#include "../Dionysus/transposition_table.h"
#include "../Dionysus/transposition_table.cpp"

TEST(BoardInitialisation, SquaresInitialiseFromStartingPos) {
	Board b;
	std::vector<int> squares = b.get_squares();

	ASSERT_EQ(squares.size(), 64);

	//black back row
	EXPECT_EQ(squares[0], BLACK * 6 + ROOK);
	EXPECT_EQ(squares[1], BLACK * 6 + KNIGHT);
	EXPECT_EQ(squares[2], BLACK * 6 + BISHOP);
	EXPECT_EQ(squares[3], BLACK * 6 + QUEEN);
	EXPECT_EQ(squares[4], BLACK * 6 + KING);
	EXPECT_EQ(squares[5], BLACK * 6 + BISHOP);
	EXPECT_EQ(squares[6], BLACK * 6 + KNIGHT);
	EXPECT_EQ(squares[7], BLACK * 6 + ROOK);

	//black pawns
	for (int i = 8; i < 16; i++) {
		EXPECT_EQ(squares[i], BLACK * 6 + PAWN);
	}

	//empty space in middle of board
	for (int i = 16; i < 48; i++) {
		EXPECT_EQ(squares[i], EMPTY_SQUARE);
	}

	//white pawns
	for (int i = 48; i < 56; i++) {
		EXPECT_EQ(squares[i], WHITE * 6 + PAWN);
	}

	//white back row
	EXPECT_EQ(squares[56], WHITE * 6 + ROOK);
	EXPECT_EQ(squares[57], WHITE * 6 + KNIGHT);
	EXPECT_EQ(squares[58], WHITE * 6 + BISHOP);
	EXPECT_EQ(squares[59], WHITE * 6 + QUEEN);
	EXPECT_EQ(squares[60], WHITE * 6 + KING);
	EXPECT_EQ(squares[61], WHITE * 6 + BISHOP);
	EXPECT_EQ(squares[62], WHITE * 6 + KNIGHT);
	EXPECT_EQ(squares[63], WHITE * 6 + ROOK);
}

TEST(BoardInitialisation, WhiteToMoveInitialiseFromStartingPos) {
	Board b;
	bool white_to_move = b.is_white_to_move();

	EXPECT_EQ(white_to_move, true);
}

TEST(BoardInitialisation, CastleRightsInitialiseFromStartingPos) {
	Board b;
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();

	ASSERT_EQ(can_castle.size(), 2);
	ASSERT_EQ(can_castle[WHITE].size(), 2);
	ASSERT_EQ(can_castle[BLACK].size(), 2);

	EXPECT_EQ(can_castle[WHITE][LEFT], true);
	EXPECT_EQ(can_castle[WHITE][RIGHT], true);
	EXPECT_EQ(can_castle[BLACK][LEFT], true);
	EXPECT_EQ(can_castle[BLACK][RIGHT], true);
}

TEST(BoardInitialisation, HalfMoveClockInitialiseFromStartingPos) {
	Board b;
	int move_clock = b.get_half_move_clock();

	EXPECT_EQ(move_clock, 0);
}

TEST(BoardInitialisation, EnPassantTargetInitialiseFromStartingPos) {
	Board b;
	int en_passant_target = b.get_en_passant_target();

	EXPECT_EQ(en_passant_target, EMPTY_SQUARE);
}

TEST(BoardInitialisation, SquaresInitialiseFromRandomPos) {
	Board b("3k2nr/p3pp1p/7b/p3N3/Q2P1PpP/P3P3/1r4P1/RN2K2R b KQ h3 0 1");
	std::vector<int> squares = b.get_squares();

	ASSERT_EQ(squares.size(), 64);

	//black pieces
	EXPECT_EQ(squares[3], BLACK * 6 + KING);
	EXPECT_EQ(squares[6], BLACK * 6 + KNIGHT);
	EXPECT_EQ(squares[7], BLACK * 6 + ROOK);
	EXPECT_EQ(squares[23], BLACK * 6 + BISHOP);
	EXPECT_EQ(squares[49], BLACK * 6 + ROOK);

	//black pawns
	EXPECT_EQ(squares[8], BLACK * 6 + PAWN);
	EXPECT_EQ(squares[12], BLACK * 6 + PAWN);
	EXPECT_EQ(squares[13], BLACK * 6 + PAWN);
	EXPECT_EQ(squares[15], BLACK * 6 + PAWN);
	EXPECT_EQ(squares[24], BLACK * 6 + PAWN);
	EXPECT_EQ(squares[38], BLACK * 6 + PAWN);

	//empty squares
	std::vector<int> empty_squares = {0, 1, 2, 4, 5, 9, 10, 11, 14, 16, 17, 18, 19, 20, 21, 22, 25, 26, 27, 29, 30, 31, 33, 34, 36, 41, 42, 43, 45, 46, 47, 48, 50, 51, 52, 53, 55, 58, 59, 61, 62};
	for (int i : empty_squares) {
		EXPECT_EQ(squares[i], EMPTY_SQUARE);
	}
	EXPECT_EQ(empty_squares.size(), 41); //(check we have remembered all of them)

	//white pawns
	EXPECT_EQ(squares[35], WHITE * 6 + PAWN);
	EXPECT_EQ(squares[37], WHITE * 6 + PAWN);
	EXPECT_EQ(squares[39], WHITE * 6 + PAWN);
	EXPECT_EQ(squares[40], WHITE * 6 + PAWN);
	EXPECT_EQ(squares[44], WHITE * 6 + PAWN);
	EXPECT_EQ(squares[54], WHITE * 6 + PAWN);

	//white pieces
	EXPECT_EQ(squares[28], WHITE * 6 + KNIGHT);
	EXPECT_EQ(squares[32], WHITE * 6 + QUEEN);
	EXPECT_EQ(squares[56], WHITE * 6 + ROOK);
	EXPECT_EQ(squares[57], WHITE * 6 + KNIGHT);
	EXPECT_EQ(squares[60], WHITE * 6 + KING);
	EXPECT_EQ(squares[63], WHITE * 6 + ROOK);
}

TEST(BoardInitialisation, WhiteToMoveInitialiseFromRandomPos) {
	Board b("3k2nr/p3pp1p/7b/p3N3/Q2P1PpP/P3P3/1r4P1/RN2K2R b KQ h3 0 1");
	bool white_to_move = b.is_white_to_move();

	EXPECT_EQ(white_to_move, false);
}

TEST(BoardInitialisation, CastleRightsInitialiseFromRandomPos) {
	Board b("3k2nr/p3pp1p/7b/p3N3/Q2P1PpP/P3P3/1r4P1/RN2K2R b KQ h3 0 1");
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();

	ASSERT_EQ(can_castle.size(), 2);
	ASSERT_EQ(can_castle[WHITE].size(), 2);
	ASSERT_EQ(can_castle[BLACK].size(), 2);

	EXPECT_EQ(can_castle[WHITE][LEFT], true);
	EXPECT_EQ(can_castle[WHITE][RIGHT], true);
	EXPECT_EQ(can_castle[BLACK][LEFT], false);
	EXPECT_EQ(can_castle[BLACK][RIGHT], false);
}

TEST(BoardInitialisation, HalfMoveClockInitialiseFromRandomPos) {
	Board b("3k2nr/p3pp1p/7b/p3N3/Q2P1PpP/P3P3/1r4P1/RN2K2R b KQ h3 0 1");
	int move_clock = b.get_half_move_clock();

	EXPECT_EQ(move_clock, 0);
}

TEST(BoardInitialisation, EnPassantTargetInitialiseFromRandomPos) {
	Board b("3k2nr/p3pp1p/7b/p3N3/Q2P1PpP/P3P3/1r4P1/RN2K2R b KQ h3 0 1");
	int en_passant_target = b.get_en_passant_target();

	EXPECT_EQ(en_passant_target, 47);
}

TEST(BoardMakeMove, PawnPushUpdatesBoard) {
	Board b;

	std::vector<int> squares = b.get_squares();
	EXPECT_EQ(squares[36], EMPTY_SQUARE);
	EXPECT_EQ(squares[52], WHITE * 6 + PAWN);

	Move e4 = { WHITE, 52, 36, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(e4);

	squares = b.get_squares();
	EXPECT_EQ(squares[52], EMPTY_SQUARE);
	EXPECT_EQ(squares[36], WHITE * 6 + PAWN);
}

TEST(BoardMakeMove, PawnPushKeepsClockAtZero) {
	Board b;

	EXPECT_EQ(b.get_half_move_clock(), 0);

	Move e4 = { WHITE, 52, 36, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(e4);

	EXPECT_EQ(b.get_half_move_clock(), 0);
}

TEST(BoardMakeMove, PawnPushRetainsCastlingRights) {
	Board b;

	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	EXPECT_TRUE(can_castle[0][0]);
	EXPECT_TRUE(can_castle[0][1]);
	EXPECT_TRUE(can_castle[1][0]);
	EXPECT_TRUE(can_castle[1][1]);

	Move e4 = { WHITE, 52, 36, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(e4);

	can_castle = b.get_castling_rights();
	EXPECT_TRUE(can_castle[0][0]);
	EXPECT_TRUE(can_castle[0][1]);
	EXPECT_TRUE(can_castle[1][0]);
	EXPECT_TRUE(can_castle[1][1]);
}

TEST(BoardMakeMove, PawnPushCreatesEnPassantTarget) {
	Board b;

	int en_passant_target = b.get_en_passant_target();
	EXPECT_EQ(en_passant_target, EMPTY_SQUARE);

	Move e4 = { WHITE, 52, 36, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(e4);

	en_passant_target = b.get_en_passant_target();
	EXPECT_EQ(en_passant_target, 44);
}

TEST(BoardMakeMove, SingularPawnPushDoesntCreateEnPassantTarget) {
	Board b;

	int en_passant_target = b.get_en_passant_target();
	EXPECT_EQ(en_passant_target, EMPTY_SQUARE);

	Move e3 = { WHITE, 52, 44, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(e3);

	en_passant_target = b.get_en_passant_target();
	EXPECT_EQ(en_passant_target, EMPTY_SQUARE);
}

TEST(BoardMakeMove, PawnPushIsUndoneCorrectly) {
	Board b;

	int en_passant_target = b.get_en_passant_target();
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	int half_move_clock = b.get_half_move_clock();
	std::vector<int> squares = b.get_squares();

	Move e4 = { WHITE, 52, 36, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(e4);
	b.undo_move(e4);

	EXPECT_EQ(en_passant_target, b.get_en_passant_target());
	EXPECT_EQ(half_move_clock, b.get_half_move_clock());

	std::vector<std::vector<bool>> new_can_castle = b.get_castling_rights();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			EXPECT_EQ(can_castle[i][j], new_can_castle[i][j]);
		}
	}

	std::vector<int> new_squares = b.get_squares();
	for (int i = 0; i < 64; i++) {
		EXPECT_EQ(squares[i], new_squares[i]);
	}
}

TEST(BoardMakeMove, MultipleMovesAreUndoneCorrectly) {
	Board b;

	int en_passant_target = b.get_en_passant_target();
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	int half_move_clock = b.get_half_move_clock();
	std::vector<int> squares = b.get_squares();

	Move e4 = { WHITE, 52, 36, PAWN, PAWN, EMPTY_SQUARE };
	Move d5 = { BLACK, 11, 27, PAWN, PAWN, EMPTY_SQUARE };
	Move exd5 = { WHITE, 36, 27, PAWN, PAWN, BLACK * 6 + PAWN };
	b.make_move(e4);
	b.make_move(d5);
	b.make_move(exd5);
	b.undo_move(exd5);
	b.undo_move(d5);
	b.undo_move(e4);

	EXPECT_EQ(en_passant_target, b.get_en_passant_target());
	EXPECT_EQ(half_move_clock, b.get_half_move_clock());

	std::vector<std::vector<bool>> new_can_castle = b.get_castling_rights();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			EXPECT_EQ(can_castle[i][j], new_can_castle[i][j]);
		}
	}

	std::vector<int> new_squares = b.get_squares();
	for (int i = 0; i < 64; i++) {
		EXPECT_EQ(squares[i], new_squares[i]);
	}
}

TEST(BoardMakeMove, PawnCaptureIsUndoneCorrectly) {
	Board b("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");

	int en_passant_target = b.get_en_passant_target();
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	int half_move_clock = b.get_half_move_clock();
	std::vector<int> squares = b.get_squares();

	Move exd5 = { WHITE, 36, 27, PAWN, PAWN, BLACK * 6 + PAWN };
	b.make_move(exd5);
	b.undo_move(exd5);

	EXPECT_EQ(en_passant_target, b.get_en_passant_target());
	EXPECT_EQ(half_move_clock, b.get_half_move_clock());

	std::vector<std::vector<bool>> new_can_castle = b.get_castling_rights();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			EXPECT_EQ(can_castle[i][j], new_can_castle[i][j]);
		}
	}

	std::vector<int> new_squares = b.get_squares();
	for (int i = 0; i < 64; i++) {
		EXPECT_EQ(squares[i], new_squares[i]);
	}
}

TEST(BoardMakeMove, KingMoveRemovesCastlingRights) {
	Board b("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");

	Move ke2 = { WHITE, 60, 52, KING, KING, EMPTY_SQUARE };
	b.make_move(ke2);

	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	EXPECT_FALSE(can_castle[0][0]);
	EXPECT_FALSE(can_castle[0][1]);
	EXPECT_TRUE(can_castle[1][0]);
	EXPECT_TRUE(can_castle[1][1]);
}

TEST(BoardMakeMove, UndoingKingMoveRestoresCastlingRights) {
	Board b("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");

	Move ke2 = { WHITE, 60, 52, KING, KING, EMPTY_SQUARE };
	b.make_move(ke2);
	b.undo_move(ke2);

	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	EXPECT_TRUE(can_castle[0][0]);
	EXPECT_TRUE(can_castle[0][1]);
	EXPECT_TRUE(can_castle[1][0]);
	EXPECT_TRUE(can_castle[1][1]);
}

TEST(BoardMakeMove, RightRookMoveRemovesCastlingRights) {
	Board b("rnbqkbnr/1pp1ppp1/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

	Move rh8 = { BLACK, 7, 15, ROOK, ROOK, EMPTY_SQUARE };
	b.make_move(rh8);

	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	EXPECT_TRUE(can_castle[0][0]);
	EXPECT_TRUE(can_castle[0][1]);
	EXPECT_TRUE(can_castle[1][0]);
	EXPECT_FALSE(can_castle[1][1]);
}

TEST(BoardMakeMove, LeftRookMoveRemovesCastlingRights) {
	Board b("rnbqkbnr/1pp1ppp1/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

	Move rh1 = { BLACK, 0, 8, ROOK, ROOK, EMPTY_SQUARE };
	b.make_move(rh1);

	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	EXPECT_TRUE(can_castle[0][0]);
	EXPECT_TRUE(can_castle[0][1]);
	EXPECT_FALSE(can_castle[1][0]);
	EXPECT_TRUE(can_castle[1][1]);
}

TEST(BoardMakeMove, UndoingRookMoveRestoresCastlingRights) {
	Board b("rnbqkbnr/1pp1ppp1/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");

	Move rh8 = { BLACK, 7, 15, ROOK, ROOK, EMPTY_SQUARE };
	b.make_move(rh8);
	b.undo_move(rh8);

	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	EXPECT_TRUE(can_castle[0][0]);
	EXPECT_TRUE(can_castle[0][1]);
	EXPECT_TRUE(can_castle[1][0]);
	EXPECT_TRUE(can_castle[1][1]);
}

TEST(BoardMakeMove, PawnPromotionWorks) {
	Board b("8/P7/8/8/8/8/8/8 w - - 0 1");

	Move a8_q = { WHITE, 8, 0, PAWN, QUEEN, EMPTY_SQUARE };
	b.make_move(a8_q);

	std::vector<int> squares = b.get_squares();
	EXPECT_EQ(squares[0], WHITE * 6 + QUEEN);
	for (int i = 1; i < 64; i++) {
		EXPECT_EQ(squares[i], EMPTY_SQUARE);
	}
}

TEST(BoardMakeMove, PawnPromotionCanBeUndone) {
	Board b("8/P7/8/8/8/8/8/8 w - - 0 1");

	int en_passant_target = b.get_en_passant_target();
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	int half_move_clock = b.get_half_move_clock();
	std::vector<int> squares = b.get_squares();

	Move a8_n = { WHITE, 8, 0, PAWN, KNIGHT, EMPTY_SQUARE };
	b.make_move(a8_n);
	b.undo_move(a8_n);

	EXPECT_EQ(en_passant_target, b.get_en_passant_target());
	EXPECT_EQ(half_move_clock, b.get_half_move_clock());

	std::vector<std::vector<bool>> new_can_castle = b.get_castling_rights();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			EXPECT_EQ(can_castle[i][j], new_can_castle[i][j]);
		}
	}

	std::vector<int> new_squares = b.get_squares();
	for (int i = 0; i < 64; i++) {
		EXPECT_EQ(squares[i], new_squares[i]);
	}
}

TEST(BoardMakeMove, CaptureKeepsClockAtZero) {
	Board b("rB6/8/8/8/8/8/8/8 b - - 0 1");

	EXPECT_EQ(b.get_half_move_clock(), 0);

	Move rxb8 = { BLACK, 0, 1, ROOK, ROOK, WHITE * 6 + BISHOP };
	b.make_move(rxb8);

	EXPECT_EQ(b.get_half_move_clock(), 0);
}

TEST(BoardMakeMove, KnightNonCaptureIncrementsClock) {
	Board b("8/8/8/8/8/5N2/8/8 w - - 3 1");

	EXPECT_EQ(b.get_half_move_clock(), 3);

	Move ne5 = { WHITE, 45, 28, KNIGHT, KNIGHT, EMPTY_SQUARE };
	b.make_move(ne5);

	EXPECT_EQ(b.get_half_move_clock(), 4);
}

TEST(BoardMakeMove, UndoingKnightNonCaptureDecrementsClock) {
	Board b("8/8/8/8/8/5N2/8/8 w - - 3 1");

	EXPECT_EQ(b.get_half_move_clock(), 3);

	Move ne5 = { WHITE, 45, 28, KNIGHT, KNIGHT, EMPTY_SQUARE };
	b.make_move(ne5);
	EXPECT_EQ(b.get_half_move_clock(), 4);
	b.undo_move(ne5);
	EXPECT_EQ(b.get_half_move_clock(), 3);
}

TEST(BoardMakeMove, CannotMoveIntoCheck) {
	Board b("4k3/8/8/4r3/8/8/8/4Q3 b - - 0 1");

	Move rf5 = { BLACK, 28, 29, ROOK, ROOK, EMPTY_SQUARE };
	EXPECT_FALSE(b.make_move(rf5));
}

TEST(BoardMakeMove, CanMoveNotIntoCheck) {
	Board b("4k3/8/8/4r3/8/8/8/4Q3 b - - 0 1");

	Move rxe8 = { BLACK, 28, 60, ROOK, ROOK, WHITE * 6 + QUEEN };
	EXPECT_TRUE(b.make_move(rxe8));
}

TEST(BoardMakeMove, EnPassantMoveWorks) {
	Board b("8/8/8/3pP3/8/8/8/8 w - e4 0 1");

	Move dxe4 = { BLACK, 27, 36, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(dxe4);

	std::vector<int> squares = b.get_squares();
	EXPECT_EQ(squares[28], EMPTY_SQUARE);
	EXPECT_EQ(squares[36], BLACK * 6 + PAWN);
	EXPECT_EQ(squares[27], EMPTY_SQUARE);
}

TEST(BoardMakeMove, EnPassantCanBeUndone) {
	Board b("8/8/8/3pP3/8/8/8/8 w - e4 0 1");

	int en_passant_target = b.get_en_passant_target();
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	int half_move_clock = b.get_half_move_clock();
	std::vector<int> squares = b.get_squares();

	Move dxe4 = { BLACK, 27, 36, PAWN, PAWN, EMPTY_SQUARE };
	b.make_move(dxe4);
	b.undo_move(dxe4);

	EXPECT_EQ(en_passant_target, b.get_en_passant_target());
	EXPECT_EQ(half_move_clock, b.get_half_move_clock());

	std::vector<std::vector<bool>> new_can_castle = b.get_castling_rights();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			EXPECT_EQ(can_castle[i][j], new_can_castle[i][j]);
		}
	}

	std::vector<int> new_squares = b.get_squares();
	for (int i = 0; i < 64; i++) {
		EXPECT_EQ(squares[i], new_squares[i]);
	}
}

TEST(BoardMakeMove, CastlingKingsideWorks) {
	Board b("8/8/8/8/8/8/8/4K2R w K - 0 1");

	Move O_O = { WHITE, 60, 62, KING, KING, EMPTY_SQUARE };
	b.make_move(O_O);

	std::vector<int> squares = b.get_squares();
	EXPECT_EQ(squares[60], EMPTY_SQUARE);
	EXPECT_EQ(squares[61], WHITE * 6 + ROOK);
	EXPECT_EQ(squares[62], WHITE * 6 + KING);
	EXPECT_EQ(squares[63], EMPTY_SQUARE);
}

TEST(BoardMakeMove, CastlingQueensideWorks) {
	Board b("r3k3/8/8/8/8/8/8/8 w q - 0 1");

	Move O_O_O = { BLACK, 4, 2, KING, KING, EMPTY_SQUARE };
	b.make_move(O_O_O);

	std::vector<int> squares = b.get_squares();
	EXPECT_EQ(squares[0], EMPTY_SQUARE);
	EXPECT_EQ(squares[1], EMPTY_SQUARE);
	EXPECT_EQ(squares[2], BLACK * 6 + KING);
	EXPECT_EQ(squares[3], BLACK * 6 + ROOK);
	EXPECT_EQ(squares[4], EMPTY_SQUARE);
}

TEST(BoardMakeMove, CastlingCanBeUndone) {
	Board b("r3k3/8/8/8/8/8/8/8 w q - 0 1");

	int en_passant_target = b.get_en_passant_target();
	std::vector<std::vector<bool>> can_castle = b.get_castling_rights();
	int half_move_clock = b.get_half_move_clock();
	std::vector<int> squares = b.get_squares();

	Move O_O_O = { BLACK, 4, 2, KING, KING, EMPTY_SQUARE };
	b.make_move(O_O_O);
	b.undo_move(O_O_O);

	EXPECT_EQ(en_passant_target, b.get_en_passant_target());
	EXPECT_EQ(half_move_clock, b.get_half_move_clock());

	std::vector<std::vector<bool>> new_can_castle = b.get_castling_rights();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 2; j++) {
			EXPECT_EQ(can_castle[i][j], new_can_castle[i][j]);
		}
	}

	std::vector<int> new_squares = b.get_squares();
	for (int i = 0; i < 64; i++) {
		EXPECT_EQ(squares[i], new_squares[i]);
	}
}

TEST(BoardMakeMove, CannotCastleOutOfCheck) {
	Board b("r3k3/8/8/8/8/8/8/4Q3 w q - 0 1");

	Move O_O_O = { BLACK, 4, 2, KING, KING, EMPTY_SQUARE };
	EXPECT_FALSE(b.make_move(O_O_O));
}

TEST(BoardMakeMove, CannotCastleThroughk) {
	Board b("r3k3/8/8/8/8/8/8/3Q4 w q - 0 1");

	Move O_O_O = { BLACK, 4, 2, KING, KING, EMPTY_SQUARE };
	EXPECT_FALSE(b.make_move(O_O_O));
}