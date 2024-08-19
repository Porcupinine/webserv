/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/07/31 15:50:10 by lpraca-l          #+#    #+#             */
/*   Updated: 2024/08/19 12:56:48 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/ParseRequest.hpp"
#include "../../inc/CgiHandler.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <filesystem>
#include <cctype>
#include "../../inc/defines.hpp"
#include <cerrno>
#include <cstdio>
#include <sys/epoll.h>

//TODO need the server info

namespace {
	void addToEpoll(SharedData* shared, int fd){
		epoll_event newEvent {};

		newEvent.data.fd = fd;
		newEvent.events = EPOLLHUP;
		newEvent.data.ptr = shared;
		if (epoll_ctl(shared->epoll_fd, EPOLL_CTL_ADD, fd, &newEvent) < 0)
			std::cerr<<"Failed to poll\n"; //TODO error handle
//			throw ServerException("Failed to register with epoll");
	}

	char **getEnv(ParseRequest &request, SharedData* shared) {
		auto copy = request.getHeaders();
		char **env = new char *[copy.size() + 5];
		size_t count = 0;
		for (const auto &copy: copy) {
			std::string reform = copy.first;
			std::transform(reform.begin(), reform.end(), reform.begin(), ::toupper);
			std::replace(reform.begin(), reform.end(), '-', '_');
			std::string temp = reform + '=' + copy.second;
			env[count] = new char[copy.first.size() + copy.second.size() + 2];
			size_t pos = 0;
			for (char &x: temp) {
				env[count][pos] = x;
				pos++;
			}
			env[count][pos] = '\0';
			count++;
		}
		std::string tmp = "REQUEST_METHOD=" + request.getMethod();
		env[count] = new char[tmp.size()];
		std::strcpy(env[count], tmp.data());
		count++;
		tmp = "QUERY_STRING=" + request.getQuery();
		env[count] = new char[tmp.size()];
		std::strcpy(env[count], tmp.data());
		count++;
		tmp = "UPLOAD_DIR=" + request.getAbsPath() + shared->server_config->upload_dir; //TODO remove fucking point??
		env[count] = new char[tmp.size()];
		std::strcpy(env[count], tmp.data());
		count++;
		tmp = "SERVER=" + shared->server_config->server_name;
		env[count] = new char[tmp.size()];
		std::strcpy(env[count], tmp.data());
		count++;
		env[count] = nullptr;
		return env;
	}

	void freeEnv(char **env) {
		for (int x = 0; env[x] != nullptr; x++) {
			delete env[x];
		}
		delete[] env;
	}

	int runChild(ParseRequest &request, int pipeRead, int pipeWrite, SharedData* shared) {
		std::string cgiPtah = request.getPath();
		char *argv[] = {cgiPtah.data(), nullptr}; //path and NULL
		char **env = getEnv(request, shared);
		int x = 0;
		std::cout << "------env----\n";
		while (env[x] != nullptr) {
			std::cout << env[x] << "\n";
			x++;
		}
		std::cout << "------endv----\n";
		if (dup2(pipeRead, STDIN_FILENO) == -1 || dup2(pipeWrite, STDOUT_FILENO) == -1) {
			std::cerr << "Leave the kids alone!\n";
			close(pipeRead); // Close unused read end
			close(pipeWrite); // Close the original pipe write end
			freeEnv(env);
			return 1;
		}
		if (execve(argv[0], argv, env) == -1) {
			std::cerr << "This is no middle age\n";
			std::cerr << strerror(errno)<< "\n";
			close(pipeRead); // Close unused read end
			close(pipeWrite); // Close the original pipe write end
			freeEnv(env);
			return 1;
		}
		return 0;
	}
}

int cgiHandler(SharedData* shared, ParseRequest& request) {
//	(void) shared;
	std::cout<<"file path: \'"<<request.getPath()<<"\'\n";
	try {
		(void)std::filesystem::exists(request.getPath());
	}
	catch (std::exception &ex) {
		std::cerr<<"Error: "<<ex.what();
	}
	int pipeParentToChild[2]; // 0 - child parent read, 1 - parent write
	int pipeChildToParent[2]; // 0 - parent read, 1 - child write

	if (pipe(pipeParentToChild) == -1) {
		std::cerr<<"Pipe failed, you gotta call Mario!\n";
		return 1;
	}
	if (pipe(pipeChildToParent) == -1) {
		std::cerr<<"Pipe child failed, you gotta call Mario!\n";
		return 1;
	}

	int pid = fork();
	if (pid == -1) {
		std::cerr<<"There are no forks, you can try the philos!\n";
		return 1;
	}
	if (pid == 0) {
		close(pipeParentToChild[1]);
		close(pipeChildToParent[0]);
		runChild(request, pipeParentToChild[0], pipeChildToParent[1], shared);
	} else {
		close(pipeParentToChild[0]);
		close(pipeChildToParent[1]);
		addToEpoll(shared, pipeParentToChild[1]);
		addToEpoll(shared, pipeChildToParent[0]);
		auto body = request.getBodyMsg();
		if(write(pipeParentToChild[1], body.data(), body.size()) == -1){
			if (errno == EPIPE) {
				std::cerr << "Broken pipe while writing to child process!\n";
			}
		}
		close(pipeParentToChild[1]);
		std::cout<<"body done!\n";

		ssize_t buffLen = 0;
		std::string response;
		std::string buffer(BUFFER_SIZE, '\0');
		while ((buffLen = ::read(pipeChildToParent[0], buffer.data(), BUFFER_SIZE)) > 0) {
			response.append(buffer.data(), buffLen);
		}
		if (buffLen < 0) {
			std::cerr<<"Failed to read!\n";
		}
		std::cout<<"------------response-------------\n\'"<<response<<"\'\n";
		shared->response = response + "\0";
		shared->status = Status::writing;
		close(pipeParentToChild[0]); // Close the read end after reading
		int status;
		if (waitpid(pid, &status, 0) == -1) {
			std::cerr << "Failed to wait for child process\n";
		} else if (WIFEXITED(status)) {
			std::cout << "Child exited with status " << WEXITSTATUS(status) << "\n";
		}
	}
	return 0;
}

//TODO make index page with cgi to show cookie after user fills the name and intra login?

//TODO open pipes on Luś end
/* getting a struct with server info and epolFD
 * get the parameters needed for the env and add the pipe fds to the epoll
 * add pipes to poll, fix things that take server info*/

