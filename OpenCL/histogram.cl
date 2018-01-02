__kernel  void histogram(__global unsigned int *img, volatile __global unsigned int *ptr) {
    int i = get_global_id(0);
    int index;
	// histogram of R
		index = img[3*i];
		atomic_inc(&ptr[index]);
	// histogram of G
		index = img[3*i+1];
		atomic_inc(&(ptr+256)[index]);
	// histogram of B
	    index = img[3*i+2];
		atomic_inc(&(ptr+256+256)[index]);
}

