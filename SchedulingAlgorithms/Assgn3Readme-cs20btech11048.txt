OPERATING SYSTEMS - 2 (CS3523)
PROGRAMMING ASSIGNMENT 3 README FILE

The GNU compiler has to be downloaded to compile the program.

--To compile and run the program using G++ command--

1. Given file is of the type Assgn3-cs20btech11048.zip
2. Extract the zip file and open the directory containing the C++ files Assgn3-RMScs20btech11048.c and 
     Assgn3-EDFcs20btech11048.c
3. Use the following command to compile the two c++ programs.
     g++ -o <executable_name> <c++ file name>  
4. Use the following command to run the executables.
    ./<executable_name>
5. When the program runs the input is taken from the file named "input.txt" which has the inputs in a
	in the required order (as mentioned below).
6. The files RM-Log.txt and RF-Stats.txt files are created as per the question when the RM executable is run.
    Similarly, EDF-Log.txt and EDF-Stats.txt files are formed when the EDF executable is run

--Input File Format--
1. Name of input file must be "input.txt" (an input.txt file is in the zip file for reference)
2. The first line of the file must contain the number of process, n value.
3. The n next lines contain the processId, processing time, period, no of times the process should run for each of
	the processes (one line for each process) in the respective order
4. On the execution of the executables, the required files are created.
6. An input file is included in the zip file for reference.

