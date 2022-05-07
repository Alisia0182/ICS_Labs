 # Part A
 write some simple Y86-64 programs 
目录:
sim/misc
1. sum.ys
   
2. rsum.ys
## tips
1. save and restore callee-save registers that you use
   
## How to test
```
> ./yas sum.ys # assembly
> ./yis sum.yo # run 
```

 # Part B
目录： 
sim/seq

modify seq-full.hcl

**iaddq**

extend the SEQ simulator with 2 new instructions

```
# Stage			iaddq V,rB
# Fetch			icode:ifun 	<== M1[PC]
# 				rA:rB	<== M1[PC+1]
# 				valC	<== M8[PC+2]
# 				valP	<== PC+10
# Decode
#				valB	<== R[rB]
# Execute
#				valE	<== valB + valC
#				Set CC
# Memory
# Write back
# 				R[rB]	<== valE
# PC update
#				PC		<== valP
```



 - [ ] A description of the computations required for the iaddq instruction.
  irmovq and OPq in Figure 4.18 in the CS:APP3e text as a guide.
## How to test
 ```
 > make VERSION=full   
 > ./ssim -t ../y86-code/asumi.yo
 > ./ssim -g ../y86-code/asumi.yo
 ```
 # Part C
 sim/pipe
 **loop unrolling sec5.8**
 1. iaddq
 2. section 5.8
  
```
../misc/yas ncopy.ys 生成ncopy.yo

./check-len.pl < ncopy.yo 检测长度

make drivers 生成ncopy.ys的测试程序

make psim VERSION=full 生成新的psim

./psim -t sdriver.yo 测试small 4-element array

./psim -t ldriver.yo测试larger 63-element array

./correctness.pl测试不同Block length下code range是否符合

./benchmark.pl 评分
————————————————
版权声明：本文为CSDN博主「热爱学习的贾克斯」的原创文章，遵循CC 4.0 BY-SA版权协议，转载请附上原文出处链接及本声明。
原文链接：https://blog.csdn.net/qq_42234461/article/details/108720264
```

# 问题
1. y86-64 tools？
2. ISA simulations?

## tools










# 注意

因为助教的失误，在打包lab的时候没有给脚本可执行权限，所以麻烦大家在 make 之前查阅一下文档的第十节-Trouble Shooting 中的相关信息，给脚本添加可执行权限。

chmod +x ./pipe/*.pl ./pipe/pipe.tcl ./ptest/*.pl

之后，make才会成功。

# Knowledges?
1. callee-save registers?
   

   # Name: 万金易
# ID:   519021911049
#/* $begin ncopy-ys */
##################################################################
# ncopy.ys - Copy a src block of len words to dst.
# Return the number of positive words (>0) contained in src.
# Name: 	万金易
# ID:   	519021911049
# Describe how and why you modified the baseline code.
# 1. add iaddq
# 2. we will set CC at iaddq, so if put iaddq $-1,%rdx before jp loop
#    there will be no need to calculate andq %rdx,%rdx.
##################################################################
# Do not modify this portion
# Function prologue.
# %rdi = src, %rsi = dst, %rdx = len
ncopy:

##################################################################
# You can modify this portion
	# Loop header
	# xorq %rax,%rax		# count = 0;
	andq %rdx,%rdx		# len <= 0?
	jle Done		# if so, goto Done:

Loop:	
	mrmovq (%rdi), %r10	# read val from src...
	rmmovq %r10, (%rsi)	# ...and store it to dst
	andq %r10, %r10		# val <= 0?
	jle Npos		# if so, goto Npos:
	iaddq $1,%rax		# count++
Npos:	
	iaddq $8, %rdi		# src++
	iaddq $8,%rsi		# dst++
	iaddq $-1,%rdx		# len--
	jg Loop			# if so, goto Loop:
##################################################################
# Do not modify the following section of code
# Function epilogue.
Done:
	ret
##################################################################
# Keep the following label at the end of your function
End:
#/* $end ncopy-ys */
