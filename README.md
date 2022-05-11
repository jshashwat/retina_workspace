# Retina Scheduler Design Implementation



Basic Algorithm Flow: (Initial Iteration, No error handling)


Functions:
poll_running_queue():
	for tasks in running_queue:
		status = non_blocking_read(tasks[i].hlsstreamOut)
		if(status):
			if(tasks[i].nextstage == done):
				write_to_out_buffer();
				remove from queue;
			else if (cu_available(tasks[i].stages[next_stage].CU) && buffer_available):
				schedule tasks[i]; // next stage
				update_running_queue();
			else:
				add_to_wait_queue();

args (GMEM buffer start pointer, HLS stream connections)
arbitrar start:
load tasks: // Load and schedule tasks until CU exhaustion
	burst access gmem data to kernel memory;
	while (available CUs > 0 && pipelines > 0):
		if(pipeline[i] != SCHEDULED):
			if(cu_available(schedule pipeline[i].stages[0].CU)):
				schedule pipeline[i];
				CU_Counter[schedule pipeline[i].stages[0].CU]++;
				mark schedule pipeline[i] as SCHEDULED;
				i++;
				pipelines--;
			else:
				add_to_wait_queue(pipeline[i]);
		available_CUs = calculate_cus();

start scheduler:  // loop until wait queue and running queue both are empty
	while (!(wait_queue.empty() && running_queue.empty())):
poll_running_queue ();	
		exhaust_available_cus:
			// Check for more entries from host to load. (not sure??)
			for pipelines from host (available CUs > 0 && pipelines > 0):
				if(pipeline[i] != SCHEDULED):
					add_to_wait_queue();
				
			for (tasks in wait_queue): 
			if (cu_available(tasks[i].stages[current_stage].CU) && buffer_available):
					schedule tasks[i];
					add_to_running_queue();
					remove_from_wait_queue();
return:

Kernels Interface:

Input Arguments : (*in , *sub , HLS_IN, HLS_OUT) 

*in : Pointer to start the input buffer.
*sub : Pointer to start of the buffer used for internal processing.
HLS_IN: Input HLS stream connection with arbitrar to receive the processing header with all the information mentioned before
HLS_OUT: Output  HLS stream connection with arbitrar to send an acknowledgement to arbitrar to know that the processing is completed on the given input.

Header exchange needs to have a fields separately for kernel algorithm related arguments : Ex: Resizing (sizing window), Brightness (percentage) (Since kernels will be static and logic in arbitrer will have static knowledge of kernels, header population will be different for different kernels)

Output from Kernels: (Input Header for acknowledgement without the kernel specific inputs)’


General Algo for Computational Kernels:

kernel start:
	read kernel header();
	input_sanity_checks();
	call_standard_computational_api()
	prepare acknowledgement header;
	write to output stream;
end:
