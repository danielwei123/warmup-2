warmup-2
========


1. To compile the program just give make in the command line.

2. My Interrupt handler works as follows:
	 When Ctrl+C is pressed the control goes to the main function.
	 The main function calls the interrupt function.
	 In the interrupt function: The thread,arrival  is cancelled. A signal is broadcasted to the server to wakeup then it
   serves the cureent packet and then exits. 
