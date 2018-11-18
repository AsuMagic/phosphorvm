#pragma once

#include "contextstack.hpp"
#include "framestack.hpp"
#include "mainstack.hpp"
#include "variablereference.hpp"
#include "../config.hpp"
#include "../unpack/chunk/form.hpp"
#include "../util/compilersupport.hpp"
#include "../util/errormanagement.hpp"
#include "../bytecode/types.hpp"

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class VM
{
	const Form& form;

	MainStack stack;
	FrameStack frames;
	ContextStack contexts;

	std::size_t argument_offset(ArgId arg_id = 0) const;
	std::size_t local_offset(VarId var_id = 0) const;

public:
	VM(const Form& p_form);

	//! Calls a function 'f' with parameter types corresponding to the given
	//! 'types'. e.g. dispatcher(f, std::array{DataType::f32, DataType::f64})
	//! will call f(0.0f, 0.0);
	// TODO: make this usable to implement builtins sanely
	template<std::size_t Left, class F, class... Ts>
	void dispatcher(F f, std::array<DataType, Left> types);

	void read_special(SpecialVar var);

	template<class T>
	void push_stack_variable(const T& value);

	template<class T>
	VariableReference<T> read_variable_reference(
		InstType inst_type = InstType::stack_top_or_global,
		VarId var_id = 0
	);

	template<class T>
	auto value(T& value);

	void print_stack_frame();

	void execute(const Script& script);
};

#include "vminline.hpp"
