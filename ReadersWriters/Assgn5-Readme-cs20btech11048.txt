OPERATING SYSTEMS - 2 (CS3523)
PROGRAMMING ASSIGNMENT 5 README FILE


--To compile and run the program using G++ command--

1. Commands to compile the source files.
     	g++ -o <executable_name> SrcAssgn5-RW-cs20btech11048.cpp -pthread
	g++ -o <executable_name> SrcAssgn5-FRW-cs20btech11048.cpp -pthread

2. Command to run the executables.
    ./<executable_name>

5. When the program runs the input is taken from the file named "inp-params.txt" which has the inputs in a
	in the required order (as mentioned below) and the log file of that algorithm is created containing
	the logs.

6. The output log files are RW-Log.txt and FairRW-Log.txt .


--Input File Format--

1. Name of input file must be "inp-params.txt" (an inp-params.txt file is in the zip file for reference).
2. The input file should contain nw nr kw kr lambda1 lambda2 in the same order where,
	nw - number of writer threads
	nr - number of reader threads
	kw - number of times each writer thread should run
	kr - number of times each reader thread should run

	lambda1 - mean of the exponential distribution for critical section
	lambda2 - mean of the exponential distribution for remainder section


