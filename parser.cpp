#include "stdafx.h"

#include <format>
#include <parsertl/generator.hpp>
#include "predefined.h"
#include "structs.h"

void process_interface(data_t& data)
{
	const auto& results = data._results_stack.top();
	const auto& productions = data._productions_stack.top();

	data._interfaces.emplace_back();
	data._curr_if = &data._interfaces.back().first;
	data._curr_if->_uuid = std::move(data._curr_attrs._uuid);
	data._curr_if->_level = data._results_stack.size();
	data._curr_if->_namespace = data._namespace.back();
	data._curr_if->_name = results.dollar(1, data_t::_gsm, productions).str();
	data._curr_if->_hidden = data._curr_attrs._hidden;
	data._curr_if->_help = std::move(data._curr_attrs._helpstring);
	data._curr_attrs.clear();
}

void process_name(const bool has_name, data_t& data)
{
	if (data._curr_param._kind == param_t::kind::unknown)
		data._curr_param._kind = param_t::kind::in;

	if (data._curr_attrs._lcid)
		return;

	if (data._curr_if)
	{
		if (data._curr_param._kind == param_t::kind::retval)
		{
			auto& fun = data._interfaces.back().second.back();

			fun._ret_com_type = data._curr_param._com_type;
			fun._ret_cpp_type = data._curr_param._cpp_type;
			fun._ret_stars = data._curr_param._com_stars ?
				data._curr_param._com_stars - 1 : 0;

			if (data._cpp)
			{
				fun._ret_vt = data._curr_vt;
				fun._ret_vts = data._curr_vts;

				if (fun._ret_stars && !fun._ret_vts.empty())
					fun._ret_vts.insert(4, 1, 'P');
			}
		}
		else
		{
			const auto& results = data._results_stack.top();
			const auto& productions = data._productions_stack.top();
			auto& params = data._interfaces.back().second.back()._params;
			param_t* param = nullptr;

			params.emplace_back();
			param = &params.back();
			param->_name = has_name ?
				results.dollar(0, data_t::_gsm, productions).str() :
				"newValue";
			param->_com_type = data._curr_param._com_type;
			param->_cpp_type = data._curr_param._cpp_type;
			param->_kind = data._curr_param._kind;
			param->_com_stars = data._curr_param._com_stars;
			param->_cpp_stars = data._curr_param._cpp_stars;
			param->_default_value = data._curr_param._default_value;
			param->_optional = data._curr_param._optional;

			if (data._cpp)
			{
				param->_vt = data._curr_vt;
				param->_vts = data._curr_vts;

				if (param->_cpp_stars && !param->_vts.empty() &&
					!(param->_cpp_stars == 1 && (param->_com_type == "IDispatch" ||
						param->_cpp_type == "IUnknown")))
					param->_vts.insert(4, 1, 'P');
			}
		}
	}
}

std::string ret_to_vt(const std::string& vt)
{
	static type_conv list_[] =
	{
		{ "CString", "VT_BSTR" },
		{ "void", "VT_VOID" },
		{ "BSTR", "VT_BSTR" },
		{ "CURRENCY", "VT_CY" },
		{ "DATE", "VT_DATE" },
		{ "double", "VT_R8" },
		{ "IDispatch", "VT_DISPATCH" },
		{ "IUnknown", "VT_UNKNOWN" },
		{ "int64_t", "VT_I8" },
		{ "OLE_CANCELBOOL", "VT_BOOL" },
		{ "OLE_COLOR", "VT_COLOR" },
		{ "OLE_ENABLEDEFAULTBOOL", "VT_BOOL" },
		{ "OLE_HANDLE", "VT_HANDLE" },
		{ "OLE_OPTEXCLUSIVE", "VT_OPTEXCLUSIVE" },
		{ "OLE_XPOS_CONTAINER", "VT_R4" },
		{ "OLE_XPOS_HIMETRIC", "VT_XPOS_HIMETRIC" },
		{ "OLE_XPOS_PIXELS", "VT_XPOS_PIXELS" },
		{ "OLE_XSIZE_CONTAINER", "VT_R4" },
		{ "OLE_XSIZE_HIMETRIC", "VT_XSIZE_HIMETRIC" },
		{ "OLE_XSIZE_PIXELS", "VT_XSIZE_PIXELS" },
		{ "OLE_YPOS_CONTAINER", "VT_R4" },
		{ "OLE_YPOS_HIMETRIC", "VT_YPOS_HIMETRIC" },
		{ "OLE_YPOS_PIXELS", "VT_YPOS_PIXELS" },
		{ "OLE_YSIZE_CONTAINER", "VT_R4" },
		{ "OLE_YSIZE_HIMETRIC", "VT_YSIZE_HIMETRIC" },
		{ "OLE_YSIZE_PIXELS", "VT_YSIZE_PIXELS" },
		{ "VARIANT", "VT_VARIANT" },
		{ "SCODE", "VT_ERROR" },
		{ "float", "VT_R4" },
		{ "uint64_t", "VT_UI8" },
		{ "BOOL", "VT_BOOL" },
		{ "void", "VT_EMPTY" },
		{ "char", "VT_I1" },
		{ "int", "VT_I4" },
		{ "long", "VT_I4" },
		{ "short", "VT_I2" },
		{ "wchar_t", "VT_I2" }
	};
	auto iter = std::ranges::find_if(list_, [&vt](const auto& rhs)
		{
			return vt == rhs._in;
		});
	std::string ret;

	if (iter != std::end(list_))
		ret = iter->_out;

	return ret;
}

