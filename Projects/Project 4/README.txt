In this directory, you should write the program `scheduler.c` and compile it into the binary `scheduler` (e.g., `gcc -o scheduler scheduler.c -Wall -Werror`). You should do it inside the Ubuntu machine you have setup, as grading will be done so inside the Ubuntu.

After doing so, you can run the tests from this directory by running the
`./run_tests.sh` script. If all goes well, you will see standard output that indicate that all tests are passed.


We used a linkedlist type for our job list. In the main function, we opened the input file and initalized our job list. We then scanned through the file, getting the time a job
takes to complete and putting that in out list. We get the final list when we reach the end of the file and close the file. After that, we used conditionals with strcmp to 
determine which scheduling policy to run. 

For FIFO, we looped through the singly linkedlist, printing out the execution trace along the way. We then reinitlaize the looping
variable, and loop through the list again, this time calculating the metrics asked for part 2. We decided to loop through twice in order to get matching output to the sample. 
In hindsight, we could have used an array of structs to hold the data from the first loop like we did for Round Robin, but we didn't. After getting the metrics, we can print out the 
current job metrics in the loop, then out of the loop, we calculate the average metrics, by dividing the totals by the number of jobs, which we got from a counter. We print and done. 

For SJF, we first get the count for the total number of jobs. Then we use a array of job structs, which we use for sorting by time. We get this array by looping through linkedlist again,
and set the values at their correct indicies. Once we have the array, we use bubble sort to sort the job array in ascending order based on their length. Once we have the sorted array, 
we can run the jobs in the SJF order and print out the execution trace and the statistics. 

For Round Robin, we first get the number of jobs by looping through the linkedlist. We then initalized two arrays to keep trach of the remaining times for each job and the arrival times. 
We also have an array of job_analysis structs, in which we store the per-job analysis. For the execution, we handled the case of only 1 job by having a conditional to check for it. If there
is only one job, that job should run without any interference. So we have the code there to match the sample test output. For the else case where we have multiple jobs, we have a loop checking 
if the number of jobs completed is less than the number of jobs. If this is not the case, then the jobs have all finished running, and we can print out the statistics. We have another while loop
which processes each job in the list. we store the arrival times in the array by the job id, and determine the exection time/how long we execute the job. We choose between the time slide or the
remaining time, whichever is smaller. We print the exection trace, then update the remaing time. We check if a job is complete by the remaining time and store the metrics in the analysis array to be printed later. 
we also update the total metrics. After these two loops are complete, we free the arrays from memory, print the per-job analysis from the analysis array, calculate the average metrics and print them as well. 
THAT'S IT!

____________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________________
WORKLOADS

For workload_1.in: 
RR will have the wait time equal to the response time if we have all of the jobs be the same length which is that time slice. 
In this way it works because there is no prememtive measures.


For workload_2.in: 
SJF always gives the same turnaround time as FIFO except when a shorter job arrives after a longer job. 
As long as all job lengths are either equal or arrive in ascending order, the turnaround times will be the same.

We want to put the large numbers in the input file first. In this way, we can have the FIFO scheduler run the 
longest jobs first, which will increase the average Turnaround time. On the other hand, SJF will have a lower average
Turnaround time because it runs the shortest job first. 


For workload_3.in: 
For FIFO, all jobs we set them to the same length so that FIFO will execute them in the order they arrive, resulting in the completion order
and the resulting metrics (response time, turnaround time, and wait time) will be the same for all jobs for FIFO, meaning a Known Average Value

For SJF, all jobs have the same length (3 units), so SJF will prioritize them based on their arrival order, and this order wont matter. 
And the resulting metrics (response time, turnaround time, and wait time)  will be the same for all jobs of SJF, meaning a known Average value, the same value as FIFO.

For Round Robin, the time slice is set to 3 units, and each job has a length of 3 units. This means that each job can complete within a single time slice. 
The order of execution will follow the input file sequence. Since all jobs complete within the same time slice, the metrics for each job will be the same. 

So with an input file of 3,3,3,3,3  We are able to get the same average response time, turnaround time, and wait time across all Policies.


For workload_4.in: 
The strategy is to minimize the wait time. We accomplish this by first putting short jobs, of time unit 1, which is smaller than given time slice of 3
Such that we minimize the time they spend waiting in ready, which reduces overall wait time. Also, for the average turnaround time, we decided on this design, as 
interleaving the long and short jobs would allow for the long job to take much more time to turnaround, therefore increasing our overall average turnaround time.  


For workload_5: 
Logic behind this answer is that first of all we know the first job is 3 time units. Also, we know how to calculate average response time, which is 
the (0(for first job) + (prev job turnaround time 'job1' ) + (prev job turnaround time 'job2' )) / (3). Knowing our intended Average respond time of 5, solve for 
the amount of total 'time' for job 2, apart from that 3 time unit, to essentially do (3+x)/3 = 5. You should find job2 to have a time unit of 9.

Once we find the amount of 'time' for job2, we move onto the Avg turnaround time 
which is intended to be 13. Average turnaround time is calculated by doing ((Time unit of job1)+(time unit job 1 + time unit job 2)+(time unit job 1 + time unit job 2 + time unit job 3))/(3)
Knowing this info, plug in our time unit for job2.

Once we do this, plug in values. In our case we did (3+ 3+9 +3+9+x)/(3) = 13, x being our remaining'time' for slot 3 that we are looking for. Solve for this slot3 time unit, 
then check answer either mathmatically or running the code. Solution can be seen in workload_5.in