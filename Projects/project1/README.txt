Regarding the slugrace program, I had many difficulties in getting the clock working. Notice that the at top of the program, I defined constantas for POSIX_C_SOURCE and _DEFAULT_SOURCE. 
This was done to enable me to use things like the monotonic clock and the clock_gettime function. Not sure why I had to include those things or if the additions are uneccessary.

I tested my programs by comparing my output to the example output and also by reading the outputs to make sure that they are correct.

For the slugrace program, I used an array of pid_t size 4 to store the PID numbers of the children/slugs. This was so that I could print out information regarding the race progress using the children's PIDS. 

