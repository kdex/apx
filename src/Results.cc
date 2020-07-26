#include "Argument.h"
#include "Option.h"
#include "Results.h"
#include "Results.Internal.h"
using Results = arx::Results;
Results::Results(const std::vector<Argument> &arguments, const std::vector<Option> &options)
	: arguments(arguments)
	, argument(arguments)
	, options(options)
	, option(options)
	, internal(std::make_unique<Internal>()) {}
Results::~Results() = default;
Results::Results(Results &&other) = default;