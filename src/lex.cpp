#include "lex.h"

#include <fstream>
#include <map>

static const std::map<std::string_view, token_type> keywords {
	{"break", token_type::kw_break},
	{"do", token_type::kw_do},
	{"else", token_type::kw_else},
	{"false", token_type::kw_false},
	{"for", token_type::kw_for},
	{"fn", token_type::kw_fn},
	{"if", token_type::kw_if},
	{"in", token_type::kw_in},
	{"let", token_type::kw_let},
	{"loop", token_type::kw_loop},
	{"return", token_type::kw_return},
	{"true", token_type::kw_true},
	{"until", token_type::kw_until},
	{"while", token_type::kw_while},
	{"struct", token_type::kw_struct},
	{"type", token_type::kw_type},
	{"extern", token_type::kw_extern},
	{"import", token_type::kw_import},
	{"export", token_type::kw_export},
	{"match", token_type::kw_match}
};

token_type LexState::read_ident() {
	token.raw_string = take_while([](char c) {
		return isalnum(c) || (c == '_');
	});
	token.string = token.raw_string;

	auto keyword = keywords.find(token.raw_string);
	return keyword != keywords.end()
		? keyword->second
		: token_type::identifier;
}

token_type LexState::read_number() {
	int32_t number = 0;
	token.raw_string = take_while([&number](char c) -> bool {
		if (!isdigit(c)) return false;
		number = number * 10 + (c - '0');
		return true;
	});
	token.integer = number;
	token.string = token.raw_string;
	return token_type::integer_literal;
}

token_type LexState::read_string() {
	advance();

	std::string buf;

	token.raw_string = take_while([this, &buf](char c) -> bool {
		switch (c) {
		case '"':
		case '\n':
			return false;
		case '\\':
			advance();
			switch (peek()) {
			case 'n': c = '\n'; break;
			case 'r': c = '\r'; break;
			case 't': c = '\t'; break;
			case 'v': c = '\v'; break;
			default:
				fprintf(stderr, "invalid char '%c'\n", peek());
				abort();
			}
			[[fallthrough]];
		default:
			buf.push_back(c);
			return true;
		}
	});

	token.string = std::move(buf);

	if (peek() != '"') {
		fprintf(stderr, "unterminated string\n");
		abort();
	}
	advance();

	return token_type::string_literal;
}

token_type LexState::read_token() {
	while (!eof()) {
		switch (peek()) {
		case '\n': case '\r':
		case ' ': case '\t':
		case '\x0C':
			advance();
			continue;
		case 'a'...'z':
		case 'A'...'Z':
			return read_ident();
		case '0'...'9':
			return read_number();
		case '"':
			return read_string();
		case '+':
			advance();
			if (peek() == '+') {
				advance();
				return token_type::plusplus;
			}
			if (peek() == '=') {
				advance();
				return token_type::plus_assign;
			}
			return token_type::plus;
		case '-':
			advance();
			if (peek() == '-') {
				advance();
				return token_type::minusminus;
			}
			if (peek() == '=') {
				advance();
				return token_type::minus_assign;
			}
			return token_type::minus;
		case '*':
			advance();
			if (peek() == '=') {
				advance();
				return token_type::multiply_assign;
			}
			return token_type::multiply;
		case '/':
			advance();
			if (peek() == '=') {
				advance();
				return token_type::divide_assign;
			}
			return token_type::divide;
		case '%':
			advance();
			if (peek() == '=') {
				advance();
				return token_type::modulo_assign;
			}
			return token_type::modulo;
		case '&':
			advance();
			if (peek() == '&') {
				return token_type::logical_and;
			}
			return token_type::bitwise_and;
		case '|':
			advance();
			if (peek() == '|') {
				return token_type::logical_or;
			}
			return token_type::bitwise_or;
		case '!':
			advance();
			if (peek() == '=') {
				return token_type::ne;
			}
			return token_type::logical_not;
		case '~':
			advance();
			return token_type::bitwise_not;
		case '^':
			advance();
			return token_type::bitwise_xor;
		case '[':
			advance();
			return token_type::left_brace;
		case ']':
			advance();
			return token_type::right_brace;
		case '(':
			advance();
			return token_type::left_paren;
		case ')':
			advance();
			return token_type::right_paren;
		case '{':
			advance();
			return token_type::left_curve;
		case '}':
			advance();
			return token_type::right_curve;
		case '<':
			advance();
			if (peek() == '=') {
				advance();
				return token_type::le;
			}
			return token_type::lt;
		case '>':
			advance();
			if (peek() == '=') {
				advance();
				return token_type::ge;
			}
			return token_type::gt;
		case '=':
			advance();
			if (peek() == '=') {
				advance();
				return token_type::eq;
			}
			return token_type::assign;
		case ':':
			advance();
			return token_type::colon;
		case ';':
			advance();
			return token_type::semicolon;
		case '.':
			advance();
			if (peek() == '.') {
				advance();
				if (peek() == '.') {
					advance();
					return token_type::args;
				}
				return token_type::concat;
			}
			return token_type::dot;
		case ',':
			advance();
			return token_type::comma;
		case '#':
			advance();
			return token_type::LEN;
		default:
			fprintf(stderr, "invalid char '%c'\n", peek());
			abort();
		}
	}

	return token_type::end_of_source;
}