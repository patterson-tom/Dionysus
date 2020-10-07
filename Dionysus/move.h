#pragma once

struct Move {
	int player;
	int start, end;
	int start_type, end_type;
	int prev_square;
};