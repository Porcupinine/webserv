#include <iostream>
#include <exception>

class SomeException : public std::exception {
	public:
		SomeException(const std::string& msg);
		const char* what() const noexcept override;

	private:
		std::string	_message;
};

SomeException::SomeException(const std::string& msg) : _message("Some important message, needing to be caught: " + msg) {}

const char* SomeException::what() const noexcept {
	return _message.c_str();
}

int main(){
	try {
		throw SomeException("HEEL BELANGRIJK!!");
	} catch (const SomeException& e){
		std::cerr << e.what() << std::endl;
	}
}
