< start log >
$ sudo insmod mymodule.ko
$ sudo lsmod | grep mymodule
$ sudo rmmod mymodule
$ sudo dmesg
< end log >
```

chekanov@chekanov-VirtualBox:~/work/gl-kernel-training-2019/04-KernelModule$ sudo insmod simp_mod.ko
chekanov@chekanov-VirtualBox:~/work/gl-kernel-training-2019/04-KernelModule$ lsmod | grep simp
simp_mod               16384  0
chekanov@chekanov-VirtualBox:~/work/gl-kernel-training-2019/04-KernelModule$ sudo rmmod simp_mod
chekanov@chekanov-VirtualBox:~/work/gl-kernel-training-2019/04-KernelModule$ lsmod | grep simp

[10987.180944] hello: Hello, world!
[10987.180944] hello: module loading
[10987.180945] hello: myparam = 0
[11073.271800] hello: module shutdown


