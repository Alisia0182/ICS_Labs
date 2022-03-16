/* 
 * CS:APP Data Lab 
 * Author: Alice Wan
 * 
 * WARNING: Do not include the <stdio.h> header; it confuses the dlc
 * compiler. You can still use printf for debugging without including
 * <stdio.h>, although you might get a compiler warning. In general,
 * it's not good practice to ignore compiler warnings, but in this
 * case it's OK.  
 */
 /** dlc compiler **/
 
#include "btest.h"
#include <limits.h>

/*
 * Instructions to Students:
 *
 * STEP 1: Fill in the following struct with your identifying info.
 */
team_struct team =
{
   /* Team name: Replace with either:
      Your login ID if working as a one person team
      or, ID1+ID2 where ID1 is the login ID of the first team member
      and ID2 is the login ID of the second team member */
    "519021911049", 
   /* Student name 1: Replace with the full name of first team member */
   "Wan Jinyi",
   /* Login ID 1: Replace with the login ID of first team member */
   "519021911049",

   /* The following should only be changed if there are two team members */
   /* Student name 2: Full name of the second team member */
   "",
   /* Login ID 2: Login ID of the second team member */
   ""
};

#if 0
/*
 * STEP 2: Read the following instructions carefully.
 */

You will provide your solution to the Data Lab by
editing the collection of functions in this source file.

CODING RULES:
 
  Replace the "return" statement in each function with one
  or more lines of C code that implements the function. Your code 
  must conform to the following style:
 
  int Funct(arg1, arg2, ...) {
      /* brief description of how your implementation works */
      int var1 = Expr1;
      ...
      int varM = ExprM;

      varJ = ExprJ;
      ...
      varN = ExprN;
      return ExprR;
  }

  Each "Expr" is an expression using ONLY the following:
  1. Integer constants 0 through 255 (0xFF), inclusive. You are
      not allowed to use big constants such as 0xffffffff.
  2. Function arguments and local variables (no global variables).
  3. Unary integer operations ! ~
  4. Binary integer operations & ^ | + << >>
    
  Some of the problems restrict the set of allowed operators even further.
  Each "Expr" may consist of multiple operators. You are not restricted to
  one operator per line.

  You are expressly forbidden to:
  1. Use any control constructs such as if, do, while, for, switch, etc.
  2. Define or use any macros.
  3. Define any additional functions in this file.
  4. Call any functions."ATTENTION: No -"
  5. Use any other operations, such as &&, ||, -, or ?:
  6. Use any form of casting.
 
  You may assume that your machine:
  1. Uses 2s complement, 32-bit representations of integers.
  2. "Performs right shifts arithmetically."算术右移
  有符号负数 右移 高位补1
  
  3. Has unpredictable behavior when shifting an integer by more
     than the word size.

EXAMPLES OF ACCEPTABLE CODING STYLE:
  /*
   * pow2plus1 - returns 2^x + 1, where 0 <= x <= 31
   */
  int pow2plus1(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     return (1 << x) + 1;
  }

  /*
   * pow2plus4 - returns 2^x + 4, where 0 <= x <= 31
   */
  int pow2plus4(int x) {
     /* exploit ability of shifts to compute powers of 2 */
     int result = (1 << x);
     result += 4;
     return result;
  }


NOTES:
  1. Use the dlc (data lab checker) compiler (described in the handout) to 
     check the legality of your solutions.
  2. Each function has a maximum number of operators (! ~ & ^ | + << >>)
     that you are allowed to use for your implementation of the function. 
     The max operator count is checked by dlc. Note that '=' is not 
     counted; you may use as many of these as you want without penalty.
  3. Use the btest test "harness" to check your functions for correctness.
  4. The maximum number of ops for each function is given in the
     header comment for each function. If there are any inconsistencies 
     between the maximum ops in the writeup and in this file, consider
     this file the authoritative source.
#endif

/*
 * STEP 3: Modify the following functions according the coding rules.
 * 
 *   IMPORTANT. TO AVOID GRADING SURPRISES:
 *   1. Use the dlc compiler to check that your solutions conform
 *      to the coding rules.
 *   2. Use the btest test harness to check that your solutions produce 
 *      the correct answers. Watch out for corner cases around Tmin and Tmax.
 */
/* Copyright (C) 1991-2020 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <https://www.gnu.org/licenses/>.  */
/* This header is separate from features.h so that the compiler can
   include it implicitly at the start of every compilation.  It must
   not itself include <features.h> or any other header that includes
   <features.h> because the implicit include comes before any feature
   test macros that may be defined in a source file before it first
   explicitly includes a system header.  GCC knows the name of this
   header in order to preinclude it.  */
