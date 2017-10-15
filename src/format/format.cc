#include <algorithm>
#include <numeric>
#include <stdexcept>
#include "format/format.h"
namespace format = apx::format;
std::string format::repeat(const std::string &string, std::size_t n) {
	auto copy = std::string(string);
	if (!n) {
		copy.clear();
		copy.shrink_to_fit();
		return copy;
	}
	if (n == 1 || copy.empty()) {
		return copy;
	}
	const auto period = copy.size();
	if (period == 1) {
		copy.append(n - 1, copy.front());
		return copy;
	}
	copy.reserve(n * period);
	std::size_t m = 2;
	while (m < n) {
		copy += copy;
		m <<= 1;
	}
	copy.append(copy.c_str(), (n - (m / 2)) * period);
	return copy;
}
std::string format::join(const std::vector<std::string> &range, const std::string &separator) {
	if (range.empty()) {
		return std::string();
	}
	return std::accumulate(std::next(std::begin(range)), std::end(range), range.front(), [&separator](const auto &result, const auto &value) {
		return result + separator + value;
	});
}
std::string format::parseNumber(const std::string &input) {
	std::size_t llLength = 0, ldLength = 0;
	try {
		std::stoll(input.data(), &llLength, 0);
	}
	catch (...) {}
	try {
		std::stold(input.data(), &ldLength);
	}
	catch (...) {}
	if (!llLength && !ldLength) {
		/* No number read */
		throw std::invalid_argument("Input was not a number");
	}
	else {
		const auto numericLength = std::max({ llLength, ldLength });
		const auto number = input.substr(0, numericLength);
		return number;
	}
}