#pragma once
namespace zobrist_keys {
	unsigned long long white_to_move;
	std::vector<std::vector<unsigned long long>> can_castle;
	std::vector<unsigned long long> en_passant_target;

	//square_pieces[SQUARE][COLOR][PIECE]
	std::vector<std::vector<std::vector<unsigned long long>>> piece_locations;
};