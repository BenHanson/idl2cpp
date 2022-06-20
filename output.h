#pragma once

#include <string>

struct data_t;

std::string idl2cpp_comment();
std::string base(const std::string& str, const data_t& data);
void output_if_namespace(const std::string& name, const data_t& data,
	std::ostream& ss);
std::string convert_prop(const std::string& type, data_t& data);
void convert_ret(std::string& type, std::size_t& stars);
void output_enum_namespace(const std::string& name, const data_t& data,
	std::ostream& ss);
void format_output(std::stringstream& ss, const char c, const std::size_t tabs,
	std::size_t offset = 0);
std::string parse_idl_comment();
