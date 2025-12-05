### 平台信息
> Macbook Air M2 2022 16GB

> Sequoia 15.6

```shell
$ uname -m
arm64
$ uname -a
Darwin MacBook-Air.local 24.6.0 Darwin Kernel Version 24.6.0: Mon Jul 14 11:30:51 PDT 2025; root:xnu-11417.140.69~1/RELEASE_ARM64_T8112 arm64
$ cc --version
Apple clang version 16.0.0 (clang-1600.0.26.6)
Target: arm64-apple-darwin24.6.0
Thread model: posix
InstalledDir: /Library/Developer/CommandLineTools/usr/bin
```
### 出错现象
目前 macos_debug 分支上版本将stdin改为了文件输入，但出错现象一致
```shell
$ sh -c "./espresso.bin ./examples/examples/alu1"
zsh: trace trap  sh -c "./espresso.bin ./examples/examples/alu1"
```

### 已做过的debug尝试

* 修改1：更改了CMakeLists中的espresso目标名espresso.bin，由于目标名和espresso文件夹名字相同，macos上做ld时会报错，故更改了目标名
* 修改2：怀疑是stdin解析问题，所以更改为file文件输入数据的方式
* 修改3：增加了打印点，定位到是espresso.c中，make_sparse函数的问题
* 修改4：在CMakeLists中增加了-g 和 -fsanitize=address的lldb debug参数
 > 以上修改均不改变出错现象

### 目前看到的现象
1. 在make_sparse函数中增加了大量打印
```C
pcover make_sparse(pcover F, pcover D, pcover R) {
    cost_t cost, best_cost;

    cover_cost(F, &best_cost);

    do {
        F = mv_reduce(F, D);
        if (cost.total == best_cost.total) {
            printf("break1 \n");
            break;
        }
        copy_cost(&cost, &best_cost);

        F = expand(F, R, TRUE);
        if (cost.total == best_cost.total) {
            printf("break2 \n");
            break;
        }
        copy_cost(&cost, &best_cost);
    } while (TRUE);

    return F;
}
```
```C
pcover make_sparse(pcover F, pcover D, pcover R) {
    cost_t cost, best_cost;

    cover_cost(F, &best_cost);

    do {
        printf("break1 \n");
        printf("break1 \n");
        printf("break1 \n");
        printf("break1 \n");
        printf("break1 \n");
        F = mv_reduce(F, D);
        if (cost.total == best_cost.total) {
            printf("break1 \n");
            break;
        }
        copy_cost(&cost, &best_cost);

        F = expand(F, R, TRUE);
        if (cost.total == best_cost.total) {
            printf("break2 \n");
            break;
        }
        copy_cost(&cost, &best_cost);
    } while (TRUE);

    return F;
}
```
执行后可以通过

2. 通过lldb进行debug

```shell
$ lldb ./espresso.bin 
(lldb) target create "./espresso.bin"
Current executable set to '/Users/wenjunnan/espresso/espresso.bin' (arm64).
(lldb) run ./examples/examples/alu2
Process 35996 launched: '/Users/wenjunnan/espresso/espresso.bin' (arm64)
ssssssss
run espresso......
sss 105827994176016 
inner espresso 
inner espresso2 
inner espresso3 
inner espresso4 
inner espresso5 
inner espresso6 
Process 35996 stopped
* thread #1, queue = 'com.apple.main-thread', stop reason = EXC_BREAKPOINT (code=1, subcode=0x100037df8)
    frame #0: 0x0000000100037df8 espresso.bin`make_sparse(F=0x0000604000000d10, D=0x0000604000000a10, R=<unavailable>) at sparse.c:0:13 [opt]
   11  	
   12  	#include "espresso.h"
   13  	
-> 14  	pcover make_sparse(pcover F, pcover D, pcover R) {
   15  	    cost_t cost, best_cost;
   16  	
   17  	    cover_cost(F, &best_cost);
Note: this address is compiler-generated code in function make_sparse that has no source code associated with it.
Target 0: (espresso.bin) stopped.
warning: espresso.bin was compiled with optimization - stepping may behave oddly; variables may not be available.
```
至此，未找到出错原因