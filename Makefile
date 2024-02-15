NAME		= vox
OBJS_DIR	= Objs
SRCS_DIR	= Sources

FILES		= main callbacks fluids inputs item3D light player random screenshot tickUpdate utils \
				Blocks Camera Chat ChestInstance Chunk DayCycle Entity FurnaceInstance Inventory JSON Menu OpenGL_Manager Particle Text Ui

SRCS		= $(addprefix $(SRCS_DIR)/, $(addsuffix .cpp, $(FILES)))
OBJS 		= $(addprefix $(OBJS_DIR)/, $(addsuffix .o, $(FILES)))

# ===---===---===---===---===---===---===---===---===---===---===---===---

ifeq ($(shell uname), Linux)
LINKS		= -L Libs `pkg-config --static --libs glfw3 glew` -lGL -lX11 -lpthread -lXrandr -lXi -ldl Libs/libSOIL.a
else
LINKS		=  -framework OpenGl -framework AppKit -framework IOkit Libs/mac/libglfw3.a Libs/mac/libGLEW.a Libs/mac/libSOIL.a
endif

# ===---===---===---===---===---===---===---===---===---===---===---===---

CC = clang++
CPPFLAGS = -Wall -Wextra -Werror -O3
SAN = -fsanitize=address -g3
INCLUDES	= -I Includes -I glm

# ===---===---===---===---===---===---===---===---===---===---===---===---

all: $(OBJS_DIR) $(NAME)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(SAN) $(INCLUDES) -std=c++17 $(OBJS) -o $(NAME) $(LINKS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(SAN) $(INCLUDES) -std=c++17 -c $< -o $@

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -f $(NAME) extrude

re: fclean all

run: all
	@./$(NAME)

log: all
	@mkdir -p Logs
	@./$(NAME) > Logs/.log 2> Logs/err.log

rer: re
	@./$(NAME)

extrusion:
	$(CC) $(CPPFLAGS) $(SAN) -I Includes Sources/Extrusion/main.cpp -o extrude $(LINKS)

.PHONY: all clean fclean re run log rer extrusion