std::string ret_to_vts(const std::string& vt)
{
	static type_conv list_[] =
	{
		{ "BSTR", "VTS_BSTR" },
		{ "CURRENCY", "VTS_CY" },
		{ "DATE", "VTS_DATE" },
		{ "double", "VTS_R8" },
		{ "IDispatch", "VTS_DISPATCH" },
		{ "IUnknown", "VTS_UNKNOWN" },
		{ "int64_t", "VTS_I8" },
		{ "OLE_CANCELBOOL", "VT_BOOL" },
		{ "OLE_COLOR", "VTS_COLOR" },
		{ "OLE_ENABLEDEFAULTBOOL", "VTS_BOOL" },
		{ "OLE_HANDLE", "VTS_HANDLE" },
		{ "OLE_OPTEXCLUSIVE", "VTS_OPTEXCLUSIVE" },
		{ "OLE_XPOS_CONTAINER", "VTS_R4" },
		{ "OLE_XPOS_HIMETRIC", "VTS_XPOS_HIMETRIC" },
		{ "OLE_XPOS_PIXELS", "VTS_XPOS_PIXELS" },
		{ "OLE_XSIZE_CONTAINER", "VTS_R4" },
		{ "OLE_XSIZE_HIMETRIC", "VTS_XSIZE_HIMETRIC" },
		{ "OLE_XSIZE_PIXELS", "VTS_XSIZE_PIXELS" },
		{ "OLE_YPOS_CONTAINER", "VTS_R4" },
		{ "OLE_YPOS_HIMETRIC", "VTS_YPOS_HIMETRIC" },
		{ "OLE_YPOS_PIXELS", "VTS_YPOS_PIXELS" },
		{ "OLE_YSIZE_CONTAINER", "VTS_R4" },
		{ "OLE_YSIZE_HIMETRIC", "VTS_YSIZE_HIMETRIC" },
		{ "OLE_YSIZE_PIXELS", "VTS_YSIZE_PIXELS" },
		{ "VARIANT", "VTS_VARIANT" },
		{ "SCODE", "VTS_SCODE" },
		{ "float", "VTS_R4" },
		{ "uint64_t", "VTS_UI8" },
		{ "BOOL", "VTS_BOOL" },
		{ "void", "" },
		{ "char", "VTS_I1" },
		{ "int", "VTS_I4" },
		{ "long", "VTS_I4" },
		{ "short", "VTS_I2" },
		{ "wchar_t", "VTS_I2" }
	};
	auto iter = std::ranges::find_if(list_, [&vt](const auto& rhs)
		{
			return vt == rhs._in;
		});
	std::string ret;

	if (iter != std::end(list_))
		ret = iter->_out;

	return ret;
}

