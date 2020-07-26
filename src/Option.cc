#include <exception>
#include "Argument.h"
#include "Option.h"
using Argument = arx::Argument;
using Option = arx::Option;
Option &Option::flag(const char flag) {
	providedShortFlag = flag;
	return *this;
}
char Option::shortFlag() const {
	return providedShortFlag.front();
}
Option &Option::flag(const std::string &&flag) {
	providedLongFlag = flag;
	return *this;
}
const std::string &Option::longFlag() const {
	return providedLongFlag;
}
Option &Option::description(const std::string &&text) {
	providedDescription = text;
	return *this;
}
const std::string &Option::description() const {
	return providedDescription;
}
const Argument Option::findArgument(const std::string &findName) const {
	const auto &&iterator = (std::find_if(arguments.begin(), arguments.end(), [=](auto &&argument) {
		return argument.name() == findName;
	}));
	if (iterator == arguments.end()) {
		throw std::runtime_error("No such argument: " + findName);
	}
	return *iterator;
}
const Argument Option::operator[](const std::string &name) const {
	return findArgument(name);
}
const Argument Option::operator[](const char *string) const {
	return findArgument(string);
}
const Argument Option::operator[](std::size_t index) const {
	return arguments[index];
}