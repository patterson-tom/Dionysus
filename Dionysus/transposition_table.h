#pragma once

#include <unordered_map>
#include <functional>

#include "search_result.h"

#define NOT_PRESENT -1
#define EXACT 0
#define LOWER_BOUND 1
#define UPPER_BOUND 2

struct TransTableEntry {
	int flag;
	SearchResult sr;
	int depth;
};

typedef std::unordered_map<unsigned long long, TransTableEntry, std::function<size_t(unsigned long long key)>> tt_internal_map_type;

class TranspositionTable {

	tt_internal_map_type trans_table;
	TransTableEntry not_present = { NOT_PRESENT };

public:
	TranspositionTable();
	TransTableEntry* get_if_exists(unsigned long long);
	void store(unsigned long long, TransTableEntry);
	void clear();
};

