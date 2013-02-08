namespace is{
	//in_place filter
	inline void filter_1d_MA_LPF(
			double *data, size_t length, size_t kernel_size){
		const size_t end = length-kernel_size;
		double s = 1.0/kernel_size;
		for (size_t i=0; i<end; i++){
			double sum = 0;
			for (size_t j = 0; j<kernel_size; j++){
				sum += data[i+j];
			}
			data[i]=sum*s;
		}
	}

	//in_place filter
	inline void filter_1d_MA_HPF(
			double *data, size_t length, size_t kernel_size){
		const size_t end = length-kernel_size;
		double s = 1.0/kernel_size;
		size_t half_kernel_size = kernel_size/2;
		for (size_t i=0; i<end; i++){
			double sum = 0;
			for (size_t j = 0; j<kernel_size; j++){
				sum += data[i+j];
			}
			data[i]=data[i+half_kernel_size]-sum*s;
		}
	}
}
