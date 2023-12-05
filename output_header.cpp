#include "stdafx.h"

#include "output.h"
#include "output_header.h"
#include "predefined.h"
#include <sstream>
#include "structs.h"

void output_header(data_t& data)
{
	bool seen_typedefs = false;

	data.post_process();

	std::cout << "#pragma once\n\n" << idl2cpp_comment();

	if (!data._enum_set.empty())
		data._includes.insert(data._namespace.back() + "_enums.h");

	data._includes.insert(data._namespace.back() + "_fwd.h");

	for (const std::string& inc : data._includes)
	{
		std::cout << "#include \"" << to_lower(inc) << "\"\n";
	}

	std::cout << '\n';

	if (data._seen_i64)
		std::cout << "#include <cstdint>\n\n";

	std::cout << "// https://learn.microsoft.com/en-us/cpp/error-messages/compiler-warnings/compiler-warning-level-2-c4275?view=msvc-170\n"
		"#pragma warning(push)\n"
		"#pragma warning(disable:4275)\n\n";

	if (!data._lib_help.empty())
		std::cout << "// " << data._lib_help << '\n';

	std::cout << "namespace " << data._namespace.back() << "\n{";

	for (auto& [name, tuple] : data._typedefs)
	{
		if (std::get<0>(tuple) == 1)
		{
			if (!is_predefined(name))
			{
				std::cout << '\n' << "using " << name << " = " <<
					std::get<2>(tuple) << ';';
				seen_typedefs = true;
			}
		}
	}

	if (seen_typedefs)
		std::cout << '\n';

	for (auto& [iface, functions] : data._interfaces)
	{
		if (iface._level > 1)
			continue;

		if (base(iface._name, data) != "IDispatch")
			continue;

		if (data._events.contains(std::make_pair(iface._namespace, iface._name)))
			continue;

		std::string base_type =
			data._inherits.find(iface._name)->second;

		if (base_type == "IDispatch")
			base_type = "COleDispatchDriver";

		if (!iface._help.empty())
			std::cout << "\n// " << iface._help;

		if (iface._hidden)
		{
			std::cout << "\n// [hidden]";
		}

		std::cout << "\nstruct ";

		if (data._afx_ext_class)
			std::cout << "AFX_EXT_CLASS ";

		std::cout << iface._name << " : ";
		output_if_namespace(base_type, data, std::cout);
		std::cout << base_type << "\n{\n";
		std::cout << '\t' << iface._name << "() {}\n";
		std::cout << '\t' << iface._name << "(LPDISPATCH pDispatch) :\n\t\t";
		output_if_namespace(base_type, data, std::cout);
		std::cout << base_type << "(pDispatch) {}\n";
		std::cout << '\t' << iface._name << "(const " << iface._name <<
			"& dispatchSrc) :\n\t\t";
		output_if_namespace(base_type, data, std::cout);
		std::cout << base_type << "(dispatchSrc) {}\n\n";
		std::cout << "\toperator LPDISPATCH() = delete;";

		if (!functions.empty())
			std::cout << '\n';

		for (auto& f : functions)
		{
			std::stringstream ss;

			if (f._restricted || f._name.find("Dummy") != std::string::npos ||
				(f._name.size() >= 3 && f._name.substr(f._name.size() - 3) == "Old"))
			{
				continue;
			}

			bool first = true;
			bool seen_optional = false;
			bool ignore_optionals = false;

			for (const auto& help : f._help)
			{
				std::cout << "\n\t// " << help;
			}

			if (f._hidden)
			{
				std::cout << "\n\t// [hidden]";
			}

			for (const auto& p : f._params)
			{
				if (p._optional)
					seen_optional = true;
				else if (seen_optional)
				{
					// Optional followed by mandatory param.
					ignore_optionals = true;
					break;
				}
			}

			for (auto& p : f._params)
			{
				if ((f._kind == func_t::kind::propput ||
					f._kind == func_t::kind::propputref) &&
					(f._params.size() == 1 || p._name == "prop" ||
						p._name == "rhs"))
				{
					p._name = convert_prop(p._cpp_type, data);
				}

				if (p._cpp_type != p._com_type ||
					(data._inherits.contains(p._cpp_type) &&
					(p._cpp_stars > 1 || p._default_value == "0")))
				{
					std::cout << "\n\t// ";
					output_if_namespace(p._com_type, data, std::cout);
					std::cout << p._com_type <<
						std::string(p._com_stars, '*') << ' ' << p._name;
				}

				if (ignore_optionals && p._optional)
				{
					std::cout << "\n\t// ";
					output_if_namespace(p._name, data, std::cout);
					std::cout << p._name << " optional";

					if (!p._default_value.empty())
						std::cout << ", defaultvalue " << p._default_value;
				}
			}

			convert_ret(f._ret_cpp_type);

			if (f._ret_com_type != f._ret_cpp_type && f._ret_cpp_type != "CString")
			{
				std::cout << "\n\t// returns ";
				output_if_namespace(f._ret_com_type, data, std::cout);
				std::cout << f._ret_com_type <<
					std::string(f._ret_stars, '*');
			}

			std::cout << '\n';
			ss << '\t';
			output_if_namespace(f._ret_cpp_type, data, ss);
			output_enum_namespace(f._ret_cpp_type, data, ss);
			ss << f._ret_cpp_type;

			if (!data._inherits.contains(f._ret_cpp_type))
				ss << std::string(f._ret_stars, '*');

			ss << ' ';

			// Lower case so as not to clash with other functions
			switch (f._kind)
			{
				using enum func_t::kind;

			case propget:
				ss << "get";
				break;
			case propput:
				ss << "set";
				break;
			case propputref:
				ss << "setRef";
				break;
			default:
				break;
			}

			ss << f._name << '(';

			for (const auto& p : f._params)
			{
				if (!first)
				{
					ss << ", ";
					format_output(ss, ',', 2);
				}

				output_if_namespace(p._cpp_type, data, ss);
				output_enum_namespace(p._cpp_type, data, ss);

				if ((p._cpp_type == "CURRENCY" || p._cpp_type == "VARIANT") &&
					p._kind == param_t::kind::in && p._cpp_stars == 0)
				{
					ss << "const ";
				}

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

				if (!ignore_optionals)
				{
					if (!p._default_value.empty())
					{
						auto iter = data._enum_map.find(p._cpp_type);

						if (iter != data._enum_map.cend())
						{
							const std::string dv = p._default_value;
							auto i = iter->second._enums.find(dv);

							if (i == iter->second._enums.cend())
							{
								ss << " = ";
								output_enum_namespace(p._cpp_type, data, ss);
								ss << p._cpp_type << '(' <<
									p._default_value << ')';
							}
							else
							{
								ss << " = ";
								output_enum_namespace(p._cpp_type, data, ss);

								if (data._enum_class)
									ss << p._cpp_type << "::";

								ss << i->second;
							}
						}
						else
						{
							ss << " = ";

							if (p._default_value == "0" && p._cpp_stars)
								ss << "nullptr";
							else
								ss << p._default_value;
						}
					}
				}

				first = false;
			}

			ss << ");";
			format_output(ss, ',', 2, 2);
			std::cout << ss.str();
		}

		std::cout << "\n};\n";
	}

	std::cout << "}\n\n"
		"#pragma warning(pop)\n";
}
