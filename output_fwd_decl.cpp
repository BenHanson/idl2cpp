#include "stdafx.h"

#include <iostream>
#include "output.h"
#include "output_fwd_decl.h"
#include "structs.h"

void output_fwd_decl(data_t& data)
{
	std::cout << "#pragma once\n\n" << idl2cpp_comment() << '\n' <<
		"namespace " << data._namespace.back() << "\n{\n";

	for (auto& inf : data._interfaces)
	{
		if (inf.first._level == 1)
			std::cout << '\t' << "struct " << inf.first._name << ";\n";
	}

	std::cout << "}\n";
}
