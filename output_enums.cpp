#include "stdafx.h"

#include "output.h"
#include "structs.h"

void output_enums(const data_t& data)
{
	std::cout << "#pragma once\n\n" << idl2cpp_comment();

	std::cout << "namespace " << data._namespace.back() << "\n{";

	for (const auto& [name, enum_data] : data._enum_map)
	{
		if (enum_data._level > 1)
			continue;

		if (!data._filter.empty() && name != data._filter)
			continue;

		bool first = true;

		if (!enum_data._help.empty())
		{
			std::cout << "\n// " << enum_data._help;
		}

		std::cout << "\nenum ";

		if (data._enum_class)
			std::cout << "class ";

		std::cout << name;
		std::cout << "\n{\n";

		for (const auto& [value, id] : enum_data._enums)
		{
			if (!first) std::cout << ",\n";

			std::cout << '\t' << id << " = ";

			if (value.starts_with("0x") && value.size() == 10 &&
				value[2] != '0')
			{
				std::cout << "static_cast<int>(" << value << ')';
			}
			else
				std::cout << value;

			first = false;
		}

		std::cout << "\n};\n";
	}

	std::cout << "}\n";
}
