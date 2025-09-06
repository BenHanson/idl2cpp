#include "stdafx.h"

#include "output.h"
#include "predefined.h"
#include "structs.h"

#include <chrono>
#include <format>
#include <iostream>
#include <sstream>

std::string idl2cpp_comment()
{
	return std::format("// GENERATED CODE by idl2cpp {:%Y-%m-%d %H:%M:%S}\n"
		"// See https://github.com/BenHanson/idl2cpp\n",
		floor<std::chrono::seconds>(std::chrono::system_clock::now()));
}

std::string base(const std::string& str, const data_t& data)
{
	if (is_predefined(str))
		return str;

	std::string ret = str;
	auto iter = data._inherits.find(ret);
	auto end = data._inherits.cend();

	for (;;)
	{
		/*if (iter == end)
		{
			auto typedef_iter = data._typedefs.find(ret);

			if (typedef_iter != data._typedefs.cend())
			{
				ret = std::get<2>(typedef_iter->second);
				iter = data._inherits.find(ret);
			}
		}*/

		if (iter != end)
			ret = iter->second;
		else
			break;

		iter = data._inherits.find(ret);
	}

	return ret;
}

std::string to_lower(const std::string& str)
{
	std::string ret;

	for (char c : str)
	{
		if (c >= 'A' && c <= 'Z')
		{
			c -= 'A';
			c += 'a';
		}

		ret += c;
	}

	return ret;
}

void output_if_namespace(const std::string& name, const data_t& data,
	std::ostream& ss)
{
	const std::string curr_ns = data._namespace.back();

	if (const std::pair key(curr_ns, name);
		data._coclass.contains(key))
	{
		return;
	}

	if (std::ranges::find_if(data._interfaces, [&curr_ns, &name]
	(const auto& pair)
		{
			return pair.first._namespace == curr_ns && pair.first._name == name;
		}) != data._interfaces.cend())
	{
		return;
	}

	if (auto coclass_iter = std::ranges::find_if(data._coclass,
		[&name](const auto& pair)
		{
			return pair.first.second == name;
		});
		coclass_iter != data._coclass.cend())
	{
		ss << coclass_iter->first.first << "::";
		return;
	}

	if (auto typedef_iter = std::ranges::find_if(data._typedefs,
		[&name](const auto& pair)
		{
			return pair.first == name;
		}); typedef_iter != data._typedefs.cend())
	{
		if (const auto& td_ns = std::get<1>(typedef_iter->second);
			td_ns != curr_ns)
		{
			ss << td_ns << "::";
		}

		return;
	}

	auto if_iter = std::ranges::find_if(data._interfaces, [&name]
		(const auto& pair)
	{
		return pair.first._name == name;
	});

	if (if_iter != data._interfaces.cend() &&
		if_iter->first._namespace != curr_ns)
	{
		ss << if_iter->first._namespace << "::";
	}
	else
	{
		auto ty_iter = std::ranges::find_if(data._typedefs, [&name](const auto& pair)
			{
				return pair.first == name;
			});

		if (ty_iter != data._typedefs.cend())
			ss << std::get<1>(ty_iter->second) << "::";
	}
}

std::string convert_prop(const std::string& type, const data_t& data)
{
	static type_conv list_[] =
	{
		{ "BOOL", "bNewValue" },
		{ "int", "nNewValue" },
		{ "LPCTSTR", "lpszNewValue" },
		{ "long", "nNewValue" },
		{ "short", "nNewValue" }
	};
	auto iter = std::ranges::find_if(list_, [&type](const auto& rhs)
		{
			return type == rhs._in;
		});
	std::string ret;

	if (iter != std::end(list_))
		ret = iter->_out;
	else if (data._enum_set.contains(type))
		ret = "nNewValue";
	else
		ret = "newValue";

	return ret;
}

