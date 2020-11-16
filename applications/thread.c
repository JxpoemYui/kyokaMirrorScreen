
#include <stdlib.h>
#include <string.h>

#include <rtthread.h>
#include <at.h>
#include <webclient.h>
#include <test.h>

#define THREAD_PRIORITY         10
#define THREAD_STACK_SIZE       8192
#define THREAD_TIMESLICE        5

#define LOG_TAG              "at.sample"
#include <at_log.h>

#define GET_HEADER_BUFSZ               1024
#define GET_RESP_BUFSZ                 1024

#define GET_LOCAL_URI2          "http://192.168.1.120:8000/account/show/"

static rt_thread_t tid1 = RT_NULL;
static rt_thread_t tid2 = RT_NULL;

#include "display_controller.h"
#include "../../libraries/ls1c_pwm.h"
#include "../../libraries/ls1c_public.h"
#include "../../libraries/ls1c_gpio.h"
#include "../../libraries/ls1c_pin.h"


/* 邮箱控制块 */
static struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[256];

static char mb_str3[] = "over";
int bytes_read;
static void pwminit3(void)
{
    pwm_info_t pwm_info;
    pwm_info.gpio = LS1C_PWM0_GPIO06;           // pwm引脚位gpio06
    pwm_info.mode = PWM_MODE_NORMAL;            // 正常模式--连续输出pwm波形
    pwm_info.duty = 0.85;                       // pwm占空比 85%
    pwm_info.period_ns = 5*1000*1000;            // pwm周期5ms

    /*pwm初始化，初始化后立即产生pwm波形*/
    pwm_init(&pwm_info);

    /* 使能pwm */
    pwm_enable(&pwm_info);
}

