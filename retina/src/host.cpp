#include<stdlib.h>
#include "headers.h"
// This extension file is required for stream APIs
#include "CL/cl_ext_xilinx.h"
// This file is required for OpenCL C++ wrapper APIs
#include "xcl2.hpp"

/*
    Using hardocded values for intial design. Input will be received
*/
#define CONST_ADD 2
#define CONST_SUB 1
#define CONST_MUL 3
#define IN_NUM_1 14
#define IN_NUM_2 18
#define MEM_ALIGN 4 * 1024


/*
    Populate input/output offsets in GMEM, Pipeline order and input arg. 
    To be chnaged while integration with read/write retina framework.
*/
int populate_input(data *d, int *sw_result, int *out) {
	  
  data *x = d;
  for(int i=0;i <= N_INPUT;++i)
  {
    x->op_id[0] = OP_ADD;
    x->op_id[1] = OP_SUB;
    x->op_id[2] = OP_MUL;
    x->in_off = i;
    x->out_off = i;
    // Kernel arguments will be received from tensorflow interface.
    x->kargs[0] = CONST_ADD;
    x->kargs[1] = CONST_SUB;
    x->kargs[2] = CONST_MUL;
    x += 1;
  }
  out[0] = -1;
  out[1] = -1;
  sw_result[0] = (((d->num[0] + CONST_ADD) - CONST_SUB) * CONST_MUL);
  sw_result[1] = (((d->num[1] + CONST_ADD) - CONST_SUB) * CONST_MUL);
  return 0;
}

/*
    To be removed from final design. Used for implementation verification for now.
*/
bool verify(int *sw_results, int *out) {
  bool match = true;

  std::cout << "Verifying the Results ..." << std::endl;
    if (sw_results[0] != out[0] || sw_results[1] != out [1]) {
      match = false;
    }
  std::cout << "expt_output: " << sw_results[0] << "\t" << sw_results[1] << "\n";
  std::cout << "true_output: " << out[0] << "\t"<< out[1] << "\n";
  std::cout << "TEST " << (match ? "PASSED" : "FAILED") << std::endl;
  return match;
}