void convert_ret(std::string& type)
{
	static type_conv list_[] =
	{
		{ "", "void" },
		{ "BSTR", "CString" }
	};
	auto iter = std::ranges::find_if(list_, [&type](const auto& rhs)
		{
			return type == rhs._in;
		});

	if (iter != std::end(list_))
		type = iter->_out;
}

void output_enum_namespace(const std::string& name, const data_t& data,
	std::ostream& ss)
{
	auto iter = data._enum_map.find(name);

	if (iter != data._enum_map.cend() &&
		iter->second._namespace != data._namespace.back())
	{
		ss << iter->second._namespace << "::";
	}
}

std::size_t last_char(const char c, const std::string& str,
	const std::size_t where, int& parens)
{
	std::size_t ret = std::string::npos;
	const char* first = str.c_str();
	const char* second = first + where - 1;

	for (; first != second; --second)
	{
		if (*second == ')')
			++parens;
		else if (*second == '(')
			--parens;
		else if (*second == c && !parens)
		{
			ret = std::distance(first, second);
			break;
		}
	}

	return ret;
}

void format_output(std::stringstream& ss, const char c, const std::size_t tabs,
	std::size_t offset)
{
	ss.seekg(0, std::ios::end);

	if (ss.tellg() > 80)
	{
		std::string str = ss.str();
		int parens = 0;
		std::size_t prev_idx = std::string::npos;
		std::size_t idx = last_char(c, str, str.size() - offset, parens);

		while (idx != std::string::npos && idx > 80)
		{
			prev_idx = idx;
			idx = last_char(c, str, idx, parens);
		}

		ss.seekg(0, std::ios::beg);

		if (idx == std::string::npos && prev_idx == std::string::npos)
		{
			std::cout << str;
			ss.str("");
		}
		else
		{
			if (idx == std::string::npos)
				idx = prev_idx;

			std::cout << str.substr(0, idx + (c == ' ' ? 0 : 1)) << '\n';

			for (std::size_t i = 0; i < tabs; ++i)
			{
				std::cout << '\t';
			}

			ss.str("");
			ss << str.substr(idx + (c == ' ' ? 1 : 2));
		}
	}
	else
	{
		ss.seekg(0, std::ios::beg);
	}
}

std::string vt_to_member(const std::string& com_type, const std::size_t com_stars,
	const std::string& vt, const std::map<std::string, enum_data>& enum_map)
{
	std::string ret;

	if (com_type != "void")
	{
		switch (com_stars)
		{
		case 1:
			ret = 'p';
			break;
		case 2:
			ret = "pp";
			break;
		default:
			break;
		}
	}

	if (com_type == "void")
		ret = "byref";
	else if (enum_map.contains(com_type))
		ret += "lVal";
	else
	{
		static type_conv list_[] =
		{
			{ "VT_I8", "llVal" },
			{ "VT_I4", "lVal" },
			{ "VT_UI1", "bVal" },
			{ "VT_I2", "iVal" },
			{ "VT_R4", "fltVal" },
			{ "VT_R8", "dblVal" },
			{ "VT_BOOL", "boolVal" },
			{ "VT_ERROR", "scode" },
			{ "VT_CY", "cyVal" },
			{ "VT_DATE", "date" },
			{ "VT_BSTR", "bstrVal" },
			{ "VT_UNKNOWN", "unkVal" },
			{ "VT_DISPATCH", "dispVal" },
			{ "VT_ARRAY", "array" },
			{ "VT_I1", "cVal" },
			{ "VT_UI2", "uiVal" },
			{ "VT_UI4", "ulVal" },
			{ "VT_UI8", "ullVal" },
			{ "VT_INT", "intVal" },
			{ "VT_UINT", "uintVal" },
			{ "VT_VARIANT", "varVal" }
		};
		const auto& type = vt;
		auto iter = std::ranges::find_if(list_, [&type](const auto& rhs)
			{
				return type == rhs._in;
			});

		if (iter != std::end(list_))
			ret += iter->_out;
	}

	return ret;
}
