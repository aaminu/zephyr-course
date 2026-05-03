#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

static int board_init_msg(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(board_init_msg, APPLICATION, 0);