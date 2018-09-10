/**
 * Name: Shuangchang Li, Qiuan Wu
 * Assignment 1
 * 
 * I pledge my honor that I have abided by the Stevens Honor System.
 * 
**/

Result Analysis

Case1:
./assign1 4 4 100 10 0 0 10
Result:
-----------------------------
         INFORMATION
-----------------------------
Total consumption time:      2.52341s
-----------------------------
Min turn-around time:        0.00195312s
Max turn-around time:        0.0117188s
Avg turn-around time:        0.00666016s
-----------------------------
Min waiting:                 0.00195312s
Max waiting:                 0.0117188s
Avg wait time:               0.000117187s
-----------------------------
Producer throughput:         2280 products per minute
Consumer throughput:         2340 products per minute
-----------------------------
Total run time:              2.61252s

Case2:
./assign1 1 1 15 10 1 20 5

Result:
-----------------------------
         INFORMATION
-----------------------------
Total consumption time:      17.2547s
stoptime: 17741
c_starttime: 17723
-----------------------------
Min turn-around time:        0.101562s
Max turn-around time:        12.3379s
Avg turn-around time:        2.72174s
-----------------------------
Min waiting:                 0s
Max waiting:                 17.2539s
Avg wait time:               2.72174s
-----------------------------
Producer throughput:         0 products per minute
Consumer throughput:         0 products per minute
-----------------------------
Total run time:              17.2559s

Case3:
./assign1 1 2 15 10 1 20 5

-----------------------------
         INFORMATION
-----------------------------
Total consumption time:      8.62638s
stoptime: 17822
c_starttime: 17813
-----------------------------
Min turn-around time:        0s
Max turn-around time:        6.11719s
Avg turn-around time:        1.33737s
-----------------------------
Min waiting:                 0s
Max waiting:                 8.42773s
Avg wait time:               1.33737s
-----------------------------
Producer throughput:         60 products per minute
Consumer throughput:         60 products per minute
-----------------------------
Total run time:              8.62712s

In Conclusion:
Size of quantum affects the RR heavily. The higher the quantum is, the faster the higher the throughput will be.

First come first serve is much faster than the Round Robin algorithms in the test cases above. 

In cases of small size or large size, first come first serve always beats the RR in terms of overall speed. However.
in some cases the RR will have faster MIN time for waiting slightly.
