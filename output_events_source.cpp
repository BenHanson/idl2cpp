#include "stdafx.h"

#include "events_template.cpp"
#include "output.h"
#include <regex>
#include <sstream>
#include "structs.h"

void output_events_source(data_t& data)
{
	const std::regex endl_rx("\r\n");
	const std::regex name_rx(R"(\$Name\$)");
	const std::regex switch_rx(R"(\$Switch\$)");

	data.post_process();

	std::cout << idl2cpp_comment() <<
		"namespace " << data._namespace.back() << "\n{";

	for (auto& [iface, functions] : data._interfaces)
	{
		if (iface._level > 1)
			continue;

		if (!data._filter.empty() && iface._name != data._filter)
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

					if (p._kind != param_t::kind::unknown &&
						p._kind != param_t::kind::retval)
					{
						ss << '[' << (p._kind == param_t::kind::in ?
							"in" :
							p._kind == param_t::kind::out ?
							"out" :
							"in, out") << "] ";
					}

					ss << p._com_type << std::string(p._com_stars, '*') << ' ' << p._name;
					++idx;
				}

				ss << ')' << "\n\t\tif (pDispParams->cArgs != " <<
					f._params.size() << ")\n\t\t"
					"{\n\t\t\treturn E_INVALIDARG;\n\t\t}\n"
					"\t\telse\n\t\t{\n";

				for (const auto& p : f._params)
				{
					ss << "\t\t\t// " << p._com_type;

					if (!(p._vt == "VT_DISPATCH" && p._com_type != "IDispatch"))
						ss << std::string(p._com_stars, '*');
						
					ss << ' ' << p._name;

					if (p._com_type == p._name)
						ss << '_';

					ss << " = pDispParams->rgvarg[" << idx - 1 << "]." <<
						vt_to_member(p._com_type, p._com_stars, p._vt,
							data._enum_map) << ";\n";
					--idx;
				}

				if (f._ret_vt != "VT_EMPTY")
				{
					ss << "\t\t\t// " << "pVarResult->" <<
						vt_to_member(f._ret_com_type, f._ret_stars,
							f._ret_vt, data._enum_map) << " = your_ret;\n";
				}

				ss << "\t\t}\n\n\t\tbreak;\n";
			}

			ss << '\t';
			source = std::regex_replace(source, switch_rx, ss.str());
			std::cout << source;
		}
	}

	std::cout << "}\n";
}
