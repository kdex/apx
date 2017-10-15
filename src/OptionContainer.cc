#include <algorithm>
#include "OptionContainer.h"
using Option = apx::Option;
using OptionContainer = apx::OptionContainer;
void OptionContainer::option(const std::function<void(Option &)> &&setup) {
	Option option;
	setup(option);
	if (option.providedShortFlag.empty() && option.longFlag().empty()) {
		/* TODO: We might want to auto-generate a long flag, perhaps? */
		throw std::runtime_error("Unable to create option that lacks both short and long flag");
	}
	option.dedupe();
	options.emplace_back(option);
}
bool OptionContainer::hasOption(char character) const noexcept {
	return std::find_if(options.begin(), options.end(), [=](auto &&option) {
		return option.shortFlag() == character;
	}) != options.end();
}
std::vector<Option>::iterator OptionContainer::findOption(char character) noexcept {
	return std::find_if(options.begin(), options.end(), [=](auto &&option) {
		return option.shortFlag() == character;
	});
}
std::vector<Option>::iterator OptionContainer::findOption(const std::string &text) noexcept {
	return std::find_if(options.begin(), options.end(), [=](auto &&option) {
		return option.longFlag() == text;
	});
}