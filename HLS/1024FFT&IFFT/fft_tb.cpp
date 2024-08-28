#include <iostream>
#include <fstream>
#include <vector>
#include "hls_fft.h"

int main() {
    // Preparation of FFT sample data
    std::fstream fin("sim_sample.dat");
    std::vector<float> vec_data;
    float num;
    int nfft = 10;
    int data_size = 1 << nfft;
    for (int i = 0; i < data_size; i++) {
        fin >> num;
        vec_data.push_back(num);
    }

    // Writing FFT sample data to the stream
    fft_stream input_data, output_data;
    for (int i = 0; i < data_size; i++) {
        data_t in_data;
        in_data.data = vec_data[i];
        in_data.last = (i == data_size - 1);
        input_data.write(in_data);
    }

    // FFT Execution
    hls_fft(input_data, output_data, nfft);
    // Reading FFT results from a stream
    data_t dout;
    do {
        dout = output_data.read();
        std::cout << dout.data << ", ";
    } while (!dout.last);
	std::fstream fin1("output.dat");
	    std::vector<float> vec_data1;
	    float num1;
	    int nfft1 = 10;
	    int data_size1 = 1 << nfft1;
	    for (int i = 0; i < data_size1; i++) {
	        fin1 >> num1;
	        vec_data1.push_back(num1);
	    }

	    //read data from stream.
	    fft_stream ifft_input,ifft_output;
	    for (int i = 0; i < data_size1; i++) {
	            data_t in_dataifft;
	            in_dataifft.data = vec_data1[i];
	            in_dataifft.last = (i == data_size1 - 1);
	            ifft_input.write(in_dataifft);
	        }
	    //ifft
	    hls_ifft(ifft_input,ifft_output,nfft1);
	    data_t ifftdout;
	    do{
	        ifftdout = ifft_output.read();
	        std::cout << ifftdout.data<<",";
	        } while (!ifftdout.last);
}
