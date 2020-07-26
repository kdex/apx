#pragma once
#include <algorithm>
#include <vector>
namespace arx {
	class ArgumentMap {
		public:
			ArgumentMap(const std::vector<Argument> &arguments) : arguments(arguments) {}
			auto operator[](std::size_t index) const {
				return arguments[index];
			}
			auto operator[](const std::string &name) const {
				const auto &&iterator = std::find_if(arguments.begin(), arguments.end(), [=](auto &&option) {
					return option.name() == name;
				});
				if (iterator == arguments.end()) {
					throw std::invalid_argument("No argument named: " + name);
				}
				return *iterator;
			}
		private:
			const std::vector<Argument> &arguments;
	};
}