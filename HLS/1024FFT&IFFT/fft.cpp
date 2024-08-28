
#include <complex>
#include "hls_fft.h"

void call_fft_ip(std::complex<float> fft_input[FFT_LENGTH_MAX], std::complex<float> fft_output[FFT_LENGTH_MAX],
        unsigned size) {
    config_t fft_config;
    status_t fft_status;
    fft_config.setDir(1);
    fft_config.setNfft(size);
    hls::fft<fft_param>(fft_input, fft_output, &fft_status, &fft_config);
}

void conj(std::complex<float> in_conj[FFT_LENGTH_MAX],std::complex<float> out_conj[FFT_LENGTH_MAX]){
	std::complex<float> con=(1,-1);
	for(int i;i<FFT_LENGTH_MAX;i++){
		out_conj[i]=in_conj[i]*con;
	}
}
void call_hls_ifft(std::complex<float> ifft_input[FFT_LENGTH_MAX],std::complex<float> ifft_output[FFT_LENGTH_MAX],unsigned size){
	std::complex<float> k[FFT_LENGTH_MAX];
	std::complex<float> ifft_out[FFT_LENGTH_MAX];
	conj(ifft_input,k);
    call_fft_ip(k, ifft_out, size);
    conj(ifft_out,ifft_output);
}

void read_input_data_from_stream(fft_stream &stream_input, std::complex<float> fft_input[FFT_LENGTH_MAX]) {
    data_t data_in;
    int i = 0;
    do {
#pragma HLS PIPELINE II = 1
        data_in = stream_input.read();
        fft_input[i++] = data_in.data;
    } while (!data_in.last);
}

void write_output_data_to_stream(fft_stream &stream_output, std::complex<float> fft_output[FFT_LENGTH_MAX],
        unsigned size) {
    data_t data_out;
    data_out.keep = -1;
    data_out.strb = -1;

    for (int i = 0; i < size; i++) {
#pragma HLS PIPELINE II = 1
        data_out.data = fft_output[i];
        data_out.last = (i == size - 1);
        stream_output.write(data_out);

    }
}

void hls_fft(fft_stream &stream_input, fft_stream &stream_output, unsigned size) {
#pragma HLS INTERFACE axis port = stream_input
#pragma HLS INTERFACE axis port = stream_output
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return

    std::complex<float> fft_in[FFT_LENGTH_MAX] __attribute__((no_ctor));
    std::complex<float> fft_out[FFT_LENGTH_MAX] __attribute__((no_ctor));
#pragma HLS STREAM variable = fft_in
#pragma HLS STREAM variable = fft_out
#pragma HLS dataflow

    read_input_data_from_stream(stream_input, fft_in);
    call_fft_ip(fft_in, fft_out, size);
    write_output_data_to_stream(stream_output, fft_out, (1 << size));
}

void hls_ifft(fft_stream &stream_input, fft_stream &stream_output, unsigned size){
#pragma HLS INTERFACE axis port = stream_input
#pragma HLS INTERFACE axis port = stream_output
#pragma HLS INTERFACE s_axilite port = size
#pragma HLS INTERFACE s_axilite port = return

    std::complex<float> ifft_in[FFT_LENGTH_MAX] __attribute__((no_ctor));
    std::complex<float> ifft_out[FFT_LENGTH_MAX] __attribute__((no_ctor));
#pragma HLS STREAM variable = ifft_in
#pragma HLS STREAM variable = ifft_out
#pragma HLS dataflow
    read_input_data_from_stream(stream_input, ifft_in);
    call_hls_ifft(ifft_in,ifft_out,size);
    write_output_data_to_stream(stream_output, ifft_out, (1 << size));
}
