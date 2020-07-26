#pragma once
#include <iostream>
#include <string>
namespace arx {
	class Castable {
		public:
			Castable(const std::string &value);
			operator std::string() const noexcept;
			template<typename T, typename = typename std::enable_if<std::is_same<T, std::string>::value>::type>
			std::string as() const noexcept {
				return value;
			}
			template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
			T as() const {
				if (std::is_integral<T>()) {
					return static_cast<T>(std::stoll(value, nullptr, 0));
				}
				if (std::is_floating_point<T>()) {
					return static_cast<T>(std::stold(value));
				}
				else {
					throw std::runtime_error("Unknown arithmetic type cast");
				}
			}
			const std::string value;
		private:
			friend std::ostream &operator<<(std::ostream &os, const Castable &castable) {
				os << castable.value;
				return os;
			}
	};
}