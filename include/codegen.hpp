#pragma once

#include "ir.hpp"

#include <vector>

struct Bytecode {
	std::vector<Instruction> instructions;

	size_t EmitABC(OPCODE op, uint32_t A, uint32_t B, uint32_t C) {
		auto& I = instructions.emplace_back();
		I.op = op;
		I.A = A;
		I.B = B;
		I.C = C;
		return instructions.size() - 1;
	}
	size_t EmitABx(OPCODE op, uint32_t A, uint32_t Bx) {
		auto& I = instructions.emplace_back();
		I.op = op;
		I.A = A;
		I.Bx = Bx;
		return instructions.size() - 1;
	}
	size_t EmitAsBx(OPCODE op, uint32_t A, int32_t sBx) {
		auto& I = instructions.emplace_back();
		I.op = op;
		I.A = A;
		I.sBx = sBx;
		return instructions.size() - 1;
	}
	size_t EmitAx(OPCODE op, uint32_t Ax) {
		auto& I = instructions.emplace_back();
		I.op = op;
		I.Ax = Ax;
		return instructions.size() - 1;
	}
	size_t EmitsAx(OPCODE op, int32_t sAx) {
		auto& I = instructions.emplace_back();
		I.op = op;
		I.sAx = sAx;
		return instructions.size() - 1;
	}

	size_t Store(uint32_t A, uint32_t B) {
		return EmitABC(LOAD, A, B, 0);
	}

	size_t LoadInt(uint32_t sp, int32_t imm) {
		return EmitAsBx(ISTORE, sp, imm);
	}

	size_t AddInt(uint32_t A, uint32_t B, uint32_t C) {
		return EmitABC(IADD, A, B, C);
	}

	size_t SubInt(uint32_t A, uint32_t B, uint32_t C) {
		return EmitABC(ISUB, A, B, C);
	}

	size_t MulInt(uint32_t A, uint32_t B, uint32_t C) {
		return EmitABC(IMUL, A, B, C);
	}

	size_t DivInt(uint32_t A, uint32_t B, uint32_t C) {
		return EmitABC(IDIV, A, B, C);
	}

	size_t ModInt(uint32_t A, uint32_t B, uint32_t C) {
		return EmitABC(IMOD, A, B, C);
	}

	size_t xPushChar(uint32_t sp) {
		return EmitABC(xPUSHc, sp, 0, 0);
	}
	size_t xPushShort(uint32_t sp) {
		return EmitABC(xPUSHs, sp, 0, 0);
	}
	size_t xPushInt(uint32_t sp) {
		return EmitABC(xPUSHi, sp, 0, 0);
	}
	size_t xPushLong(uint32_t sp) {
		return EmitABC(xPUSHl, sp, 0, 0);
	}
	size_t xPushLongLong(uint32_t sp) {
		return EmitABC(xPUSHll, sp, 0, 0);
	}
	size_t xPushFloat(uint32_t sp) {
		return EmitABC(xPUSHf, sp, 0, 0);
	}
	size_t xPushDouble(uint32_t sp) {
		return EmitABC(xPUSHd, sp, 0, 0);
	}
	size_t xPushPointer(uint32_t sp) {
		return EmitABC(xPUSHp, sp, 0, 0);
	}
	size_t xCallVoid(uint32_t sp) {
		return EmitABC(xCALLv, sp, 0, 0);
	}
	size_t Ret() {
		return EmitABC(RET, 0, 0, 0);
	}
};