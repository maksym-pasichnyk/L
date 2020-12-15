#pragma once

#include "func.hpp"

#include <string>
#include <vector>
#include <cstdio>
#include <unordered_map>
#include <list>
#include <stack>
#include <set>

#include "codegen.hpp"

enum token_type : uint8_t {
	end_of_source,

	identifier,
	integer_literal,
	float_literal,
	string_literal,

	kw_break,
	kw_do,
	kw_else,
	kw_false,
	kw_for,
	kw_fn,
	kw_if,
	kw_in,
	kw_let,
	kw_loop,
	kw_return,
	kw_true,
	kw_until,
	kw_while,
	kw_struct,
	kw_type,
	kw_extern,
	kw_import,
	kw_export,
	kw_match,

	plus,
	minus,
	multiply,
	divide,
	modulo,
	plusplus,
	minusminus,

	logical_and,
	logical_or,
	logical_not,

	bitwise_and,
	bitwise_or,
	bitwise_not,
	bitwise_xor,
	LEN,
	eq,
	ne,
	le,
	ge,
	lt,
	gt,
	cmp,
	assign,
	plus_assign,
	minus_assign,
	multiply_assign,
	divide_assign,
	modulo_assign,
	left_paren,
	right_paren,
	left_curve,
	right_curve,
	left_brace,
	right_brace,
	semicolon,
	colon,
	comma,
	dot,
	concat,
	args
};

struct Token {
	token_type type;
	int32_t integer;
	double number;
	std::string string;
	std::string_view raw_string;

	static auto to_string(token_type type) {
		switch (type) {
		case end_of_source:
			return "[end]";
		case identifier:
			return "[identifier]";
		case integer_literal:
			return "[integer_literal]";
		case float_literal:
			return "[float_literal]";
		case string_literal:
			return "[string_literal]";
		case kw_break:
			return "break";
		case kw_do:
			return "do";
		case kw_else:
			return "else";
		case kw_false:
			return "false";
		case kw_for:
			return "for";
		case kw_fn:
			return "fn";
		case kw_if:
			return "if";
		case kw_in:
			return "in";
		case kw_let:
			return "let";
		case kw_return:
			return "return";
		case kw_true:
			return "true";
		case kw_until:
			return "until";
		case kw_while:
			return "while";
		case kw_loop:
			return "loop";
		case plus:
			return "+";
		case minus:
			return "-";
		case multiply:
			return "*";
		case divide:
			return "/";
		case modulo:
			return "%";
		case plusplus:
			return "++";
		case minusminus:
			return "--";
		case logical_and:
			return "&&";
		case logical_or:
			return "||";
		case logical_not:
			return "!";
		case bitwise_and:
			return "&";
		case bitwise_or:
			return "|";
		case bitwise_not:
			return "~";
		case bitwise_xor:
			return "^";
		case LEN:
			return "#";
		case eq:
			return "==";
		case ne:
			return "!=";
		case le:
			return "<=";
		case ge:
			return ">=";
		case lt:
			return "<";
		case gt:
			return ">";
		case cmp:
			return "<=>";
		case assign:
			return "=";
		case plus_assign:
			return "+=";
		case minus_assign:
			return "-=";
		case multiply_assign:
			return "*=";
		case divide_assign:
			return "/=";
		case modulo_assign:
			return "%=";
		case left_paren:
			return "(";
		case right_paren:
			return ")";
		case left_curve:
			return "{";
		case right_curve:
			return "}";
		case left_brace:
			return "[";
		case right_brace:
			return "]";
		case semicolon:
			return ";";
		case colon:
			return ":";
		case comma:
			return ",";
		case dot:
			return ".";
		case concat:
			return "..";
		case args:
			return "...";
		case kw_struct:
			return "struct";
		case kw_type:
			return "type";
		case kw_extern:
			return "extern";
		case kw_import:
			return "import";
		case kw_export:
			return "export";
		case kw_match:
			return "match";
		}
		return "[token]";
	}
};

struct LexState {
	FuncState* fs = nullptr;

	Token prev_token;
	Token token;

	std::string_view src;
	size_t line_number = 0;
	size_t char_index = 0;

	LexState(std::string_view src) : src(src) {}

	template <typename Fn>
	std::string_view take_while(Fn&& fn) {
		auto start_index = char_index;
		while (!eof() && fn(peek())) {
			advance();
		}

		return src.substr(start_index, char_index - start_index);
	}

	token_type read_ident();
	token_type read_string();
	token_type read_number();

	char peek() const {
		return src[char_index];
	}

	void advance() {
		char_index++;
	}

	bool eof() const {
		return char_index >= src.size();
	}

	void reset() {
		char_index = 0;
	}

	token_type read_token();

	void next() {
		prev_token = std::move(token);
		token.type = read_token();
	}
};