#include <iostream>
#include <stack>
#include <lex.h>

#include "vm.hpp"
#include "parser.h"

#include <fmt/format.h>

void test(int i) {
	std::cout << "Hello: " << i << std::endl;
}

void dump(FuncState& fs) {
	size_t pc = 0;

	fmt::print("function {}(", fs.name);
	if (!fs.args.empty()) {
		fmt::print("%0", 0);
		for (int i = 1; i < fs.args.size(); i++) {
			fmt::print(", %{}", i);
		}
	}
	fmt::print(")\n");
	fmt::print(".stack_size {}\n", fs.stack_size);

	for (auto const& I : fs.ir.instructions) {
		switch ((OPCODE) I.op) {
		case NOP:
			fmt::print("{}: nop\n", pc, I.A, I.Bx);
			break;
		case LOAD:
			fmt::print("{}: load   %{} %{}\n", pc, I.A, I.B);
			break;
		case ISTORE:
			fmt::print("{}: istore %{} {}\n", pc, I.A, I.Bx);
			break;
		case IADD:
			fmt::print("{}: iadd   %{} %{} %{}\n", pc, I.A, I.B, I.C);
			break;
		case ISUB:
			fmt::print("{}: isub   %{} %{} %{}\n", pc, I.A, I.B, I.C);
			break;
		case IMUL:
			fmt::print("{}: imul   %{} %{} %{}\n", pc, I.A, I.B, I.C);
			break;
		case IDIV:
			fmt::print("{}: idiv   %{} %{} %{}\n", pc, I.A, I.B, I.C);
			break;
		case IMOD:
			fmt::print("{}: imod   %{} %{} %{}\n", pc, I.A, I.B, I.C);
			break;
		case ICMP:
			fmt::print("{}: icmp   %{} {}\n", pc, I.A, I.B);
			break;
		case TEST:
			fmt::print("{}: test   %{}\n", pc, I.A);
			break;
		case JMP:
			fmt::print("{}: jmp    ${}\n", pc, I.sAx);
			break;
		case JE:
			fmt::print("{}: je     ${}\n", pc, I.sAx);
			break;
		case JNE:
			fmt::print("{}: jne    ${}\n", pc, I.sAx);
			break;
		case JLT:
			fmt::print("{}: jlt    ${}\n", pc, I.sAx);
			break;
		case JLE:
			fmt::print("{}: jle    ${}\n", pc, I.sAx);
			break;
		case JGT:
			fmt::print("{}: jgt    ${}\n", pc, I.sAx);
			break;
		case JGE:
			fmt::print("{}: jge    ${}\n", pc, I.sAx);
			break;
		case CALL:
			fmt::print("{}: call: unimplemented\n", pc);
			break;
		case xCALLv:
			fmt::print("{}: xcallv: unimplemented\n", pc);
			break;
		case xPUSHb:
			fmt::print("{}: xpushb: unimplemented\n", pc);
			break;
		case xPUSHc:
			fmt::print("{}: xpushc: unimplemented\n", pc);
			break;
		case xPUSHs:
			fmt::print("{}: xpushs: unimplemented\n", pc);
			break;
		case xPUSHi:
			fmt::print("{}: xpushi: unimplemented\n", pc);
			break;
		case xPUSHl:
			fmt::print("{}: xpushl: unimplemented\n", pc);
			break;
		case xPUSHll:
			fmt::print("{}: xpushll: unimplemented\n", pc);
			break;
		case xPUSHf:
			fmt::print("{}: xpushf: unimplemented\n", pc);
			break;
		case xPUSHd:
			fmt::print("{}: xpushd: unimplemented\n", pc);
			break;
		case xPUSHp:
			fmt::print("{}: xpushp: unimplemented\n", pc);
			break;
		case RET:
			fmt::print("{}: ret: unimplemented\n", pc);
			break;
		}
		pc++;
	}
	fmt::print("\n");
}

int main() {
	Environment env {};
	parse(env, R"(
		let a = 10
		let b = 11
		let c = (a + b) * (a - b * (b - a) / (a + b) + a + b)

		fn test(a: int, b: int) {
			let c = (a + b) * (a - b * (b - a) / (a + b) + a + b)
		}

		if (true) {
			let c = (a + b) * (a - b * (b - a) / (a + b) + a + b)
		}

		while (false) {
			let c = (a + b) * (a - b * (b - a) / (a + b) + a + b)
		}
	)");

	return 0;
}