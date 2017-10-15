#include <functional>
#include "ArgumentContainer.h"
using Argument = apx::Argument;
using ArgumentContainer = apx::ArgumentContainer;
ArgumentContainer &ArgumentContainer::argument(const std::function<void(Argument &)> &&setup) {
	auto &&argument = Argument();
	setup(argument);
	if (!arguments.empty()) {
		/* User has specified arguments before */
		auto &&storedArgument = arguments.back();
		if (!storedArgument.minimum && argument.minimum) {
			throw std::invalid_argument("Error in argument `" + argument.name() + "`: Can not specify a required argument after an optional argument");
		}
		if (storedArgument.minimum != storedArgument.maximum) {
			/* Optional arguments are okay */
			if (!(storedArgument.minimum == 0 && storedArgument.maximum == 1)) {
				throw std::invalid_argument("Error in argument `" + argument.name() + "`: A variadic range with variable parameters can only be specified at the end. However, `" + storedArgument.name() + "` conflicts with this.");
			}
		}
	}
	if (argument.minimum && argument.defaultValues.size()) {
		throw std::invalid_argument("Error in `" + argument.name() + "`: Default values in required arguments can never be applied");
	}
	argument.validateDefaults();
	arguments.emplace_back(argument);
	return *this;
}
ArgumentContainer &ArgumentContainer::argument(std::size_t n, const std::function<void(Argument &)> &&setup) {
	if (n) {
		this->argument(std::move(setup));
		auto &&last = arguments.back();
		/* If the user didn't mark this as optional… */
		if (last.minimum) {
			last.minimum = n;
			last.maximum = n;
		}
		else {
			throw std::invalid_argument("Error in argument `" + last.name() + "`: Can not mark an argument with exactly " + std::to_string(n) + " values as optional");
		}
	}
	return *this;
}
void ArgumentContainer::dedupe() {
	using Appearances = std::vector<std::size_t>;
	auto map = std::unordered_map<std::string, Appearances>();
	std::size_t i = 0;
	for (auto &&argument : arguments) {
		auto &&name = argument.name();
		if (map.find(name) == map.end()) {
			map.emplace(name, Appearances({ i }));
		}
		else {
			auto &&appearances = (*map.find(name)).second;
			appearances.emplace_back(i);
		}
		++i;
	}
	for (auto &&[name, appearances] : map) {
		if (appearances.size() > 1) {
			std::size_t nthAppearance = 1;
			for (auto &&index : appearances) {
				const auto newName = name + std::to_string(nthAppearance++);
				arguments[index].providedName = newName;
			}
		}
	}
}