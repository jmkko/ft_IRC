include ../colors.mk

export PATH := $(HOME)/local/bin:$(PATH)

NAME			:=	test

CXX				:=	clang++

CXXFLAGS		:=	-Wall -Wextra -Werror -std=c++20

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
					testKick.cpp\
					testNick.cpp\

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
					commands/Nick.cpp\
					commands/Pass.cpp\
					commands/User.cpp\
					commands/Join.cpp\

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

###########
# Targets #
###########

all: $(NAME)

$(DIRS):
	@mkdir -p $@

$(NAME): $(OBJS)
	@echo "\n$(GREEN)Create binaries$(NOC)"
	@$(CXX) $(OBJS) -o $@

$(BUILD_DIR)/%.o: %.cpp | $(DIRS)
	@mkdir -p $(BUILD_DIR)
	@$(CXX) $(CXXFLAGS) $(INC) $< -c -o $@

comp-data:
	@intercept-build make re

debug: CXXFLAGS += -g -DDEB=1
debug: all

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

re: fclean
	@make

.PHONY: all clean fclean re $(DIRS)
