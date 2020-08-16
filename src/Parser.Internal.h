#pragma once
#include <utility>
#include <vector>
#include <string>
#include "Argument.h"
#include "ArgumentContainer.h"
#include "Option.h"
#include "OptionContainer.h"
#include "Results.h"
struct arx::Parser::Internal : ArgumentContainer, OptionContainer {
	class HelpGenerator {
		public:
			constexpr HelpGenerator(const Internal *internal)
				: internal(internal) {}
			void showHelp(const char *argv[]) const {
				using namespace format;
				auto indenter = Indenter();
				std::stringstream ss;
				ss
					<< "SYNTAX:"
					<< EOL
					<< indenter.indent()
					<< indenter.fit(printSyntax(argv))
					<< indenter.unindent();
				if (!internal->arguments.empty()) {
					ss
						<< EOL
						<< "ARGUMENTS:"
						<< EOL
						<< indenter.indent()
						<< indenter.fit(printArguments(internal->arguments))
						<< indenter.unindent();
				}
				if (!internal->options.empty()) {
					ss
						<< EOL
						<< "OPTIONS:"
						<< EOL
						<< indenter.indent()
						<< indenter.fit(printOptions())
						<< indenter.unindent();
				}
				ss << EOL;
				std::cout << ss.str();
			}
		private:
			const Internal *internal;
			const std::string printSyntax(const char *argv[]) const {
				std::stringstream ss;
				ss << argv[0];
				if (!internal->options.empty()) {
					ss << " [OPTIONS]";
				}
				for (auto &&argument : internal->arguments) {
					ss << " " << static_cast<std::string>(argument);
				}
				return ss.str();
			}
			const std::string printOptions() const {
				using namespace format;
				auto indenter = Indenter();
				std::stringstream ss;
				auto i = 0u;
				for (auto &&option : internal->options) {
					std::string title = "";
					if (!option.providedShortFlag.empty()) {
						title += internal->shortPrefix + option.shortFlag();
					}
					if (!option.providedShortFlag.empty() && !option.longFlag().empty()) {
						title += ", ";
					}
					if (!option.longFlag().empty()) {
						title += internal->longPrefix + option.longFlag();
					}
					ss
						<< indenter.fit(title)
						<< " "
						<< format::join(([&option] {
							std::vector<std::string> names;
							for (auto &&argument : option.arguments) {
								names.emplace_back(static_cast<std::string>(argument));
							}
							return names;
						})(), " ")
						<< indenter.indent();
					if (!option.description().empty()) {
						ss
							<< EOL
							<< indenter.fit(option.description());
					}
					if (!option.arguments.empty()) {
						if (!option.description().empty()) {
							ss << EOL;
						}
						ss
							<< EOL
							<< indenter.fit(printArguments(option.arguments));
					}
					ss << indenter.unindent();
					if (i != internal->options.size() - 1) {
						ss << EOL;
					}
					++i;
				}
				return ss.str();
			}
			const std::string printArguments(const std::vector<Argument> &arguments) const {
				using namespace format;
				auto indenter = Indenter();
				std::stringstream ss;
				auto i = 0u;
				for (auto &&argument : arguments) {
					if (argument.hasMetadata) {
						const auto hasDescription = !argument.description().empty();
						ss << indenter.fit(argument.name());
						if (!argument.defaultValues.empty()) {
							ss
								<< "[="
								<< ((argument.defaultValues.size() == 1)
									? argument.defaultValues[0]
									: "{" + format::join(argument.defaultValues) + "}"
								)
								<< "]";
						}
						if (hasDescription) {
							ss
								<< EOL
								<< indenter.indent()
								<< indenter.fit(argument.description());
						}
						if (argument.minimum != 1 || argument.maximum != 1) {
							if (argument.minimum == argument.maximum) {
								ss
									<< EOL
									<< indenter.fit("Must specify exactly " + std::to_string(argument.minimum) + " values.");
							}
							else {
								if (argument.minimum > 1) {
									ss
										<< EOL
										<< indenter.fit("Must specify at least " + std::to_string(argument.minimum) + " values.");
								}
								if (argument.maximum > 1 && argument.maximum != std::numeric_limits<std::size_t>::max()) {
									ss
										<< EOL
										<< indenter.fit("Must specify at most " + std::to_string(argument.maximum) + " values.");
								}
							}
						}
						if (argument.isNumber) {
							ss
								<< EOL
								<< indenter.fit("Must be a number.");
						}
						if (!argument.restrictions.empty()) {
							if (hasDescription) {
								ss << EOL;
							}
							ss
								<< indenter.fit("Must be one of:")
								<< EOL
								<< EOL;
							auto v = 0u;
							for (auto &&value : argument.restrictions) {
								ss << indenter.fit("- ") + value;
								if (v != argument.restrictions.size() - 1) {
									ss << EOL;
								}
								++v;
							}
						}
						if (i != arguments.size() - 1) {
							ss << EOL;
						}
						if (hasDescription) {
							indenter.unindent();
						}
					}
					++i;
				}
				return ss.str();
			}
	};
	using ArgumentIterator = std::vector<Argument>::iterator;
	bool isOptionShort = false;
	template<typename T>
	std::optional<ArgumentIterator> changeOption(const T &flag) {
		using namespace std::literals;
		if (std::is_same<T, char>::value) {
			isOptionShort = true;
		}
		auto &&optionIterator = findOption(flag);
		if (optionIterator != options.end()) {
			/* Close any active option */
			closeOption();
			/* We might need a reference to this option later. */
			activeOption = &*optionIterator;
			if (activeOption->isSpecified) {
				throw std::runtime_error("Option has been specified more than once");
			}
			return std::make_optional(optionIterator->arguments.begin());
		}
		else {
			throw std::runtime_error("Unrecognized option: "s + flag);
		}
	}
	bool hasMoreArguments(ArgumentIterator &iterator) {
		auto &&next = std::next(iterator);
		if (activeOption) {
			return next != activeOption->arguments.end();
		}
		else {
			return next != arguments.end();
		}
	}
	bool isArgumentFull(ArgumentIterator &iterator) {
		auto &&argument = *iterator;
		const auto stored = argument.values.size();
		return stored >= argument.maximum;
	}
	void closeOption() {
		if (!activeOption) {
			return;
		}
		/*
		* There was an option specified before.
		* This means that the previous option was not fully specified.
		* We have to check if this was legal.
		*/
		for (auto &&argument : activeOption->arguments) {
			if (argument.values.size() < argument.minimum) {
				throw std::runtime_error("Not enough arguments specified");
			}
		}
		activeOption->isSpecified = true;
		activeOption = nullptr;
		// TODO: Remove?
		optionArgumentIterator = {};
	}
	std::size_t consume(const std::string &start, ArgumentIterator &iterator, bool entireToken) {
		using namespace std::literals;
		if (iterator == arguments.end()) {
			/* Move back to last argument so that error can read activeArgument */
			std::advance(iterator, -1);
			throw std::runtime_error("Too many arguments (unexpected token: "s + start + ")"s);
		}
		auto &&argument = *iterator;
		std::string consumed;
		/* Consume exactly one argument (either a string or a number) */
		if (argument.isNumber) {
			const auto numericError = std::runtime_error("Expected value `"s + start + "` to be numeric"s);
			try {
				const auto number = format::parseNumber(start);
				argument.values.emplace_back(number);
				if (entireToken && number.size() != start.size()) {
					throw numericError;
				}
				consumed = number;
			}
			catch (...) {
				throw numericError;
			}
		}
		else {
			argument.values.emplace_back(start);
			consumed = start;
		}
		/* Check if value meets restriction requirements */
		if (!argument.restrictions.empty() && argument.restrictions.find(consumed) == argument.restrictions.end()) {
			throw std::runtime_error("Value did not match any of the allowed choices");
		}
		/* Mark arguments as specified */
		if (!activeOption) {
			/* We've consumed an argument */
			if (iterator != arguments.end()) {
				activeArgument->isSpecified = true;
			}
		}
		if (isArgumentFull(iterator)) {
			if (hasMoreArguments(iterator)) {
				std::advance(iterator, 1);
			}
			else if (activeOption) {
				closeOption();
			}
			else {
				/* Invalidate argument iterator */
				std::advance(iterator, 1);
			}
		}
		return consumed.size();
	}
	void parseDashedBlock(const std::string &token) {
		std::string block;
		if (activeOption && (*optionArgumentIterator)->isNumber) {
			/* If we can consume a number, always prefer it over a flag */
			block = token;
		}
		else {
			/* Otherwise, it's an option block */
			closeOption();
			block = token.substr(shortPrefix.size());
		}
		std::size_t characterIndex = 0;
		while (characterIndex < block.size()) {
			if (activeOption) {
				characterIndex += consume(std::string(&block[characterIndex]), *optionArgumentIterator, false);
			}
			else {
				const auto character = block[characterIndex];
				/* No option yet, try to set */
				optionArgumentIterator = changeOption(character);
				++characterIndex;
			}
		}
	}
	void merge(std::vector<Argument> &arguments) {
		for (auto &&argument : arguments) {
			if (!argument.isSpecified && !argument.defaultValues.empty()) {
				std::copy(argument.defaultValues.begin(), argument.defaultValues.end(), std::back_inserter(argument.values));
			}
		}
	}
	const Results parse(int argc, const char *argv[]) {
		using namespace std::literals;
		try {
			bool parseOptions = true;
			const std::vector<std::string> tokens(argv + 1, argv + argc);
			activeArgument = arguments.begin();
			for (auto &&token : tokens) {
				if (token == optionDelimiter) {
					parseOptions = false;
					closeOption();
				}
				else if (
					parseOptions &&
					std::equal(std::begin(longPrefix), std::end(longPrefix), std::begin(token)) &&
					token.size() > longPrefix.size()
				) {
					optionArgumentIterator = changeOption(token.substr(longPrefix.size()));
				}
				else if (
					parseOptions &&
					std::equal(std::begin(shortPrefix), std::end(shortPrefix), std::begin(token)) &&
					token.size() > shortPrefix.size()
				) {
					parseDashedBlock(token);
				}
				else {
					if (activeOption) {
						if (activeOption->arguments.empty()) {
							closeOption();
							consume(token, activeArgument, true);
						}
						else {
							consume(token, *optionArgumentIterator, true);
						}
					}
					else {
						if (!arguments.empty()) {
							consume(token, activeArgument, true);
						}
						else {
							throw std::runtime_error("Invalid attempt to specify argument `"s + token + "` with no arguments being defined");
						}
					}
				}
			}
			/* If there is any option still open after parsing all tokens, try to close it */
			closeOption();
			/* Have all necessary arguments been read? */
			if (activeArgument != arguments.end()) {
				const auto argument = *activeArgument;
				if (argument.values.size() < argument.minimum) {
					throw std::logic_error("Not enough arguments specified");
				}
			}
			/* Apply defaults */
			merge(arguments);
			for (auto &&option: options) {
				merge(option.arguments);
			}
			return Results(arguments, options);
		}
		catch (std::exception &e) {
			std::cerr << "Parse error";
			if (!arguments.empty()) {
				std::cerr << " at ";
				if (activeOption) {
					std::string optionName = "";
					optionName += isOptionShort
						? shortPrefix + activeOption->shortFlag()
						: longPrefix + activeOption->longFlag();
					std::cerr << "option `" << optionName << "`, argument `" << (*optionArgumentIterator)->name() << "`";
				}
				else {
					std::string argumentName = activeArgument->name();
					std::cerr << "argument `" << argumentName << "`";
				}
			}
			std::cerr << ": " << e.what() << std::endl;
			std::exit(EXIT_FAILURE);
		}
	}
	const std::string shortPrefix = "-";
	const std::string longPrefix = "--";
	const std::string optionDelimiter = longPrefix;
	const HelpGenerator helpGenerator = HelpGenerator(this);
	Option *activeOption = nullptr;
	ArgumentIterator activeArgument = arguments.begin();
	std::optional<ArgumentIterator> optionArgumentIterator;
};