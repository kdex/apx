#pragma once
#include <functional>
#include <optional>
#include "Argument.h"
#include "ArgumentContainer.h"
namespace apx {
	class Option : public ArgumentContainer {
		/* TODO: Later? */
// 		Option() = default;
// 		Option(const Option &option) = delete;
// 		Option &operator=(const Option &option) = delete;
		public:
			Option &flag(const char flag);
			char shortFlag() const;
			Option &flag(const std::string &&flag);
			const std::string &longFlag() const;
			Option &description(const std::string &&text);
			const std::string &description() const;
			bool isSpecified = false;
			const Argument operator[](const std::string &name) const;
			const Argument operator[](const char *string) const;
			const Argument operator[](std::size_t index) const;
			operator bool() {
				return isSpecified;
			}
			std::string providedShortFlag = "", providedLongFlag = "", providedDescription = "";
// 			friend struct ArgumentContainer;
			friend std::ostream &operator<<(std::ostream &os, const Option &option) {
				os
					<< "Option("
					<< "flags = [" << option.providedShortFlag << ", " << option.providedLongFlag << "], "
					<< "arguments = {" << option.arguments
					<< " })";
				return os;
			}
		private:
			const Argument findArgument(const std::string &findName) const;
	};
}