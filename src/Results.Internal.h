#pragma once
#include <map>
#include <optional>
#include <string>
struct apx::Results::Internal {
	std::map<std::string, std::optional<std::string>> map;
};