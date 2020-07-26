#include "Argument.h"
#include <limits>
using Argument = apx::Argument;
Argument &Argument::name(const std::string &text) {
	providedName = text;
	hasMetadata = true;
	return *this;
}
const std::string &Argument::name() const {
	return providedName;
}
Argument &Argument::description(const std::string &text) {
	providedDescription = text;
	hasMetadata = true;
	return *this;
}
const std::string &Argument::description() const {
	return providedDescription;
}
Argument &Argument::optional() {
	return min(0);
}
Argument &Argument::optional(const std::string &value) {
	defaultValues.emplace_back(value);
	return optional();
}
Argument &Argument::optional(std::initializer_list<std::string> &&values) {
	defaultValues = values;
	return optional();
}
Argument &Argument::min(const std::size_t min) {
	minimum = min;
	return *this;
}
std::size_t Argument::min() const {
	return minimum;
}
Argument &Argument::number() {
	isNumber = true;
	return *this;
}
Argument &Argument::max(const std::size_t max) {
	maximum = max;
	return *this;
}
std::size_t Argument::max() const {
	return maximum;
}
Argument &Argument::restrict(const std::initializer_list<std::string> &set) {
	restrictions = set;
	return *this;
}
Argument &Argument::variadic() {
	maximum = std::numeric_limits<std::size_t>::max();
	return *this;
}
Argument::operator std::string() const {
	auto result = minimum ?
		"<" + name() + ">" :
		"[" + name() + "]";
	if (maximum > 1) {
		result += "...";
	}
	return result;
}
void Argument::validateDefaults() const {
	/*
	* The user may have provided defaults.
	* These don't go through parsing and need to be validated.
	*/
	if (isNumber) {
		for (auto &&value : defaultValues) {
			try {
				format::parseNumber(value);
			}
			catch (...) {
				throw std::invalid_argument("Error in `" + name() + "`: Default value `" + value + "` is not a valid number");
			}
		}
	}
	const auto amount = defaultValues.size();
	if (amount && amount < minimum) {
		throw std::invalid_argument("Error in `" + name() + "`: Only " + std::to_string(amount) + " out of " + std::to_string(minimum) + " default values specified");
	}
	if (amount > maximum) {
		for (auto &&x : defaultValues) {
			std::cout << x << std::endl;
		}
		throw std::invalid_argument("Error in `" + name() + "`: Too many default arguments specified. (" + std::to_string(amount) + " present, but only " + std::to_string(maximum) + " allowed)");
	}
	if (!restrictions.empty()) {
		for (auto &&value : defaultValues) {
			if (restrictions.find(value) == restrictions.end()) {
				throw std::invalid_argument("Error in `" + name() + "`: Default value `" + value + "` contradicts restrictions.");
			}
		}
	}
}