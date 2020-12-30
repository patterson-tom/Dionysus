#pragma once

#include "Move.h"
#include "Board.h"

Move create_move_from_lan(std::string, Board*);
std::string create_lan_from_move(Move);

int get_square_index_from_notation(std::string);
std::string get_notation_from_square_index(int);

int get_piece_from_char(char);
char get_char_from_piece(int);

unsigned short endian_swap_u16(unsigned short x);
unsigned int endian_swap_u32(unsigned int x);
unsigned long long endian_swap_u64(unsigned long long x);