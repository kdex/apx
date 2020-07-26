#pragma once
#include <string>
#include <sstream>
#include <vector>
namespace apx::format {
	std::string repeat(const std::string &string, std::size_t n);
	std::string join(const std::vector<std::string> &range, const std::string &separator = ", ");
	std::string parseNumber(const std::string &input);
	constexpr std::size_t indentationWidth = 8;
	constexpr auto indentationSymbol = "\t";
	constexpr auto wrapWidth = 80;
	constexpr auto EOL = '\n';
}