#pragma once
#include <functional>
#include "Option.h"
namespace arx {
	class OptionContainer {
		public:
			void option(const std::function<void(Option &)> &&setup);
			bool hasOption(char character) const noexcept;
			std::vector<Option>::iterator findOption(char character) noexcept;
			std::vector<Option>::iterator findOption(const std::string &text) noexcept;
			std::vector<Option> options;
	};
}