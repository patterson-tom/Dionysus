#include "transposition_table.h"

//dont need a real hash function, as the zobrist hashes are already hashed. Just mod by max value of size_t to get them down to correct size
size_t hash_function(unsigned long long key) {
	return key % ((size_t)-1);
}

TranspositionTable::TranspositionTable() {
	trans_table = tt_internal_map_type(1000000, hash_function);
}

TransTableEntry* TranspositionTable::get_if_exists(unsigned long long zobrist_hash) {
	auto it = trans_table.find(zobrist_hash);
	if (it == trans_table.end()) {
		return &not_present;
	}
	else {
		return &(it->second);
	}
}

void TranspositionTable::store(unsigned long long zobrist_hash, TransTableEntry entry) {
	trans_table[zobrist_hash] = entry;
}

void TranspositionTable::clear() {
	trans_table.clear();
}