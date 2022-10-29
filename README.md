# HIT-OSLAB
@[toc]
# 前言
此文为哈工大李治军老师所教授操作系统课程的配套实验中的实验一 —— 操作系统的引导 的记录。

在学完李志军老师所教授课程后，决定通过实验加深对OS的进一步理解。

所需要说明的是，linux-0.11是linux最早的版本。当前的linux和其相比，早已天差地别。

同时，硬件也发生了较大的变化。（0.11还考虑软盘 floppy disk）

因此，在现在的环境下学习linux0.11，其意义更多的在于扫除操作系统的神秘感。

那为什么不选择更现代的操作系统来进行实验呢？

因为操作系统是真正的复杂系统。太过于庞大，太过于复杂，容易在细节中迷失对操作系统整体的把握。
（太难了， 代码看不懂）

linux0.11相较于其他OS，已算小型，但代码量也不小。学起来也不算容易。

同时，其较好地实现了操作系统的几个视图（进程，内存，文件），可供初学者仔细研究。
（多看看进程调度函数）
# 实验环境
该实验可在[蓝桥云课对应实验课](https://www.lanqiao.cn/courses/115)中进行实验。

但是很麻烦，不保存之前结果，还吃网络，操作的很烦。

于是，我就在我的虚拟机上配置了相关环境。

环境配置可参考[该博文](https://gitcode.net/mirrors/Wangzhike/HIT-Linux-0.11)。
（记得关注自己的实验文件夹，我曾以为自己没出最后的oslab，还自己一个一个解压，配置环境变量，最后发现别人的脚本早做好了）

另外，建议做一个共享文件夹，用git去管理源码。能省不少麻烦。

软件环境：Bochs + Ubuntu14.04 + gcc3.4 + vim

模拟硬件环境：IA-32 X86架构

汇编语言：as86语法

## 实验内容
## 说明：
原实验内容如下：

此次实验的基本内容是：

阅读《Linux内核完全注释》的第6章，对计算机和Linux 0.11的引导过程进行初步的了解；
按照下面的要求改写0.11的引导程序bootsect.s
有兴趣同学可以做做进入保护模式前的设置程序setup.s。
改写bootsect.s主要完成如下功能：

bootsect.s能在屏幕上打印一段提示信息“XXX is booting...”，其中XXX是你给自己的操作系统起的名字，例如LZJos、Sunix等（可以上论坛上秀秀谁的OS名字最帅，也可以显示一个特色logo，以表示自己操作系统的与众不同。）
改写setup.s主要完成如下功能：

bootsect.s能完成setup.s的载入，并跳转到setup.s开始地址执行。而setup.s向屏幕输出一行"Now we are in SETUP"。
setup.s能获取至少一个基本的硬件参数（如内存参数、显卡参数、硬盘参数等），将其存放在内存的特定地址，并输出到屏幕上。
setup.s不再加载Linux内核，保持上述信息显示在屏幕上即可。

如果直接按照它的来，则每次不再编译全部的OS，只是编译一小部分。
这样的话，就不能直接用它给的命令快速完成，而且无法看到这么修改是否会对OS整体产生影响。（输出能产生什么影响呢）
又因为我懒（不想写硬件读写，太折磨了），所以我直接在linux-0.11源代码的基础上做出了要求实现的功能。

## 需要知道的基础知识
由于当前仍然处于操作系统的引导时期，因此并不存在操作系统所提供给你的系统调用。

可以用的，只有BIOS的中断。

在实验中会用到的是int 0x10 

具体的解释可见[该博文](https://blog.csdn.net/S1998F/article/details/125596206)

另外就是基础的汇编知识
在此仅仅对几个在接下来的实验中容易出错的进行说明。
bp默认寻址es，bx默认寻址ds。
si默认寻址ds，di默认寻址es
sp寻址ss 压栈 sp = sp - 2 出栈sp = sp + 2

call 会将当前的cs,ip进行压栈
ret 会从栈中弹出ip,cs。

rep movsb 串传送指令（按照字节传输cx次 依据df决定传输方向，cld指令将df置为0 , 正向传输 传输完一个后  si,di自增, std将DF置为1，逆向传输，传输完一字节后，si，di自减）。
movsw 按照字（此时是2byte）其余类似


lds reg, mem 将内存处的字 赋值给 对应寄存器
lds si, [4*0x41] 即将 4*0x41 = 0x104地址处的 字 赋值给 si

以及
盘片（platter）
磁头（head）
磁道（track）
扇区（sector）
柱面（cylinder）

## 修改/boot/bootsect.s 使其能在屏幕上输出提示信息(xxx is booting)
很简单，直接修改源码中的输出字符串（msg1） 和 对应的长度(cx)即可。
```
! Print some inane message
! read the cursor position
	mov	ah,#0x03		! read cursor pos
	xor	bh,bh
	int	0x10
	
	mov	cx,#46 ! length of string for output # 
	mov	bx,#0x0007		! page 0, attribute 7 (normal)
	mov	bp,#msg1
	mov	ax,#0x1301		! write string, move cursor
	int	0x10

msg1:
	.byte 13,10
	.ascii "Lucifer system Created by rebelOverWaist"
	.byte 13,10,13,10
```

之后用提供的make clean && make all 命令进行编译
再用./run 脚本运行

## 修改/boot/setup.s 使其能输出(now we are in SETUP)

这个也没什么难的。

只不过是自己写了bootsect中的字符串输出代码罢了。

（注意，此时需要将es置为9200 因为要寻址字符串 否则结果不对）
```
	 mov ax, #SETUPSEG
    mov es, ax
    
    mov ah, #0x03
    xor bh, bh
    int 0x10

    mov cx, #12
    mov bx, #0x0007
    mov bp, #msg
    mov ax, #0x1301
    int 0x10


msg:
    .byte 13, 10
    .ascii "setup..."
    .byte 13, 10, 13, 10
```

然后再编译运行即可。
就可看到字符显示了。
## 将硬件参数(内存大小、硬盘参数)输出到屏幕上
分为两步，第一步读取硬件参数，第二步，拿到硬件参数，通过一定格式显示。
第一步我没写，直接用linux源码去解决了。
第二步需要将数据按16进制进行输出，所以需要写以下两个函数，将一个16位数按16进制输出，输出回车换行。实验指导给了如下实例代码。
（也不一定非要用给的代码 实现这两个函数的功能的方法有很多）
（在此提供一下思路	对于16进制的输出，可以先打表 "0123..f"然后利用si这种去偏移寻址 就不用去看是数字还是字符了	至于回车换行，要是懒的话 可以直接在字符串后面加一个CRLF（13，10））
如果按照实验指导给的代码直接写：
```
    !以16进制方式打印栈顶的16位数
    print_hex:
        mov    cx,#4         ! 4个十六进制数字
        mov    dx,(bp)     ! 将(bp)所指的值放入dx中，如果bp是指向栈顶的话
    print_digit:
        rol    dx,#4        ! 循环以使低4比特用上 !! 取dx的高4比特移到低4比特处。
        mov    ax,#0xe0f     ! ah = 请求的功能值，al = 半字节(4个比特)掩码。
        and    al,dl         ! 取dl的低4比特值。
        add    al,#0x30     ! 给al数字加上十六进制0x30
        cmp    al,#0x3a
        jl    outp          !是一个不大于十的数字
        add    al,#0x07      !是a～f，要多加7
    outp: 
        int    0x10
        loop    print_digit
        ret
```

```
    !打印回车换行
    print_nl:
        mov    ax,#0xe0d     ! CR
        int    0x10
        mov    al,#0xa     ! LF
        int    0x10
        ret
```

会被栈搞得头晕。
因为call 和 ret 存在入栈和出栈的存在， 会对SS和SP进行修改。
如果按照默认的，可能会不小心修改了什么代码，什么数据。
（这个时候还在实地址模式，只有BIOS这种才是只读的）
这种时候会出现各类奇奇怪怪的BUG，只有调试了。

设置好ss和sp :-)
还有es :-) 
注意0x10 和 10 别掉0x了 :-)
记得跳过两个函数:-)


