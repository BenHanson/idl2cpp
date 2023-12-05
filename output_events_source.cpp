#include "stdafx.h"

#include "events_template.cpp"
#include "output.h"
#include <regex>
#include <sstream>
#include "structs.h"

void output_events_source(const data_t& data)
{
	const std::regex endl_rx("\r\n");
	const std::regex name_rx(R"(\$Name\$)");
	const std::regex switch_rx(R"(\$Switch\$)");

	std::cout << idl2cpp_comment() <<
		"namespace " << data._namespace.back() << "\n{";

	for (auto& [iface, functions] : data._interfaces)
	{
		if (iface._level > 1)
			continue;

		if (data._events.contains(std::make_pair(iface._namespace, iface._name)) &&
			!functions.empty())
		{
			std::string source(std::begin(g_events_template),
				std::end(g_events_template));
			std::ostringstream ss;

			source = std::regex_replace(source, endl_rx, "\n");
			source = std::regex_replace(source, name_rx, iface._name);

			for (const auto& f : functions)
			{
				std::size_t idx = 0;

				ss << "\tcase 0x" << std::hex << f._id << std::dec <<
					": // " << f._ret_cpp_type << std::string(f._ret_stars, '*') <<
					' ' << f._name << '(';

				for (const auto& p : f._params)
				{
					if (idx > 0)
						ss << ", ";

					ss << p._com_type << std::string(p._cpp_stars, '*') << ' ' << p._name;

					++idx;
				}

				ss << ')' << '\n' << "\t\tif (pDispParams->cArgs != " <<
					f._params.size() << ")\n\t\t"
					"{\n\t\t\treturn E_INVALIDARG;\n\t\t}\n"
					"\t\telse\n\t\t{\n"
					"\t\t\t// Params are stored in reverse order. e.g:\n"
					"\t\t\t// VARIANT_BOOL* pCancel = "
					"pDispParams->rgvarg[0].pboolVal;\n\t\t}\n\n"
					"\t\tbreak;\n";
			}

			ss << '\t';
			source = std::regex_replace(source, switch_rx, ss.str());
			std::cout << source;
		}
	}

	std::cout << "}\n";
}
