#include "Server.hpp"

#define LOCALHOST   "127.0.0.1"
#define DEFAULTHTTP "8080"

void sigHandler(int signum){
    std::cout << "Received signal (" << signum << ")." << std::endl;
    exit(1); // Needs attention
}

/*
    int main(int argc, char **argv){
        if (argc != 2) {
            std::cerr << "usage : " << argv[0] << " [configuration file]" << std:: endl;
            return 1;
        }

        std::string ipAdress, port; // Maybe make port a vector instead (multiport)? 

        if (loadConfig(argv[1], ipAdress, port) == -1){
            std::cerr << "Failed to load configuration from " << argv[1] << std::endl;
        }
    }
*/
int main(){
    Server server(LOCALHOST, DEFAULTHTTP);

    signal(SIGINT, sigHandler);
    signal(SIGTERM, sigHandler);

    server.run();
    server.closeServer();
    return (0);
}