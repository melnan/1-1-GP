# 1_1_GP
Here you can find a realization of the (1+1)GP with (and without) bloat control on +c-Majority and 2/3-Majority. The description of the algorithm can be found in the paper https://arxiv.org/abs/1805.10169.

Input.
All input parameters are listed in the preamble of the source file (lines 13-20). The description of each variable is given in the code.
We use s_init = 10*n, c = 2.
The realization performs a sequence of the algorithm runs iteratively increasing the number of variables (n) (see line 338).


Output.
Output consists of 8 txt-files named [type of the content]\_[type of the fitness function]\_[s_init]\_Tinit\_[number of algorithm runs]. Each file contains a list of the corresponding output values for each n value over all runs of the algorithm. 
The only exception is the file time\_[type of the fitness function]\_[s_init]\_[number of algorithm runs] which contains the total number of iterations performed to reach the optimal value on each run of the algorithm. 

