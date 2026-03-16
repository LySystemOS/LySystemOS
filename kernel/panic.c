#include <LySys/LySystem.h>

void panic(const char * s)
{
	printk("Kernel panic: %s\n\r",s);
	for(;;);
}
