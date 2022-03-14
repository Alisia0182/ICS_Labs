
# 需要处理的问题
```cpp

vm addr



    {".byte", 5, HPACK(I_DIRECTIVE, D_DATA), 1 },
    {".word", 5, HPACK(I_DIRECTIVE, D_DATA), 2 },
    {".long", 5, HPACK(I_DIRECTIVE, D_DATA), 4 },
    {".quad", 5, HPACK(I_DIRECTIVE, D_DATA), 8 },
    {".pos", 4,  HPACK(I_DIRECTIVE, D_POS), 0 },
    {".align", 6,HPACK(I_DIRECTIVE, D_ALIGN), 0 },

 /* Directive code */
typedef enum { D_DATA, D_POS, D_ALIGN } dtv_t;

init and finit symbol table???


malloc
free
sy



parse_instr是？？

```


long long 8 bytes

```cpp
unsigned long strtoul(const char* str, char ** str_end, int base)

size_t fwrite(const void* restrict buffer,
size_t size, size_t count, FILE * restrict stream
)

```
```cpp
fseek(out,addr,SEEK_SET) != 0
```


# 不急着处理的问题

```cpp
line_head
line_tail


lineno怎么更新

vmaddr怎么获得？
从0开始计算？code

```







# 奇怪的问题

```cpp
#define err_print(_s, _a ...) do { \
  if (lineno < 0) \
    fprintf(stderr, "[--]: "_s"\n", ## _a); \
  else \
    fprintf(stderr, "[L%d]: "_s"\n", lineno, ## _a); \
} while (0);
```


#define IS_BLANK(s) (*(s)==' ' || *(s)=='\t')

lineno有啥用？？
```cpp
fgets?


```


```

is_end()

```


typedef enum { D_DATA, D_POS, D_ALIGN } dtv_t;


    {".byte", 5, HPACK(I_DIRECTIVE, D_DATA), 1 },
    {".word", 5, HPACK(I_DIRECTIVE, D_DATA), 2 },
    {".long", 5, HPACK(I_DIRECTIVE, D_DATA), 4 },
    {".quad", 5, HPACK(I_DIRECTIVE, D_DATA), 8 },
    {".pos", 4,  HPACK(I_DIRECTIVE, D_POS), 0 },
    {".align", 6,HPACK(I_DIRECTIVE, D_ALIGN), 0 },



        reltab = (reloc_t *)malloc(sizeof(reloc_t)); // free in finit
    memset(reltab, 0, sizeof(reloc_t));


## 如何单步调试

http://www.atjiang.com/Beejs-quick-guide-to-GDB/

https://ipads.se.sjtu.edu.cn/courses/ics/tutorials/gdb-ref.txt

makefile里

```
CFLAGS=-g -Wall
```

```
> make clean
> make
> gdb y64asm
> b main    # b y64asm.c:483
> r -v ./y64-ins/jmp.ys # r: run
>layout src
> clear main #删除main处断点
```
./y64asm -v ./y64-ins/halt.ys



 return
 *     PARSE_DIGIT: success, data token is a digit,
 *                            and move 'ptr' to the first char after token,
 *                            and store the value of digit to 'value'
 *     PARSE_SYMBOL: success, data token is a symbol,
 *                            and move 'ptr' to the first char after token,
 *                            and allocate and store name to 'name' 
 *     PARSE_ERR: error, the value of 'ptr', 'name' and 'value' are undefined
 */
parse_t parse_data(char **ptr, char **name, long *value)