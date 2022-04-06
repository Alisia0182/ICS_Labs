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
   