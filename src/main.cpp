// #include "../inc/Config.hpp"
#include "../inc/WebServ.hpp"

int main(int argc, char **argv){
	// if (argc != 2) {
	// 	std::cout << "Please provide me with a file." << std::endl;
	// 	return 1;
	// }



	try {

		WebServ server(argc, argv);
		server.run();
	// Config test(argv[1]);
	// if (test.hasErrorOccurred()) {
	// 	std::cout << RED << test.buildErrorMessage(test.getError()) << RESET << std::endl;
	// } else {
	// 	test.printConfigs();
	// }
	} catch (Config::ParseException &e) {
		std::cout << "Fuck " << e.what() << std::endl;
	} catch (std::exception &e) {
		std::cout << "Fuck#2 " << e.what() << std::endl;
	}


}