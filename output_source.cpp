#include "stdafx.h"

#include "output.h"
#include "output_source.h"
#include <sstream>
#include "structs.h"

void output_source(data_t& data)
{
	data.post_process();

	std::cout << idl2cpp_comment() <<
		"namespace " << data._namespace.back() << "\n{";

	for (auto& inf : data._interfaces)
	{
		if (inf.first._level > 1)
			continue;

		if (base(inf.first._name, data) != "IDispatch")
			continue;

		if (inf.second.empty() && data._inherits[inf.first._name] == "IDispatch")
			continue;

		for (auto& f : inf.second)
		{
			std::stringstream ss;
			bool first = true;

			if (f._restricted || f._name.find("Dummy") != std::string::npos ||
				(f._name.size() >= 3 && f._name.substr(f._name.size() - 3) == "Old"))
			{
				continue;
			}

			std::cout << '\n';

			bool ret_is_enum = data._enum_map.find(f._ret_cpp_type) != data._enum_map.cend();

			if (ret_is_enum)
				output_enum_namespace(f._ret_cpp_type, data, std::cout);

			convert_ret(f._ret_cpp_type);
			output_if_namespace(f._ret_cpp_type, data, ss);
			ss << f._ret_cpp_type;

			if (!data._inherits.contains(f._ret_cpp_type))
				ss << std::string(f._ret_stars, '*');

			ss << ' ' << inf.first._name << "::";

			// Lower case so as not to clash with other functions
			switch (f._kind)
			{
			case func_t::kind::propget:
				ss << "get";
				break;
			case func_t::kind::propput:
				ss << "set";
				break;
			case func_t::kind::propputref:
				ss << "setRef";
				break;
			default:
				break;
			}

			ss << f._name << '(';

			for (auto& p : f._params)
			{
				if (!first)
				{
					ss << ", ";
					format_output(ss, ',', 1);
				}

				if ((p._cpp_type == "CURRENCY" || p._cpp_type == "VARIANT") &&
					p._kind == param_t::kind::in && p._cpp_stars == 0)
				{
					ss << "const ";
				}

				if ((f._kind == func_t::kind::propput ||
					f._kind == func_t::kind::propputref) &&
					(f._params.size() == 1 || p._name == "prop" ||
						p._name == "rhs"))
				{
					p._name = convert_prop(p._cpp_type, data);
				}

				output_if_namespace(p._cpp_type, data, ss);
				if (data._enum_map.find(p._cpp_type) != data._enum_map.cend())
					output_enum_namespace(p._cpp_type, data, ss);

				if (data._inherits.contains(p._cpp_type) &&
					(p._cpp_stars > 1 || p._default_value == "0"))
				{
					ss << "IDispatch";
				}
				else
					ss << p._cpp_type;

				if (data._inherits.contains(p._cpp_type) &&
					p._cpp_stars < 2 && p._default_value != "0")
				{
					ss << '&';
				}
				else
					ss << std::string(p._cpp_stars, '*');

				if ((p._cpp_type == "CURRENCY" || p._cpp_type == "VARIANT") &&
					p._cpp_stars == 0)
				{
					ss << '&';
				}

				ss << ' ' << p._name;
				first = false;
			}

			ss << ')';
			format_output(ss, ',', 1, 1);
			std::cout << ss.str();
			ss.str("");
			std::cout << "\n{\n";

			if (f._ret_cpp_type != "void")
			{
				std::cout << '\t';

				if (ret_is_enum)
					output_enum_namespace(f._ret_cpp_type, data, std::cout);
				else if (!data._inherits.contains(f._ret_cpp_type))
					output_if_namespace(f._ret_cpp_type, data, std::cout);

				std::cout << base(f._ret_cpp_type, data) <<
					std::string(f._ret_stars, '*') << " result{};\n";
			}

			if (!f._params.empty())
			{
				ss << "\tstatic BYTE params[] =";
			}
			else if (f._ret_cpp_type != "void")
			{
				std::cout << '\n';
			}

			for (const auto& p : f._params)
			{
				ss << ' ';
				format_output(ss, ' ', 2);

				if (data._enum_set.find(p._cpp_type) == data._enum_set.cend())
				{
					ss << p._vts;
				}
				else
				{
					ss << "VTS_I4";
				}
			}

			if (!f._params.empty())
			{
				ss << ';';
				format_output(ss, ' ', 2);
				std::cout << ss.str();
				ss.str("");
				std::cout << "\n\n";
			}

			ss << "\tInvokeHelper(0x" << std::hex <<
				f._id << ", ";

			switch (f._kind)
			{
			case func_t::kind::propget:
				ss << "DISPATCH_PROPERTYGET";
				break;
			case func_t::kind::propput:
				ss << "DISPATCH_PROPERTYPUT";
				break;
			case func_t::kind::propputref:
				ss << "DISPATCH_PROPERTYPUTREF";
				break;
			default:
				ss << "DISPATCH_METHOD";
				break;
			}

			ss << ", ";

			if (f._ret_cpp_type == "void")
			{
				ss << "VT_EMPTY, nullptr, ";
			}
			else
			{
				if (data._enum_set.find(f._ret_cpp_type) == data._enum_set.cend())
				{
					ss << f._ret_vt;
				}
				else
				{
					ss << "VT_I4";
				}

				ss << ", &result, ";
			}

			if (f._params.empty())
			{
				ss << "nullptr";
			}
			else
			{
				ss << "params";
			}

			for (const auto& p : f._params)
			{
				ss << ", ";
				format_output(ss, ',', 2);

				if ((p._cpp_type == "CURRENCY" || p._cpp_type == "VARIANT") &&
					p._cpp_stars == 0)
				{
					ss << '&';
				}

				ss << p._name;

				if (data._inherits.contains(p._cpp_type) &&
					p._cpp_stars < 2 && p._default_value != "0")
				{
					ss << ".m_lpDispatch";
				}
			}

			ss << ");";
			format_output(ss, ',', 2, 2);
			std::cout << ss.str();
			ss.str("");
			std::cout << '\n';

			if (f._ret_cpp_type != "void")
			{
				std::cout << "\treturn result;\n";
			}

			std::cout << "}\n";
		}
	}

	std::cout << "}\n";
}
