# DoraOS

DoraOS 是我个人所写的RTOS内核，结合FreeRTOS、uCOS, RT-Thread, LiteOS 的特性所写，取其精华，去其糟粕，本项目将持续维护，欢迎大家fork与star。

名字的由来：Dora 谐音为哆啦，我个人非常喜欢哆啦A梦，梦想有一天能将哆啦A梦2577集看完（可惜程序员太忙没时间）.... OS则为操作系统 Operating System。

目前已实现M3内核的任务切换与调度。理论上支持无限多个优先级，采用bit map方式寻找最高优先级任务，支持时间片调度。

内存管理算法采用“最佳匹配”算法，每个内存块节点信息结构只需 8 Byte，在内存堆首部存在内存管理信息控制块，大小为 16 Byte。采用内存切割与合并管理，最大化降低内存碎片的产生。
实现icp通信机制...
```
文件目录如下：
DoraOS/
├── common
│   ├── lwip
│   │   ├── lwip相关文件
│   │   └── lwip相关文件
│   └── salof
│       ├── fifo.c
│       ├── format.c
│       └── salof.c
├── demo
│   ├── stm32f103
│   │   ├── Doc
│   │   │   └── readme.txt
│   │   └── Project
│   │       └── stm32f103.uvprojx
│   └── stm32f429
│       ├── Doc
│       │   └── readme.txt
│       └── Project
│           └── stm32f429.uvprojx
├── example
│   ├── app_event.c
│   ├── app_mem.c
│   ├── app_memp.c
│   ├── app_mutex.c
│   ├── app_queue.c
│   ├── app_sem.c
│   └── app_swtmr.c
├── include
│   ├── common
│   │   └── salof
│   │       ├── fifo.h
│   │       ├── format.h
│   │       └── salof.h
│   ├── config
│   │   ├── debug.h
│   │   ├── dos_config.h
│   │   └── dos_def.h
│   ├── event.h
│   ├── list.h
│   ├── log.h
│   ├── mem
│   │   ├── mem.h
│   │   └── memp.h
│   ├── mutex.h
│   ├── port
│   │   ├── arm
│   │   │   ├── cortex-m3
│   │   │   │   ├── arm_cmsis.h
│   │   │   │   └── port.h
│   │   │   └── cortex-m4
│   │   │       └── port.h
│   │   └── cmsis
│   │       └── arm_cmsis.h
│   ├── queue.h
│   ├── sem.h
│   ├── swtmr.h
│   ├── sys.h
│   └── task.h
├── LICENSE
├── mem
│   ├── mem.c
│   └── memp.c
├── port
│   └── arm
│       ├── cortex-m3
│       │   ├── port.c
│       │   └── port.s
│       └── cortex-m4
│           ├── port.c
│           └── port.s
├── README.md
└── src
    ├── event.c
    ├── list.c
    ├── log.c
    ├── mutex.c
    ├── queue.c
    ├── sem.c
    ├── swtmr.c
    ├── sys.c
    └── task.c
```