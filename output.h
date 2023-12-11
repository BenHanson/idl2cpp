#pragma once

#include "structs.h"

struct data_t;

std::string idl2cpp_comment();
std::string to_lower(const std::string& str);
std::string base(const std::string& str, const data_t& data);
void output_if_namespace(const std::string& name, const data_t& data,
	std::ostream& ss);
std::string convert_prop(const std::string& type, const data_t& data);
void convert_ret(std::string& type);
void output_enum_namespace(const std::string& name, const data_t& data,
	std::ostream& ss);
void format_output(std::stringstream& ss, const char c, const std::size_t tabs,
	std::size_t offset = 0);

std::string vt_to_member(const std::string& com_type, const std::size_t com_stars,
	const std::string& vt, const std::map<std::string, enum_data>& enum_map);
