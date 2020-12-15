#pragma once

#include "ir.hpp"

#include <dyncall.h>
#include <cstdint>
#include <span>

#include "func.hpp"

union Value {
	int64_t i64;
	uint64_t u64;
	double f64;
	void* p;
};

struct VM {
	DCCallVM* dc;
	Value stack[1000];
	Value* sp = std::end(stack);

	size_t ip = 0;
	std::partial_ordering cmp_flag = std::partial_ordering::unordered;

	VM();
	~VM();

	void call(FuncState& fs);
};