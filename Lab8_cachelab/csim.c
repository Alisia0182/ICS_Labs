/*
 * name:        万金易
 * studentID:   519021911049
 * loginID:     ics519021911049
 * 
 * arbitrary s E b
 * 每个block存一下用了多少个？（初始顺序寻空可以快一点
 * 有必要么
 */
//for parse command line arguments
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include "cachelab.h"
#include <stdio.h>//fopen 打开文件
#include <stdbool.h>
#include <time.h> //to get update_time from clock()
#include <string.h>
#define line_sz 30

typedef struct line_t{
    bool is_valid;
    size_t tag;
    clock_t last_time;
} line_t;

typedef struct set_t{
    line_t* line_array; 
} set_t;

set_t * my_cache;           //cache是一个set的数组
size_t s = 0,S = 0, 
             E = 0,
       b = 0,B = 0;
       
void init_cache(){
    my_cache = (set_t *)malloc(sizeof(set_t) * S);
    for(size_t i = 0; i < S; ++ i)
    {
        my_cache[i].line_array = (line_t*)malloc(sizeof(line_t) * E);//E lines
        //is_valid初始化为0？
    }    
}

void print_help(){
    printf("Usage: ./csim-ref [-hv] -s <num> -E <num> -b <num> -t <file>\n");
    printf("Options:\n");
    printf("  -h         Print this help message.\n");
    printf("  -v         Optional verbose flag.\n");
    printf("  -s <num>   Number of set index bits.\n");
    printf("  -E <num>   Number of lines per set.\n");
    printf("  -b <num>   Number of block offset bits.\n");
    printf("  -t <file>  Trace file.\n");
    printf("\n");
    printf("Examples:\n");
    printf("  linux>  ./csim-ref -s 4 -E 1 -b 4 -t traces/yi.trace\n");
    printf("  linux>  ./csim-ref -v -s 8 -E 2 -b 4 -t traces/yi.trace\n");
}

//print_verb(ins_op,addr,size,hit_flag,miss_flag);
void print_verb(char ins_op,size_t addr, int size, bool hit_flag, bool miss_flag){
    printf("S 600a9c,4 hit\n");
    
    if(hit_flag)
        printf("%c %lx,%d hit",ins_op,addr,size);
    else if(miss_flag)
        printf("%c %lx,%d miss",ins_op,addr,size);
    else
        printf("%c %lx,%d miss eviction",ins_op,addr,size);
    if(ins_op == 'M')
        printf(" hit");
    printf("\n");
}
//set select and line match
line_t* find_hit(size_t _tag,size_t _set){
    line_t * line_ptr = (my_cache + _set)->line_array;
    for(size_t i = 0; i < E; ++ i){
        if(line_ptr->tag == _tag && line_ptr->is_valid)
            return line_ptr;
        ++line_ptr;
    }
    return NULL;
}


//miss的情况下 寻找插的位置
//find_place(find_set_ptr);//
line_t* find_place(set_t* _set_ptr){
    line_t * line_ptr = _set_ptr->line_array;
    line_t * found_line_ptr= _set_ptr->line_array;
    for(size_t i = 0; i < E; ++i){
        if(!line_ptr->is_valid){
            found_line_ptr = line_ptr;
            break;
        }
        //
        if(line_ptr->last_time < found_line_ptr->last_time)
            found_line_ptr = line_ptr;//LRU policy
        ++line_ptr;
    }
    return found_line_ptr;
}
void update_status(line_t * line_ptr,bool _valid,size_t _tag){
    line_ptr->is_valid = _valid;
    line_ptr->tag = _tag;
    line_ptr->last_time = clock();
}


int main(int argc,char * argv[])
{
    //get options
    //getopt:单个字符后接一个冒号：表示该选项后必须跟一个参数。参数紧跟在选项后或者以空格隔开。该参数的指针赋给optarg
    int opt;
    bool help_flag,verb_flag;
    char** trace_file;
    trace_file = (char**)malloc(sizeof(char*));
    
    while((opt = getopt(argc,argv,"hvs:E:b:t:"))!= -1){
        switch(opt){
            case 'h':
                help_flag = true;
                break;
            case 'v':
                verb_flag = true;
                break;
            case 's':
                s = atoi(optarg);
                S = 1 << s;
                break;
            case 'E':
                E = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                B = 1 << b;
                break;
            case 't':
                *trace_file = optarg;
                break;
            default:
                break;
        }
    }
    
    if(help_flag){
        print_help();
        return 0;
    }
    
    init_cache();
    
    FILE * file_pt;
    if((file_pt = fopen(*trace_file,"rt")) == NULL){
        puts("Fail to open file");
        exit(0);
    }
    char  str[line_sz+1];
    char  ins_op;
    size_t addr;//64-bit address
    int size;//the number of bytes
    line_t* find_line_ptr;
    set_t*  find_set_ptr;
    int hit_cnt = 0, miss_cnt = 0, evict_cnt = 0;
    
    size_t tag_bits,set_bits;//,blk_bits;
    size_t tag_mask,set_mask,blk_mask;
    //init_masks
    blk_mask = B - 1;
    set_mask = (S - 1) << b;
    tag_mask = ~(blk_mask|set_mask);
    bool hit_flag;
    bool miss_flag;
    
    while(fgets(str,line_sz,file_pt)!=NULL){
        if(str[0] == 'I')
            continue;
        //parse line
        sscanf(str," %c %lx,%d",&ins_op,&addr,&size);
        //parse masks
        tag_bits = (addr & tag_mask) >> (s + b);
        set_bits = (addr & set_mask) >> b;
        //blk_bits = addr & blk_mask;
        
        //t s b
        find_set_ptr = my_cache + set_bits;
        if((find_line_ptr = find_hit(tag_bits,set_bits) )!= NULL){//在cache里
            hit_flag = true;
            ++hit_cnt;
        }
        else{
            miss_flag = true;
            ++miss_cnt;
            find_line_ptr = find_place(find_set_ptr);
            //驱逐？结合instr_type
            if(find_line_ptr->is_valid)//找到的那一条存了数据 需要被驱逐出cache
                ++evict_cnt;
        }
        hit_cnt += (ins_op == 'M');
        update_status(find_line_ptr,1,tag_bits);
        
        //if verb_flag
        if(verb_flag)
            print_verb(ins_op,addr,size,hit_flag,miss_flag);
    }
    
    
    printSummary(hit_cnt,miss_cnt,evict_cnt);
    return 0;
}
