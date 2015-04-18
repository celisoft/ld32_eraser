#Define vars
CC = g++
FLAGS = -Wall -std=c++11
LIBS = -lSDL2 -lSDL2_image -lSDL2_mixer 
EXEC = eraser
OBJ_FILES = main.o game_window.o menu.o mouse_cursor.o level_manager.o level.o player.o door.o position.o

#Define the ALL scope (default)
all: clean game 

#Create the executable file
game : main
	$(CC) -o $(EXEC) $(LIBS) $(OBJ_FILES) 

main: complex_components
	$(CC) -c $(FLAGS) main.cpp

complex_components: basic_components
	$(CC) -c $(FLAGS) game_window.cpp level_manager.cpp level.cpp

basic_components: 
	$(CC) -c $(FLAGS) player.cpp menu.cpp door.cpp mouse_cursor.cpp position.cpp

clean: 
	rm -f *.o $(EXEC)
