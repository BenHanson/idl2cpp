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

	if (data._seen_i64)
		std::cout << "#include <cstdint>\n\n";

	std::cout << "#pragma once\n\n" << idl2cpp_comment() << '\n' <<
		"// http://support.microsoft.com/kb/134980\n"
		"#pragma warning(push)\n"
		"#pragma warning(disable:4275)\n\n";

	if (!data._lib_help.empty())
		std::cout << "// " << data._lib_help << '\n';

	std::cout << "namespace " << data._namespace.back() << "\n{";

	for (auto& pair : data._typedefs)
	{
		if (std::get<0>(pair.second) == 1)
		{
			if (!is_predefined(pair.first))
			{
				std::cout << '\n' << "using " << pair.first << " = " <<
					std::get<2>(pair.second) << ';';
				seen_typedefs = true;
			}
		}
	}

	if (seen_typedefs)
		std::cout << '\n';

	for (auto& inf : data._interfaces)
	{
		if (inf.first._level > 1 || inf.second.empty())
			continue;

		if (base(inf.first._name, data) != "IDispatch")
			continue;

		std::string base_type =
			data._inherits.find(inf.first._name)->second;

		if (base_type == "IDispatch")
			base_type = "COleDispatchDriver";

		if (!inf.first._help.empty())
			std::cout << "\n// " << inf.first._help;

		std::cout << "\nstruct AFX_EXT_CLASS " << inf.first._name << " : ";
		std::cout << base_type << "\n{\n";
		std::cout << '\t' << inf.first._name << "() {}\n";
		std::cout << '\t' << inf.first._name << "(LPDISPATCH pDispatch) :\n\t\t";
		output_if_namespace(base_type, data, std::cout);
		std::cout << base_type << "(pDispatch) {}\n";
		std::cout << '\t' << inf.first._name << "(const " << inf.first._name <<
			"& dispatchSrc) :\n\t\t";
		output_if_namespace(base_type, data, std::cout);
		std::cout << base_type << "(dispatchSrc) {}";

		if (!inf.second.empty())
			std::cout << '\n';

		for (auto& f : inf.second)
		{
			std::stringstream ss;

			if (f._restricted || f._name.find("Dummy") != std::string::npos ||
				f._name.size() >= 3 && f._name.substr(f._name.size() - 3) == "Old")
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

			for (auto& p : f._params)
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

				if (p._cpp_type != p._com_type)
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

			convert_ret(f._ret_cpp_type, f._ret_stars);

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
			ss << f._ret_cpp_type << std::string(f._ret_stars, '*') << ' ';

			switch (f._kind)
			{
			case func_t::kind::propget:
				ss << "Get";
				break;
			case func_t::kind::propput:
				ss << "Set";
				break;
			case func_t::kind::propputref:
				ss << "SetRef";
				break;
			}

			ss << f._name << '(';

			for (auto& p : f._params)
			{
				if (!first)
				{
					ss << ", ";
					format_output(ss, ',', 2);
				}

				output_if_namespace(p._cpp_type, data, ss);
				output_enum_namespace(p._cpp_type, data, ss);

				if (p._cpp_type == "VARIANT" && p._cpp_stars == 0)
					ss << "const ";

				ss << p._cpp_type << std::string(p._cpp_stars, '*');

				if (p._cpp_type == "VARIANT" && p._cpp_stars == 0)
					ss << '&';

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
