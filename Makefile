NAME		= vox
OBJS_DIR	= Objs
SRCS_DIR	= Sources

FILES		= main blockUpdate callbacks fluids inputs item2D item3D light pathfinding physics player random redstone screenshot threadChunkUpdate tickUpdate utils worldGeneration \
				AMob Blocks Camera Chat ChestInstance Chunk DayCycle Entity FurnaceInstance Inventory JSON Menu OpenGL_Manager Particle Settings Shader SignInstance Skybox Text Ui WorldEdit Zombie

SRCS		= $(addprefix $(SRCS_DIR)/, $(addsuffix .cpp, $(FILES)))
OBJS 		= $(addprefix $(OBJS_DIR)/, $(addsuffix .o, $(FILES)))

# ===---===---===---===---===---===---===---===---===---===---===---===---

CC 			= clang++
CPPFLAGS 	= -Wall -Wextra -Werror -O3 -std=c++17
SAN 		= -fsanitize=address -g3
INCLUDES	= -I Includes -I Libs/glm -I Libs/glfw/include -I Libs/glew-2.2.0/include -I Libs/SOIL/build/include
LDFLAGS		= Libs/glm/glm/libglm.a Libs/glfw/src/libglfw3.a Libs/glew-2.2.0/build/lib/libGLEW.a Libs/SOIL/build/lib/libSOIL.a

# ===---===---===---===---===---===---===---===---===---===---===---===---

ifeq ($(shell uname), Linux)
LDFLAGS		+= -lGL -lX11 -lpthread -lXrandr -lXi -ldl
else
LDFLAGS		+= -framework OpenGl -framework AppKit -framework IOkit
endif

# ===---===---===---===---===---===---===---===---===---===---===---===---

all: $(OBJS_DIR) $(NAME)

$(OBJS_DIR):
	@mkdir -p $(OBJS_DIR)

setup:
	cd Libs/glm && cmake . && make
	cd Libs/glew-2.2.0/build && cmake ./cmake && make
	cd Libs/glfw && cmake . && make
	cd Libs/SOIL && ./configure && make

cleanLibs:
	cd Libs/glm && make clean
	cd Libs/glew-2.2.0 && make clean
	cd Libs/glfw && make clean
	cd Libs/SOIL && make clean

$(NAME): $(OBJS)
	$(CC) $(CPPFLAGS) $(SAN) $(INCLUDES) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	$(CC) $(CPPFLAGS) $(SAN) $(INCLUDES) -c $< -o $@

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
	$(CC) $(CPPFLAGS) $(SAN) $(INCLUDES) Sources/Extrusion/main.cpp -o extrude $(LDFLAGS)

.PHONY: all setup cleanLibs clean fclean re run log rer extrusion
