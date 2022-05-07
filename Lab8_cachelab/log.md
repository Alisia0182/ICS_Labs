 
modify:
```
csim.c
trans.c
```
to compile：
```
> make clean
> make
```
# part A. simulate the behavior of a cache memory
```
csim.c
```
simulate the **hit/miss** behavior of a cache memory of this trace
output the total number of **hits,misses and evictions**


csim-ref
```
> ./csim-ref -h

Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>
Options:
  -h         Print this help message.
  -v         Optional verbose flag.
  -s <num>   Number of set index bits.
  -E <num>   Number of lines per set.
  -b <num>   Number of block offset bits.
  -t <file>  Trace file.

Examples:
  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace
  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace

```

arbitary s, E, and b



# part B. optimize a small matrix transpose function
minimize the number of **cache misses**


# Trace Files
to evaluate the correctness of Part A
```
> valgrind --log-fd=1 --tool=lackey -v --trace-mem=yes ls -l
```
runs "ls -l"

```
I: instruction load     // ignore I
 L: data load
 S: data store
 M: data modify(a data load followed by a data store)
```

# Problems
1. hit miss eviction?
2. 
   LRU:
   least recently used

# tips:
1. getopt
   
   字符串optstring可以下列元素，
1.单个字符，表示选项，
2.单个字符后接一个冒号：表示该选项后必须跟一个参数。参数紧跟在选项后或者以空格隔开。该参数的指针赋给optarg。
3 单个字符后跟两个冒号，表示该选项后必须跟一个参数。参数必须紧跟在选项后不能以空格隔开。该参数的指针赋给optarg。（这个特性是GNU的扩张）。

2. 文件读写
   <stdio.h>
   fopen()函数
   ```c
   FILE *fopen(char * filename, char * mode);
   ```

   ```c
       
    //File read test
    
    FILE* file_pointer;
    if((file_pointer = fopen(*trace_file,"rt")) == NULL){
        puts("Fail to open");
        exit(0);
    }
    char str[N+1];
    while(fgets(str,N,file_pointer)!=NULL){
        printf("%s",str);//带换行
    }
    s
    ```

9 * 9 2093
9 * 8 2073
8 * 9 2139

16 * 8 1954
16 * 16 1993

17 * 6 1891
17 * 8 1911
17 * 12 1970
17 * 16 1954

24 * 24 2016

20 * 8 1925
20 * 12 1991
20 * 16 1959