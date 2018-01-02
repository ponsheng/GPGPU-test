#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <fstream>
#include <iostream>


#include <vector>

#define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#include <CL/cl.h>
#include <cassert>
/*
unsigned int * histogram(unsigned int *image_data, unsigned int _size) {

	unsigned int *img = image_data;
	unsigned int *ref_histogram_results;
	unsigned int *ptr;

	ref_histogram_results = (unsigned int *)malloc(256 * 3 * sizeof(unsigned int));
	ptr = ref_histogram_results;
	memset (ref_histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));

	// histogram of R
	for (unsigned int i = 0; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of G
	ptr += 256;
	for (unsigned int i = 1; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	// histogram of B
	ptr += 256;
	for (unsigned int i = 2; i < _size; i += 3)
	{
		unsigned int index = img[i];
		ptr[index]++;
	}

	return ref_histogram_results;
}
*/
int main(int argc, char const *argv[])
{
	// OpenCL related declarations
    cl_int err;
    cl_platform_id platform;
    cl_device_id device;
    cl_context_properties props[3] = { CL_CONTEXT_PLATFORM, 0, 0 };
    cl_context ctx;
    cl_program program;
    cl_command_queue queue;
    cl_event event = NULL;
    cl_kernel kernel;
    cl_uint platform_num;

	err = clGetPlatformIDs(0, NULL, &platform_num);
    assert( err == CL_SUCCESS );
    printf("There is %d platform\n", platform_num);

	err = clGetPlatformIDs(1, &platform, NULL);
    assert( err == CL_SUCCESS );

    err = clGetDeviceIDs( platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL );
    assert( err == CL_SUCCESS );

    props[1] = (cl_context_properties)platform;
    ctx = clCreateContext( props, 1, &device, NULL, NULL, &err );
    assert( err == CL_SUCCESS );

    queue = clCreateCommandQueue( ctx, device, 0, &err );
    assert( err == CL_SUCCESS );

#define MAX_SOURCE_SIZE 4096
	FILE *fp;
	const char fileName[] = "histogram.cl";
	size_t source_size;
	char *source_str;
//	cl_int i;

	/* Load kernel source code */
	fp = fopen(fileName, "r");
	if (!fp) {
		printf("Nothing in %s\nAbort!",fileName);
		exit(-1);
	}
	source_str = (char *)malloc(MAX_SOURCE_SIZE);
	source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
	fclose(fp);

    program = clCreateProgramWithSource(ctx, 1, (const char **) &source_str, NULL, &err);
    assert( err == CL_SUCCESS );

    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
//    assert( err == CL_SUCCESS );

	if (err != CL_SUCCESS) {
        printf("building program failed\n");
        if (err == CL_BUILD_PROGRAM_FAILURE) {
            size_t log_size;
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
            char *log = (char *) malloc(log_size);
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log, NULL);
            printf("%s\n", log);
        }
    }
	kernel = clCreateKernel(program, "histogram", &err);

	unsigned int histogram_results[256 * 3];
	unsigned int i=0, a, input_size;
	std::fstream inFile("input", std::ios_base::in);
	std::ofstream outFile("0656066.out", std::ios_base::out);

	inFile >> input_size;
	unsigned int *image = new unsigned int[input_size];
	while( inFile >> a ) {
		image[i++] = a;
	}

    printf("%d\n",input_size);
    printf("%d\n",image[0]);
    printf("%d\n",image[1]);


	memset (histogram_results, 0x0, 256 * 3 * sizeof(unsigned int));

/*********************************************************************************/
    // Write buffer
	cl_mem input, result;
    input = clCreateBuffer(ctx, CL_MEM_READ_ONLY, input_size * sizeof(unsigned int), NULL, NULL);
    result = clCreateBuffer(ctx, CL_MEM_READ_WRITE, 256 * 3 * sizeof(unsigned int), NULL, NULL);

	// Copy data
    err = clEnqueueWriteBuffer(queue, input, CL_TRUE, 0, input_size * sizeof(unsigned int), image, 0, NULL, NULL);
    assert( err == CL_SUCCESS );
    err = clEnqueueWriteBuffer(queue, result, CL_TRUE, 0, 256 * 3 * sizeof(unsigned int), histogram_results, 0, NULL, NULL);
    assert( err == CL_SUCCESS );
    err = clFinish(queue);
    assert( err == CL_SUCCESS );

    size_t globalSize, localSize;
    globalSize = input_size / 3;
    localSize = 0;

	// Run kernel
	//histogram_results = histogram(image, input_size);
	err  = clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
	err  = clSetKernelArg(kernel, 1, sizeof(unsigned int), &input_size);
	err  = clSetKernelArg(kernel, 2, sizeof(cl_mem), &result);
    assert( err == CL_SUCCESS );

    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &globalSize, NULL, 0, NULL, NULL);
    printf("Error: %d\n", err);
    assert( err == CL_SUCCESS );
    err = clFinish(queue);
    printf("Error: %d\n", err);
    assert( err == CL_SUCCESS );

	histogram_results[2] = 948;

    //Read buffer
    err = clEnqueueReadBuffer(queue, result, CL_TRUE, 0, 256 * 3 * sizeof(unsigned int), histogram_results, 0, NULL, NULL );
    printf("Error: %d\n", err);
    assert( err == CL_SUCCESS );
    err = clFinish(queue);
    printf("Error: %d\n", err);
    assert( err == CL_SUCCESS );

/*****************************************************************************/
    for(unsigned int i = 0; i < 256 * 3; ++i) {
		if (i % 256 == 0 && i != 0)
			outFile << std::endl;
		outFile << histogram_results[i]<< ' ';
	}

	inFile.close();
	outFile.close();

    clReleaseMemObject(input);
    clReleaseMemObject(result);
    clReleaseProgram(program);
    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseContext(ctx);

	return 0;
}
