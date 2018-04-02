//#include <linux/init.h>
//#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
//#include <linux/time64.h>
//#include <linux/delay.h>
//#include <linux/interrupt.h>
//#include <linux/slab.h>
//#include <linux/ktime.h>
//#include <linux/hrtimer.h>
//#include <linux/kobject.h>


//#include "dht22.h"
//#include "dht22_sm.h"
#include "dht22_hck.h"

// Manipulate by gpio_desc flags directly to allow quick switching from GPIO IN +IRQ mode to GPIO OUT
#include <../drivers/gpio/gpiolib.h>

void dht_clear_irq_flag(int gpio)
{
	struct gpio_desc *desc;
	desc = gpio_to_desc(gpio);
    clear_bit(FLAG_USED_AS_IRQ, &desc->flags);
}
void dht_set_irq_flag(int gpio)
{
	struct gpio_desc *desc;
	desc = gpio_to_desc(gpio);
    set_bit(FLAG_USED_AS_IRQ, &desc->flags);
}
