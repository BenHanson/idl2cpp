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
					"\t\telse\n\t\t{\n";

				for (const auto& p : f._params)
				{
					ss << "\t\t\t// " << p._com_type;

					if (!(p._vt == "VT_DISPATCH" && p._com_type != "IDispatch"))
						ss << std::string(p._cpp_stars, '*');
						
					ss << ' ' << p._name;

					if (p._com_type == p._name)
						ss << '_';

					ss << " = pDispParams->rgvarg[" << idx - 1 << "].";

					if (p._com_type == "void")
						ss << "byref";
					else
					{
						switch (p._com_stars)
						{
						case 1:
							ss << 'p';
							break;
						case 2:
							ss << "pp";
							break;
						default:
							break;
						}

						if (p._vt == "VT_I8")
							ss << "llVal";
						else if (p._vt == "VT_I4")
							ss << "lVal";
						else if (p._vt == "VT_UI1")
							ss << "bVal";
						else if (p._com_type == "VT_I2")
							ss << "iVal";
						else if (p._vt == "VT_R4")
							ss << "fltVal";
						else if (p._vt == "VT_R8")
							ss << "dblVal";
						else if (p._vt == "VT_BOOL")
							ss << "boolVal";
						else if (p._vt == "VT_ERROR")
							ss << "scode";
						else if (p._vt == "VT_CY")
							ss << "cyVal";
						else if (p._vt == "VT_DATE")
							ss << "date";
						else if (p._vt == "VT_BSTR")
							ss << "bstrVal";
						else if (p._vt == "VT_UNKNOWN")
							ss << "unkVal";
						else if (p._vt == "VT_DISPATCH")
							ss << "dispVal";
						else if (p._vt == "VT_ARRAY")
							ss << "array";
						else if (p._vt == "VT_I1")
							ss << "cVal";
						else if (p._vt == "VT_UI2")
							ss << "uiVal";
						else if (p._vt == "VT_UI4")
							ss << "ulVal";
						else if (p._vt == "VT_UI8")
							ss << "ullVal";
						else if (p._vt == "VT_INT")
							ss << "intVal";
						else if (p._vt == "VT_UINT")
							ss << "uintVal";
						else if (p._vt == "VT_VARIANT")
							ss << "varVal";
						else if (data._enum_map.contains(p._com_type))
							ss << "lVal";
					}

					ss << ";\n";
					--idx;
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
