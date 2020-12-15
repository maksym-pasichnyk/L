#pragma once

#include <vector>
#include <variant>

struct Type;
struct VoidType {};
struct BoolType {};
struct IntType {};
struct FloatType {};
struct StringType {};
struct FunctionType;
struct StructType {
	std::vector<Type> fields;
};

struct Type : std::variant<
	VoidType,
	BoolType,
	IntType,
	FloatType,
	StringType,
	FunctionType*,
	StructType
> {
	using variant::variant;
	using variant::operator=;
};

struct FunctionType {
	Type return_type;
	std::vector<Type> args;
};