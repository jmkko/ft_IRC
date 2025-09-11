NAME = ircserv

CXX= c++ -g -MMD
CXXFLAGS= -Wall -Wextra -Werror -std=c++98
INCLUDES= -Iincludes/server


SRCS= 	srcs/main.cpp \
		srcs/server/Server.cpp srcs/server/TcpSocket.cpp

OBJS_DIR= objs

OBJS= $(SRCS:%.cpp=$(OBJS_DIR)/%.o)
DEPS= $(OBJS:%.o=%.d)

all : $(NAME)

$(NAME) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@

$(OBJS) :$(OBJS_DIR)/%.o : %.cpp | $(OBJS_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(OBJS_DIR) :
	@mkdir -p $(OBJS_DIR)

clean :
	rm -rf $(OBJS)
	@echo "=== Obj cleaned ==="

fclean : clean
	rm -rf $(NAME)
	@echo "=== Full cleaned ==="

re : fclean all

.PHONY : all clean fclean re

-include $(DEPS)

