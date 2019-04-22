all:
	mkdir -p bin
	g++ examples/llv3.cpp src/lidarlite_v3.cpp -I . -o bin/llv3.out
