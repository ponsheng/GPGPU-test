


all: histogram

histogram: %: %.cpp
	g++ -o $@ $^ -lOpenCL

clean:
	histogram
