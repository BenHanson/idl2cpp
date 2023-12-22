#include "stdafx.h"

#include <filesystem>
#include "output_enums.h"
#include "output_events_header.h"
#include "output_events_source.h"
#include "output_fwd_decl.h"
#include "output_header.h"
#include "output_source.h"
#include "parser.h"
#include <span>
#include "structs.h"

[[noreturn]] static void throw_switch()
{
	throw std::runtime_error("Switches are mutually exclusive");
}

std::pair<switches, std::string> params(std::span<const char*> params, data_t& data)
{
	std::pair<switches, std::string> ret;

	if (params.empty())
		throw std::runtime_error("USAGE: idl2cpp <pathname.idl> [/enums "
			"| /events_h | /events_cpp | /fwd_decls | /h [/no_afx] | /cpp] "
			"[/name <if name>]");

	ret.first = switches::none;

	for (std::size_t idx = 0, size = params.size(); idx < size; ++idx)
	{
		const char* param = params[idx];

		if (::strcmp(param, "/enums") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::enums;
		}
		else if (::strcmp(param, "/events_h") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::events_header;
		}
		else if (::strcmp(param, "/events_cpp") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::events_source;
		}
		else if (::strcmp(param, "/fwd_decls") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::fwd_decls;
		}
		else if (::strcmp(param, "/h") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::header;
		}
		else if (::strcmp(param, "/cpp") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::source;
		}
		else if (::strcmp(param, "/no_afx") == 0)
		{
			data._afx_ext_class = false;
		}
		else if (::strcmp(param, "/name") == 0)
		{
			++idx;

			if (idx == size)
				throw std::runtime_error("Missing interface name");

			data._filter = params[idx];
		}
		else
		{
			if (!ret.second.empty())
				throw std::runtime_error("Only one input file allowed");

			ret.second = param;
		}
	}

	return ret;
}

void parse_all()
{
	namespace fs = std::filesystem;

	for (auto iter = fs::directory_iterator("D:\\Ben\\Dev\\IDL"),
		end = fs::directory_iterator(); iter != end; ++iter)
	{
		if (iter->path().extension() == ".idl")
		{
			const std::string pathname = iter->path().string();
			data_t data;

			data._output = switches::events_source;
			data.parse(pathname);
		}
	}
}

int main(int argc, const char* argv[])
{
	try
	{
		data_t data;
		auto [flag, pathname] = params(std::span(argv + 1, argc - 1), data);

		data._output = flag;
		build_parser();
		//parse_all();

		data.parse(pathname);

		switch (flag)
		{
		case switches::none:
			std::cout << pathname << " parsed OK.";
			break;
		case switches::enums:
			output_enums(data);
			break;
		case switches::events_header:
			output_events_header(data);
			break;
		case switches::events_source:
			output_events_source(data);
			break;
		case switches::fwd_decls:
			output_fwd_decl(data);
			break;
		case switches::header:
			output_header(data);
			break;
		case switches::source:
			output_source(data);
			break;
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}

	return 0;
}
