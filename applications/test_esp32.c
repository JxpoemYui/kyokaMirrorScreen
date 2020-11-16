#include <rtthread.h>
#include <stdlib.h>  
#include <string.h>
#include <drivers/pin.h>
#include <drivers/serial.h>
#include "../drivers/drv_uart.h"


#include "ls1c_public.h"
#include "ls1c_uart.h"
#include "ls1c_pin.h"
#include "test.h"

#include "jsmn.h"

#include "display_controller.h"
#include "../../libraries/ls1c_pwm.h"
#include "../../libraries/ls1c_public.h"
#include "../../libraries/ls1c_gpio.h"
#include "../../libraries/ls1c_pin.h"

struct app_date{
    unsigned char   wlanState[10];
    unsigned char   netState[10];
    unsigned char   ip[10];
    unsigned char   mac[10];
    unsigned char   lastSync[10];
    unsigned int    UserNum;
};

int user_num = 2;
static char json_mb_str[] = "over";

struct user_id_date {
    unsigned int id;
    unsigned char account[20];
    unsigned char psw[20];
};

/* UART接收消息结构*/
struct rx_msg
{
    rt_device_t dev;
    rt_size_t size;
};
/*struct app_msg
{
    rt_device_t dev;
    rt_size_t size;
};*/
/* 用于接收消息的消息队列*/
static struct rt_messagequeue rx_mq;
//static struct rt_messagequeue app_mq;
/* 接收线程的接收缓冲区*/
static char uart_rx_buffer[128];
static char app_buffer[128];

/* 邮箱控制块 */
static struct rt_mailbox json_mb;
/* 用于放邮件的内存池 */
static char json_mb_pool[256];

int json_length = 0;

static void json_state_thread_entry(void* parameter);

/* 数据到达回调函数*/
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    struct rx_msg msg;
    msg.dev = dev;
    msg.size = size;
    //struct app_msg appmsg;
    //appmsg.dev = dev;
    //msg.size = size;

    /* 发送消息到消息队列中*/
    rt_mq_send(&rx_mq, &msg, sizeof(struct rx_msg));
    //rt_mq_send(&app_mq, &appmsg, sizeof(struct app_msg));


    return RT_EOK;
}
static int fetch_key1(const char *JSON_STRING, jsmntok_t *t, const char *str)
{   
    int length1;
    length1 = strlen(str);
    if ((t->type == JSMN_STRING) && (length1 == (t->end - t->start)))
    {   
        return strncmp(JSON_STRING + t->start, str, length1);
    }
    else
    {
        return -1;
    }
}

static void pwminit2(void)
{
    pwm_info_t pwm_info;
    pwm_info.gpio = LS1C_PWM0_GPIO06;           // pwm引脚位gpio06
    pwm_info.mode = PWM_MODE_NORMAL;            // 正常模式--连续输出pwm波形
    pwm_info.duty = 0.05;                       // pwm占空比 85%
    pwm_info.period_ns = 5*1000*1000;            // pwm周期5ms

    /*pwm初始化，初始化后立即产生pwm波形*/
    pwm_init(&pwm_info);

    /* 使能pwm */
    pwm_enable(&pwm_info);
}

