#pragma once

#include "codegen.hpp"
#include "type.hpp"

#include <set>
#include <string>
#include <list>
#include <stack>
#include <unordered_map>

#include <fmt/format.h>

struct Slot {
	int32_t location;
};

struct Variable {
	Slot slot;
	Type type;
};

struct Scope {
	std::unordered_map<std::string, Variable> variables;

	void declare(std::string name, Slot slot, Type type) {
		if (variables.contains(name)) {
			fmt::print("redefinition of '{}'", name);
		}
		variables.insert_or_assign(std::move(name), Variable{slot, std::move(type)});
	}
};

struct FuncState {
	FuncState* top{nullptr};
	Bytecode ir;

	std::string name;
	std::vector<std::string> args;

	std::list<Scope> scopes;
	std::stack<int> temp;
	int stack_size = 0;

	Slot allocate() {
		if (temp.empty()) {
			return { stack_size++ };
		}
		int index = temp.top();
		temp.pop();
		return { index };
	}
	void deallocate(Slot slot) {
		temp.push(slot.location);
	}

	void declare(std::string name, Slot slot, Type type) {
		scopes.front().declare(std::move(name), slot, std::move(type));
	}

	Variable get(const std::string& name) {
		for (auto& scope : scopes) {
			auto it = scope.variables.find(name);
			if (it != scope.variables.end()) {
				return it->second;
			}
		}
		fmt::print("'{}' is not defined\n", name);
		abort();
	}
};

struct Environment {
};