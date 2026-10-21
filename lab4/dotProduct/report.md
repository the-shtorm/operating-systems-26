```
CASE WITH 120 ELEMENTS:
Procs   | Avg Time (mc sec)
------------------------
1       | 139.800000
6       | 446.600000
20      | 1429.500000
60      | 3947.100000
120     | 7788.000000

CASE WITH 1M ELEMENTS:
Procs   | Avg Time (mc sec)
------------------------
1       | 194.600000
10      | 753.200000
100     | 7521.300000
1000    | 60488.400000
10000   | 493845.300000
// it's hard to run more. At least my system didn't allow me to create more than 10 000 forks
```

### General explanation

Here any process usage increased program execution time. I believed, 10 processes for 1M elements will speedup program, but even in that case it is not true. It happened because process creation is heavy task, which is too hard even with optimizations i tried to implement. According to some googling (and my table), system spends around 50 mcsec to allocate memory for process, find available pid in process tree, and then clearing all process remainings after it's done caclulating. So with 194 mcsec total time for 1M elements, 50 mcsec systems spends to create that single child, then it calculates everything in 150 mcsec and we are done. so if we create 10 processes, it increases total time by at least 500 mcsec

### Some random thoughts

```
// -- Small explanation of what i did --
// I used shared memory with mmap to avoid array copying
// between thousands of child processes 
// Also i used pwrite (at least tried) to avoid races in file
// writing between multiple processes, because i think 10 000
// processes will definetly somehow interfere during file writing
// Additionally i executed each experiment several times to get 
// somewhat more stable results
// Also i used `short` data type for arrays to reduce memory load 
// (i'm not sure is it really useful here)
```

I tried to optimize system heavy tasks (like reqd/write and array copying), but it didn't do much. Maybe in case of threads or some other async/multiprocess computation we will get some time benefits, but right now processes are too slow

Also i cannot even theoretically put 1M processes in my ram, because i have 16gb, active processes cannot be put into swap (so zram cannot help) and i will have to use at least 32gb of ram, which is more than i have