#include "format/Indenter.h"
namespace format = apx::format;
using Indenter = format::Indenter;
std::string Indenter::indent() {
	++indentation;
	return "";
}
std::string Indenter::indent(const std::string &string, std::size_t levels) {
	const auto indentation = repeat(indentationSymbol, levels);
	std::string result = indentation;
	for (auto &&character : string) {
		if (character == format::EOL) {
			result += character;
			result += indentation;
		}
		else {
			result += character;
		}
	}
	return result;
}
std::string Indenter::unindent() {
	--indentation;
	return "";
}
std::string Indenter::wrap(const std::string &string, const std::size_t length) {
	std::string result = "";
	std::size_t column = 0;
	std::vector<std::string> tokens;
	std::string currentToken = "";
	const auto commitOutstanding = [&tokens, &currentToken]() {
		tokens.emplace_back(currentToken);
		currentToken = "";
	};
	/* Tokenize */
	for (auto &&character : string) {
		if (character == format::EOL) {
			if (currentToken.size()) {
				commitOutstanding();
			}
			tokens.emplace_back(std::string({ character }));
		}
		else {
			if (character != ' ') {
				currentToken += character;
			}
			else {
				commitOutstanding();
			}
		}
	}
	commitOutstanding();
	/* Use tokens to build wrapped string */
	for (auto &&token : tokens) {
		if (token.front() == format::EOL) {
			result += token;
			column = 0;
		}
		else {
			if (column && column + 1 < length) {
				result += " ";
				++column;
			}
			if (column + token.size() < length) {
				result += token;
			}
			else {
				result += format::EOL;
				result += token;
				column = 0;
			}
			column += token.size();
		}
	}
	return result;
}
std::string Indenter::fit(const std::string &string) {
	/* Wrap and indent */
	return indent(
		wrap(string, wrapWidth - indentationWidth * indentation),
		indentation
	);
}