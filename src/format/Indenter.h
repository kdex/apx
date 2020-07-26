#pragma once
#include "format.h"
namespace arx::format {
	class Indenter {
		public:
			std::string indent();
			std::string indent(const std::string &string, std::size_t levels);
			std::string unindent();
			std::string wrap(const std::string &string, const std::size_t length);
			std::string fit(const std::string &string);
		private:
			std::size_t indentation = 0;
	};
}