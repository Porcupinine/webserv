#include "myServer.hpp"

#define LOCALHOST   "127.0.0.1"
#define DEFAULTHTTP "8080"

int main(){
    MyServer test(LOCALHOST, DEFAULTHTTP);
    test.startListening();

    return (0);
}