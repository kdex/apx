#pragma once
#include <functional>
#include <vector>
#include "Argument.h"
namespace apx {
	struct ArgumentContainer {
		ArgumentContainer &argument(const std::function<void(Argument &)> &&setup);
		ArgumentContainer &argument(std::size_t n, const std::function<void(Argument &)> &&setup);
		std::vector<Argument> arguments;
		void dedupe();
	};
}