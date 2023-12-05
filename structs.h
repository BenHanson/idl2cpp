#pragma once

#include <lexertl/iterator.hpp>
#include <parsertl/match_results.hpp>
#include <parsertl/state_machine.hpp>
#include <parsertl/token.hpp>

struct param_t
{
	enum class kind
	{
		unknown, in, out, in_out, retval
	};

	kind _kind = kind::unknown;
	std::string _help;
	std::string _com_type;
	std::size_t _com_stars = 0;
	std::string _cpp_type;
	std::size_t _cpp_stars = 0;
	std::string _name;
	std::string _vt;
	std::string _vts;
	std::string _default_value;
	bool _optional = false;
	bool _readonly = false;

	void clear();
};

struct func_t
{
	enum class kind
	{
		function, propget, propput, propputref
	};

	std::size_t _id = static_cast<std::size_t>(~0);
	bool _hidden = false;
	bool _restricted = false;
	std::vector<std::string> _help;
	kind _kind = kind::function;
	std::string _ret_com_type;
	std::string _ret_cpp_type;
	std::size_t _ret_stars = 0;
	std::string _ret_vt;
	std::string _ret_vts;
	std::string _name;
	std::vector<param_t> _params;

	void clear();
};

struct interface_t
{
	bool _hidden = false;
	std::string _uuid;
	std::size_t _level = 0;
	std::string _namespace;
	std::string _name;
	std::string _help;
};

struct type_conv
{
	const char* _in;
	const char* _out;
};

struct enum_data
{
	std::string _namespace;
	std::string _help;
	std::size_t _level = 0;
	std::map<std::string, std::string> _enums;
};

struct attrs_t
{
	std::string _defaultvalue;
	std::string _helpstring;
	bool _hidden = false;
	std::size_t _id = static_cast<std::size_t>(~0);
	bool _lcid = false;
	bool _propget = false;
	bool _propput = false;
	bool _propputref = false;
	bool _restricted = false;
	bool _source = false;
	std::string _uuid;

	void clear();
};

struct data_t;

using actions_t = std::map<std::size_t, void(*) (data_t&)>;

struct data_t
{
	using token = parsertl::token<lexertl::citerator>;
	inline static parsertl::state_machine _gsm;
	inline static lexertl::state_machine _lsm;
	inline static actions_t _actions;

	bool _afx_ext_class = true;
	bool _cpp = false;
	bool _enum_class = true;
	bool _seen_i64 = false;
	std::string _path;
	std::set<std::string> _files;
	std::vector<std::string> _namespace;
	std::map<std::string, std::string> _inherits;
	std::set<std::string> _includes;

	bool _func = false;
	std::string _lib_help;
	interface_t* _curr_if = nullptr;
	attrs_t _curr_attrs;
	param_t _curr_param;
	std::string _curr_vt;
	std::string _curr_vts;

	bool _properties = false;

	std::stack<parsertl::match_results> _results_stack;
	std::stack<token::token_vector> _productions_stack;

	std::set<std::string> _enum_set;
	std::map<std::string, std::string> _enums;
	std::map<std::string, enum_data> _enum_map;
	std::map<std::string, std::tuple<std::size_t, std::string, std::string>> _typedefs;
	std::vector<std::pair<interface_t, std::vector<func_t>>> _interfaces;
	std::set<std::pair<std::string, std::string>> _coclass;
	std::set<std::pair<std::string, std::string>> _events;

	void parse(const std::string& pathname);
	void post_process();

	void com_to_cpp(std::string &com_type, std::string& cpp_type,
		std::string& vt, std::string& vts);
};