/* glibc's intent is to support the IEC 559 math functionality, real
   and complex.  If the GCC (4.9 and later) predefined macros
   specifying compiler intent are available, use them to determine
   whether the overall intent is to support these features; otherwise,
   presume an older compiler has intent to support these features and
   define these macros by default.  */
/* wchar_t uses Unicode 10.0.0.  Version 10.0 of the Unicode Standard is
   synchronized with ISO/IEC 10646:2017, fifth edition, plus
   the following additions from Amendment 1 to the fifth edition:
   - 56 emoji characters
   - 285 hentaigana
   - 3 additional Zanabazar Square characters */

/* 
 * bang - Compute !x without using !
 *   Examples: bang(3) = 0, bang(0) = 1
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 12
 *   Rating: 4 
 */

int bang(int x) {
/* Explanation:
 * return 1 if and only if (x==0)
 * otherwise return 0
 * 
 * Example:
 * 0000 0000 0000 0000 maps to 0000 0000 0000 0001
 * 1010 0101 0101 0101 maps to 0000 0000 0000 0000   
 * 0101 1010_1010 1010
 * 0101 0010 0000 0000 maps to 0000 0000 0000 0000
 * 
 * Hints:
 * 1. a|b is 0 if and only if both a and b are 0
 * 2. how to map 0000 0000 to 1 and any other 8_bit_num to 0?
 *    remember the carry
 *    ((~x) + 1) >> 8
 *    only 0000 0000 will result in the ‘1’ in bit_pos_8(lowest to highest, 0-base)
 */
/*Wrong way
 * uses too many ops
 * but the idea is similar
int lowest_8_bits_reverse_and_255 = (~x) & 0xFF;
int lower_8_bits_reverse_and_255 = ((~x) >> 8) & 0xFF;
int higher_8_bits_reverse_and_255 = ((~x) >> 16) & 0xFF;
int highest_8_bits_reverse_and_255 = ((~x) >> 32) & 0xFF;
int lower_higher_and = lower_8_bits_reverse_and_255 & higher_8_bits_reverse_and_255;
//equal to 0xFF if and only if x == 0
int res = (lower_higher_and + 1) >> 8;
return res;
*/

/* 9 ops*/
    int compress_8_bits = ((x)|(x>>8)|(x>>16)|(x>>24)); //its lowest 8 bits contains all information I need
    int answer = (((~compress_8_bits)&0xFF) + 1) >> 8;
    return answer;
}
/*
 * bitCount - returns count of number of 1's in word
 *   Examples: bitCount(5) = 2, bitCount(7) = 3
 *   101    111
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 40
 *   Rating: 4
 */ 
int bitCount(int x) {
    /* 分治
     * https://zhuanlan.zhihu.com/p/353348665
     * 分治思想 本题使用了一个简单的分治思想，对于一个二进制数，要对其中为1的位做计数， 对于1位二进制数来说，1的个数无非就是其本身所表示的1或0。利用这个特性，我们可以先将一个二进制数每一位独立分开为相间隔的两部分, 其每位表示的就是自身的二进制个数，再将两串二进制数对其相加，所得到的每两位分隔的二进制数就是表达这个位置的位为1的个数。
     * 用这些掩码:
     * 0101 0101 0101 0101 0101 0101 0101 0101
     * 0011 0011 0011 0011 0011 0011 0011 0011
     * 0000 1111 0000 1111 0000 1111 0000 1111
     * 0000 0000 1111 1111 0000 0000 1111 1111
     * 0000 0000 0000 0000 1111 1111 1111 1111
     **/
    int temp;
    int l1;
    int l2;
    int l4;
    int l8;
    int l16;
    
    //generate masks:
    temp = (0x55 <<  8) + 0x55;
    l1   = (temp << 16) + temp;
    temp = (0x33 <<  8) + 0x33;
    l2   = (temp << 16) + temp;
    temp = (0x0F <<  8) + 0x0F;
    l4   = (temp << 16) + temp;
    temp =  0xFF;
    l8   = (temp << 16) + temp;
    l16  = (0xFF <<  8) + 0xFF;
    
    //add up：
    x = (x & l1 ) + ((x >> 1)  & l1 );
    x = (x & l2 ) + ((x >> 2)  & l2 );
    x = (x & l4 ) + ((x >> 4)  & l4 );
    x = (x & l8 ) + ((x >> 8)  & l8 );
    x = (x & l16) + ((x >> 16) & l16);
    
    return x;
}
/* 
 * copyLSB - set all bits of result to least significant bit of x
 *   Example: copyLSB(5) = 0xFFFFFFFF, copyLSB(6) = 0x00000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 5
 *   Rating: 2
 *   
 */
