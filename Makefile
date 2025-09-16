include	colors.mk

NAME			:=	ircserv

CXX				:=	clang++
CXXFLAGS		:=	-Wall -Wextra -Werror -std=c++98 -g -MMD
MAKEFLAGS		:=	--no-print-directory

INCLUDES		:=	-Iincludes\
					-Iincludes/channels\
					-Iincludes/clients\
					-Iincludes/commands\
					-Iincludes/server\

SRCS			:=	srcs/main.cpp\
					srcs/utils.cpp\
					srcs/clients/Client.cpp\
					srcs/channels/Channel.cpp\
					srcs/server/Server.cpp\
					srcs/server/TcpSocket.cpp\
					srcs/server/Logger.cpp \
					srcs/server/LogManager.cpp

OBJS_DIR		:=	objs
OBJS			:=	$(SRCS:%.cpp=$(OBJS_DIR)/%.o)
DEPS			:=	$(OBJS:%.o=%.d)
OBJ_DIRS		:=	$(sort $(dir $(OBJS)))

################	LINTERS

# Paths for clang-format / clang-tidy-12 if manually installed
export PATH 	:=	$(HOME)/local/bin:$(PATH)

HEADERS			:=	$(wildcard includes/*.h)
FILES_TO_FORMAT	:=	$(SRCS) $(HEADERS)

TIDYFLAGS_CPL	:=	-- -std=c++98 -I./includes -I./includes/channels -I./includes/commands -I./includes/server -I./srcs
TIDYFLAGS		:=	--use-color --config-file=.clang-tidy

################	LOADER

NB_COMP			:=	1

ifndef RECURSION
TO_COMP 		:=	$(shell make RECURSION=1 -n | grep Compiling | wc -l)
else
TO_COMP			:=	1
endif

PERCENT			:= 0

################	TARGETS

all : $(NAME)

$(NAME) : $(OBJS)
	@echo "\n$(BOLD)=== Linkage ... generating binaries ===$(NOC)"
	@$(CXX) $(INCLUDES) $^ -o $@

$(OBJS) :$(OBJS_DIR)/%.o : %.cpp | $(OBJ_DIRS)
	@if [ $(NB_COMP) -eq 1 ]; then echo "=== $(BOLD)Compilation of source files$(NOC) ===";fi
	$(eval PERCENT=$(shell if [ $(TO_COMP) -eq 0 ]; then echo 100; else expr $(NB_COMP)00 "/" $(TO_COMP); fi))
	@if [ $(PERCENT) -le 30 ]; then echo -n "$(RED)"; elif [ $(PERCENT) -le 66 ]; then echo -n "$(YELLOW)"; elif [ $(PERCENT) -gt 66 ]; then echo -n "$(GREEN)"; fi
	@echo -n "\r"; for i in $$(seq 1 $$(/usr/bin/tput cols)); do echo -n " "; done
	@echo -n "\r"; for i in $$(seq 1 25); do if [ $$(expr $$i "*" 4) -le $(PERCENT) ]; then echo -n "█"; else echo -n " "; fi; done; echo -n "";
	@printf " $(NB_COMP)/$(TO_COMP) - Compiling $<"
	@echo -n "$(NOC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	$(eval NB_COMP=$(shell expr $(NB_COMP) + 1))

$(OBJ_DIRS) :
	@mkdir -p $@

format-check:
	@echo "$(YELLOW)=== Checking code format ===$(NOC)"
	@clang-format --dry-run --Werror $(FILES_TO_FORMAT)

format:
	@echo "$(YELLOW)=== Formatting code ===$(NOC)"
	@clang-format --Werror $(FILES_TO_FORMAT)

tidy:
	@echo "$(YELLOW)=== Code analysis ===$(NOC)"
	@clang-tidy-12 $(FILES_TO_FORMAT) $(TIDYFLAGS) $(TIDYFLAGS_CPL)

clean :
	@rm -rf $(OBJS)
	@echo "$(BOLD)=== Obj cleaned ===$(NC)"

fclean : clean
	@rm -rf $(NAME)
	@rm -rf logs
	@echo "$(BOLD)=== Fully cleaned ===$(NC)"

re : fclean 
	@make

.PHONY : all clean fclean re

-include $(DEPS)

