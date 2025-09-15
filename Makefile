NAME = ircserv

CXX= c++ -g -MMD
CXXFLAGS= -Wall -Wextra -Werror -std=c++98
INCLUDES= -Iincludes/server -Iincludes


SRCS= 	srcs/main.cpp \
		srcs/server/Server.cpp srcs/server/TcpSocket.cpp srcs/server/Logger.cpp \
		srcs/server/LogManager.cpp

OBJS_DIR= objs

OBJS= $(SRCS:%.cpp=$(OBJS_DIR)/%.o)
DEPS= $(OBJS:%.o=%.d)
OBJ_DIRS = $(sort $(dir $(OBJS)))

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(OBJS) :$(OBJS_DIR)/%.o : %.cpp | $(OBJ_DIRS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJ_DIRS) :
	@mkdir -p $@

clean :
	rm -rf $(OBJS)
	@echo "=== Obj cleaned ==="

fclean : clean
	rm -rf $(NAME)
	rm -rf logs
	@echo "=== Full cleaned ==="

re : fclean all

.PHONY : all clean fclean re

-include $(DEPS)