由于我没做第一步，因此花了一点时间阅读了一下给的源代码。
源代码实现的功能是把硬件各个参数读到的9000:0 - 9000:200处。
最开始的0是光标
2是内存大小
4是display page
6 的低字节是video mode, 高字节为 window width
8
10
12放的是EGA/VGA（不懂）
0080-008f存放的是硬盘0的相关参数
0090-009f存放的是硬件1的相关参数（如果有的话）
格式为：

| 位移 | 大小 | 说明         |
| ---- | ---- | ------------ |
| 0x00 | word | 柱面数       |
| ...  | ...  | ...          |
| 0x02 | byte | 磁头数       |
| 0x0E | byte | 每磁道扇区数 |
| 0x0F | byte | 保留         |

这个位移更精确的叫法应该是偏移，是距离0080,0090的偏移

注意了上述细节，明确了内存图像。
就可以很轻松地写出下列代码了
```
! start print hardware
    mov ax, #INITSEG
	! in case ss:sp will alter code
    mov ss, ax
    mov sp, #0xff00
    !ss:sp =  9ff00 
	! es:bp 
    mov ax, #SETUPSEG 
    mov es, ax

Print_Cursor:
    mov ah, #0x03
    xor bh, bh
    int 0x10
! addition
    mov cx, #11
    mov bx, #0x0007
    mov bp, #Cursor
    mov ax, #0x1301
    int 0x10
! set bp = 0x0000 
    mov ax, #0
    mov bp, ax
    mov dx, (bp)
    call print_hex
    call print_ln
! end print Cursor
Print_Memory:
    mov ah, #0x03
    xor bh, bh
    int 0x10

    mov cx, #12
    mov bx, #0x0007
    mov bp, #Memory
    mov ax, #0x1301
    int 0x10

    mov ax, #2
    mov bp, ax
    mov dx, (bp)
    call print_hex
    !show KB
    mov ah, #0x03
    xor bh, bh
    int 0x10

    mov cx, #2
    mov bx, #0x0007
    mov bp, #KB
    mov ax, #0x1301
    int 0x10
    call print_ln

Print_Cyl_hd0:
    mov ah, #0x03
    xor bh, bh
    int 0x10

    mov cx, #9
    mov bx, #0x0007
    mov bp, #Cyl_hd0
    mov ax, #0x1301
    int 0x10

    mov ax, #0x0080
    mov bp, ax

    mov dx, (bp)
    call print_hex
    call print_ln
   
Print_Head_hd0:
    mov ah, #0x03
    xor bh, bh
    int 0x10

    mov cx, #9
    mov bx, #0x0007
    mov bp, #Head_hd0
    mov ax, #0x1301
    int 0x10
    
    !show Head_hd0
    mov ax, #0x0082
    mov bp, ax
    mov dx, (bp)
    and dx, #0x00ff

    call print_hex
    call print_ln
Print_Sector_hd0:
    mov ah, #0x03
    xor bh, bh
    int 0x10

    mov cx, #11
    mov bx, #0x0007
    mov bp, #Sector_hd0
    mov ax, #0x1301
    int 0x10

    mov ax, #0x008e
    mov bp, ax
    mov dx, (bp)
    and dx, #0x00ff

    call print_hex
    call print_ln

    jmp end_print
! function 
print_hex:
    mov cx, #4
    print_digital:
    rol dx, #4
    mov ax, #0x0e0f
    and al, dl
    add al, #0x30
    cmp al, #0x3a
    jl out_p
    add al, #0x07 
    
    out_p:
    int 0x10
    loop print_digital
    ret 

print_ln:
    mov ax, #0x0e0d
    int 0x10
    mov al, #0xA
    int 0x10 
    ret 

end_print:
! end print hardware data
```
```
Cursor:
    .ascii "Cursor Pos:"

Memory:
    .ascii "Memory Size:"

Cyl_hd0:
    .ascii "Cyls_hd0:"
Head_hd0:
    .ascii "Head_hd0:"
Sector_hd0:
    .ascii "Sector_hd0:"
KB:
    .ascii "KB"
```

整体很简单，找光标，输出提示字符串，输出对应参数，输出换行
再按这种方式输出所有要的进行了。
最后的最后，需要将这些参数和/bochs/bochsrc.bxrc中的

```
……
megs: 16
……
ata0-master: type=disk, mode=flat, cylinders=410, heads=16, spt=38
……
```
相关参数进行对比，都吻合了，就成功了。（这不是我的参数）
