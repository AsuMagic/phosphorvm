#include "vm.hpp"

#include <fmt/core.h>
#include <fmt/color.h>
#include <utility>
#include <type_traits>
#include "vmtraits.hpp"

#define BINOP_ARITH(name, op) case Instr::name : binop_arithmetic([&](auto a, auto b) { return a op b; }); break;

void fail_impossible()
{
	if constexpr (debug_mode)
	{
		throw std::logic_error{"VM state should never be reached"};
	}

	__builtin_unreachable();
}

void VM::execute(const Script& script)
{
	const Block* block = script.data.data();
	const Block* end_block = block + script.data.size();

	auto decode_type_pair = [&] {
		return std::pair{
			DataType((*block >> 16) & 0xF),
			DataType((*block >> 12) & 0xF)
		};
	};

	auto decode_opcode = [&] {
		return *block >> 24;
	};

	while (block != end_block)
	{
		auto opcode = decode_opcode();
		auto [t1, t2] = decode_type_pair();

		fmt::print(
			"Execution trace: at ${:08x} opcode ${:02x}. stack data: {:02x}\n",
			std::distance(script.data.data(), block),
			opcode,
			fmt::join(std::vector(&stack.raw[0], &stack.raw[stack.offset]), " ")
		);

		// TODO: implement dispatcher to pop variables while taking care of
		// properly reading out variables. Should split cases:
		// {var, notvar}, {notvar, var}, {var, var}, {notvar, notvar}.

		auto binop = [&, t1=t1, t2=t2](auto handler) {
			hell([&](auto a, auto b) {
				handler(stack.pop<decltype(a)>(), stack.pop<decltype(b)>());
			}, std::array{t1, t2});
		};

		auto binop_arithmetic = [&](auto handler) {
			binop([&](auto a, auto b) {
				if constexpr (is_arith_like<decltype(a), decltype(b)>())
				{
					stack.push(handler(a, b));
				}
			});
		};

		switch (Instr(opcode))
		{
		// TODO: check if multiplying strings with int is actually possible
		BINOP_ARITH(opmul, *)
		BINOP_ARITH(opdiv, /)
		// case Instr::oprem: // TODO
		// case Instr::opmod: // TODO
		BINOP_ARITH(opadd, +) // TODO: you can add strings together, too
		BINOP_ARITH(opsub, -)
		//BINOP_ARITH(opand, &)
		//BINOP_ARITH(opor,  |)
		//BINOP_ARITH(opxor, ^)
		// case Instr::opneg: // TODO
		// case Instr::opnot: // TODO
		// case Instr::opshl: // TODO
		// case Instr::opshr: // TODO
		// case Instr::opcmp: // TODO
		// case Instr::oppop: // TODO
		// case Instr::oppushi16: // TODO
		// case Instr::opdup: // TODO
		// case Instr::opret: // TODO
		// case Instr::opexit: // TODO
		// case Instr::oppopz: // TODO
		// case Instr::opb: // TODO
		// case Instr::opbt: // TODO
		// case Instr::opbf: // TODO
		// case Instr::oppushenv: // TODO
		// case Instr::oppopenv: // TODO

		//case Instr::oppushcst: break;

		//case Instr::oppushloc:
		//case Instr::oppushglb:

		case Instr::oppushspc:
			push_special(SpecialVar(*(++block) & 0x00FFFFFF));
			break;

		case Instr::oppushi16:
			stack.push<s16>(*block & 0xFFFF);
			break;

		// case Instr::opcall: // TODO
		// case Instr::opbreak: // TODO

		default:
			fmt::print(fmt::color::red, "Unhandled op ${:02x}\n", opcode);
			return;
		}

		++block;
	}
}

void VM::push_special(SpecialVar var)
{
	// argumentn
	if (unsigned(var) != 0 && unsigned(var) <= 17)
	{
		stack.push_raw(
			&stack.raw[frames.top().stack_offset - Variable::stack_variable_size * unsigned(var)],
			Variable::stack_variable_size
		);

		return;
	}
}

VarType VM::pop_variable_var_type(InstType inst_type)
{
	switch (inst_type)
	{
	case InstType::stack_top_or_global:
		return stack.pop<VarType>();

	default:
		throw std::runtime_error{"Unhandled variable type"};
	}
}
