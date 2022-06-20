#include "stdafx.h"

#include "output.h"
#include "structs.h"

void output_enums(const data_t& data)
{
	std::cout << "#pragma once\n\n" << idl2cpp_comment();

	std::cout << "namespace " << data._namespace.back() << "\n{";

	for (const auto& pair : data._enum_map)
	{
		if (pair.second._level > 1)
			continue;

		bool first = true;

		if (!pair.second._help.empty())
		{
			std::cout << "\n// " << pair.second._help;
		}

		std::cout << "\nenum ";

		if (data._enum_class)
			std::cout << "class ";

		std::cout << pair.first;
		std::cout << "\n{\n";

		for (auto& p : pair.second._enums)
		{
			if (!first) std::cout << ",\n";

			std::cout << '\t' << p.second << " = " << p.first;
			first = false;
		}

		std::cout << "\n};\n";
	}

	std::cout << "}\n";
}
