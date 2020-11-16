#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 10
#define RT_ALIGN_SIZE 8
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
#define RT_DEBUG
#define RT_DEBUG_COLOR

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
#define RT_USING_SIGNALS

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_MEMHEAP
#define RT_USING_SMALL_MEM
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_INTERRUPT_INFO
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 128
#define RT_CONSOLE_DEVICE_NAME "uart2"
#define RT_VER_NUM 0x40003

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10

/* C++ features */


/* Command shell */

#define RT_USING_FINSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_USING_DESCRIPTION
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_CMD_SIZE 80
#define FINSH_USING_MSH
#define FINSH_USING_MSH_DEFAULT
#define FINSH_ARG_MAX 10

/* Device virtual file system */

#define RT_USING_DFS
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX 2
#define DFS_FILESYSTEM_TYPES_MAX 6
#define DFS_FD_MAX 32
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 936
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_0
#define RT_DFS_ELM_USE_LFN 0
#define RT_DFS_ELM_MAX_LFN 64
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_USING_DFS_DEVFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_PIPE_BUFSZ 512
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 4096
#define RT_USING_CAN
#define RT_CAN_USING_HDR
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_PWM
#define RT_USING_RTC
#define RT_USING_SPI
#define RT_USING_SPI_MSD

/* Using USB */


/* POSIX layer and C standard library */

#define RT_USING_LIBC
#define RT_USING_PTHREADS
#define PTHREAD_NUM_MAX 8
#define RT_USING_POSIX

/* Network */

/* Socket abstraction layer */

#define RT_USING_SAL

/* protocol stack implement */

#define SAL_USING_AT
#define SAL_USING_TLS
#define SAL_USING_POSIX

/* Network interface device */

#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0

/* light weight TCP/IP stack */


/* AT commands */

#define RT_USING_AT
#define AT_DEBUG
#define AT_USING_CLIENT
#define AT_CLIENT_NUM_MAX 2
#define AT_USING_SOCKET
#define AT_USING_CLI
#define AT_CMD_MAX_LEN 128
#define AT_SW_VERSION_NUM 0x10301

/* VBUS(Virtual Software BUS) */


/* Utilities */


/* RT-Thread MIPS CPU */


/* RT-Thread online packages */

/* IoT - internet of things */

#define PKG_USING_WEBCLIENT
#define WEBCLIENT_USING_SAMPLES
#define WEBCLIENT_USING_TLS
#define PKG_USING_WEBCLIENT_V201
#define PKG_WEBCLIENT_VER_NUM 0x20001
#define PKG_USING_JSMN
#define PKG_USING_JSMN_V010

/* jsmn Options */

#define JSMN_USING_EXAMPLE
#define JSMN_USING_TEST

/* Wi-Fi */

/* Marvell WiFi */


/* Wiced WiFi */

#define PKG_USING_AT_DEVICE
#define AT_DEVICE_USING_ESP8266
#define AT_DEVICE_ESP8266_SAMPLE
#define ESP8266_SAMPLE_WIFI_SSID "PDCN"
#define ESP8266_SAMPLE_WIFI_PASSWORD "12345687"
#define ESP8266_SAMPLE_CLIENT_NAME "uart1"
#define ESP8266_SAMPLE_RECV_BUFF_LEN 2048
#define PKG_USING_AT_DEVICE_LATEST_VERSION
#define PKG_AT_DEVICE_VER_NUM 0x99999

/* IoT Cloud */


/* security packages */

#define PKG_USING_MBEDTLS

/* Select Root Certificate */

#define MBEDTLS_AES_ROM_TABLES
#define MBEDTLS_ECP_WINDOW_SIZE 2
#define MBEDTLS_SSL_MAX_CONTENT_LEN 3584
#define MBEDTLS_MPI_MAX_SIZE 1024
#define MBEDTLS_CTR_DRBG_KEYSIZE 32
#define PKG_USING_MBEDTLS_V2710

/* language packages */


/* multimedia packages */


/* tools packages */


/* system packages */

#define PKG_USING_GUIENGINE
#define PKG_USING_GUIENGINE_LATEST_VERSION
#define PKG_USING_RGB888_PIXEL_BITS_32
#define PKG_USING_RGB888_PIXEL_BITS 32
#define GUIENGINE_CMD_STRING_MAX 16
#define GUIENGINE_USING_FONT16
#define GUIENGINE_USING_FONT12
#define GUIENGINE_USING_FONTHZ
#define GUIENGINE_USING_HZ_FILE
#define GUIENGINE_USING_PNG
#define GUIENGINE_IMAGE_LODEPNG
#define GUIENGINE_IMAGE_CONTAINER
#define GUIENGINE_USING_DEMO
#define GUIENGINE_NAME_MAX 16

/* peripheral libraries and drivers */


/* miscellaneous packages */


/* samples: kernel and components samples */

#define SOC_1C300
#define RT_LS1C_OPENLOONGSON
#define RT_USING_UART2
#define RT_USING_UART1
#define RT_USING_UART3
#define RT_USING_UART9
#define RT_UART_RX_BUFFER_SIZE 64
#define RT_USING_SPI0
#define RT_USING_SPI1
#define RT_USING_I2C2
#define USING_BXCAN0
#define USING_BXCAN1
#define NO_TOUCH
#define RT_RTC_NAME "RTC"

#endif
