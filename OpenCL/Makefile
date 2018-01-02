PROG=histogram
SIZE=38M

all: histogram

histogram: %: %.cpp
	g++ -g -o $@ $^ -I/usr/include/CL -lOpenCL

check:
	cat /home/data/OpenCL/input-$(SIZE) > input &
	./$(PROG)
	diff /home/data/OpenCL/ReferenceOutput/output-$(SIZE) 0656066.out

clean:
	rm -f histogram *.out  input output
