#include "stdafx.h"

#include <filesystem>
#include "output_enums.h"
#include "output_events_header.h"
#include "output_events_source.h"
#include "output_fwd_decl.h"
#include "output_header.h"
#include "output_source.h"
#include "parser.h"
#include "structs.h"

// Bit flags
enum class switches
{
	none,
	enums,
	events_header,
	events_source,
	fwd_decls,
	header,
	source
};

void throw_switch()
{
	throw std::runtime_error("Switches are mutually exclusive");
}

std::pair<switches, std::string> params(int argc, const char* argv[])
{
	std::pair<switches, std::string> ret;

	if (argc == 1)
		throw std::runtime_error("USAGE: idl2cpp <pathname.idl> [/enums "
			"| /events_h | /events_cpp | /fwd_decls | /h | /cpp]");

	ret.first = switches::none;

	for (int i = 1; i < argc; ++i)
	{
		const char* param = argv[i];

		if (::_stricmp(param, "/enums") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::enums;
		}
		else if (::_stricmp(param, "/events_h") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::events_header;
		}
		else if (::_stricmp(param, "/events_cpp") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::events_source;
		}
		else if (::_stricmp(param, "/fwd_decls") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::fwd_decls;
		}
		else if (::_stricmp(param, "/h") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::header;
		}
		else if (::_stricmp(param, "/cpp") == 0)
		{
			if (ret.first != switches::none)
				throw_switch();

			ret.first = switches::source;
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

	for (auto iter = fs::directory_iterator("D:\\IDL"),
		end = fs::directory_iterator(); iter != end; ++iter)
	{
		if (iter->path().extension() == ".idl")
		{
			const std::string pathname = iter->path().string();
			data_t data;

			data.parse(pathname);
		}
	}
}

int main(int argc, const char* argv[])
{
	try
	{
		auto [flag, pathname] = params(argc, argv);
		data_t data;

		build_parser();
		//parse_all();

		if (flag == switches::source)
			data._cpp = true;

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
