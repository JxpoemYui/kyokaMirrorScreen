#include <string.h>

#include <rtthread.h>
#include <webclient.h>

#define POST_RESP_BUFSZ                1024
#define POST_HEADER_BUFSZ              1024
#define GET_HEADER_BUFSZ               1024
#define GET_RESP_BUFSZ                 1024

#define POST_LOCAL_URI                  "http://192.168.1.120:8000/account/login/"
#define GET_LOCAL_URI1                  "http://192.168.1.120:8000/account/my-information/"

char *post_data2 = "username=zwl&password=123qweasd";
    
int webclient_post_test1(int argc, char **argv)
{
    struct webclient_session* POST_session = RT_NULL;
    struct webclient_session* GET_session = RT_NULL;
    unsigned char *POST_buffer = RT_NULL;
    unsigned char *GET_buffer = RT_NULL;
    char *POST_URI = RT_NULL;
    char *GET_URI = RT_NULL;
    int index, ret = 0;
    int GET_bytes_read,POST_bytes_read, GET_resp_status, POST_resp_status;
    int content_length = -1;

    if (argc == 1)
    {
        POST_URI = web_strdup(POST_LOCAL_URI);
        if(POST_URI == RT_NULL)
        {
            rt_kprintf("no memory for create URI buffer.\n");
            return -1;
        }
        /*GET_URI = web_strdup(GET_LOCAL_URI1);
        if(GET_URI == RT_NULL)
        {
            rt_kprintf("no memory for create URI buffer.\n");
            return -1;
        }*/
    }
    else if (argc == 2)
    {
        POST_URI = web_strdup(argv[1]);
        if(POST_URI == RT_NULL)
        {
            rt_kprintf("no memory for create URI buffer.\n");
            return -1;
        }
        /*GET_URI = web_strdup(argv[1]);
        if(GET_URI == RT_NULL)
        {
            rt_kprintf("no memory for create URI buffer.\n");
            return -1;
        }*/
    }
    else
    {
        rt_kprintf("webclient_post_test [URI]  - webclient POST request test.\n");
        return -1;
    }

    POST_buffer = (unsigned char *) web_malloc(POST_RESP_BUFSZ);
    if (POST_buffer == RT_NULL)
    {
        rt_kprintf("no memory for receive response buffer.\n");
        ret = -RT_ENOMEM;
        goto __exit;

    }
    /*GET_buffer = (unsigned char *) web_malloc(1024);
    if (GET_buffer == RT_NULL)
    {
        rt_kprintf("no memory for receive buffer.\n");
        ret = -RT_ENOMEM;
        goto __exit;

    }*/

    /* create webclient session and set header response size */
    POST_session = webclient_session_create(POST_HEADER_BUFSZ);
    if (POST_session == RT_NULL)
    {
        ret = -RT_ENOMEM;
        goto __exit;
    }
    GET_session = webclient_session_create(1024);
    if (GET_session == RT_NULL)
    {
        ret = -RT_ENOMEM;
        goto __exit;
    }

    /* build header for upload */
    webclient_header_fields_add(POST_session, "Content-Length: %d\r\n", strlen(post_data2));
    webclient_header_fields_add(POST_session, "Content-Type: application/x-www-form-urlencoded\r\n");

    /* send POST request by default header */
    if ((POST_resp_status = webclient_post(POST_session, POST_URI, post_data2)) != 200)
    {
        rt_kprintf("webclient POST request failed, response(%d) error.\n", POST_resp_status);
        ret = -RT_ERROR;
        goto __exit;
    }

    rt_kprintf("webclient POST request response data :\n");

    do
    {
        POST_bytes_read = webclient_read(POST_session, POST_buffer, POST_RESP_BUFSZ);
        if (POST_bytes_read <= 0)
        {
            break;
        }

        for (index = 0; index < POST_bytes_read; index++)
        {
            rt_kprintf("%c", POST_buffer[index]);
        }
    } while (1);

    /*if ((GET_resp_status = webclient_get(GET_session, GET_URI)) != 200)
    {
        rt_kprintf("webclient GET request failed, response(%d) error.\n", GET_resp_status);
        ret = -RT_ERROR;
        goto __exit;
    }

    content_length = webclient_content_length_get(GET_session);
    if (content_length < 0)
    {
        rt_kprintf("webclient GET request type is chunked.\n");
        do
        {
            GET_bytes_read = webclient_read(GET_session, GET_buffer, GET_RESP_BUFSZ);
            if (GET_bytes_read <= 0)
            {
                break;
            }

            for (index = 0; index < GET_bytes_read; index++)
            {
                rt_kprintf("%c", GET_buffer[index]);
            }
        } while (1);

        rt_kprintf("\n");
    }
    else
    {
        int content_pos = 0;

        do
        {
            GET_bytes_read = webclient_read(GET_session, GET_buffer, 
                    content_length - content_pos > GET_RESP_BUFSZ ?
                            GET_RESP_BUFSZ : content_length - content_pos);
            if (GET_bytes_read <= 0)
            {
                break;
            }

            for (index = 0; index < GET_bytes_read; index++)
            {
                rt_kprintf("%c", GET_buffer[index]);
            }

            content_pos += GET_bytes_read;
        } while (content_pos < content_length);

        rt_kprintf("\n");
    }*/
    

    rt_kprintf("\n");

__exit:
    if (POST_session)
    {
        webclient_close(POST_session);

    }
    if (GET_session)
    {
        webclient_close(GET_session);

    }

    if (POST_buffer)
    {
        web_free(POST_buffer);
    }
    if (GET_buffer)
    {
        web_free(GET_buffer);
    }

    if (POST_URI)
    {
        web_free(POST_URI);
    }
    if (GET_URI)
    {
        web_free(GET_URI);
    }

    return ret;
}

#ifdef FINSH_USING_MSH
#include <finsh.h>
MSH_CMD_EXPORT_ALIAS(webclient_post_test1, web_post_test1, webclient_post_test1 [URI]  - webclient POST request test.);
#endif /* FINSH_USING_MSH */