// IDL reference: https://msdn.microsoft.com/en-us/library/windows/desktop/aa367088(v=vs.85).aspx
// - This parser only needs to parse what OLE Viewer outputs.
// https://github.com/microsoft/VCSamples/tree/master/VC2010Samples/MFC/ole/oleview
void build_parser()
{
	parsertl::rules grules;
	lexertl::rules lrules;

	// Start of new grammar 02/06/2018
	grules.token("Name Number String Uuid");

	// attr_list cleared in lib_name
	grules.push("file", "attr_list 'library' lib_name '{' lib_stmts '}' ';'");
	data_t::_actions[grules.push("lib_name", "Name")] = [](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();
		std::string ns = results.dollar(0, data_t::_gsm, productions).str();

		if (data._lib_help.empty())
			data._lib_help = data._curr_attrs._helpstring;

		data._curr_attrs.clear();
		data._namespace.push_back(ns);
	};
	grules.push("lib_stmts", "lib_stmt ';' "
		"| lib_stmts lib_stmt ';'");
	grules.push("lib_stmt", "'dispinterface' Name");
	data_t::_actions[grules.push("lib_stmt", "'importlib' '(' String ')'")] = [](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();
		std::string name = results.dollar(2, data_t::_gsm, productions).substr(1, 1);
		std::string pathname;

		name = name.substr(0, name.rfind('.'));

		if (data._namespace.size() == 1)
			data._includes.insert(name + ".h");

		name += ".idl";

		pathname = data._path + name;

		if (!data._files.contains(pathname))
		{
			data.parse(pathname);

			if (data._namespace.size() > 1)
				data._namespace.pop_back();
		}
	};
	grules.push("lib_stmt", "'interface' if_type");
	// attr_list cleared in coclass_name
	data_t::_actions[grules.push("lib_stmt",
		"attr_list 'coclass' coclass_name '{' interface_list '}'")] = [](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();
		std::string name = results.dollar(2, data_t::_gsm, productions).str();

		data._coclass.emplace(data._namespace.back(), std::move(name));
		data._curr_attrs.clear();
	};
	data_t::_actions[grules.push("coclass_name", "Name")] = [](data_t& data)
	{
		data._curr_attrs.clear();
	};
	// attr_list cleared in dispinterface
	data_t::_actions[grules.push("lib_stmt",
		"attr_list dispinterface '{' member_list '}'")] = [](data_t& data)
	{
		data._curr_if = nullptr;
	};
	// attr_list cleared in unknown
	grules.push("lib_stmt",
		"attr_list 'interface' 'IDispatch' ':' unknown '{' member_list '}' "
		"| attr_list 'interface' unknown '{' member_list '}' "
		"| attr_list 'interface' Name ':' unknown '{' member_list '}'");
	data_t::_actions[grules.push("unknown", "'IUnknown'")] = [](data_t& data)
	{
		data._curr_attrs.clear();
	};
	data_t::_actions[grules.push("lib_stmt",
		"attr_list interface '{' member_list '}'")] = [](data_t& data)
	{
		data._curr_attrs.clear();
		data._curr_if = nullptr;
	};
	data_t::_actions[grules.push("lib_stmt", "attr_list 'module' Name '{' member_list '}'")] =
		[](data_t& data)
	{
		data._curr_attrs.clear();
	};
	data_t::_actions[grules.push("lib_stmt", "'typedef' opt_attr_list type_specifier ")] =
		[](data_t& data)
	{
		data._curr_attrs.clear();
	};
	data_t::_actions[grules.push("dispinterface", "'dispinterface' Name")] =
		[](data_t& data)
	{
		process_interface(data);
		data._inherits[data._curr_if->_name] = "IDispatch";
	};

	data_t::_actions[grules.push("interface", "'interface' Name ':' 'IDispatch'")] =
		[](data_t& data)
	{
		process_interface(data);
		data._inherits[data._curr_if->_name] = "IDispatch";
	};

	data_t::_actions[grules.push("interface", "'interface' Name ':' Name")] =
		[](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();
		std::string name = results.dollar(3, data_t::_gsm, productions).str();

		process_interface(data);
		data._inherits[data._curr_if->_name] = std::move(name);
	};

	data_t::_actions[grules.push("type_specifier",
		"'enum' opt_name '{' enum_items '}' Name")] =
		[](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();
		const std::string name = results.dollar(5, data_t::_gsm, productions).str();

		if (!data._enums.empty())
		{
			auto& pair = data._enum_map[name];

			pair._namespace = data._namespace.back();
			pair._level = data._namespace.size();
			pair._help = data._curr_attrs._helpstring;
			std::swap(pair._enums, data._enums);
		}

		data._enum_set.insert(name);
		data._curr_attrs.clear();
	};
	grules.push("type_specifier", "struct_union Name '{' struct_members '}' Name");
	data_t::_actions[grules.push("type_specifier", "type Name")] =
		[](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();
		const std::string lhs = results.dollar(0, data_t::_gsm, productions).str();
		const std::size_t pos = lhs.find_first_not_of(" \t\r\n");
		const std::string rhs = results.dollar(1, data_t::_gsm, productions).str();

		if (!is_predefined(rhs))
			data._typedefs[rhs] = std::make_tuple(data._namespace.size(),
				data._namespace.back(),
				pos == std::string::npos ? lhs : lhs.substr(pos));
	};

	grules.push("struct_union", "'struct' | 'union'");
	grules.push("struct_members", "%empty "
		"| struct_members member");
	data_t::_actions[grules.push("member", "opt_attr_list type Name opt_array ';'")] =
		[](data_t& data)
	{
		data._curr_attrs.clear();
	};
	grules.push("opt_array", "%empty | '[' Number ']'");

	grules.push("interface_list", "%empty");
	data_t::_actions[grules.push("interface_list",
		"interface_list opt_attr_list 'interface' if_type ';'")] =
		[](data_t& data)
	{
		if (data._curr_attrs._source)
		{
			const auto& results = data._results_stack.top();
			const auto& productions = data._productions_stack.top();
			const std::string name = results.dollar(3, data_t::_gsm, productions).str();

			data._events.emplace(data._namespace.back(), name);
		}

		data._curr_attrs.clear();
	};
	data_t::_actions[grules.push("interface_list",
		"interface_list opt_attr_list 'dispinterface' Name ';'")] =
		[](data_t& data)
	{
		if (data._curr_attrs._source)
		{
			const auto& results = data._results_stack.top();
			const auto& productions = data._productions_stack.top();
			const std::string name = results.dollar(3, data_t::_gsm, productions).str();

			data._events.emplace(data._namespace.back(), name);
		}

		data._curr_attrs.clear();
	};
	grules.push("if_type", "'IDispatch' | 'IUnknown' | Name");

	grules.push("member_list", "%empty");
	data_t::_actions[grules.push("member_list", "member_list 'properties:'")] =
		[](data_t& data)
	{
		data._properties = true;
	};
	data_t::_actions[grules.push("member_list", "member_list 'methods:'")] =
		[](data_t& data)
	{
		data._properties = false;
	};
	grules.push("member_list", "member_list function "
		"| member_list property ");
	data_t::_actions[grules.push("function",
		"opt_attr_list type opt_call function_name '(' param_list ')' ';'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			auto& f = data._interfaces.back().second.back();

			if (data._cpp)
			{
				if (f._ret_vt.empty())
					f._ret_vt = ret_to_vt(f._ret_cpp_type.empty() ?
						f._ret_com_type : f._ret_cpp_type);

				if (f._ret_vts.empty())
					f._ret_vts = ret_to_vts(f._ret_cpp_type.empty() ?
						f._ret_com_type : f._ret_cpp_type);
			}

			if (data._curr_attrs._restricted)
			{
				f._restricted = data._curr_attrs._restricted;
			}

			if (f._ret_com_type == f._name &&
				f._kind == func_t::kind::function)
			{
				f._name = "Get" + f._name;
			}

			if (f._id == std::string::npos)
			{
				data._interfaces.back().second.pop_back();
			}
		}

		data._curr_attrs.clear();
		data._func = false;
	};
	data_t::_actions[grules.push("function_name", "Name")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			const auto& results = data._results_stack.top();
			const auto& productions = data._productions_stack.top();
			std::string name = results.dollar(0, data_t::_gsm, productions).str();

			data._interfaces.back().second.emplace_back();

			func_t* func = &data._interfaces.back().second.back();

			func->_name = name;

			if (func->_name == "DialogBox" ||
				func->_name == "ExitWindows")
			{
				func->_name += '_';
			}

			func->_kind = data._curr_attrs._propget ?
				func_t::kind::propget :
				data._curr_attrs._propput ?
				func_t::kind::propput :
				data._curr_attrs._propputref ?
				func_t::kind::propputref :
				func_t::kind::function;
			func->_hidden = data._curr_attrs._hidden;
			func->_restricted = data._curr_attrs._restricted;

			if (!data._curr_attrs._helpstring.empty())
				func->_help.push_back(data._curr_attrs._helpstring);

			if (func->_kind != func_t::kind::propput &&
				func->_kind != func_t::kind::propputref &&
				data._curr_param._com_type != "HRESULT")
			{
				func->_ret_com_type = data._curr_param._com_type;
				func->_ret_stars = data._curr_param._com_stars;
				func->_ret_cpp_type = data._curr_param._cpp_type == "LPCTSTR" ?
					"CString" :
					data._curr_param._cpp_type;
			}

			func->_id = data._curr_attrs._id;
			data._func = true;
		}

		data._curr_attrs.clear();
		data._curr_param.clear();
	};
	grules.push("property", "opt_attr_list type opt_call property_name opt_assign_value ';'");
	data_t::_actions[grules.push("property_name", "Name")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			const auto& results = data._results_stack.top();
			const auto& productions = data._productions_stack.top();
			const std::string name = results.dollar(0, data_t::_gsm, productions).str();

			data._interfaces.back().second.emplace_back();

			func_t* func = &data._interfaces.back().second.back();

			func->_id = data._curr_attrs._id;
			func->_name = name;
			func->_kind = func_t::kind::propget;
			func->_ret_com_type = data._curr_param._com_type;
			func->_ret_cpp_type = data._curr_param._cpp_type == "BSTR" ?
				"CString" :
				data._curr_param._cpp_type;
			func->_hidden = data._curr_attrs._hidden;
			func->_restricted = data._curr_attrs._restricted;

			if (data._cpp)
			{
				func->_ret_vt = data._curr_vt;
				func->_ret_vts = data._curr_vts;
			}

			if (!data._curr_param._readonly)
			{
				param_t* param = nullptr;

				data._interfaces.back().second.emplace_back();
				func = &data._interfaces.back().second.back();
				func->_id = data._curr_attrs._id;
				func->_name = name;
				func->_kind = func_t::kind::propput;
				func->_params.emplace_back();
				param = &func->_params.back();
				param->_com_type = data._curr_param._com_type;
				param->_com_stars = data._curr_param._com_stars;

				if (data._curr_param._cpp_type == "BSTR" && !param->_com_stars)
				{
					// It's not interesting to record the original type here
					param->_com_type = param->_cpp_type = "LPCTSTR";
				}
				else
					param->_cpp_type = data._curr_param._com_type;
					
				param->_cpp_stars = data._curr_param._cpp_stars;

				if (data._cpp)
				{
					param->_vt = data._curr_vt;
					param->_vts = data._curr_vts;
				}
			}
		}

		data._curr_attrs.clear();
		data._curr_param.clear();
	};
	grules.push("opt_assign_value", "%empty | '=' value");
	grules.push("value", "Number | String");
	grules.push("param_list", "%empty "
		"| param "
		"| param_list ',' param");
	data_t::_actions[grules.push("param", "opt_attr_list type opt_param_name")] =
		[](data_t& data)
	{
		data._curr_attrs.clear();
		data._curr_param.clear();
	};
	data_t::_actions[grules.push("opt_param_name", "%empty")] =
		[](data_t& data)
	{
		process_name(false, data);
	};
	data_t::_actions[grules.push("opt_param_name", "param_name opt_array")] =
		[](data_t& data)
	{
		process_name(true, data);
	};
	grules.push("param_name", "Name");

	grules.push("opt_name", "%empty "
		"| Name");
	grules.push("enum_items", "enum_item "
		"| enum_items ',' enum_item");
	data_t::_actions[grules.push("enum_item", "Name '=' Number")] =
		[](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();

		data._enums[results.dollar(2, data_t::_gsm, productions).str()] =
			results.dollar(0, data_t::_gsm, productions).str();
	};
	grules.push("name_star_list", "Name opt_stars "
		"| name_star_list ',' Name opt_stars");
	data_t::_actions[grules.push("type", "opt_const raw_type opt_stars")] =
		[](data_t& data)
	{
		if (data._curr_param._cpp_type == "BSTR" && !data._properties &&
			(data._curr_param._kind == param_t::kind::unknown ||
			data._curr_param._kind == param_t::kind::in) &&
			data._curr_param._com_stars == 0)
		{
			// It's not interesting to record the original type here
			data._curr_param._com_type = data._curr_param._cpp_type = "LPCTSTR";
		}

		if (data._curr_param._optional && data._curr_param._default_value.empty())
		{
			if (data._curr_param._cpp_stars)
				data._curr_param._default_value = "nullptr";
			else
				data._curr_param._default_value = "0";
		}
	};
	grules.push("opt_const", "%empty | 'const'");
	data_t::_actions[grules.push("raw_type", "'BSTR'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "BSTR";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("BSTR");
				data._curr_vts = ret_to_vts("BSTR");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'CURRENCY'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "CURRENCY";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("CURRENCY");
				data._curr_vts = ret_to_vts("CURRENCY");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'DATE'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "DATE";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("DATE");
				data._curr_vts = ret_to_vts("DATE");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'double'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "double";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("double");
				data._curr_vts = ret_to_vts("double");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'IDispatch'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "IDispatch";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("IDispatch");
				data._curr_vts = ret_to_vts("IDispatch");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'IUnknown'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "IUnknown";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("IUnknown");
				data._curr_vts = ret_to_vts("IUnknown");
			}
		}
	};
	grules.push("raw_type", "int");
	data_t::_actions[grules.push("raw_type", "'int64'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			// It's not interesting to record the original type here
			data._curr_param._com_type = data._curr_param._cpp_type = "int64_t";
			data._seen_i64 = true;

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("int64_t");
				data._curr_vts = ret_to_vts("int64_t"); 
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "Name")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			const auto& results = data._results_stack.top();
			const auto& productions = data._productions_stack.top();

			data._curr_param._com_type =
				results.dollar(0, data_t::_gsm, productions).str();

			if (data._curr_param._com_type == "OLE_CANCELBOOL")
			{
				data._curr_param._cpp_type = "OLE_CANCELBOOL";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_CANCELBOOL");
					data._curr_vts = ret_to_vts("OLE_CANCELBOOL");
				}
			}
			else if (data._curr_param._com_type == "OLE_COLOR")
			{
				data._curr_param._cpp_type = "OLE_COLOR";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_COLOR");
					data._curr_vts = ret_to_vts("OLE_COLOR");
				}
			}
			else if (data._curr_param._com_type == "OLE_ENABLEDEFAULTBOOL")
			{
				data._curr_param._cpp_type = "OLE_ENABLEDEFAULTBOOL";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_ENABLEDEFAULTBOOL");
					data._curr_vts = ret_to_vts("OLE_ENABLEDEFAULTBOOL");
				}
			}
			else if (data._curr_param._com_type == "OLE_HANDLE")
			{
				data._curr_param._cpp_type = "OLE_HANDLE";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_HANDLE");
					data._curr_vts = ret_to_vts("OLE_HANDLE");
				}
			}
			else if (data._curr_param._com_type == "OLE_OPTEXCLUSIVE")
			{
				data._curr_param._cpp_type = "OLE_OPTEXCLUSIVE";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_OPTEXCLUSIVE");
					data._curr_vts = ret_to_vts("OLE_OPTEXCLUSIVE");
				}
			}
			else if (data._curr_param._com_type == "OLE_XPOS_CONTAINER")
			{
				data._curr_param._cpp_type = "OLE_XPOS_CONTAINER";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_XPOS_CONTAINER");
					data._curr_vts = ret_to_vts("OLE_XPOS_CONTAINER");
				}
			}
			else if (data._curr_param._com_type == "OLE_XPOS_HIMETRIC")
			{
				data._curr_param._cpp_type = "OLE_XPOS_HIMETRIC";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_XPOS_HIMETRIC");
					data._curr_vts = ret_to_vts("OLE_XPOS_HIMETRIC");
				}
			}
			else if (data._curr_param._com_type == "OLE_XPOS_PIXELS")
			{
				data._curr_param._cpp_type = "OLE_XPOS_PIXELS";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_XPOS_PIXELS");
					data._curr_vts = ret_to_vts("OLE_XPOS_PIXELS");
				}
			}
			else if (data._curr_param._com_type == "OLE_XSIZE_CONTAINER")
			{
				data._curr_param._cpp_type = "OLE_XSIZE_CONTAINER";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_XSIZE_CONTAINER");
					data._curr_vts = ret_to_vts("OLE_XSIZE_CONTAINER");
				}
			}
			else if (data._curr_param._com_type == "OLE_XSIZE_HIMETRIC")
			{
				data._curr_param._cpp_type = "OLE_XSIZE_HIMETRIC";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_XSIZE_HIMETRIC");
					data._curr_vts = ret_to_vts("OLE_XSIZE_HIMETRIC");
				}
			}
			else if (data._curr_param._com_type == "OLE_XSIZE_PIXELS")
			{
				data._curr_param._cpp_type = "OLE_XSIZE_PIXELS";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_XSIZE_PIXELS");
					data._curr_vts = ret_to_vts("OLE_XSIZE_PIXELS");
				}
			}
			else if (data._curr_param._com_type == "OLE_YPOS_CONTAINER")
			{
				data._curr_param._cpp_type = "OLE_YPOS_CONTAINER";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_YPOS_CONTAINER");
					data._curr_vts = ret_to_vts("OLE_YPOS_CONTAINER");
				}
			}
			else if (data._curr_param._com_type == "OLE_YPOS_HIMETRIC")
			{
				data._curr_param._cpp_type = "OLE_YPOS_HIMETRIC";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_YPOS_HIMETRIC");
					data._curr_vts = ret_to_vts("OLE_YPOS_HIMETRIC");
				}
			}
			else if (data._curr_param._com_type == "OLE_YPOS_PIXELS")
			{
				data._curr_param._cpp_type = "OLE_YPOS_PIXELS";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_YPOS_PIXELS");
					data._curr_vts = ret_to_vts("OLE_YPOS_PIXELS");
				}
			}
			else if (data._curr_param._com_type == "OLE_YSIZE_CONTAINER")
			{
				data._curr_param._cpp_type = "OLE_YSIZE_CONTAINER";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_YSIZE_CONTAINER");
					data._curr_vts = ret_to_vts("OLE_YSIZE_CONTAINER");
				}
			}
			else if (data._curr_param._com_type == "OLE_YSIZE_HIMETRIC")
			{
				data._curr_param._cpp_type = "OLE_YSIZE_HIMETRIC";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_YSIZE_HIMETRIC");
					data._curr_vts = ret_to_vts("OLE_YSIZE_HIMETRIC");
				}
			}
			else if (data._curr_param._com_type == "OLE_YSIZE_PIXELS")
			{
				data._curr_param._cpp_type = "OLE_YSIZE_PIXELS";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("OLE_YSIZE_PIXELS");
					data._curr_vts = ret_to_vts("OLE_YSIZE_PIXELS");
				}
			}
			else
			{
				data._curr_param._cpp_type.clear();
				data._curr_vt.clear();
				data._curr_vts.clear();
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'SAFEARRAY' '(' type ')'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			const auto& results = data._results_stack.top();
			const auto& productions = data._productions_stack.top();

			data._curr_param._com_type.assign(results.dollar(0, data_t::_gsm, productions).first,
				results.dollar(3, data_t::_gsm, productions).second);
			data._curr_param._cpp_type = "VARIANT";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("VARIANT");
				data._curr_vts = ret_to_vts("VARIANT");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'SCODE'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "SCODE";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("SCODE");
				data._curr_vts = ret_to_vts("SCODE");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'single'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			// It's not interesting to record the original type here
			data._curr_param._com_type = data._curr_param._cpp_type = "float";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("float");
				data._curr_vts = ret_to_vts("float");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'uint64'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			// It's not interesting to record the original type here
			data._curr_param._com_type = data._curr_param._cpp_type = "uint64_t";
			data._seen_i64 = true;

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("uint64_t");
				data._curr_vts = ret_to_vts("uint64_t");
			}
		}
	};
	grules.push("raw_type", "unsigned");
	data_t::_actions[grules.push("raw_type", "'VARIANT'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "VARIANT";

			if (!data._curr_param._default_value.empty())
			{
				if (data._curr_param._default_value.starts_with("_T(\""))
					data._curr_param._default_value =
						"COleVariant(" + data._curr_param._default_value + ')';
				else
					data._curr_param._default_value =
						std::format("COleVariant(long({}))", data._curr_param._default_value);
			}
			else if (data._curr_param._optional)
				data._curr_param._default_value = "COleVariant(DISP_E_PARAMNOTFOUND, VT_ERROR)";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("VARIANT");
				data._curr_vts = ret_to_vts("VARIANT");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'VARIANT_BOOL'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			// It's not interesting to record the original type here
			data._curr_param._com_type = data._curr_param._cpp_type = "BOOL";

			if (data._curr_param._default_value == "-1")
				data._curr_param._default_value = "TRUE";
			else if (data._curr_param._default_value == "0")
				data._curr_param._default_value = "FALSE";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("BOOL");
				data._curr_vts = ret_to_vts("BOOL");
			}
		}
	};
	data_t::_actions[grules.push("raw_type", "'void'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = "void";

			if ((!data._func && data._curr_param._kind == param_t::kind::unknown) ||
				data._curr_param._kind == param_t::kind::retval)
			{
				data._curr_param._cpp_type = "void";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("void");
					data._curr_vts.clear();
				}
			}
			else
			{
				data._curr_param._cpp_type = "VARIANT";

				if (data._cpp)
				{
					data._curr_vt = ret_to_vt("VARIANT");
					data._curr_vts = ret_to_vts("VARIANT");
				}
			}
		}
	};
	data_t::_actions[grules.push("int", "'char'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "char";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("char");
				data._curr_vts = ret_to_vts("char");
			}
		}
	};
	data_t::_actions[grules.push("int", "'int'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "int";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("int");
				data._curr_vts = ret_to_vts("int");
			}
		}
	};
	data_t::_actions[grules.push("int", "'long'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "long";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("long");
				data._curr_vts = ret_to_vts("long");
			}
		}
	};
	data_t::_actions[grules.push("int", "'short'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "short";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("short");
				data._curr_vts = ret_to_vts("short");
			}
		}
	};
	data_t::_actions[grules.push("int", "'wchar_t'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._com_type = data._curr_param._cpp_type = "wchar_t";

			if (data._cpp)
			{
				data._curr_vt = ret_to_vt("wchar_t");
				data._curr_vts = ret_to_vts("wchar_t");
			}
		}
	};
	data_t::_actions[grules.push("unsigned", "'unsigned' int")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			data._curr_param._cpp_type.insert(0, "unsigned ");
			data._curr_param._com_type = data._curr_param._com_type;

			if (data._cpp)
			{
				data._curr_vt.insert(3, 1, 'U');
				data._curr_vts.insert(4, 1, 'U');
			}
		}
	};
	grules.push("opt_stars", "%empty");
	data_t::_actions[grules.push("opt_stars", "opt_stars '*'")] =
		[](data_t& data)
	{
		++data._curr_param._com_stars;

		if (!(data._curr_param._cpp_type == "VARIANT" &&
			data._curr_param._com_type != data._curr_param._cpp_type))
		{
			++data._curr_param._cpp_stars;
		}
	};

	grules.push("opt_call", "%empty "
		"| '_stdcall' "
		"| '_cdecl' "
		"| '_pascal' "
		"| '_macpascal' "
		"| '_mpwcdecl' "
		"| '_mpwpascal'");

	grules.push("attr_list", "'[' attr_list ']'");
	grules.push("opt_attr_list", "%empty "
		"| '[' attr_list ']'");
	grules.push("attr_list", "attr "
		"| attr_list opt_comma attr");
	grules.push("opt_comma", "%empty | ','");
	// I have lumped all attributes together as it was unclear which
	// context all attributes occurred in from the MS documentation.
	grules.push("attr", "'appobject' "
		"| 'bindable' "
		"| 'control' "
		"| 'custom' '(' Uuid ',' number_string ')' "
		// String could be another data type
		"| 'custom' '(' '{' Uuid '}' ',' String ')' "
		"| 'default' "
		"| 'defaultbind'");
	data_t::_actions[grules.push("attr", "'defaultvalue' '(' number_string ')'")] =
		[](data_t& data)
	{
		if (data._curr_if)
		{
			const auto& results = data._results_stack.top();
			const auto& production = data._productions_stack.top();

			data._curr_param._default_value = results.dollar(2, data_t::_gsm, production).str();

			if (data._curr_param._default_value[0] == '"')
			{
				data._curr_param._default_value =
					"_T(" + data._curr_param._default_value + ')';
			}
		}
	};
	grules.push("attr", "'displaybind' "
		"| 'dllname' '(' String ')' "
		"| 'dual' "
		"| 'entry' '(' number_string ')' "
		"| 'helpcontext' '(' Number ')' "
		"| 'helpfile' '(' String ')'");
	data_t::_actions[grules.push("attr", "'helpstring' '(' String ')'")] = [](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();

		data._curr_attrs._helpstring = results.dollar(2, data_t::_gsm, productions).substr(1, 1);
	};
	data_t::_actions[grules.push("attr", "'hidden'")] =
		[](data_t& data)
	{
		data._curr_attrs._hidden = true;
	};
	data_t::_actions[grules.push("attr", "'id' '(' Number ')'")] =
		[](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();
		std::stringstream ss;

		ss << std::hex << results.dollar(2, data_t::_gsm, productions).str();
		ss >> data._curr_attrs._id;
	};
	grules.push("attr", "'immediatebind'");
	data_t::_actions[grules.push("attr", "'in' ")] = [](data_t& data)
	{
		using enum param_t::kind;

		if (data._curr_param._kind == out)
			data._curr_param._kind = in_out;
		else
			data._curr_param._kind = in;
	};
	data_t::_actions[grules.push("attr", "'lcid'")] = [](data_t& data)
	{
		// InvokeHelper() does not support LCID!
		data._curr_attrs._lcid = true;
	};
	grules.push("attr", "'lcid' '(' Number ')' "
		"| 'licensed' "
		"| 'noncreatable' "
		"| 'nonextensible' "
		"| 'notify' "
		"| 'odl' "
		// https://msdn.microsoft.com/en-us/library/windows/desktop/aa367129(v=vs.85).aspx
		"| 'oleautomation'");
	data_t::_actions[grules.push("attr", "'optional'")] = [](data_t& data)
	{
		data._curr_param._optional = true;
	};
	data_t::_actions[grules.push("attr", "'out'")] = [](data_t& data)
	{
		using enum param_t::kind;

		if (data._curr_param._kind == in)
			data._curr_param._kind = in_out;
		else
			data._curr_param._kind = out;
	};
	data_t::_actions[grules.push("attr", "'propget'")] = [](data_t& data)
	{
		data._curr_attrs._propget = true;
	};
	data_t::_actions[grules.push("attr", "'propput'")] = [](data_t& data)
	{
		data._curr_attrs._propput = true;
	};
	data_t::_actions[grules.push("attr", "'propputref'")] = [](data_t& data)
	{
		data._curr_attrs._propputref = true;
	};
	grules.push("attr", "'public' "
		"| 'range' '(' number_name ',' number_name ')'");
	data_t::_actions[grules.push("attr", "'readonly'")] = [](data_t& data)
	{
		data._curr_param._readonly = true;
	};
	grules.push("attr", "'requestedit'");
	data_t::_actions[grules.push("attr", "'restricted'")] = [](data_t& data)
	{
		data._curr_attrs._restricted = true;
	};
	data_t::_actions[grules.push("attr", "'retval'")] = [](data_t& data)
	{
		data._curr_param._kind = param_t::kind::retval;
	};
	data_t::_actions[grules.push("attr", "'source'")] =
		[](data_t& data)
	{
		data._curr_attrs._source = true;
	};
	data_t::_actions[grules.push("attr", "'uuid' '(' uuid ')'")] =
		[](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();

		data._curr_attrs._uuid = results.dollar(2, data_t::_gsm, productions).str();
	};
	grules.push("attr", "'vararg'"
		"| 'version' '(' Number ')'");

	grules.push("string_list", "String "
		"| string_list String");

	grules.push("number_string", "Number | String");
	grules.push("number_name", "Number | Name");
	data_t::_actions[grules.push("uuid", "Uuid")] = [](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();

		data._curr_attrs._uuid = results.dollar(0, data_t::_gsm, productions).str();
	};
	data_t::_actions[grules.push("uuid", "String")] = [](data_t& data)
	{
		const auto& results = data._results_stack.top();
		const auto& productions = data._productions_stack.top();

		data._curr_attrs._uuid = results.dollar(0, data_t::_gsm, productions).substr(1, 1);
	};
	parsertl::generator::build(grules, data_t::_gsm);

	// All keywords from Microsoft site:
	lrules.push_state("ATTRIBUTES");

	// Attributes:
	lrules.push("INITIAL", "\\[", grules.token_id("'['"), "ATTRIBUTES");

	// Determined by running strings.exe on IViewer.dll
	lrules.push("ATTRIBUTES", "appobject", grules.token_id("'appobject'"), ".");
	lrules.push("ATTRIBUTES", "bindable", grules.token_id("'bindable'"), ".");
	lrules.push("ATTRIBUTES", "control", grules.token_id("'control'"), ".");
	lrules.push("ATTRIBUTES", "custom", grules.token_id("'custom'"), ".");
	lrules.push("ATTRIBUTES", "default", grules.token_id("'default'"), ".");
	lrules.push("ATTRIBUTES", "defaultbind", grules.token_id("'defaultbind'"), ".");
	lrules.push("ATTRIBUTES", "defaultvalue", grules.token_id("'defaultvalue'"), ".");
	lrules.push("ATTRIBUTES", "displaybind", grules.token_id("'displaybind'"), ".");
	lrules.push("ATTRIBUTES", "dllname", grules.token_id("'dllname'"), ".");
	lrules.push("ATTRIBUTES", "dual", grules.token_id("'dual'"), ".");
	lrules.push("ATTRIBUTES", "entry", grules.token_id("'entry'"), ".");
	lrules.push("ATTRIBUTES", "helpcontext", grules.token_id("'helpcontext'"), ".");
	lrules.push("ATTRIBUTES", "helpfile", grules.token_id("'helpfile'"), ".");
	lrules.push("ATTRIBUTES", "helpstring", grules.token_id("'helpstring'"), ".");
	lrules.push("ATTRIBUTES", "hidden", grules.token_id("'hidden'"), ".");
	lrules.push("ATTRIBUTES", "id", grules.token_id("'id'"), ".");
	lrules.push("ATTRIBUTES", "immediatebind", grules.token_id("'immediatebind'"), ".");
	lrules.push("ATTRIBUTES", "in", grules.token_id("'in'"), ".");
	lrules.push("ATTRIBUTES", "lcid", grules.token_id("'lcid'"), ".");
	lrules.push("ATTRIBUTES", "licensed", grules.token_id("'licensed'"), ".");
	lrules.push("ATTRIBUTES", "noncreatable", grules.token_id("'noncreatable'"), ".");
	lrules.push("ATTRIBUTES", "nonextensible", grules.token_id("'nonextensible'"), ".");
	lrules.push("ATTRIBUTES", "notify", grules.token_id("'notify'"), ".");
	lrules.push("ATTRIBUTES", "odl", grules.token_id("'odl'"), ".");
	lrules.push("ATTRIBUTES", "oleautomation", grules.token_id("'oleautomation'"), ".");
	lrules.push("ATTRIBUTES", "optional", grules.token_id("'optional'"), ".");
	lrules.push("ATTRIBUTES", "out", grules.token_id("'out'"), ".");
	lrules.push("ATTRIBUTES", "propget", grules.token_id("'propget'"), ".");
	lrules.push("ATTRIBUTES", "propput", grules.token_id("'propput'"), ".");
	lrules.push("ATTRIBUTES", "propputref", grules.token_id("'propputref'"), ".");
	lrules.push("ATTRIBUTES", "public", grules.token_id("'public'"), ".");
	lrules.push("ATTRIBUTES", "range", grules.token_id("'range'"), ".");
	lrules.push("ATTRIBUTES", "readonly", grules.token_id("'readonly'"), ".");
	lrules.push("ATTRIBUTES", "requestedit", grules.token_id("'requestedit'"), ".");
	lrules.push("ATTRIBUTES", "restricted", grules.token_id("'restricted'"), ".");
	lrules.push("ATTRIBUTES", "retval", grules.token_id("'retval'"), ".");
	lrules.push("ATTRIBUTES", "source", grules.token_id("'source'"), ".");
	lrules.push("ATTRIBUTES", "uuid", grules.token_id("'uuid'"), ".");
	lrules.push("ATTRIBUTES", "vararg", grules.token_id("'vararg'"), ".");
	lrules.push("ATTRIBUTES", "version", grules.token_id("'version'"), ".");
	lrules.push("ATTRIBUTES", "\\]", grules.token_id("']'"), "INITIAL");

	// Keywords:
	// Determined by running strings.exe on IViewer.dll
	lrules.push("BSTR", grules.token_id("'BSTR'"));
	lrules.push("DATE", grules.token_id("'DATE'"));
	lrules.push("CURRENCY", grules.token_id("'CURRENCY'"));
	lrules.push("IDispatch", grules.token_id("'IDispatch'"));
	lrules.push("IUnknown", grules.token_id("'IUnknown'"));
	lrules.push("SAFEARRAY", grules.token_id("'SAFEARRAY'"));
	lrules.push("SCODE", grules.token_id("'SCODE'"));
	lrules.push("VARIANT", grules.token_id("'VARIANT'"));
	lrules.push("VARIANT_BOOL", grules.token_id("'VARIANT_BOOL'"));
	lrules.push("_cdecl", grules.token_id("'_cdecl'"));
	lrules.push("_macpascal", grules.token_id("'_macpascal'"));
	lrules.push("_mpwcdecl", grules.token_id("'_mpwcdecl'"));
	lrules.push("_mpwpascal", grules.token_id("'_mpwpascal'"));
	lrules.push("_pascal", grules.token_id("'_pascal'"));
	lrules.push("_stdcall", grules.token_id("'_stdcall'"));
	lrules.push("char", grules.token_id("'char'"));
	lrules.push("coclass", grules.token_id("'coclass'"));
	lrules.push("const", grules.token_id("'const'"));
	lrules.push("dispinterface", grules.token_id("'dispinterface'"));
	lrules.push("double", grules.token_id("'double'"));
	lrules.push("enum", grules.token_id("'enum'"));
	lrules.push("importlib", grules.token_id("'importlib'"));
	lrules.push("int", grules.token_id("'int'"));
	lrules.push("int64", grules.token_id("'int64'"));
	lrules.push("interface", grules.token_id("'interface'"));
	lrules.push("library", grules.token_id("'library'"));
	lrules.push("long", grules.token_id("'long'"));
	lrules.push("methods:", grules.token_id("'methods:'"));
	lrules.push("module", grules.token_id("'module'"));
	lrules.push("properties:", grules.token_id("'properties:'"));
	lrules.push("short", grules.token_id("'short'"));
	lrules.push("single", grules.token_id("'single'"));
	lrules.push("struct", grules.token_id("'struct'"));
	lrules.push("typedef", grules.token_id("'typedef'"));
	lrules.push("uint64", grules.token_id("'uint64'"));
	lrules.push("union", grules.token_id("'union'"));
	lrules.push("unsigned", grules.token_id("'unsigned'"));
	lrules.push("void", grules.token_id("'void'"));
	lrules.push("wchar_t", grules.token_id("'wchar_t'"));

	lrules.push("*", "[(]", grules.token_id("'('"), ".");
	lrules.push("*", "[)]", grules.token_id("')'"), ".");
	lrules.push("*", "[*]", grules.token_id("'*'"), ".");
	lrules.push("*", ",", grules.token_id("','"), ".");
	lrules.push(":", grules.token_id("':'"));
	lrules.push(";", grules.token_id("';'"));
	lrules.push("=", grules.token_id("'='"));
	lrules.push("*", "[{]", grules.token_id("'{'"), ".");
	lrules.push("*", "[}]", grules.token_id("'}'"), ".");
	lrules.push("[A-Z_a-z][0-9A-Z_a-z]*", grules.token_id("Name"));
	lrules.push("*", "-?\\d+([.]\\d+)?|0x[0-9A-Fa-f]{8}", grules.token_id("Number"), ".");
	lrules.push("*", R"(["]([^"\\]|\\.)*["])", grules.token_id("String"), ".");
	lrules.push("*", "[0-9A-Fa-f]{8}(-[0-9A-Fa-f]{4}){3}-[0-9A-Fa-f]{12}",
		grules.token_id("Uuid"), ".");
	lrules.push("[/][/].*|[/][*](?s:.)*?[*][/]", lexertl::rules::skip());
	lrules.push("*", "\\s+", lexertl::rules::skip(), ".");

	lexertl::generator::build(lrules, data_t::_lsm);
}
