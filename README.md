# arx
`arx` is a functional-style C++ library that allows you to parse command-line arguments.
## Features
- Automatically generated help
- CMake-consumable
- Options can take multiple arguments
- Configurable argument arity
- Numerical arguments
- Functional API
- Portable

## Quick start
This following example makes a `Parser` that accepts `--help` and `-h` with arbitrarily many input arguments. The input arguments are only valid if they can be parsed as numerical values. If the user doesn't specify any input numbers, the numbers `1` and `2` are assumed by default.
```c++
#include "arx/Parser.h"
int main(int argc, const char *argv[]) {
	auto parser = arx::Parser();
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
	const auto &&config = parser.parse(argc, argv);
	if (config.option["help"]) {
		parser.showHelp(argv);
	}
	else {
		for (auto &&input : config.argument["INPUT"].values) {
			std::cout << input.as<int>() << std::endl;
		}
	}
}
```
The program above will behave like this:
```bash
$ ./example
1
2
```
```bash
$ ./example 2 3 4
2
3
4
```
```bash
$ ./example one two
Parse error at argument `INPUT`: Expected value `one` to be numeric
```
```bash
$ ./example --help
SYNTAX:
        ./example [OPTIONS] [INPUT]...
ARGUMENTS:
        INPUT[={1, 2}]
                The input numbers to process
                Must be a number.
OPTIONS:
        -h, --help
                Displays this help
```
## Requirements
You need a compiler that supports C++17.
## Installation
Cloning, compiling and linking `arx` can be fully automated. One way to do this in CMake is shown below; you can also find this example [here](https://github.com/kdex/arx/tree/master/example).
```cmake
cmake_minimum_required(VERSION 3.15)
project(example)
include(ExternalProject)
set(INSTALLATION_PREFIX installation)
ExternalProject_Add(arx-git
	CMAKE_ARGS
		-DCMAKE_INSTALL_PREFIX:PATH=${INSTALLATION_PREFIX}
		-DCMAKE_BUILD_TYPE=Release
	GIT_REPOSITORY git@github.com:kdex/arx.git
	PREFIX ${CMAKE_CURRENT_BINARY_DIR}/arx-git
	UPDATE_DISCONNECTED true
)
ExternalProject_Get_Property(arx-git BINARY_DIR)
set(INSTALLATION_PATH "${BINARY_DIR}/${INSTALLATION_PREFIX}")
add_executable(${PROJECT_NAME})
add_dependencies(${PROJECT_NAME} arx-git)
target_include_directories(${PROJECT_NAME} PRIVATE "${INSTALLATION_PATH}/include")
target_link_directories(${PROJECT_NAME} PRIVATE "${INSTALLATION_PATH}/lib")
target_link_libraries(${PROJECT_NAME} PRIVATE arx)
target_sources(${PROJECT_NAME} PRIVATE src/example.cc)
```
Apart from this, you can also consume `arx` via `find_package(arx)` once you have a build or installation.
## API
### Parser
A `Parser` can contain `Argument`s and `Option`s.
#### `.parse(argc, argv)`
Creates a set of parse results.
#### `.showHelp(argv)`
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