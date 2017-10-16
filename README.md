# apx
`apx` is a functional-style C++ library that allows you to parse command-line arguments.
## Features
- Automatically generated help
- CMake-consumable
- Options can take multiple arguments
- Configurable argument arity
- Numerical arguments
- Functional API
- Portable

## Example

This following example makes a `Parser` that accepts `--help` and `-h` with arbitrarily many input arguments. The input arguments are only valid if they can be parsed as numerical values. If the user doesn't specify any input numbers, the numbers `1` and `2` are assumed by default.
```c++
#include "apx/Parser.h"
auto parser = apx::Parser();
parser
	.option([](auto &&config) {
		config
			.flag('h')
			.flag("help")
			.description("Displays this help");
	})
	.argument([](auto &&config) {
		config
			.name("INPUT")
			.description("The input numbers to process")
			.optional({ 1, 2 })
			.number()
			.variadic();
	});
auto config = parser.parse(argc, argv);
if (config.option["help"]) {
	parser.help(argv);
}
else {
	for (auto &&input : config.argument["INPUT"].values) {
		std::cout << input.as<int>() << std::endl;
	}
}
```
## Requirements
You need a compiler that supports C++17.
## Installation
Cloning, compiling and linking `apx` can be fully automated. One way to do this in CMake is shown below.
```cmake
include(ExternalProject)
ExternalProject_Add(apx-git
	GIT_REPOSITORY git@github.com:kdex/apx.git
	PREFIX ${CMAKE_BINARY_DIR}/apx
	CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
	UPDATE_DISCONNECTED true
)
ExternalProject_Get_Property(apx-git INSTALL_DIR)
include_directories(${INSTALL_DIR}/include)
link_directories(${INSTALL_DIR}/lib)
add_dependencies(your-executable apx-git)
target_link_libraries(your-executable apx)
```
Apart from this, you can also consume `apx` via `find_package(apx)` once you have a build or installation.
## API
### Parser
A `Parser` can contain `Argument`s and `Option`s.
#### `.parse(argc, argv)`
Creates a set of parse results.
#### `.help(argv)`
Displays a help.
#### `.option(config-function)`
Adds an `Option` to the `Parser` which can be configured in `config-function`.
#### `.argument(config-function)`
Adds an `Argument` to the `Parser` which can be configured in `config-function`.
#### `.argument(n, config-function)`
Adds an `Argument` with arity `n` to the `Parser` which can be configured in `config-function`. This is identical to an `Argument` with `.min(n)` and `.max(n)`.
### Options
An `Option` is an element that can be specified via its long flag (`--long-flag`) or it's short flag (`-s`). Options may contain an arbitrary number of `Argument`s.
#### `.flag(character)`
Allows the user to specify the option using the short flag -`character`.
#### `.flag(string)`
Allows the user to specify the option using the long flag --`string`.
#### `.description(string)`
Adds a description text for an `Option`.
#### `.argument(config-function)`
Adds an `Argument` to the `Option` which can be configured in `config-function`.
#### `.argument(n, config-function)`
Adds an `Argument` with arity `n` to the `Option` which can be configured in `config-function`. This is identical to an `Argument` with `.min(n)` and `.max(n)`.
### Arguments
An `Argument` is an element that can appear both inside as well as outside of an `Option`.
#### `.name()`
Sets the name of an argument. This is both helpful to retrieve the value later, or for the mere purpose of making the automatically generated help more enlightening.
#### `.number()`
Parses the `Argument` as a numerical value. If the user specifies a non-numerical value for this argument, this results in a parse error. If the `Argument` is within an `Option`, this helps the parser avoiding ambiguities in the grammar.
For example, consider:
```bash
$ your-program --integer -1
```
Without specifying `--integer`'s first argument as a number, this parses `-1` as a short option. Using `.number()` avoids this.
#### `.description(text)`
Sets the `Argument` description.
#### `.min(n)`
Sets the lower bound for the arity of an `Argument` to the `n`. For example, consider the following invocations of `your-program` as valid:
```bash
$ your-program
$ your-program input
$ your-program input input
```
In this example, you can see that the minimum arity of `input` is `0`, as it may appear with a minimum of `0` occurrences.
Had it been `2`, then only the last invocation would have been valid.
#### `.max(n)`
Sets the upper bound for the arity of an `Argument` to the `n`. Also, see `.min`.
#### `.optional()`
Shorthand for `min(0)`.
#### `.optional(value || { value1, value2, ... })`
Same as `.optional()`, but also sets one or multiple default values that should always be applied, unless specified otherwise by the user.
#### `.restrict(value || { value1, value2, ... })`
Restricts the set of possible values that the user may enter to the values provided.
#### `.variadic()`
Shorthand for `max(std::numeric_limits<std::size_t>::max())`.