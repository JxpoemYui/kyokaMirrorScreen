/*
 * File      : at_sample_client.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006 - 2018, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-07-06     chenyong     first version
 */

#include <stdlib.h>
#include <string.h>

#include <rtthread.h>
#include <at.h>
#include <webclient.h>
#define GET_HEADER_BUFSZ               1024
#define GET_RESP_BUFSZ                 1024


#define LOG_TAG              "at.sample"
#include <at_log.h>

#define GET_LOCAL_URI          "http://192.168.1.120:8000/account/show/"

//#define GET_LOCAL_URI          "http://192.168.1.120:8000/account/my-information/?username=zwl&password=123qweasd"


int webclient_get_test2(int argc, char **argv)
{
    struct webclient_session* session = RT_NULL;
    unsigned char *buffer = RT_NULL;
    char *URI = RT_NULL;
    int index, ret = 0;
    int bytes_read, resp_status;
    int content_length = -1;


    /*if (argc == 1)
    {
        URI = web_strdup(GET_LOCAL_URI);
        if(URI == RT_NULL)
        {
            rt_kprintf("no memory for create URI buffer.\n");
            return -1;
        }
    }
    else if (argc == 2)
    {
        URI = web_strdup(argv[1]);
        if(URI == RT_NULL)
        {
            rt_kprintf("no memory for create URI buffer.\n");
            return -1;
        }
    }
    else
    {
        rt_kprintf("webclient_get_test [URI]  - webclient GET request test.\n");
        return -1;
    }*/
    URI = web_strdup(GET_LOCAL_URI);
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

    rt_kprintf("webclient GET request response data :\n");

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

            for (index = 0; index < bytes_read; index++)
            {
                rt_kprintf("%c", buffer[index]);
            }
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

            for (index = 0; index < bytes_read; index++)
            {
                rt_kprintf("%c", buffer[index]);
            }

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

    return ret;
}


#ifdef FINSH_USING_MSH
#include <finsh.h>
//MSH_CMD_EXPORT(test_wget, my web download file);
MSH_CMD_EXPORT(webclient_get_test2, say hello to RT-Thread);
//MSH_CMD_EXPORT_ALIAS(webclient_get_test2, web_get_test1, web_get_test1 [URI]  webclient GET request test);
#endif