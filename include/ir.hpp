#pragma once

#include <cstdint>

struct Instruction {
	uint8_t op: 8;

	union {
		std::int32_t sAx: 24;
		std::uint32_t Ax: 24;
		struct {
			std::int32_t sA: 8;
			std::uint32_t A: 8;
			union {
				std::int32_t sBx: 16;
				std::uint32_t Bx: 16;
				struct {
					union {
						std::int32_t sB: 8;
						std::uint32_t B: 8;
					};
					union {
						std::int32_t sC: 8;
						std::uint32_t C: 8;
					};
				};
			};
		};
	};
};

enum OPCODE : uint8_t {
	NOP,
	LOAD,
	ISTORE,
	IADD,
	ISUB,
	IMUL,
	IDIV,
	IMOD,
	ICMP,
	TEST,
	JMP,
	JE,
	JNE,
	JLT,
	JLE,
	JGT,
	JGE,
	CALL,
	xCALLv,
	xPUSHb,
	xPUSHc,
	xPUSHs,
	xPUSHi,
	xPUSHl,
	xPUSHll,
	xPUSHf,
	xPUSHd,
	xPUSHp,
	RET
};