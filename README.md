# MINIX3_Modified
MINIX 3 is a free, open-source, operating system designed to be highly reliable, flexible, and secure. http://www.minix3.org

## 1. Scheduling Algorithms

In this part I modified MINIX 3 to include the following scheduling algorithms:

• Round Robin

• Shortest Job First (SJF)

• Priority based

• Multi-Level Feedback Queue

The user should be allowed to edit a configuration file in the operating system to provide any required parameters or
assumptions for these scheduling algorithms. Real processes was executed and each algorithm was tested by
computing the average turnaround time and waiting time for each algorithm for a set of processes that start, execute, and end
in specific times. A detailed comparative analysis and explaination of the results was included in the report.


## 2. Memory managment

In this part, I implemented hirarchical paging with all the needed parameters (page size, number
of levels, address format … etc.) are user-defined via a configuration file. Additionally, FIFO and LRU page replacement
algorithms were implemented (configuration parameters of these algorithms should be stored in configuration file too).
The performance parameters (e.g., number of page faults, number of empty frames … etc.) of the hirarchical paging as well2/2
as the replacement algorithms were collected as the the size of the pages and the number of levels are changed, with a
complete analysis were provided in the report.


## 3. Disk space managment 

I found how MINIX 3 manages empty space, then I modified disk-space management
code in MINIX 3 to use user-defined extents. Also, the disk allocation was modified to provide the allocation in terms of
the user-defined extents. The extent itself consists of a set of disk blocks where they are handled as a single unit. User should
provide all needed configuration in a configuration file that is going to be read by MINIX 3 and adopted accordingly. The
performcnce results of this method with respect to the number of blocks in the extent were collected, explained, and
presented in the report.
