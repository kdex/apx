#include "Parser.h"
int main(int argc, const char *argv[]) {
	auto parser = apx::Parser();
	parser
		.option([](auto &&config) {
			config
				.flag('s')
				.flag("set-coordinates")
				.description("Spawns the player at a set of coordinates. The two extra arguments are only needed for demo purposes; for example, to create a description text to show that line wrapping works.")
				.argument([](auto &&config) {
					config
						.name("x")
						.description("The x-coordinate to spawn the player at")
						.number();
				})
				.argument(2, [](auto &&config) {
					config
						.name("enemy-position")
// 						.optional()
						.description("The enemy's x and y coordinates");
				})
				.argument([](auto &&config) {
					config
						.name("y")
						.description("The y-coordinate to spawn the player at")
// 						.number()
						.optional(0);
				})
				.argument([](auto &&config) {
					config
						.name("seed")
						.description("Some random seeds")
						.optional({ 1, 2, 3 })
						.max(5);
				});
		})
		.option([](auto &&config) {
			config
				.flag('t')
				.argument([](auto &&config) {
					config
						.name("team")
						.description("The player's team")
						.restrict({
							"red",
							"blue"
						})
						.optional({
							"red"
						});
				});
		})
		.option([](auto &&config) {
			config
				.flag('h')
				.flag("help")
				.description("Prints this help message");
		})
		.argument([](auto &&config) {
			config
				.name("OUTPUT")
				.number()
				.description("The output file");
		})
		.argument([](auto &&config) {
			config
				.name("INPUT")
				.description("The input files")
				.optional("blah.txt")
				.variadic();
		})
		.help(argv);
	auto &&config = parser.parse(argc, argv);
	for (auto &&value : config.argument["INPUT"].values) {
		std::cout << value.as<float>() << std::endl;
	}
	for (auto &&argument : config.option['s'].arguments) {
		for (auto &&value : argument.values) {
			std::cout << value.as<std::string>() << std::endl;
		}
	}
}
// TODO: reparsing, `providedName`