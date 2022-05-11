#include "headers.h"

typedef ap_axiu<32,0,0,0> pkt;

extern "C" {

void krnl_stream_arb(data *in, int *out, hls::stream<pkt> &r_add_in,
					  hls::stream<pkt> &r_add_out, hls::stream<pkt> &r_sub_in,
					  `hls::stream<pkt> &r_sub_out, hls::stream<pkt> &r_mul_in,
					  hls::stream<pkt> &r_mul_out ) 
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

varb:


}
} /* end extern*/























