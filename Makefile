# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: dmaessen <dmaessen@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2024/08/19 12:34:37 by dmaessen          #+#    #+#              #
#    Updated: 2024/08/19 16:22:11 by dmaessen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = webserv

CC = c++

# CFLAGS = -Wall -Wextra -Werror -g -std=c++17 #-fsanitize=leak 
CFLAGS = -g -std=c++17 #-fsanitize=leak
RM = rm -f

HEADERS = inc/CgiHandler.hpp \
		inc/Config.hpp \
		inc/defines.hpp \
		inc/ParseRequest.hpp \
		inc/Response.hpp \
		inc/Server.hpp \
		inc/Utils.hpp \
		inc/VirtualHost.hpp \
		inc/WebServ.hpp

SRC =  src/main.cpp \
    src/Webserv/WebServ.cpp \
    src/Server/Server.cpp \
    src/Config/Config.cpp \
    src/VirtualHost/VirtualHost.cpp \
	src/Utils/FdManagement.cpp \
	src/Utils/StructUtils.cpp \
	src/CGI/CgiHandler.cpp \
	src/Request/ParseRequest.cpp \
	src/Request/UtilsRequest.cpp \
	src/Response/Header.cpp \
	src/Response/Response.cpp \
	src/Response/Utils.cpp \
	src/Cookies/Cookies.cpp

OBJ_DIR = obj/
OBJ = $(SRC:src/%.cpp=$(OBJ_DIR)%.o)

DIRS = $(sort $(dir $(OBJ)))

all: $(NAME)

$(OBJ_DIR)%.o: src/%.cpp $(HEADERS)
	@echo "Compiling $< to $@"
	@mkdir -p $(DIRS)
	@$(CC) $(CFLAGS) -c $< -o $@

$(NAME): $(OBJ)
	@echo "Linking $@"
	@$(CC) $(CFLAGS) $^ -o $@

clean:
	@echo "Cleaning object files in $(OBJ_DIR)"
	@$(RM) -r $(OBJ_DIR)

fclean: clean
	@echo "Cleaning executable $(NAME)"
	@$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re