#include <vm.hpp>
#include "parser.h"
#include "lex.h"

struct Expression {
	Slot slot;
	Type type;
};

static bool check(LexState &ls, token_type type) {
	return ls.token.type == type;
}

static bool skip(LexState &ls, token_type type) {
	if (check(ls, type)) {
		ls.next();
		return true;
	}
	return false;
}

static void expect(LexState& ls, token_type type) {
	if (!check(ls, type)) {
		fmt::print("unexpected token {}\n", ls.token.string);
		abort();
	}
}

static void consume(LexState& ls, token_type type) {
	expect(ls, type);
	ls.next();
}

//static DeclRefExpr get_local(FuncState& fs, const std::string& name) {
//	for (auto& scope : fs.scopes) {
//		auto it = scope.variables.find(name);
//		if (it != scope.variables.end()) {
//			return DeclRefExpr{name, it->second};
//		}
//	}
//	fmt::print("{} is not defined\n", name);
//	abort();
//}

static void enter_scope(Environment& env, LexState& ls) {
	ls.fs->scopes.emplace_front();
}

static void leave_scope(Environment& env, LexState& ls) {
	ls.fs->scopes.pop_front();
}

static FuncState* enter_func(Environment& env, LexState& ls, FuncState& new_fs) {
	new_fs.top = ls.fs;
	ls.fs = &new_fs;
	enter_scope(env, ls);
	return new_fs.top;
}

extern void dump(FuncState& fs);

static void leave_func(Environment& env, LexState& ls) {
	ls.fs->ir.Ret();

	dump(*ls.fs);
	ls.fs = ls.fs->top;
}

Expression expression(Environment& env, LexState &ls);
void statement(Environment& env, LexState &ls);
void return_statement(Environment& env, LexState &ls);
void statement_list(Environment& env, LexState &ls);

std::string checkname(Environment& env, LexState &ls) {
	expect(ls, token_type::identifier);
	auto s = std::move(ls.token.string);
	ls.next();
	return std::move(s);
}

Expression primary_expression(Environment& env, LexState &ls) {
	if (skip(ls, token_type::left_paren)) {
		auto temp = expression(env, ls);
		consume(ls, token_type::right_paren);
		return temp;
	}
	if (skip(ls, token_type::kw_false)) {
		auto temp = ls.fs->allocate();
		ls.fs->ir.LoadInt(temp.location, 0);
		return {temp, BoolType{}};
	}
	if (skip(ls, token_type::kw_true)) {
		auto temp = ls.fs->allocate();
		ls.fs->ir.LoadInt(temp.location, 1);
		return {temp, BoolType{}};
	}
	if (skip(ls, token_type::integer_literal)) {
		auto temp = ls.fs->allocate();
		ls.fs->ir.LoadInt(temp.location, ls.prev_token.integer);
		return {temp, IntType{}};
	}
	if (skip(ls, token_type::float_literal)) {
//		return FloatLiteral{ls.prev_token.number};
		abort();
	}
	if (skip(ls, token_type::string_literal)) {
//		return StringLiteral{std::move(ls.prev_token.string)};
		abort();
	}
	auto name = checkname(env, ls);
	auto [slot, type] = ls.fs->get(name);
	auto temp = ls.fs->allocate();
	ls.fs->ir.Store(temp.location, slot.location);
	return {temp, type};
}

Expression unary_expression(Environment& env, LexState& ls) {
	return primary_expression(env, ls);

//	while (true) {
//		if (skip(ls, token_type::logical_not)) {
//			unary_expression(env, ls);
//		} else if (skip(ls, token_type::bitwise_not)) {
//			unary_expression(env, ls);
//		} else {
//			break;
//		}
//	}
}

Expression multiplicative_expression(Environment& env, LexState& ls) {
	auto ret = unary_expression(env, ls);
	while (true) {
		if (skip(ls, token_type::multiply)) {
			auto rhs = unary_expression(env, ls);
			ls.fs->deallocate(rhs.slot);

			ls.fs->ir.MulInt(ret.slot.location, ret.slot.location, rhs.slot.location);
		} else if (skip(ls, token_type::divide)) {
			auto rhs = unary_expression(env, ls);
			ls.fs->deallocate(rhs.slot);

			ls.fs->ir.DivInt(ret.slot.location, ret.slot.location, rhs.slot.location);
		} else if (skip(ls, token_type::modulo)) {
			auto rhs = unary_expression(env, ls);
			ls.fs->deallocate(rhs.slot);

			ls.fs->ir.ModInt(ret.slot.location, ret.slot.location, rhs.slot.location);
		} else {
			break;
		}
	}
	return ret;
}