static void json_thread_entry(void* parameter)
{
    unsigned char *str;
    int index;
    int i;
    int res;
    jsmn_parser p;
    jsmntok_t t[128] = {0};

    jsmn_init(&p);

    while (1)
    {
        rt_kprintf("thread1: try to recv a mail\n");

        /* 从邮箱中收取邮件 */
        rt_kprintf("\n\n");
        if (rt_mb_recv(&json_mb, (rt_uint32_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {   
            if (str == json_mb_str)
                break;
            /*for (index = 0; index < json_length; index++)
            {
                rt_thread_delay(5);   
                rt_kprintf("%c", str[index]);
            }*/
            res = jsmn_parse(&p, str, strlen(str), t, sizeof(t) / sizeof(t[0]));
                if (res < 0)
                {
                    rt_kprintf("Failed to parse JSON: %d\n", res);
                    return -1;
                }

                if (res < 1 || t[0].type != JSMN_OBJECT)
                {
                    rt_kprintf("Object expected\n");
                    return -1;
                }

                for (i = 1; i < res; i++)
                {   
                    char *json_p;
                    int number = 0;
                    if (fetch_key1(str, &t[i], "id") == 0)
                    {
                        rt_kprintf("id: %.*s\n", t[i+1].end - t[i+1].start, str + t[i+1].start);
                        json_p = str + t[i+1].start;
                        for (number = 0; number < (t[i+1].end - t[i+1].start); number++){
                            user_date_t[0].id[number] = json_p[number];
                        }
                        /*for (number = 0; number < (t[i+1].end - t[i+1].start); number++){
                            rt_kprintf("%c", user_date_t[0].id[number]);
                            rt_thread_delay(1);
                        }*/
                        i++;
                    }
                    else if (fetch_key1(str, &t[i], "account") == 0)
                    {
                        rt_kprintf("account: %.*s\n", t[i+1].end - t[i+1].start, str + t[i+1].start);
                        json_p = str + t[i+1].start;
                        for (number = 0; number < (t[i+1].end - t[i+1].start); number++){
                            user_date_t[0].name[number] = json_p[number];
                        }
                        /*for (number = 0; number < (t[i+1].end - t[i+1].start); number++){
                            rt_kprintf("%c", user_date_t[0].name[number]);
                        }*/
                        i++;
                    }
                    else if (fetch_key1(str, &t[i], "psw") == 0)
                    {
                        rt_kprintf("psw: %.*s\n", t[i+1].end - t[i+1].start, str + t[i+1].start);
                        json_p = str + t[i+1].start;
                        for (number = 0; number < (t[i+1].end - t[i+1].start); number++){
                            user_date_t[0].psw[number] = json_p[number];
                        }
                        /*for (number = 0; number < (t[i+1].end - t[i+1].start); number++){
                            rt_kprintf("%c", user_date_t[0].psw[number]);
                        }*/
                        i++;
                    }
                    else
                    {
                        //rt_kprintf("un: %.*s\n", t[i].end - t[i].start, str + t[i].start);
                    }
                rt_kprintf("%d", *json_p);
                }
            user_num++;
            rt_kprintf("\n\n");
            
            rt_kprintf("thread1: get a mail from mailbox, the content\n");
            /* 延时 100ms */
            rt_thread_mdelay(100);
            rt_thread_t thread5 = rt_thread_create("json_state_entry",
            json_state_thread_entry, RT_NULL,
            1024, 25, 10);
            /* 创建成功则启动线程*/
            if (thread5 != RT_NULL){
                rt_thread_startup(thread5);
            }
                return 0;
            }
    }
    /* 执行邮箱对象脱离 */
    rt_mb_detach(&json_mb);
}

static void json_state_thread_entry(void* parameter){
    rt_thread_delay(10);
    rt_kprintf("state is success\n");
    rt_thread_t thread4 = rt_thread_create("json_entry",
        json_thread_entry, RT_NULL,
        4096, 25, 10);
    /* 创建成功则启动线程*/
    if (thread4 != RT_NULL){
        rt_thread_startup(thread4);
        }
    return;
}

static void esp32_thread_entry(void* parameter)
{
    struct rx_msg msg;
    //struct app_msg appmsg;
    rt_device_t device, write_device, openmv_d;
    rt_err_t result = RT_EOK;
    //rt_err_t result_1 = RT_EOK;



	result = rt_mq_init(&rx_mq, "rx_mq", &msg, sizeof(struct rx_msg), sizeof(struct rx_msg)*20, RT_IPC_FLAG_FIFO);
    //result_1 = rt_mq_init(&app_mq, "rx_mq", &appmsg, sizeof(struct app_msg), sizeof(struct app_msg)*20, RT_IPC_FLAG_FIFO);
	if (result != RT_EOK) 
	{ 
	    rt_kprintf("init message queue failed.\n"); 
	} 
    
    /* 查找系统中的串口9设备 */
    device = rt_device_find("uart9");
    openmv_d = rt_device_find("uart3");
	write_device = device;
    if (device != RT_NULL)
    {
        /* 设置回调函数及打开设备*/
        rt_device_set_rx_indicate(device, uart_input);
        rt_device_open(device, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
    }
    if (openmv_d != RT_NULL){
        rt_device_open(openmv_d, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
    }

    unsigned char id4[] = "1";
    unsigned char name4[] = "kyokatest001@outlook.com";
    //unsigned char psw4[] = "kyokapsw233";
    unsigned char id5[] = "2";
    unsigned char name5[] = "kyokatest002@outlook.com";
    //unsigned char psw5[] = "kyokapsw233";
    for(int m = 0; m < 2; m++){
        user_date_t[0].id[m] = id4[m];
        user_date_t[1].id[m] = id5[m];
    }
    for(int m = 0; m < 25; m++){
        user_date_t[0].name[m] = name4[m];
        user_date_t[1].name[m] = name5[m];
    }
    // for(int m = 0; m < 12; m++){
    //     user_date_t[0].psw[m] = psw4[m];
    //     user_date_t[1].psw[m] = psw5[m];
    // }
    
    int length = 0;
    char readstate = 0;
    user_quan_flag = 0;
    while (1)
    {   

        /* 从消息队列中读取消息*/
        result = rt_mq_recv(&rx_mq, &msg, sizeof(struct rx_msg), 1000);
        if (result == -RT_ETIMEOUT)
        {
            /* 接收超时*/
            /*for (int i = 0; i < length; i++){
                rt_thread_delay(5);
                rt_kprintf("%c", app_buffer[i]);
            }*/
            if(readstate == 1){
                json_length = length;
                rt_mb_send(&json_mb,(rt_uint32_t)&app_buffer);
                rt_mb_send(&json_mb, (rt_uint32_t)&json_mb_str);
            }
            rt_kprintf("\r\ntimeout count: esp32\n");
            length = 0;
            readstate = 0;
        }
        /* 成功收到消息*/
        
         if (result == RT_EOK)
        {
            rt_uint32_t rx_length;
            rx_length = (sizeof(uart_rx_buffer) - 1) > msg.size ?
                    msg.size : sizeof(uart_rx_buffer) - 1;

            /* 读取消息*/
            rx_length = rt_device_read(msg.dev, 0, &uart_rx_buffer[0],
                    rx_length);
            uart_rx_buffer[rx_length] = '\0';

            
            for (int i = 0; i < rx_length; i++){
                //rt_kprintf("%c", uart_rx_buffer[i]);
                //rt_kprintf("\n");
                rt_thread_delay(5);


                if (uart_rx_buffer[i] == 'S'&&uart_rx_buffer[i+1] == 't'&&uart_rx_buffer[i+2] == 'a'&&uart_rx_buffer[i+3] == 't'&&uart_rx_buffer[i+4] == 'e')
                {
                    
                    rt_kprintf("write_device\n");
                    if (write_device != RT_NULL){
                        rt_kprintf("is success\n");
                        rt_thread_delay(5);
                        rt_kprintf("%d", *write_device);
                        rt_device_t device, write_device;
                        device = rt_device_find("uart9");
	                        write_device = device;
                        if (device != RT_NULL)
                        {
                            /* 设置回调函数及打开设备*/
                            rt_device_open(device, RT_DEVICE_OFLAG_RDWR|RT_DEVICE_FLAG_INT_RX);
                            rt_thread_delay(5);
                        }
                        rt_device_write(write_device, 0, "{\"wlanState\":\"success\"," , 24);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "\"wlanErr\":\"\",", 14);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "\"netState\":\"true\",", 19);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "\"ip\":\"192.168.1.1\",", 20);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "\"mac\":\"FF-FF-FF-FF-FF-FF\",", 27);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "\"lastSync\":\"2020-10-01 13:48\",", 31);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "\"UserNum\":\"3", 13);
                        rt_thread_delay(5);
                        //rt_device_write(write_device, 0, num, 1);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "\"}", 3);
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0, "" , 1);
                    }
                }
                if (uart_rx_buffer[i] == 'g'&&uart_rx_buffer[i+1] == 'e'&&uart_rx_buffer[i+2] == 't')
                {
                    if(uart_rx_buffer[i+7] == 'N'){
                        int num = 0;
                        num = uart_rx_buffer[i+8];
                        rt_device_write(write_device, 0, "3", 2);
                        //rt_device_write(write_device, 0, user_date_t[num].name, 10);
                    }
                    else if(uart_rx_buffer[i+7] == '.'){
                        int num = 0;
                        num = uart_rx_buffer[i+7];
                    }
                    else{
                        rt_device_write(write_device, 0,"[",3);
                        rt_thread_delay(5);
                        int k = user_num;
                        while (k)
                        {
                            rt_thread_delay(5);
                            rt_device_write(write_device, 0,"{\"id\":\"" , 8);
                            rt_thread_delay(5);
                            rt_device_write(write_device, 0, user_date_t[user_num-k].id , sizeof(user_date_t[k].id));
                            rt_thread_delay(5);
                            rt_device_write(write_device, 0,"\",\"account\":\"" , 14);
                            rt_thread_delay(5);
                            rt_device_write(write_device, 0, user_date_t[user_num-k].name , sizeof(user_date_t[k].name));
                            rt_thread_delay(5);
                            rt_device_write(write_device, 0,"\",\"psw\":\"" , 10);
                            rt_thread_delay(5);
                            rt_device_write(write_device, 0, user_date_t[user_num-k].psw , sizeof(user_date_t[k].psw));
                            rt_thread_delay(5);
                            rt_device_write(write_device, 0, "\",\"pswSign\":\"True\"", 19);
                            rt_thread_delay(5);
                            i++;
                            k--;
                            if(k == 0)
                                rt_device_write(write_device, 0, "}",2);    
                            else
                                rt_device_write(write_device, 0, "},",3);
                        }
                        rt_thread_delay(5);
                        rt_device_write(write_device, 0,"]",2);
                    }
                }
                if (uart_rx_buffer[i] == 'a'&&uart_rx_buffer[i+1] == 'd'&&uart_rx_buffer[i+2] == 'd'){
                    unsigned char id3[] = "3";
                    unsigned char name3[] = "zwl";
                    unsigned char psw3[] = "12345687";
                    for(int m = 0; m < 2; m++){
                        user_date_t[2].id[m] = id3[m];
                    }
                     for(int m = 0; m < 4; m++){
                        user_date_t[2].name[m] = name3[m];
                    }
                     for(int m = 0; m < 9; m++){
                        user_date_t[2].psw[m] = psw3[m];
                    }
                    user_num++;
                }
                if (uart_rx_buffer[i] == 'f'&&uart_rx_buffer[i+1] == 'a'&&uart_rx_buffer[i+2] == 'c'&&uart_rx_buffer[i+3] == 'e'){
                    //rt_device_write(write_device, 0, "1", 2);
                    rt_kprintf("face_flag%d",face_flag);
                    if (face_flag == 1){
                        rt_device_write(write_device, 0, "1", 2);
                        //rt_kprintf("openmv_device%d\n", face_flag);
                        face_flag = 0;
                    }
                }
                if (uart_rx_buffer[i] == 's'&&uart_rx_buffer[i+1] == 'i'&&uart_rx_buffer[i+2] == 'g'&&uart_rx_buffer[i+3] == 'n'){
                    int nums = 0;
                    //nums = uart_rx_buffer[i+4];
                    user_quan_flag++;
                    nums = user_quan_flag;
                    if(nums==1){
                        if (openmv_d != RT_NULL)
                        {
                            rt_device_write(openmv_d, 0, "1", 2);
                            rt_kprintf("success1\n");
                        }
                        else {
                            rt_kprintf("else1\n");
                        }
                    }
                    if(nums==2){
                        if (openmv_d != RT_NULL)
                        {
                            rt_device_write(openmv_d, 0, "2", 2);
                            rt_kprintf("success2\n");
                        }
                        else {
                            rt_kprintf("else2\n");
                        }
                    }
                    if(nums==3){
                        user_quan_flag = 0;
                        if (openmv_d != RT_NULL)
                        {
                            rt_device_write(openmv_d, 0, "3", 2);
                            rt_kprintf("success3\n");
                        }
                        else {
                            rt_kprintf("else3\n");
                        }
                    }
                    
                }
                if ((readstate == 1)|| (uart_rx_buffer[i] == '{'&&uart_rx_buffer[i+1] == '\"')){
                    readstate = 1;
                    int k = 0;
                    int j = i;
                    //rt_kprintf("\nrx_length:%d\n", rx_length);
                    rt_thread_delay(5);
                    for (k = 0; k < rx_length; k++){
                        app_buffer[length+k] = uart_rx_buffer[j++];
                    }
                    length++;
                    
                

                }
            }
            
            
                        
        }
        //rt_kprintf("\nlength:%d\n",length);
    }
}

void test_rtt_uart_esp32(void)
{
    rt_err_t result;

    /* 初始化邮箱 */
    result = rt_mb_init(&json_mb,
                        "json_mbt",                      /* 名称是 mbt */
                        &json_mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                        sizeof(json_mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                        RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
        return -1;
    }


    /* 创建devt线程*/
    rt_thread_t thread = rt_thread_create("esp32_devt",
        esp32_thread_entry, RT_NULL,
        4096, 25, 10);
    /* 创建成功则启动线程*/
    if (thread!= RT_NULL)
        rt_thread_startup(thread);
    rt_thread_t thread3 = rt_thread_create("json_entry",
        json_thread_entry, RT_NULL,
        4096, 25, 10);
    /* 创建成功则启动线程*/
    if (thread3 != RT_NULL){
        rt_thread_startup(thread3);
        }
}
 #include  <finsh.h> 
FINSH_FUNCTION_EXPORT(test_rtt_uart_esp32, test uart 1-2 e.g.test_rtt_uart_esp32());
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_rtt_uart_esp32, test_rtt_uart_esp32);
