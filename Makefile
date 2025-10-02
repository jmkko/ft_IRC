include	colors.mk

NAME			:=	ircserv
OS 				:= $(shell uname)

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
					srcs/signal_handler.cpp\
					srcs/clients/Client.cpp\
					srcs/channels/Channel.cpp\
					srcs/server/Server.cpp\
					srcs/server/Config.cpp\
					srcs/server/TcpSocket.cpp\
					srcs/server/Logger.cpp \
					srcs/server/LogManager.cpp\
					srcs/commands/CmdFactory.cpp\
					srcs/commands/Nick.cpp\
					srcs/commands/Kick.cpp\
					srcs/commands/Pass.cpp\
					srcs/commands/User.cpp\
					srcs/commands/Privmsg.cpp\
					srcs/commands/Join.cpp\
					srcs/commands/Who.cpp\
					srcs/server/ReplyHandler.cpp\

HEADERS			:=	$(wildcard INCLUDES)/*.hpp)
OBJS_DIR		:=	.objs
OBJS			:=	$(SRCS:%.cpp=$(OBJS_DIR)/%.o)
DEPS			:=	$(OBJS:%.o=%.d)
OBJ_DIRS		:=	$(sort $(dir $(OBJS)))

################	LINTERS

# Paths for clang-format / clang-tidy-12 / intercept-build if manually installed
export PATH 	:=	$(PATH):$(HOME)/local/bin:/usr/bin/:/usr/share/clang/scan-build-py-12/bin/

HEADERS			:=	$(wildcard includes/*/*.hpp) $(wildcard includes/*.hpp)
FILES_TO_FORMAT	:=	$(SRCS) $(HEADERS)

TIDYFLAGS_CPL	:=	-p .

TIDYFLAGS		:=	--use-color --config-file=.clang-tidy --header-filter=.*

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
ifeq ($(OS), Linux)
	@if [ $(NB_COMP) -eq 1 ]; then echo "=== $(BOLD)Compilation of source files$(NOC) ===";fi
	$(eval PERCENT=$(shell if [ $(TO_COMP) -eq 0 ]; then echo 100; else expr $(NB_COMP)00 "/" $(TO_COMP); fi))
	@if [ $(PERCENT) -le 30 ]; then echo -n "$(RED)"; \
	elif [ $(PERCENT) -le 66 ]; then echo -n "$(YELLOW)"; \
	elif [ $(PERCENT) -gt 66 ]; then echo -n "$(GREEN)"; fi
	@echo -n "\r"; for i in $$(seq 1 $$(/usr/bin/tput cols)); do echo -n " "; done
	@echo -n "\r"; for i in $$(seq 1 25); do if [ $$(expr $$i "*" 4) -le $(PERCENT) ]; then echo -n "█"; \
	else echo -n " "; fi; \
	done; echo -n "";
	@printf " $(NB_COMP)/$(TO_COMP) - Compiling $<"
	@echo -n "$(NOC)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
	$(eval NB_COMP=$(shell expr $(NB_COMP) + 1))
else
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@
endif

$(OBJ_DIRS) :
	@mkdir -p $@
	@mkdir -p logs

forminette:
	@echo "$(YELLOW)=== Checking code format ===$(NOC)"
	@clang-format --dry-run --Werror -style=file:./.clang-format $(FILES_TO_FORMAT)

formator:
	@echo "$(YELLOW)=== Formatting code ===$(NOC)"
	@clang-format -style=file:./.clang-format -i $(FILES_TO_FORMAT)

comp-data:
	@intercept-build make re

tidy: comp-data
	@echo "$(YELLOW)=== Code analysis ===$(NOC)"
	@clang-tidy-12 $(FILES_TO_FORMAT) $(TIDYFLAGS) $(TIDYFLAGS_CPL)

debug-files:
	@echo "SRCS: $(SRCS)"
	@echo "HEADERS: $(HEADERS)"
	@echo "FILES_TO_FORMAT: $(FILES_TO_FORMAT)"

clean :
	@rm -rf $(OBJS_DIR)
	@echo "$(BOLD)=== Obj cleaned ===$(NC)"

fclean : clean
	@rm -rf $(NAME)
	@rm -rf logs
	@echo "$(BOLD)=== Fully cleaned ===$(NC)"

re : fclean
	@make

.PHONY : all clean fclean re

-include $(DEPS)

