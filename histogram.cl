__kernel  void histogram(__global unsigned int *img, unsigned int _size,__global  unsigned int *ptr) {
    int i = get_global_id(0);
    int index;
	// histogram of R
		index = img[3*i];
		ptr[index]++;

	// histogram of G
	ptr += 256;
		index = img[3*i+1];
		ptr[index]++;

	// histogram of B
	ptr += 256;
	    index = img[3*i+2];
		ptr[index]++;
}
