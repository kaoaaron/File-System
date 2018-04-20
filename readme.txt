Aaron Kao
V00773547
CSC 360 Assignment 3

The project is split into 4 pieces of code. To compile all of them, just run the makefile by typing in 'make' in the terminal.
The gnu99 standard is used so that variables could be declared within for loops. Although -lm (math.h) is declared within each file, it wasn't used as the final project.
It was kept in for troubleshooting.

Acknowledgments: This assignment was completed for CSC 360 - Operating Systems taught by Dr. Jianping Pan in Spring 2018 from the University of Victoria.
				The sample file system included is also from Dr. Pan.

==================
File 1: diskinfo.c
==================
This code gives the information contained in the superblock by using the struct provided in Tutorial 10. The struct is kept in order of the structure of the superblock, so that the contents could be accessed directly. To obtain the FAT information, 0s (free block), 1s (reserved block), and other values (allocated blocks) were counted within the FAT blocks.

--------------
Example Output
--------------
Super block information:
Block size: 512
Block count: 6400
FAT starts: 2
FAT blocks: 50
Root directory start: 53
Root directory blocks: 8

FAT Information:
Free Blocks: 6189
Reserved Blocks: 50
Allocated Blocks: 161

--------------
How to run
--------------
./diskinfo test.img

==================
File 2: disklist.c
==================

This code outputs the contents in the root folder of the file system. The contents were read from the img file similar to part 1, but within the section containing root directory files/folders info. Most of the output was printed out byte-by-byte. ***Please note that the standard spacing was NOT used that was requested because of the way this was programmed.

--------------
Example Output
--------------
F        735                              mkfile.cc	2005/11/15 12:00:00
F       2560                              foo.txt	2005/11/15 12:00:00
F       1402                              foo3.txt	2005/11/15 12:00:00
F       3940                              disk.img.gz	2009/08/04 21:11:13

--------------
How to run
--------------
./disklist test.img

==================
File 3: diskget.c
==================

This code searches the root directory section within the img file to see whether or not a file exists. If it exists, then the FAT section is navigated to find the blocks that contain the data for the file. The data is then written onto a file on the current working directory on the machine. If it doesn't exist, 'FILE NOT FOUND' is printed to the console.

--------------
How to run
--------------
./diskget test.img foo.txt

==================
File 4: diskput.c
==================

If not already in the file system, the code reads byte by byte a file and stores it within the file system. First it checks if it exists by checking the root directory section. If the file doesn't exist, it allocates blocks from the FAT section, and then stores the data in these blocks. ***Note that there is no 'overwrite' feature. If diskput is used for a file that already exists in the file system with different content, nothing will happen.

--------------
How to run
--------------
./diskput test.img foo.txt
