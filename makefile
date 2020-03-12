main: shader.o camera.o terrain.o
	g++ main.cpp shader.o camera.o terrain.o -o build.o -lGL -lglfw -lGLEW
	./build.o

shader.o: code/shader.cpp
	g++ code/shader.cpp -c -o shader.o -lGLEW

camera.o: code/camera.cpp
	g++ code/camera.cpp -c -o camera.o -lGLEW

terrain.o: code/terrain.cpp
	g++ code/terrain.cpp -c -o terrain.o
