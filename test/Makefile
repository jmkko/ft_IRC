include ../colors.mk

export PATH := $(HOME)/local/bin:$(PATH)

NAME			:=	test

IRC_EXEC		:=  ircserv

CXX				:=	clang++

CXXFLAGS		:=	-Wall -Wextra -Werror -std=c++20 -g -DTEST

MAKEFLAGS		:=	--no-print-directory

IRC_DIR			:=	../srcs
IRC_INCLUDE_DIR	:=	../includes

SRC_DIR			:=	srcs
INCLUDE_DIR		:=	include
BUILD_DIR		:=	.build

INC				:= 	-I$(INCLUDE_DIR) -I$(IRC_INCLUDE_DIR) -I$(IRC_INCLUDE_DIR)/channels -I$(IRC_INCLUDE_DIR)/clients -I$(IRC_INCLUDE_DIR)/commands -I$(IRC_INCLUDE_DIR)/server

SRCS_FILES		:= 	main.cpp\
					fakeClient.cpp\
					AssertFail.cpp\
					AssertReply.cpp\
					printUtils.cpp\
					ServerRunner.cpp\
					sequences.cpp\
					TestFixture.cpp\
					testKick.cpp\
					testMode.cpp\
					testNick.cpp\
					testWho.cpp\
					testJoin.cpp\
					testPrivmsg.cpp\
					testPrivmsg.cpp\

IRC_SRCS_FILES	:=	utils.cpp\
					signal_handler.cpp\
					clients/Client.cpp\
					channels/Channel.cpp\
					server/Server.cpp\
					server/TcpSocket.cpp\
					server/Config.cpp\
					server/Logger.cpp\
					server/LogManager.cpp\
					server/ReplyHandler.cpp\
					commands/CmdFactory.cpp\
					commands/Invite.cpp\
					commands/Join.cpp\
					commands/Kick.cpp\
					commands/Mode.cpp\
					commands/Nick.cpp\
					commands/Pass.cpp\
					commands/Privmsg.cpp\
					commands/Quit.cpp\
					commands/User.cpp\
					commands/Who.cpp\

EXT				:= .cpp

TEST_SRCS		:=	$(addprefix $(SRC_DIR)/, $(SRCS_FILES))
IRC_SRCS 		:=	$(addprefix $(IRC_DIR)/, $(IRC_SRCS_FILES))
SRCS			:=	$(TEST_SRCS) $(IRC_SRCS)

OBJS			:=	$(patsubst %.cpp,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))
DIRS			:=	$(sort $(shell dirname $(OBJS)))

vpath %.cpp $(SRC_DIR) $(IRC_DIR) $(IRC_DIR)/commands $(IRC_DIR)/clients $(IRC_DIR)/channels $(IRC_DIR)/server

# Paths for clang-format / clang-tidy-12 / intercept-build if manually installed
export PATH 	:=	$(PATH):$(HOME)/local/bin:/usr/bin/:/usr/share/clang/scan-build-py-12/bin/

HEADERS			:=	$(wildcard $(INCLUDES)/*.hpp)
FILES_TO_FORMAT	:=	$(SRCS) $(HEADERS)

################	LOADER

NB_COMP			:=	1

ifndef RECURSION
TO_COMP_IRC		:=	$(shell make -C .. RECURSION=1 -n 2>/dev/null | grep Compiling | wc -l)
TO_COMP_LOC 	:=	$(shell make RECURSION=1 -n 2>/dev/null | grep Compiling | wc -l) 
TO_COMP			:=	$(shell echo $$(( $(TO_COMP_IRC) + $(TO_COMP_LOC) )))
else
TO_COMP			:=	1
endif

PERCENT			:= 0

###########
# Targets #
###########

all:$(NAME)

$(DIRS):
	@mkdir -p $@

$(NAME): $(OBJS)
	@echo "\n$(GREEN)Create binaries$(NOC)"
	@$(CXX) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.cpp | $(DIRS)
	@mkdir -p $(BUILD_DIR)
	$(eval PERCENT :=$(shell if [ "$(TO_COMP)" -eq 0 ]; then echo 100; else expr $(NB_COMP)00 "/" $(TO_COMP); fi))
	@{ \
		i=$$(($(PERCENT) % 4)); \
		case $$i in \
			0) anim="ᵒᵒᵒᵒᵒ";; \
			1) anim="०ᵒ०ᵒ०";; \
			2) anim="०००००";; \
			3) anim="ᵒ०ᵒ०ᵒ";; \
			4) anim="००ooᵒ";; \
			5) anim="०ᵒᵒᵒ०";; \
			6) anim="००o००";; \
			7) anim="०o०ᵒ०";; \
			8) anim="०o०o०";; \
			9) anim="०ᵒ०ᵒ०";; \
			*) anim="०ooo०";; \
		esac; \
		printf "                                                                     "; \
		printf "\r[%-6s] %3d%% Compiling %s" "$$anim" $(PERCENT) "$<"; \
	} 
	@$(CXX) $(CXXFLAGS) $(INC) $< -c -o $@
	$(eval NB_COMP=$(shell expr $(NB_COMP) + 1))

comp-data:
	@intercept-build make re

debug: CXXFLAGS += -g -DDEB
debug: all

redebug: fclean debug

formator:
	@echo "$(YELLOW)=== Formatting code ===$(NOC)"
	@clang-format -style=file:../.clang-format -i $(FILES_TO_FORMAT)

clean:
	@echo "$(RED)Remove objects$(NOC)"
	@rm -rf $(BUILD_DIR)

fclean: clean
	@echo "$(RED)Remove binary$(NOC)"
	@rm -f $(NAME)
	@rm -rf ../logs
	@rm -rf logs/*

re: fclean
	@make

# run: $(NAME)
# 	@make -C ..
# 	@../$(IRC_EXEC) 4343 password > /dev/null 2>&1 & echo $$! > ircserv.pid
# 	@valgrind --track-fds=yes ./$(NAME) || true
# 	@pid=$$(cat ircserv.pid); if kill -0 $$pid 2>/dev/null; then kill $$pid; fi
# 	@rm -f ircserv.pid

.PHONY: all clean fclean re $(DIRS)
