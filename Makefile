NAME		= vox
OBJS_DIR	= Objs
SRCS_DIR	= Sources
NET_DIR		= Network

FILES		= main

FILES		+= blockUpdate callbacks drawPlayer fluids hitbox inputs item2D item3D itemExtrusion light pathfinding physics random redstone redstoneSchedule screenshot textures threadChunkUpdate tickUpdate utils worldGeneration \
				AHostileMob AMob ATag Blocks Buffer Camera Chat ChestInstance Chunk DayCycle Entity FurnaceInstance Inventory JSON Menu OpenGL_Manager Particle Player Settings Shader SignInstance Skeleton Skybox Text Ui WorldEdit Zombie

NET_FILES	= client serialize threadClientChunkUpdate threadServerChunkUpdate \
				Address Server Socket
FILES		+= $(addprefix $(NET_DIR)/, $(NET_FILES))

SRCS		= $(addprefix $(SRCS_DIR)/, $(addsuffix .cpp, $(FILES)))
OBJS 		= $(addprefix $(OBJS_DIR)/, $(addsuffix .o, $(FILES)))

# ===---===---===---===---===---===---===---===---===---===---===---===---

CC 			= clang++
CPPFLAGS 	= -Wall -Wextra -Werror -O3 -std=c++17
SAN 		= -fsanitize=address -g3
INCLUDES	= -I Includes -I Includes/Network -I Libs/glm -I Libs/glfw/include -I Libs/glew-2.2.0/include -I Libs/SOIL/build/include -I Libs/zlib
LDFLAGS		= Libs/glm/glm/libglm.a Libs/glfw/src/libglfw3.a Libs/glew-2.2.0/build/lib/libGLEW.a Libs/SOIL/build/lib/libSOIL.a Libs/zlib/libz.a

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
	@mkdir -p $(OBJS_DIR)/$(NET_DIR)

setup:
	cd Libs/glm && cmake . && make
	cd Libs/glew-2.2.0/build && cmake ./cmake && make
	cd Libs/glfw && cmake . && make
	cd Libs/SOIL && ./configure && make
	cd Libs/zlib && ./configure && make

cleanLibs:
	make -C Libs/glm clean
	make -C Libs/glew-2.2.0 clean
	make -C Libs/glfw clean
	make -C Libs/SOIL clean
	make -C Libs/zlib clean

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
