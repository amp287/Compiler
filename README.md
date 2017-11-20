Compiler

To compile run gcc main.c Lexical_Analyzer.c VM.c

To run ./a.out -a -l -v -f [fileneame]
-a prints assembly output
-l prints lexical analyzer output
-v prints Virtual Machine output
-f specify a filename to be input which should be followed by a valid filename
	example: -f test.txt

Assembly Output for the virtual machine is placed in the file Assembly.txt

To route all output to a text file use 
./a.out -a -l -v -f [filename] > Output.txt
