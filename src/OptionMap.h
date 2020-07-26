#pragma once
#include <algorithm>
#include <vector>
#include "Option.h"
namespace arx {
	class OptionMap {
		public:
			OptionMap(const std::vector<Option> &options) : options(options) {}
			auto operator[](char flag) const {
				return findOption(flag);
			}
			auto operator[](const std::string &flag) const {
				return findOption(flag);
			}
		private:
			const std::vector<Option> &options;
			const Option findOption(char character) const {
				const auto &&iterator = std::find_if(options.begin(), options.end(), [=](auto &&option) {
					return option.shortFlag() == character;
				});
				if (iterator == options.end()) {
					throw std::invalid_argument("No option named: " + std::to_string(character));
				}
				return *iterator;
			}
			const Option findOption(const std::string &text) const {
				const auto &&iterator = std::find_if(options.begin(), options.end(), [=](auto &&option) {
					return option.longFlag() == text;
				});
				if (iterator == options.end()) {
					throw std::invalid_argument("No option named: " + text);
				}
				return *iterator;
			}
	};
}