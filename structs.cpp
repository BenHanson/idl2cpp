#include "stdafx.h"

#include <format>
#include <parsertl/lookup.hpp>
#include <lexertl/memory_file.hpp>
#include "structs.h"

void param_t::clear()
{
	_kind = kind::unknown;
	_help.clear();
	_com_type.clear();
	_com_stars = 0;
	_cpp_type.clear();
	_cpp_stars = 0;
	_name.clear();
	_vt.clear();
	_vts.clear();
	_default_value.clear();
	_optional = false;
	_readonly = false;
}

void func_t::clear()
{
	_id = static_cast<std::size_t>(~0);
	_hidden = false;
	_restricted = false;
	_help.clear();
	_kind = kind::function;
	_ret_com_type.clear();
	_ret_cpp_type.clear();
	_ret_stars = 0;
	_name.clear();
	_params.clear();
}

void attrs_t::clear()
{
	_defaultvalue.clear();
	_helpstring.clear();
	_hidden = false;
	_id = static_cast<std::size_t>(~0);
	_lcid = false;
	_propget = false;
	_propput = false;
	_propputref = false;
	_restricted = false;
	_source = false;
	_uuid.clear();
}

void data_t::parse(const std::string& pathname)
{
	lexertl::memory_file mf(pathname.c_str());

	if (!mf.data())
	{
		throw std::runtime_error("Unable to open " + pathname + '\n');
	}

	lexertl::iterator<const char*, lexertl::state_machine, lexertl::crmatch> iter
		(mf.data(), mf.data() + mf.size(), _lsm);

	if (_path.empty())
	{
		_path = pathname.substr(0, pathname.rfind('\\') + 1);
	}

	_results_stack.emplace(iter->id, _gsm);
	_productions_stack.emplace();

	auto& results = _results_stack.top();
	auto& productions = _productions_stack.top();

	while (results.entry.action != parsertl::action::accept &&
		results.entry.action != parsertl::action::error)
	{
		if (results.entry.action == parsertl::action::reduce)
		{
			auto action = _actions.find(results.entry.param);

			if (action != _actions.cend())
			{
				action->second(*this);
			}
		}

		parsertl::lookup(iter, _gsm, results, productions);
	}

	if (results.entry.action == parsertl::action::error)
	{
		const std::string err = std::format("Parse error: {}({}): '{}'\n",
			pathname,
			std::count(mf.data(), iter->first, '\n') + 1,
			iter->str());

		throw std::runtime_error(err);
	}

	_results_stack.pop();
	_productions_stack.pop();
	_files.insert(pathname);
}

void data_t::post_process()
{
	for (auto& [ifs, functions] : _interfaces)
	{
		for (auto& func : functions)
		{
			if (func._ret_cpp_type.empty())
				com_to_cpp(func._ret_com_type, func._ret_cpp_type,
					func._ret_vt, func._ret_vts);

			for (auto& param : func._params)
			{
				if (param._cpp_type.empty())
					com_to_cpp(param._com_type, param._cpp_type, param._vt,
						param._vts);
			}
		}
	}
}

void data_t::com_to_cpp(std::string& com_type, std::string& cpp_type,
	std::string& vt, std::string& vts)
{
	if (com_type.empty())
	{
		com_type = "void";
		cpp_type = "void";
		vt = "VT_EMPTY";
		vts = "VTS_NONE";
	}
	else if (_coclass.contains(std::pair(_namespace.back(), com_type)))
	{
		cpp_type = "IDispatch";
		vt = "VT_DISPATCH";
		vts = "VTS_DISPATCH";
	}
	else if (_enum_set.contains(com_type))
	{
		cpp_type = com_type;
		vt = "VT_I4";
		vts = "VTS_I4";
	}
	else if (std::ranges::find_if(_interfaces, [&com_type](const auto& pair)
		{
			return com_type == pair.first._name;
		}) != _interfaces.cend())
	{
		cpp_type = com_type;
		vt = "VT_DISPATCH";
		vts = "VTS_DISPATCH";
	}
	else if (std::ranges::find_if(_coclass, [&com_type](const auto& pair)
		{
			return pair.second == com_type;
		}) != _coclass.cend())
	{
		cpp_type = "IDispatch";
		vt = "VT_DISPATCH";
		vts = "VTS_DISPATCH";
	}
	else
	{
		auto iter = _typedefs.find(com_type);

		if (iter == _typedefs.cend())
		{
			cpp_type = "VARIANT";
			vt = "VT_VARIANT";
			vts = "VTS_VARIANT";
		}
		else
		{
			com_to_cpp(std::get<2>(iter->second), cpp_type, vt, vts);

			if (!cpp_type.empty())
				cpp_type = com_type;
		}
	}
}
