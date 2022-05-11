#include <iostream>
#include <unistd.h>
#include <cstring>
#include <string>
#include <algorithm>
#include "ap_int.h"
#include "ap_axi_sdata.h"
#include "hls_stream.h"

using namespace std;
//#define IS_SW_EMU
#define OP_ADD 0x11
#define OP_SUB 0x12
#define OP_MUL 0x13

#define N_INPUT 2					// Harcoding for now. Will be taken based on Batching Size from Read/Write Kernels.
#define K_BUFFERS 8					// Hardcoding for now. Decided based on final number of computational kernels running.
#define N_PIPELINE_STAGE 3

#define BUFFER_FREE 0
#define BUFFER_READ 1
#define BUFFER_WRITE 2

#define INVALID_KERNEL 0

typedef struct data
{
	int op_id[N_PIPELINE_STAGE];
	int in_off;
	int out_off;
	int kargs[N_PIPELINE_STAGE];	// This field will be extended for images (Support Multiple arguments)

} __attribute__((packed, aligned(4))) data;

/*
	kernelHeader -> Packet to exchanged between arbitrer and computational kernels.
	seqNum: Pipeline Number to map input with final output.
	currStage: For arbiter to track current kernel stage in the pipeline
	stage_ids: order of computational kernels for the pipeline
	buff_in_off: offset for buffer holding input data to kernel
	buffer_out_off: offset for buffer holding output data from kernel
	karg: Hold argument for compuatation (For intial implementatio: holds add, sub, mult value)
*/
typedef struct kernelHeader
{
	int seqNum;
	int currStage;
	int stage_ids[N_PIPELINE_STAGE];
	int buff_in_off;
	int buff_out_off;
	int karg;					// Increase as per usage. For basic design keeping it 1.

} __attribute__((packed, aligned(4))) kernelHeader;

typedef struct bufferInfo
{
	unsigned short buffer_status;
	int kernel_instance_ID;
	
} __attribute__((packed, aligned(4))) bufferInfo;

typedef struct rQueue
{
	int seqNum;					// Seqeunce number to verify entry populated from Stream
	int kernel_instance_ID;		// Kernel ID to poll correct stream for pieline data

} __attribute__((packed, aligned(4))) rQueue;

#define wQueue kernelHeader 	// WaitQueue should have all the entries as in the kernel header.