Expression additive_expression(Environment& env, LexState& ls) {
	auto ret = multiplicative_expression(env, ls);
	while (true) {
		if (skip(ls, token_type::plus)) {
			auto rhs = multiplicative_expression(env, ls);
			ls.fs->deallocate(rhs.slot);

			ls.fs->ir.AddInt(ret.slot.location, ret.slot.location, rhs.slot.location);
		} else if (skip(ls, token_type::minus)) {
			auto rhs = multiplicative_expression(env, ls);
			ls.fs->deallocate(rhs.slot);

			ls.fs->ir.SubInt(ret.slot.location, ret.slot.location, rhs.slot.location);
		} else {
			break;
		}
	}
	return ret;
}

Expression shift_expression(Environment& env, LexState& ls) {
	return additive_expression(env, ls);
}

Expression comparison_expression(Environment& env, LexState& ls) {
	return shift_expression(env, ls);
//	while (true) {
//		if (skip(ls, token_type::eq)) {
//
//		} else if (skip(ls, token_type::ne)) {
//
//		} else if (skip(ls, token_type::lt)) {
//
//		} else if (skip(ls, token_type::le)) {
//
//		} else if (skip(ls, token_type::gt)) {
//
//		} else if (skip(ls, token_type::ge)) {
//
//		} else if (skip(ls, token_type::cmp)) {
//
//		} else {
//			break;
//		}
//	}
}

Expression bitwise_and_expression(Environment& env, LexState& ls) {
	return comparison_expression(env, ls);
}

Expression bitwise_xor_expression(Environment& env, LexState& ls) {
	return bitwise_and_expression(env, ls);
}

Expression bitwise_or_expression(Environment& env, LexState& ls) {
	return bitwise_xor_expression(env, ls);
}

Expression logical_and_expression(Environment& env, LexState& ls) {
	return bitwise_or_expression(env, ls);
}

Expression logical_or_expression(Environment& env, LexState& ls) {
	return logical_and_expression(env, ls);
}

Expression assignment_expression(Environment& env, LexState& ls) {
	return logical_or_expression(env, ls);
//	while (true) {
//		if (skip(ls, token_type::assign)) {
//		} else if (skip(ls, token_type::plus_assign)) {
//		} else if (skip(ls, token_type::minus_assign)) {
//		} else if (skip(ls, token_type::multiply_assign)) {
//		} else if (skip(ls, token_type::divide_assign)) {
//		} else if (skip(ls, token_type::modulo_assign)) {
//		} else {
//			break;
//		}
//	}
}

Expression expression(Environment& env, LexState &ls) {
	return assignment_expression(env, ls);
}

void while_statement(Environment& env, LexState &ls) {
	ls.next();
//
	enter_scope(env, ls);
	consume(ls, token_type::left_paren);
	auto condition = expression(env, ls);
	consume(ls, token_type::right_paren);

	auto loop = ls.fs->ir.instructions.size();

	ls.fs->ir.EmitABC(TEST, condition.slot.location, 0, 0);
	auto jmp_I = ls.fs->ir.EmitsAx(JE, 0);

	statement_list(env, ls);

	ls.fs->ir.EmitsAx(JMP, loop);

	auto& I = ls.fs->ir.instructions[jmp_I];
	I.sAx = ls.fs->ir.instructions.size();

	leave_scope(env, ls);
}

void loop_statement(Environment& env, LexState &ls) {
	ls.next();
//
	enter_scope(env, ls);

	auto loop = ls.fs->ir.instructions.size();

	statement_list(env, ls);

	ls.fs->ir.EmitsAx(JMP, loop);

	leave_scope(env, ls);
}

void if_statement(Environment& env, LexState &ls) {
	ls.next();

	enter_scope(env, ls);
	consume(ls, token_type::left_paren);
	auto condition = expression(env, ls);
	consume(ls, token_type::right_paren);

	ls.fs->ir.EmitABC(TEST, condition.slot.location, 0, 0);
	auto jmp_I = ls.fs->ir.EmitsAx(JNE, 0);

	statement_list(env, ls);

	auto& I = ls.fs->ir.instructions[jmp_I];
	I.sAx = ls.fs->ir.instructions.size();

	if (skip(ls, token_type::kw_else)) {
		statement_list(env, ls);
	}

	leave_scope(env, ls);
}

