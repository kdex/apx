#include <utility>
#include "Parser.h"
#include "Parser.Internal.h"
using Parser = apx::Parser;
using Results = apx::Results;
Parser::Parser() : internal(std::make_unique<Internal>()) {}
Parser::~Parser() = default;
Parser::Parser(const Parser &other) : internal(std::make_unique<Internal>(*other.internal)) {}
Parser &Parser::operator=(const Parser &other) {
	if (this != &other) {
		internal.reset(std::make_unique<Internal>(*other.internal).get());
	}
	return *this;
}
Parser::Parser(Parser &&other) noexcept = default;
Parser &Parser::operator=(Parser &&other) noexcept = default;
Parser &Parser::argument(const std::function<void(Argument &)> &&setup) {
	internal->argument(std::forward<decltype(setup)>(setup));
	return *this;
}
Parser &Parser::argument(std::size_t n, const std::function<void(Argument &)> &&setup) {
	internal->argument(n, std::forward<decltype(setup)>(setup));
	return *this;
}
Parser &Parser::help(const char *argv[]) {
	internal->helpGenerator.help(argv);
	return *this;
}
Parser &Parser::option(const std::function<void(Option &)> &&setup) {
	internal->option(std::forward<decltype(setup)>(setup));
	return *this;
}
const Results Parser::parse(int argc, const char *argv[]) {
	return internal->parse(argc, argv);
}