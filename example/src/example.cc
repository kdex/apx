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