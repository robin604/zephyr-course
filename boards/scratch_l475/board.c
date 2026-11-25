#include <zephyr/init.h>
#include <zephyr/sys/printk.h>

/* Runs once at the APPLICATION init level: after the kernel and the
 * console are up and the boot banner has printed, but before main(). */
static int board_scratch_l475_init(void)
{
	printk("Board Initialized\n");
	return 0;
}

SYS_INIT(board_scratch_l475_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);