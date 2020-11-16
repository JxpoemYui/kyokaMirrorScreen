#include <rtthread.h>
#include <stdlib.h>  
#include "../libraries/ls1c_public.h"
#include "../libraries/ls1c_gpio.h"
#include "../libraries/ls1c_delay.h"


#define led_gpio 52
#define key_gpio 85
#define GPIO_CFG1	0xbfd010c4
#define GPIO_OUT1	0xbfd010f4

#define	LCD_CS_SET reg_write_32(reg_read_32((volatile unsigned int *)GPIO_CFG1)|0x4000, (volatile unsigned int *)GPIO_OUT1)

void test_gpio_output(void)
{
 int i;
 
 gpio_init(led_gpio, gpio_mode_output);
 // 输出 10 个矩形波，如果 gpio52 上有 led，则可以看见 led 闪烁 10 次
 for (i=0; i<10; i++)
 {
 gpio_set(led_gpio, gpio_level_low);
 delay_s(1);
 gpio_set(led_gpio, gpio_level_high);
 delay_s(1);
 }

 return ;
}
#include <finsh.h>
MSH_CMD_EXPORT(test_gpio_output, gpio output  sample);