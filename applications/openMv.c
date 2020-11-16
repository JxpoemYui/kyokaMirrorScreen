#include <rtthread.h>
#include <stdlib.h>  
#include <drivers/pin.h>
#include <drivers/serial.h>
#include "../drivers/drv_uart.h"

#include <test.h>

#include "display_controller.h"
#include "../../libraries/ls1c_pwm.h"
#include "../../libraries/ls1c_public.h"
#include "../../libraries/ls1c_gpio.h"
#include "../../libraries/ls1c_pin.h"

static void pwminit_user(int light)
{
    pwm_info_t pwm_info;
    pwm_info.gpio = LS1C_PWM0_GPIO06;           // pwm引脚位gpio06
    pwm_info.mode = PWM_MODE_NORMAL;            // 正常模式--连续输出pwm波形
    if (light == 1)
    {
        pwm_info.duty = 0.85;                       // pwm占空比 85%
    }else
    {
        pwm_info.duty = 0.00;                       // pwm占空比 0%
    }
    
    
    pwm_info.period_ns = 5*1000*1000;            // pwm周期5ms

    /*pwm初始化，初始化后立即产生pwm波形*/
    pwm_init(&pwm_info);

    /* 使能pwm */
    pwm_enable(&pwm_info);
}


/* UART接收消息结构*/
struct openmv_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/* 用于接收消息的消息队列*/
static struct rt_messagequeue openmv_mq;
static struct rt_messagequeue screen;
/* 接收线程的接收缓冲区*/
static char openmv_buffer[64];
static char screen_pool[64];

/* 数据到达回调函数*/
static rt_err_t openmv_uart_input(rt_device_t dev, rt_size_t size)
{
    struct openmv_msg msg;
    msg.dev = dev;
    msg.size = size;

    /* 发送消息到消息队列中*/
    rt_mq_send(&openmv_mq, &msg, sizeof(struct openmv_msg));

    return RT_EOK;
}



static void openmv_thread_entry(void* parameter)
{
    struct openmv_msg msg;
    rt_device_t device, write_device,esp32_d;
    rt_err_t result = RT_EOK;

	result = rt_mq_init(&openmv_mq, "rx_mq", &msg, 1, sizeof(struct openmv_msg)*20, RT_IPC_FLAG_FIFO);
	if (result != RT_EOK) 
	{ 
	  rt_kprintf("init message queue failed.\n"); 
	}
	
    /* 查找系统中的串口1设备 */
    device = rt_device_find("uart3");
	write_device = device;
    esp32_d = rt_device_find("uart9");
    if (device!= RT_NULL)
    {
        /* 设置回调函数及打开设备*/
        rt_device_set_rx_indicate(device, openmv_uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
    }
    if (esp32_d != RT_NULL){
        rt_device_open(esp32_d, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
    }

    // rt_kprintf("pwmset:0.85-openmv_thread_entry-01\n");
    // pwminit_user(1);
    while (1)
    {
        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&openmv_mq, &msg, sizeof(struct openmv_msg), 15000);
        if (result == -RT_ETIMEOUT)
        {
            /* 接收超时*/
            rt_kprintf("\r\ntimeout count:openmv\n");
            rt_kprintf("pwmset:0.00-outTime\n");
            pwminit_user(0);

        }

        /* 成功收到消息*/
        if (result == RT_EOK)
        {
            rt_uint32_t rx_length;
            rx_length = (sizeof(openmv_buffer) - 1) > msg.size ?
                    msg.size : sizeof(openmv_buffer) - 1;

            /* 读取消息*/
            rx_length = rt_device_read(msg.dev, 0, &openmv_buffer[0],
                    rx_length);
            openmv_buffer[rx_length] = '\0';

            //rt_kprintf("\r\n uart1 rev %d bytes: %s \r\n", rx_length,  &openmv_buffer[0]);

            for (int i = 0; i < rx_length; i++){
                /*
                resuc    	第n位人脸注册成功
	            getpeo          		检测到有人脸出现在镜头前
	            >>>num*/

                if (openmv_buffer[i] == 'r'&&openmv_buffer[i+1] == 'e'&&openmv_buffer[i+2] == 's'&&openmv_buffer[i+3] == 'u'&&openmv_buffer[i+4] == 'c')
                {
                    rt_kprintf("openmv_device\n");
                }
                if (openmv_buffer[i] == 'g'&&openmv_buffer[i+1] == 'e'&&openmv_buffer[i+2] == 't')
                {
                    
                    rt_kprintf("get_openmv_device%d\n", face_flag);
                    rt_kprintf("face_flag");
                    face_flag = 1;
                    rt_gui_demo_init();
                    rt_thread_delay(100);
                    rt_kprintf("pwmset:0.85-openmv_face-01\n");
                    pwminit_user(1);
                }
                if (openmv_buffer[i] == '>'&&openmv_buffer[i+1] == '>'&&openmv_buffer[i+2] == '>'){
                    char nums = '0';
                    nums = openmv_buffer[i+3];
                    if(nums=='1'){
                        user_num_flag = 0;
                    }
                    if(nums=='2'){
                        user_num_flag = 1;
                    }
                    if(nums=='3'){
                        user_num_flag = 2;
                    }
                    rt_mq_send(&screen, &nums, sizeof(nums));
                    rt_gui_demo_init();
                    rt_kprintf("pwmset:0.85-openmv_face-02\n");
                    pwminit_user(1);
                    //rt_kprintf("%c", nums);
                }
            }

        }
    }
}

static void screen_thread_entry(void* parameter){
    char buf = 0;
    rt_uint8_t cnt = 0;

    while (1)
    {
        /* 从消息队列中接收消息 */
        if (rt_mq_recv(&screen, &buf, sizeof(buf), RT_WAITING_FOREVER) == RT_EOK)
        {
            rt_kprintf("thread1: recv msg from msg queue, the content:%c\n", buf);
        }
        /* 延时 50ms */
        rt_thread_mdelay(50);
    }
    rt_kprintf("thread1: detach screen mq \n");
    rt_mq_detach(&screen);
}

void test_rtt_openmv(void)
{   
    rt_err_t result = RT_EOK;
    result = rt_mq_init(&screen, "screen_mq", &screen_pool[0], 1, sizeof(screen_pool), RT_IPC_FLAG_FIFO);
    if (result != RT_EOK) 
	{ 
	  rt_kprintf("init screen message queue failed.\n"); 
	}

    /* 创建devt线程*/
    rt_thread_t thread9 = rt_thread_create("openmv_devt",
        openmv_thread_entry, RT_NULL,
        4096, 24, 7);

    /* 创建成功则启动线程*/
    if (thread9 != RT_NULL)
        rt_thread_startup(thread9);
    else
        rt_kprintf("error openmv");
    rt_thread_t thread10 = rt_thread_create("screen_devt",
        screen_thread_entry, RT_NULL,
        4096, 24, 7);
    /* 创建成功则启动线程*/
    if (thread10 != RT_NULL)
        rt_thread_startup(thread10);
    else
        rt_kprintf("error screen");

}
MSH_CMD_EXPORT(test_rtt_openmv, test_rtt_openmv);