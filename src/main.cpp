/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/08/19 12:55:19 by dmaessen          #+#    #+#             */
/*   Updated: 2024/08/19 12:55:20 by dmaessen         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/WebServ.hpp"

int main(int argc, char **argv){
	try {
		WebServ server(argc, argv);
		server.run();
	} catch (Config::ParseException &e) {
		std::cout << "Parse exception: " << e.what() << std::endl;
	} catch (WebServ::InitException &e) {
		std::cout << "InitException: " << e.what() << std::endl;
	} catch (std::exception &e) {
		std::cout << "Fuck#2 " << e.what() << std::endl;
	}
}