#include "headers.h"

typedef ap_axiu<32,0,0,0> pkt;

bufferInfo buffer_status[K_BUFFERS];

extern "C" {

void krnl_stream_arb(data *in, int *out, hls::stream<kernelHeader> &r_add_in,
					  hls::stream<kernelHeader> &r_add_out, hls::stream<kernelHeader> &r_sub_in,
					  hls::stream<kernelHeader> &r_sub_out, hls::stream<kernelHeader> &r_mul_in,
					  hls::stream<kernelHeader> &r_mul_out ) 
{

#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem
#pragma HLS INTERFACE m_axi port = out offset = slave bundle = gmem
#pragma HLS INTERFACE axis port = r_add_in
#pragma HLS INTERFACE axis port = r_add_out
#pragma HLS INTERFACE axis port = r_sub_in
#pragma HLS INTERFACE axis port = r_sub_out
#pragma HLS INTERFACE axis port = r_mul_in
#pragma HLS INTERFACE axis port = r_mul_out
#pragma HLS INTERFACE s_axilite port = in bundle = control
#pragma HLS INTERFACE s_axilite port = out bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS stream variable=r_add_in depth=16
#pragma HLS stream variable=r_add_out depth=16
#pragma HLS stream variable=r_sub_in depth=16
#pragma HLS stream variable=r_sub_out depth=16
#pragma HLS stream variable=r_mul_in depth=16
#pragma HLS stream variable=r_mul_out depth=16
#pragma HLS DATA_PACK variable=in struct_level

	initialize data:

	int itr;
	hls::stream<rQueue> running_queue;
	hls::stream<wQueue> waiting_queue;


	for(itr=0; i < K_BUFFERS; ++itr)
	{
		buffer_status[itr].buffer_status = BUFFER_FREE;
		buffer_status[itr].kernel_instance_ID = INVALID_KERNEL;
	}



load_tasks:
// Load the batch of tasks received from host in a burst



}
} /* end extern*/