int main(int argc, char **argv) {


  data *input;
  int *sw_res;
  int *kBuffers;
  int *out;	
  size_t in_buff_size = sizeof(data) * N_INPUT;
  size_t kBuffers_size = sizeof(int) * K_BUFFERS;
  size_t out_buff_size = sizeof(int) * N_INPUT;

  if (argc != 2) 
  {
    std::cout << "Usage: " << argv[0] << " <XCLBIN File>" << std::endl;
    return EXIT_FAILURE;
  }


  input = (data *)aligned_alloc(MEM_ALIGN, sizeof(*input)* N_INPUT);

  sw_res = new int[N_INPUT]; 	
  kBuffers = (int *)aligned_alloc(MEM_ALIGN, sizeof(int) * K_BUFFERS);
  out = (int *)aligned_alloc(MEM_ALIGN, sizeof(int) * N_INPUT);

  if (!input || !sw_res || !out || !kBuffers) {
	std::cout << "Memory alloc failed" << d << "\t" << sw_res << "\t" << out <<std::endl;
  }

  // OpenCL Host Code Begins.
  cl_int err;

  // OpenCL objects
  cl::Device device;
  cl::Context context;
  cl::CommandQueue q;
  cl::Program program;
  cl::Kernel krnl_arb;
  cl::Kernel krnl_vadd;
  cl::Kernel krnl_vmult;
  cl::Kernel krnl_vsub;

  auto binaryFile = argv[1];

  // get_xil_devices() is a utility API which will find the xilinx
  // platforms and will return list of devices connected to Xilinx platform
  auto devices = xcl::get_xil_devices();

  // read_binary_file() is a utility API which will load the binaryFile
  // and will return the pointer to file buffer.
  auto fileBuf = xcl::read_binary_file(binaryFile);
  cl::Program::Binaries bins{{fileBuf.data(), fileBuf.size()}};
  int valid_device = 0;
  
  
  for (unsigned int i = 0; i < devices.size(); i++) {
    device = devices[i];
    // Creating Context and Command Queue for selected Device
    OCL_CHECK(err, context = cl::Context(device, NULL, NULL, NULL, &err));
    OCL_CHECK(err, q = cl::CommandQueue(context, device, CL_QUEUE_PROFILING_ENABLE | CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE, &err));
    
    std::cout << "Trying to program device[" << i
              << "]: " << device.getInfo<CL_DEVICE_NAME>() << std::endl;
    cl::Program program(context, {device}, bins, NULL, &err);
    if (err != CL_SUCCESS) 
    {
      std::cout << "Failed to program device[" << i << "] with xclbin file!\n";
    } 
    else 
    {
      std::cout << "Device[" << i << "]: program successful!\n";
      // Creating Kernel
      OCL_CHECK(err, krnl_arb = cl::Kernel(program, "krnl_stream_arb", &err));
      OCL_CHECK(err, krnl_vadd = cl::Kernel(program, "krnl_stream_vadd", &err));
      OCL_CHECK(err,
                krnl_vsub = cl::Kernel(program, "krnl_stream_vsub", &err));
      OCL_CHECK(err,
                krnl_vmult = cl::Kernel(program, "krnl_stream_vmult", &err));
      valid_device++;
      break; // we break because we found a valid device
    }
  }
  if (valid_device == 0) {
    std::cout << "Failed to program any device found, exit!\n";
    exit(EXIT_FAILURE);
  }

  std::cout << "Stream Addition, Subtraction and Multiplication" /*<< d->num*/ << std::endl;

  populate_input(input, sw_res, out);

  // Running the kernel

  // Allocate Buffer in Global Memory
  // Buffers are allocated using CL_MEM_USE_HOST_PTR for efficient memory and
  // Device-to-host communication
  OCL_CHECK(err, cl::Buffer buffer_in(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                       in_buff_size, input, &err));
  OCL_CHECK(err, cl::Buffer buffer_kernel(context,
                                       CL_MEM_USE_HOST_PTR | CL_MEM_READ_ONLY,
                                       in_buff_size, kBuffers, &err));

  OCL_CHECK(err, cl::Buffer buffer_out(
                     context, CL_MEM_USE_HOST_PTR | CL_MEM_WRITE_ONLY,
                     out_buff_size, out, &err));

  // Setting Kernel Arguments

  OCL_CHECK(err, err = krnl_arb.setArg(0, buffer_in));
  OCL_CHECK(err, err = krnl_arb.setArg(1, buffer_out));
  OCL_CHECK(err, err = krnl_vadd.setArg(0, buffer_in));
  OCL_CHECK(err, err = krnl_vadd.setArg(1, buffer_kernel));
  OCL_CHECK(err, err = krnl_vsub.setArg(0, buffer_in));
  OCL_CHECK(err, err = krnl_vsub.setArg(1, buffer_kernel));
  OCL_CHECK(err, err = krnl_vmult.setArg(0, buffer_in));
  OCL_CHECK(err, err = krnl_vmult.setArg(1, buffer_kernel));

  // Copy input data to device global memory
  OCL_CHECK(
      err, err = q.enqueueMigrateMemObjects({buffer_in}, 0 /* 0 means from host*/));

  // Launch the Kernel
  OCL_CHECK(err, err = q.enqueueTask(krnl_arb));
  OCL_CHECK(err, err = q.enqueueTask(krnl_vadd));
  OCL_CHECK(err, err = q.enqueueTask(krnl_vsub));
  OCL_CHECK(err, err = q.enqueueTask(krnl_vmult));
  q.finish();

  // Copy Result from Device Global Memory to Host Local Memory
  OCL_CHECK(err, err = q.enqueueMigrateMemObjects({buffer_out},
                                                  CL_MIGRATE_MEM_OBJECT_HOST));
  q.finish();
  // OpenCL Host Code Ends

  // Compare the device results with software results
  bool match = verify(sw_res, out);

  return (match ? EXIT_SUCCESS : EXIT_FAILURE);
}
