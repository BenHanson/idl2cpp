#include "stdafx.h"

#include <iostream>
#include "output.h"
#include "output_fwd_decl.h"
#include "structs.h"

void output_fwd_decl(const data_t& data)
{
	std::cout << "#pragma once\n\n" << idl2cpp_comment() <<
		"namespace " << data._namespace.back() << "\n{\n";

	for (auto& [iface, functions] : data._interfaces)
	{
		if (iface._level == 1)
			std::cout << '\t' << "struct " << iface._name << ";\n";
	}

	std::cout << "}\n";
}