int copyLSB(int x) {
	int lastbit = x&0x1;
    int result = (lastbit << 31)>>31;
    return result;
}
/* 
 * divpwr2 - Compute x/(2^n), for 0 <= n <= 30
 *  Round toward zero
 *   Examples: divpwr2(15,1) = 7, divpwr2(-33,4) = -2
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 */
/**
 * observe these
 * we take n = 4
 * (-64 - 16, -64] the same answer
x0:	1111 1111 1111 1111 1111 1111 1100 0000 (-64)
y0:	1111 1111 1111 1111 1111 1111 1111 1100 

x1:	1111 1111 1111 1111 1111 1111 1011 1111 (-65)
y1:	1111 1111 1111 1111 1111 1111 1111 1100 

x2:	1111 1111 1111 1111 1111 1111 1011 1100 (-68)
y2:	1111 1111 1111 1111 1111 1111 1111 1100 

x3:	1111 1111 1111 1111 1111 1111 1011 0001 (-64-16+1)
y3:	1111 1111 1111 1111 1111 1111 1111 1100 
add them by (2^n - 1)
then the highest(32 - n) bits are the same of them

 */
/*
 */
int divpwr2(int x, int n) {
    int temp = (1 << n ) +(~0);//QAQ 高位被进位掉了
    int mask = x >> 31;//牛逼啊
    //temp == 2^n-1
    x += temp & mask;
    x >>= n;
    return x;
}
/* 
 * evenBits - return word with all even-numbered bits set to 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 8
 *   Rating: 2
 * 
 * |
 * or with 01010101010101...01
 * 0x55 4个
 */
//1
int evenBits(void) {
    int temp = 0x55;
    temp |= temp << 8;
    temp |= temp << 16;
    return temp;
}
/* 
 * fitsBits - return 1 if x can be represented as an 
 *  n-bit, two's complement integer.
 *   1 <= n <= 32
 *   Examples: fitsBits(5,3) = 0, fitsBits(-4,3) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 2
 *   2^0 + ... + 2^(n-2) = 2^(n-1) - 1
 *   [-2^(n-1),2^(n-1) - 1]
 *   >>n:
 *   正数: 000...0000
 *   
 *   负数: 111...1111
 *   负数：110101
 *   
 */
//2
int fitsBits(int x, int n) {
   
}
/* 
 * getByte - Extract byte n from word x
 *   Bytes numbered from 0 (LSB) to 3 (MSB)
 *   Examples: getByte(0x12345678,1) = 0x56
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
//3
int getByte(int x, int n) {
}
/* 
 * isGreater - if x > y  then return 1, else return 0 
 *   Example: isGreater(4,5) = 0, isGreater(5,4) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 24
 *   Rating: 3
 */
//4
int isGreater(int x, int y) {
}
/* 
 * isNonNegative - return 1 if x >= 0, return 0 otherwise 
 *   Example: isNonNegative(-1) = 0.  isNonNegative(0) = 1.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 3
 */
//5
int isNonNegative(int x) {
}
/* 
 * isNotEqual - return 0 if x == y, and 1 otherwise 
 *   Examples: isNotEqual(5,5) = 0, isNotEqual(4,5) = 1
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 2
 */
//6
int isNotEqual(int x, int y) {
}
/*
 * isPower2 - returns 1 if x is a power of 2, and 0 otherwise
 *   Examples: isPower2(5) = 0, isPower2(8) = 1, isPower2(0) = 0
 *   Note that no negative number is a power of 2.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 60
 *   Rating: 4
 */
//7
int isPower2(int x) {
	
	
}
/* 
 * leastBitPos - return a mask that marks the position of the
 *               least significant 1 bit. If x == 0, return 0
 *   Example: leastBitPos(96) = 0x20
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 6
 *   Rating: 4 
 */
//8
int leastBitPos(int x) {

}
/* 
 * logicalShift - shift x to the right by n, using a logical shift
 *   Can assume that 1 <= n <= 31
 *   Examples: logicalShift(0x87654321,4) = 0x08765432
 *   Legal ops: ~ & ^ | + << >>
 *   Max ops: 16
 *   Rating: 3 
 */
//9
int logicalShift(int x, int n) {
}
/*
 * satAdd - adds two numbers but when positive overflow occurs, returns
 *          maximum possible value, and when negative overflow occurs,
 *          it returns minimum positive value.
 *   Examples: satAdd(0x40000000,0x40000000) = 0x7fffffff
 *             satAdd(0x80000000,0xffffffff) = 0x80000000
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 30
 *   Rating: 4
 */
//10
int satAdd(int x, int y) {
	
}
/* 
 * tc2sm - Convert from two's complement to sign-magnitude 
 *   where the MSB is the sign bit
 *   You can assume that x > TMin
 *   Example: tc2sm(-5) = 0x80000005.
 *   Legal ops: ! ~ & ^ | + << >>
 *   Max ops: 15
 *   Rating: 4
 */
//11
int tc2sm(int x) {

	
}