/* 线程1入口 */ 
static void thread1_entry(void* parameter)
{       
    unsigned char *str;
    int index;
    struct user_date user_date_1;

    while (1)
    {
        rt_kprintf("thread1: try to recv a mail\n");

        /* 从邮箱中收取邮件 */
        rt_kprintf("\n\n");
        if (rt_mb_recv(&mb, (rt_uint32_t *)&str, RT_WAITING_FOREVER) == RT_EOK)
        {   
            if (str == mb_str3){
                rt_kprintf("thread1: get a mail from mailbox, the content:%s\n", str);
                break;
            }
            /*for (index = 0; index < bytes_read; index++)
            {   
                rt_kprintf("%c", user_date_1.name[index]);
            }*/
            // for (index = 0; index < bytes_read; index++)
            // {   
            //     rt_kprintf("%c", str[index]);
            // }
            //rt_kprintf("thread1: get a mail from mailbox, the content:%c\n", &str);
            rt_kprintf("\n\n");
            
            rt_kprintf("thread1: get a mail from mailbox, the content\n");

            /* 延时 100ms */
            rt_thread_mdelay(100);
        }
    }
    /* 执行邮箱对象脱离 */
    rt_mb_detach(&mb);
}
/* 线程2入口 */ 
static void thread2_entry(void* parameter)
{
    struct webclient_session* session = RT_NULL;
    unsigned char *buffer = RT_NULL;
    char *URI = RT_NULL;
    int index, ret = 0;
    int resp_status;
    int content_length = -1;
    int result;
    
    while(1)
    {
        URI = web_strdup(GET_LOCAL_URI2);
        buffer = (unsigned char *) web_malloc(GET_HEADER_BUFSZ);
        if (buffer == RT_NULL)
        {
            rt_kprintf("no memory for receive buffer.\n");
            ret = -RT_ENOMEM;
            goto __exit;
        }
        /* create webclient session and set header response size */
        session = webclient_session_create(GET_HEADER_BUFSZ);
        if (session == RT_NULL)
        {
            ret = -RT_ENOMEM;
            goto __exit;
        }
        /* send GET request by default header */
        if ((resp_status = webclient_get(session, URI)) != 200)
        {
            rt_kprintf("webclient GET request failed, response(%d) error.\n", resp_status);
            ret = -RT_ERROR;
            goto __exit;
        }

        content_length = webclient_content_length_get(session);
        if (content_length < 0)
        {
            rt_kprintf("webclient GET request type is chunked.\n");
            do
            {
                bytes_read = webclient_read(session, buffer, GET_RESP_BUFSZ);
                if (bytes_read <= 0)
                {
                    break;
                }
                rt_mb_send(&mb, (rt_uint32_t)&buffer);

                rt_thread_mdelay(5);

            } while (1);

            rt_kprintf("\n");
        }
        else
        {
            int content_pos = 0;

            do
            {
                bytes_read = webclient_read(session, buffer, 
                        content_length - content_pos > GET_RESP_BUFSZ ?
                                GET_RESP_BUFSZ : content_length - content_pos);
                if (bytes_read <= 0)
                {
                    break;
                }
                rt_kprintf("\n");
                /*for (index = 0; index < bytes_read; index++)
                {   
                    rt_kprintf("%c", buffer[index]);
                }*/
                rt_uint8_t j = 1;
                rt_uint8_t user_num = 0;
                for (index = 0; index < bytes_read; index++)
                {   
                    rt_uint8_t i=4;
                    unsigned char date1[20];
                    if (buffer[index] =='<'&&buffer[index+1]=='t'&&buffer[index+2]=='d'&&buffer[index+3]=='>'){
                        //rt_kprintf("buffer in\n");
                        while(buffer[index+i] != '<')
                        {
                            date1[i-4] = buffer[index+i];
                            rt_kprintf("date:%c\n", date1[i-4]);
                            i++;
                            rt_thread_delay(5);
                        } 
                        if(buffer[index+4]!='<'){
                            if(j == 1){
                                for (unsigned char k = 0; k < (i-4); k++){
                                    //rt_kprintf("user1%c\n", date1[k]);
                                    user_date_t[user_num].name[k] = date1[k];
                                    user_date_len[user_num].name_len = k;
                                }
                                j = 2;
                            }
                            else if(j == 2){
                                for (unsigned char k = 0; k < (i-4); k++){
                                    //rt_kprintf("user2%c\n", date1[k]);
                                    user_date_t[user_num].event[k] = date1[k];
                                    user_date_len[user_num].event_len = k;
                                }
                                j = 3;
                            }
                            else if(j == 3){
                                for (unsigned char k = 0; k < (i-4); k++){
                                    //rt_kprintf("user3%c\n", date1[k]);
                                    user_date_t[user_num].address[k] = date1[k];
                                    user_date_len[user_num].address_len = k;
                                }
                                j = 4;
                            }
                            else if(j == 4){
                                for (unsigned char k = 0; k < (i-4); k++){
                                    //rt_kprintf("user4%c\n", date1[k]);
                                    user_date_t[user_num].event_time[k] = date1[k];
                                    user_date_len[user_num].event_time_len = k;
                                }
                                j = 5;
                            }
                            else if(j == 5){
                                for (unsigned char k = 0; k < (i-4); k++){
                                    //rt_kprintf("user%5c\n", date1[k]);
                                    user_date_t[user_num].importent[k] = date1[k];
                                    user_date_len[user_num].importent_len = k;
                                }
                                j = 1;
                                user_num++;
                            }
                        }
                    }
                }
                for (int l = 0; l < 3; l++){
                rt_kprintf("user_date_t:");
                for(int i = 0; i <= user_date_len[l].name_len; i++){
                    rt_kprintf("%c", user_date_t[l].name[i]);
                }
                rt_kprintf("\n");
                rt_kprintf("user_date_t:");
                for (int i = 0; i <= user_date_len[l].event_len; i++){
                    rt_kprintf("%c", user_date_t[l].event[i]);
                }
                rt_kprintf("\n");
                rt_kprintf("user_date_t:");
                for (int i = 0; i <= user_date_len[l].address_len; i++){
                    rt_kprintf("%c", user_date_t[l].address[i]);
                }
                rt_kprintf("\n");
                rt_kprintf("user_date_t:");
                for (int i = 0; i <= user_date_len[l].event_time_len; i++){
                    rt_kprintf("%c", user_date_t[l].event_time[i]);
                }
                rt_kprintf("\n");
                rt_kprintf("user_date_t:");
                for (int i = 0; i <= user_date_len[l].importent_len; i++){
                    rt_kprintf("%c", user_date_t[l].importent[i]);
                }
                rt_kprintf("\n");
                }
                rt_kprintf("\n\n\n");
                rt_mb_send(&mb, (rt_uint32_t)&user_date_t);
                rt_mb_send(&mb,(rt_uint32_t)&user_date_len);
                rt_mb_send(&mb, (rt_uint32_t)&mb_str3);
                user_date_flag = 1;
                //pwminit3();
                rt_thread_mdelay(5);
                content_pos += bytes_read;
            } while (content_pos < content_length);
            
            rt_kprintf("\n");
        }

     __exit:
        if (session)
        {
            webclient_close(session);
        }

        if (buffer)
        {
            web_free(buffer);
        }

        if (URI)
        {
            web_free(URI);
        }

        return 0;   
    }
    
}

void test_thread_05(void)
{   
    rt_err_t result;

    /* 初始化邮箱 */
    result = rt_mb_init(&mb,
                        "mbt",                      /* 名称是 mbt */
                        &mb_pool[0],                /* 邮箱用到的内存池是 mb_pool */
                        sizeof(mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占 4 字节 */
                        RT_IPC_FLAG_FIFO);          /* 采用 FIFO 方式进行线程等待 */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
        return -1;
    }
	tid1 = rt_thread_create("thread1",
		thread1_entry, 
		RT_NULL,
		1024, THREAD_PRIORITY, THREAD_TIMESLICE); 
	if (tid1 != RT_NULL)
		rt_thread_startup(tid1);

	tid2 = rt_thread_create("thread2",
		thread2_entry, RT_NULL,
		THREAD_STACK_SIZE, THREAD_PRIORITY, THREAD_TIMESLICE); 
	if (tid2 != RT_NULL)
		rt_thread_startup(tid2);
}

#include <finsh.h>
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(test_thread_05, thread test);