void let_statement(Environment& env, LexState &ls) {
	ls.next();

	auto name = checkname(env, ls);

	consume(ls, token_type::assign);
	auto temp = expression(env, ls);

	ls.fs->declare(name, temp.slot, temp.type);
}

Type parsetype(Environment& env, LexState &ls) {
	consume(ls, token_type::identifier);
	return IntType{};
}

void fn_statement(Environment& env, LexState &ls) {
	ls.next();

	auto name = checkname(env, ls);
	auto type = new FunctionType{};

	FuncState new_fs{};
	new_fs.name = name;
	auto fs = enter_func(env, ls, new_fs);

	consume(ls, token_type::left_paren);
	if (!check(ls, token_type::right_paren)) {
		do {
			auto arg_name = checkname(env, ls);
			consume(ls, token_type::colon);
			auto arg_type = parsetype(env, ls);

			new_fs.declare(arg_name, new_fs.allocate(), arg_type);
			new_fs.args.emplace_back(arg_name);

			type->args.emplace_back(std::move(arg_type));
		} while (skip(ls, token_type::comma));
	}
	consume(ls, token_type::right_paren);

	if (skip(ls, token_type::colon)) {
		type->return_type = parsetype(env, ls);
	} else {
		type->return_type = VoidType{};
	}
//	auto alloc = fs->allocate();
//	fs->declare(name, alloc, type);
	statement_list(env, ls);
	leave_func(env, ls);
}

void return_statement(Environment& env, LexState &ls) {
	ls.next();
	expression(env, ls);
}

void expression_statement(Environment& env, LexState &ls) {
	expression(env, ls);
}

void block_statement(Environment& env, LexState &ls) {
	enter_scope(env, ls);
	ls.next();
	while (ls.token.type != token_type::right_curve) {
		statement(env, ls);
	}
	consume(ls, token_type::right_curve);
	leave_scope(env, ls);
}

void statement(Environment& env, LexState &ls) {
	switch (ls.token.type) {
	case token_type::left_curve:
		return block_statement(env, ls);
	case token_type::kw_while:
		return while_statement(env, ls);
	case token_type::kw_loop:
		return loop_statement(env, ls);
	case token_type::kw_if:
		return if_statement(env, ls);
	case token_type::kw_fn:
		return fn_statement(env, ls);
	case token_type::kw_let:
		return let_statement(env, ls);
	case token_type::kw_return:
		return return_statement(env, ls);
	default:
		return expression_statement(env, ls);
	}

	/*if (skip(ls, token_type::FOR)) {
		expect(ls, token_type::NAME);
		auto name = ls.tk.string;
		ls.next();

		if (skip(ls, token_type::ASSIGN)) {
			auto stat = new ForRange;
			stat->name = name;

			auto status = exp(env, ls);
			parser_check_ok(status);
			stat->from = parser.exp();

			consume(ls, token_type::COMMA);
			status = exp(env, ls);
			parser_check_ok(status);
			stat->to = parser.exp();

			if (skip(ls, token_type::COMMA)) {
				status = exp(env, ls);
				parser_check_ok(status);
				stat->step = parser.exp();
			}
			consume(ls, token_type::DO);
			block(env, ls);
			consume(ls, token_type::END);

			stat->block = parser.block();
			parser.push(stat);
		} else {
			auto namelist = new NameList;
			namelist->add(name);
			while (skip(ls, token_type::COMMA)) {
				expect(ls, token_type::NAME);
				namelist->add(ls.tk.string);
				ls.next();
			}
			consume(ls, token_type::IN);
			auto status = explist(env, ls);
			parser_check_ok(status);

			auto stat = new ForIn;
			stat->namelist = namelist;
			stat->explist = parser.explist();
			consume(ls, token_type::DO);
			block(env, ls);
			consume(ls, token_type::END);
			stat->block = parser.block();
			parser.push(stat);
		}
		return state_t::ok;
	}*/
}

void statement_list(Environment& env, LexState &ls) {
	consume(ls, token_type::left_curve);

	while (ls.token.type != token_type::right_curve) {
		statement(env, ls);
	}
	consume(ls, token_type::right_curve);
}


void parse(Environment& env, std::string_view src) {
	LexState ls{src};
	ls.next();

	FuncState fs{};
	fs.name = "main";

	enter_func(env, ls, fs);
	
	while (ls.token.type != token_type::end_of_source) {
		statement(env, ls);
	}
	expect(ls, token_type::end_of_source);
	leave_func(env, ls);

	VM vm;
	vm.call(fs);
}