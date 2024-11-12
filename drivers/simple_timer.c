#include <linux/module.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/gpio.h>
#include <linux/jiffies.h>
#include <linux/timer.h>

MODULE_LICENSE("GPL");

static struct timer_list my_timer;
//pin 23 P9 -> gpio49

void timer_callback(struct timer_list *data){
	gpio_set_value(49,0);
}

static int __init ledinit(void){
	printk("inside init\n");
	if(gpio_request(49, "gpio-49")){
		printk("Can not allocate GPIO 23 P9\n");
		return -1;
	}
	
	if(gpio_direction_output(49,0)){
		printk("Can not set direction GPIO 23 P9\n");
                return -1;
	}

	gpio_set_value(49,1);

	//Initialize
	timer_setup(&my_timer, timer_callback, 0);
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));

	return 0;
}

static void __exit ledexit(void){
	printk("inside exit\n");
	gpio_free(19);

}

module_init(ledinit);
module_exit(ledexit);
