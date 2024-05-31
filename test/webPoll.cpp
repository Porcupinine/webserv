//bool NetworkSocket::socketLoop(){
//
//	// Start listening on specified port
//	if (listen(this->listeningSocket, this->backlog) < 0) {
//		this->raiseSocketExceptionEvent("Failed to start listening.");
//		return false;
//	}
//
//	std::cout << "Listening for connections on port " << this->listening_port << std::endl;
//
//	// Setup pollfd set and add listening socket to the set
//	this->pfd.fd = this->listeningSocket; //
//	this->pfd.events = POLLIN;
//	this->pfd.revents = 0;
//	this->readfds.push_back(this->pfd);
//
//	while(this->run){
//
//		char buf[256];
//
//		// Poll on all file descriptors
//		if (poll(&this->readfds[0], this->readfds.size(), -1) < 0) {
//			this->raiseSocketExceptionEvent("poll() failed");
//			return false;
//		}
//
//		// Loop through all descriptors and check which ones are readable
//		for (int j = 0; j <  this->readfds.size(); j++) {
//
//			// If its the listening socket continue
//			if (this->readfds[j].revents == 0)
//				continue;
//
//			int sd = this->readfds[j].fd;
//
//			// There is data to read?
//			if (this->readfds[j].revents & POLLIN) {
//
//				// If its on the listening socket its an incomin connection
//				if (sd == this->listeningSocket) {
//					sockaddr_in address;
//					socklen_t addrlen = sizeof(address);
//
//					// Accept new connection
//					int new_socket = accept(this->listeningSocket, (struct sockaddr *) &address, &addrlen);
//					if (new_socket < 0) {
//						this->raiseSocketExceptionEvent("Failed to accept incoming connection.");
//						return false;
//					}
//
//					// Information about the new connection
//					this->raiseClientConnectedEvent(new_socket);
//
//					// Add new connection to fdset
//					this->pfd.fd = new_socket;
//					this->pfd.events = POLLIN | POLLRDHUP;
//					this->pfd.revents = 0;
//					this->readfds.push_back(this->pfd);
//
//					// Add new connection to vectors
//					this->clients.push_back(new_socket);
//
//				} else {
//
//					// Recieve data on this connection until it fails
//					ssize_t rc = recv(sd, buf, sizeof(buf), 0);
//					if (rc > 0) {
//
//						std::vector<char> data(static_cast<unsigned long>(rc));
//						for (int i = 0; i < rc; i++) {
//							data[i] = buf[i];
//						}
//
//						this->raiseDataReceivedEvent(data); // ToDo replace with Packet structure later
//					}
//						// Connection was closed by the client
//					else if (rc == 0) {
//						this->readfds[j].revents |= POLLHUP;
//					}
//					else {
//						this->readfds[j].revents |= POLLERR;
//					}
//				}
//			}
//
//			// If revents is not POLLIN its an unexpected result, exit
//			if (this->readfds[j].revents != POLLIN) {
//				if (sd == this->listeningSocket) {
//					std::cout << "Unknown exception..?" << std::endl;
//				} else {
//					if (this->readfds[j].revents & POLLERR) {
//						this->raiseSocketExceptionEvent("Error reading client");
//					} else {
//						this->raiseClientDisconnectedEvent(sd);
//					}
//					close(sd);
//					this->clients.erase(std::find(this->clients.begin(),this->clients.end(), sd));
//					this->readfds.erase(this->readfds.begin() + j);
//					continue;
//				}
//			}
//		}
//	}
//	return true;
//}