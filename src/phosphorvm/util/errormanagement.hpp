#pragma once

#include <stdexcept>
#include "phosphorvm/config.hpp"

//! Call to designate a normally unreachable spot.
//! With debug_mode set, throws an exception. Otherwise, uses
//! __builtin_unreachble (effectively becoming an optimization).
[[noreturn]]
inline void maybe_unreachable(
	const char* error = "VM state should never be reached"
) {
	if constexpr (check(debug::vm_safer))
	{
		throw std::runtime_error{error};
	}

	__builtin_unreachable();
}