#pragma once
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include "format/Indenter.h"
#include "Option.h"
#include "Results.h"
namespace arx {
	class Parser {
		public:
			Parser();
			~Parser();
			Parser(const Parser &other);
			Parser &operator=(const Parser &other);
			Parser(Parser &&other) noexcept;
			Parser &operator=(Parser &&other) noexcept;
			Parser &argument(const std::function<void(Argument &)> &&setup);
			Parser &argument(std::size_t n, const std::function<void(Argument &)> &&setup);
			Parser &option(const std::function<void(Option &)> &&setup);
			const Results parse(int argc, const char *argv[]);
			Parser &showHelp(const char *argv[]);
		private:
			struct Internal;
			std::unique_ptr<Internal> internal;
	};
}