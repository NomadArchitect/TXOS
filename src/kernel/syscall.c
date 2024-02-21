#include <xos/syscall.h>
#include <xos/interrupt.h>
#include <xos/task.h>
#include <xos/memory.h>
#include <xos/assert.h>
#include <xos/debug.h>
#include <xos/console.h>
#include <xos/device.h>
#include <xos/string.h>
#include <xos/buffer.h>

extern void sys_exit(i32 status);
extern void sys_fork();
extern pid_t sys_waitpid(pid_t pid, i32 *status);
extern time_t sys_time();
extern pid_t sys_getpid();
extern i32 sys_brk(void *addr);
extern pid_t sys_getppid();
extern void sys_yield();
extern void sys_sleep(u32 ms);

// 系统调用处理函数列表
handler_t syscall_table[SYSCALL_SIZE];

// 检测系统调用号是否合法
void syscall_check(u32 sys_num) {
    if (sys_num >= SYSCALL_SIZE) {
        panic("invalid syscall number!!!");
    }
}

// 默认系统调用处理函数
static void sys_default() {
    panic("syscall is not implemented!!!");
}

// 系统调用 test 的处理函数
static u32 sys_test() {
    // char ch;
    dev_t *device;

    // device = dev_find(DEV_KEYBOARD, 0);
    // assert(device);
    // dev_read(device->dev_id, &ch, 1, 0, 0);

    // device = dev_find(DEV_CONSOLE, 0);
    // assert(device);
    // dev_write(device->dev_id, &ch, 1, 0, DEBUG);

    device = dev_find(DEV_ATA_DISK, 0);
    assert(device);
    // 读取主磁盘第 0 个块
    buffer_t *bf = bread(device->dev_id, 0);
    // 设置第 1 个扇区为 0x5a
    memset((bf->data + SECTOR_SIZE), 0x5a, SECTOR_SIZE);
    // 写回磁盘并释放
    bf->dirty = true;
    bwrite(bf);
    brelse(bf);

    return 255;
}

// 系统调用 write 的处理函数
static i32 sys_write(fd_t fd, const void *buf, size_t len) {
    if (fd == STDOUT || fd == STDERR) {
        return console_write(NULL, (char *)buf, len, 0, TEXT);
    }
    // TODO:
    panic("unimplement write!!!");
    return 0;
}

// 初始化系统调用
void syscall_init() {
    for (size_t i = 0; i < SYSCALL_SIZE; i++) {
        syscall_table[i] = sys_default;
    }

    syscall_table[SYS_TEST]     = sys_test;
    syscall_table[SYS_SLEEP]    = sys_sleep;
    syscall_table[SYS_YIELD]    = sys_yield;
    syscall_table[SYS_WRITE]    = sys_write;
    syscall_table[SYS_BRK]      = sys_brk;
    syscall_table[SYS_GETPID]   = sys_getpid;
    syscall_table[SYS_GETPPID]  = sys_getppid;
    syscall_table[SYS_FORK]     = sys_fork;
    syscall_table[SYS_EXIT]     = sys_exit;
    syscall_table[SYS_WAITPID]  = sys_waitpid;
    syscall_table[SYS_TIME]     = sys_time;
}
