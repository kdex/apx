#pragma once
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <type_traits>
#include <vector>
#include "Castable.h"
#include "format/format.h"
/* TODO: Remove */
template<typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vector) {
	os << "[";
	bool first = true;
	for (auto &&x : vector) {
		if (first) {
			first = false;
		}
		else {
			os << ", ";
		}
		os << x;
	}
	os << "]";
	return os;
}
namespace apx {
	class Argument {
		public:
			Argument &name(const std::string &text);
			const std::string &name() const;
			Argument &description(const std::string &text);
			const std::string &description() const;
			Argument &optional();
			Argument &optional(const std::string &value);
			template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
			Argument &optional(const T &&value) {
				defaultValues.emplace_back(std::to_string(value));
				return optional();
			}
			Argument &optional(std::initializer_list<std::string> &&values);
			template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
			Argument &optional(std::initializer_list<T> &&values) {
				for (auto &&value : values) {
					defaultValues.emplace_back(std::to_string(value));
				}
				return optional();
			}
			Argument &min(const std::size_t min);
			std::size_t min() const;
			Argument &number();
			Argument &max(const std::size_t max);
			std::size_t max() const;
			Argument &restrict(const std::initializer_list<std::string> &set);
			template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
			Argument &restrict(std::initializer_list<T> &&values) {
				for (auto &&value : values) {
					restrictions.emplace(std::to_string(value));
				}
				return *this;
			}
			Argument &variadic();
			template<typename T, typename = typename std::enable_if<std::is_same<T, std::string>::value>::type>
			std::string as() const {
				return values.front();
			}
			template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
			T as() const {
				if (std::is_integral<T>()) {
					return static_cast<T>(std::stoll(values.front(), nullptr, 0));
				}
				if (std::is_floating_point<T>()) {
					return static_cast<T>(std::stold(values.front()));
				}
				else {
					throw std::runtime_error("Unknown arithmetic type cast");
				}
			}
			operator std::string() const;
			void validateDefaults() const;
			bool isSpecified = false;
			bool isNumber = false;
			bool hasMetadata = false;
			std::string providedName = "arg";
			std::string providedDescription = "";
			std::vector<std::string> defaultValues;
			std::size_t minimum = 1, maximum = 1;
			std::set<std::string> restrictions;
			std::vector<Castable> values;
		private:
			friend std::ostream &operator<<(std::ostream &os, const Argument &argument) {
				os
					<< "Argument["
					<< "name = " << argument.name() << ", "
					<< "values = " << argument.values
					<< "]";
				return os;
			}
	};
}