# threaded-matrix-mult
A multi-threaded matrix multiplication tool.

In this project, I developed a tool to optimize matrix multiplication with multiple CPU threads. 

For example, say we wanted to multiply an NxM matrix with a MxP matrix, resulting in a product matrix with dimensions NxP (N rows and P columns). The user will input the number of threads he wishes to use as numberOfThreads.

I decided to split the work for each thread by giving each thread a unique set of rows of the resulting product matrix to calculate. That way, there are no conflicts between the work each thread does. Thus, thread synchronization via semaphores is not required. To split up the work by rows or "slice" the product matrix's N rows, I knew I needed to calculate the fromRow and toRow matrix indices of the product matrix for each thread to calculate or "fill in". I also wanted the tool to work if N was not divisible by numberOfThreads.

Here are the formulas I came up with for figuring out the fromRow and toRow indices:

fromRow = ((threadNumber-1) x N)/numberOfThreads
toRow = ((threadNumber) x N)/numberOfThreads-1

As shown below, this matrix slicing algorithm works even when N is not divisible by numberOfThreads.

Case for N = 2400, numberOfThreads = 7(2400 is not divisible by 7):
Thread 1: fromRow = 0, toRow = 341 
Thread 2: fromRow = 342, toRow = 684  
Thread 3: fromRow = 685, toRow = 1027
Thread 4: fromRow = 1028, toRow = 1370
Thread 5: fromRow = 1371, toRow = 1713
Thread 6: fromRow = 1714, toRow = 2056
Thread 7: fromRow = 2057, toRow = 2399

Example calculations for threads 6 and 7:
Thread 6: fromRow = (5x2400)/7 = 1714.29 => 1714      toRow = ((6x2400)/7)-1 = 2056.14 => 2056
Thread 7: fromRow = (6x2400)/7 = 2057.14 => 2057      toRow = ((7x2400)/7)-1 = 2399 => 2399
I stored both fromRow and toRow into integers, so they rounded down.

Once the fromRow and toRow indices are computed for each thread, the threads can then use the basic matrix multiplication process to “fill in” their respective portions of the resulting product matrix. Increasing the number of threads will always result in performance gains until the # of physical CPU cores is maxed. 
