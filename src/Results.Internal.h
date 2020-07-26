#pragma once
#include <map>
#include <optional>
#include <string>
struct arx::Results::Internal {
	std::map<std::string, std::optional<std::string>> map;
};