#include "Castable.h"
using Castable = arx::Castable;
Castable::Castable(const std::string &value) : value(value) {}
Castable::operator std::string() const noexcept {
	return value;
}