#include "headers.h"


/*
	Input pointer type will change wth respect to image data type.
	All meta-data information will be passed on from HLS Stream.
*/

extern "C" 
{
	void krnl_stream_vadd(int *in, int *out, hls::stream<kernelHeader> &mul_in, 
			hls::stream<kernelHeader> &mul_out) 
	{

#pragma HLS INTERFACE m_axi port = in offset = slave bundle = gmem

#pragma HLS INTERFACE axis port = mul_in
#pragma HLS INTERFACE axis port = mul_out
#pragma HLS INTERFACE s_axilite port = in bundle = control
#pragma HLS INTERFACE s_axilite port = return bundle = control
#pragma HLS stream variable=mul_in depth=16 
#pragma HLS stream variable=mul_out depth=16

		vadd:
			kernelHeader head;
			int res;
			// Populate Kernel Header Info
			head = mul_in.read().data;
			
#ifdef IS_SW_EMU
			std::cout << "Multiplication Kernel Begin" << std::endl;
#endif
			// For real computational kernel, corresposnding API will be written and called.
			res = *(in + head.buff_in_off);
			res *= head.karg;
			*(out + head.buff_out_off) = res;
			
			// Write the original header back to the stream. Arbitrar will increment the stage.
			p.data = head;
			mul_out.write(p);
		
#ifdef IS_SW_EMU
				std::cout << "Multiplication Kernel End" << std::endl;
#endif
	}
} /* end extern*/




















