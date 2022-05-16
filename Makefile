all:
	mkdir -p bin
	g++ examples/llv3.cpp src/lidarlite_v3.cpp -I . -o bin/llv3.out
	g++ examples/llv3_2.cpp src/lidarlite_v3.cpp -I . -o bin/llv3_2.out
	g++ examples/llv3_stream_all.cpp src/lidarlite_v3.cpp -I . -o bin/llv3_stream_all.out
