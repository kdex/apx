#pragma once
#include <map>
#include <memory>
#include "Argument.h"
#include "ArgumentMap.h"
#include "Option.h"
#include "OptionMap.h"
namespace arx {
	class Results {
		public:
			Results(const std::vector<Argument> &arguments, const std::vector<Option> &options);
			~Results();
			Results(Results &&other);
			Results &operator=(Results &&other) noexcept = default;
			const std::vector<Argument> arguments;
			ArgumentMap argument;
			const std::vector<Option> options;
			OptionMap option;
		private:
			struct Internal;
			std::unique_ptr<Internal> internal;
	};
}