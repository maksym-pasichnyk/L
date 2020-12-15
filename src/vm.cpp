#include "vm.hpp"

VM::VM() {
	dc = dcNewCallVM(4096);
}

VM::~VM() {
	dcFree(dc);
}

void VM::call(FuncState &fs) {
	std::span instructions = fs.ir.instructions;
	sp -= fs.stack_size;
	ip = 0;

	while (true) {
		auto I = instructions[ip++];
		switch (I.op) {
		case NOP:
			break;
		case LOAD:
			sp[I.A] = sp[I.B];
			break;
		case ISTORE:
			sp[I.A].i64 = I.Bx;
			break;
		case IADD:
			sp[I.A].i64 = sp[I.B].i64 + sp[I.C].i64;
			break;
		case ISUB:
			sp[I.A].i64 = sp[I.B].i64 - sp[I.C].i64;
			break;
		case IMUL:
			sp[I.A].i64 = sp[I.B].i64 * sp[I.C].i64;
			break;
		case IDIV:
			sp[I.A].i64 = sp[I.B].i64 / sp[I.C].i64;
			break;
		case IMOD:
			sp[I.A].i64 = sp[I.B].i64 % sp[I.C].i64;
			break;
		case ICMP:
			cmp_flag = sp[I.A].i64 <=> sp[I.B].i64;
			break;
		case TEST:
			cmp_flag = sp[I.A].i64 <=> 0;
			break;
		case JMP:
			ip = I.sAx;
			break;
		case JE:
			ip = std::is_eq(cmp_flag) ? I.sAx : ip;
			break;
		case JNE:
			ip = std::is_neq(cmp_flag) ? I.sAx : ip;
			break;
		case JLT:
			ip = std::is_lt(cmp_flag) ? I.sAx : ip;
			break;
		case JLE:
			ip = std::is_lteq(cmp_flag) ? I.sAx : ip;
			break;
		case JGT:
			ip = std::is_gt(cmp_flag) ? I.sAx : ip;
			break;
		case JGE:
			ip = std::is_gteq(cmp_flag) ? I.sAx : ip;
			break;
		case CALL:
			break;
		case xCALLv:
			dcCallVoid(dc, (DCpointer) sp[I.A].p);
			dcReset(dc);
			break;
		case xPUSHb:
			dcArgBool(dc, (bool) sp[I.A].i64);
			break;
		case xPUSHc:
			dcArgChar(dc, sp[I.A].i64);
			break;
		case xPUSHs:
			dcArgShort(dc, sp[I.A].i64);
			break;
		case xPUSHi:
			dcArgInt(dc, sp[I.A].i64);
			break;
		case xPUSHl:
			dcArgLong(dc, sp[I.A].i64);
			break;
		case xPUSHll:
			dcArgLongLong(dc, sp[I.A].i64);
			break;
		case xPUSHf:
			dcArgFloat(dc, sp[I.A].f64);
			break;
		case xPUSHd:
			dcArgDouble(dc, sp[I.A].f64);
			break;
		case xPUSHp:
			dcArgPointer(dc, (DCpointer) sp[I.A].p);
			break;
		case RET:
			return;
		default:
			fmt::print("illegal instruction '{}", OPCODE(I.op));
			exit(1);
		}
	}

	sp += fs.stack_size;
}
