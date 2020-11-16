#ifndef __AT_DEVICE_H__
#define __AT_DEVICE_H__

struct user_date{
    unsigned char id[2];
    unsigned char name[20];
    unsigned char psw[20];
    unsigned char event[50];
    unsigned char address[20];
    unsigned char event_time[20];
    unsigned char importent[20];
}user_date_t[10];

//unsigned char *event;
//unsigned char *address;
//unsigned char *event_time;
//unsigned char *importent;

unsigned int face_flag;
unsigned int user_date_flag;
unsigned int user_num_flag;
unsigned int user_quan_flag;


struct user_date_length{
    unsigned char name_len;
    unsigned char event_len;
    unsigned char address_len;
    unsigned char event_time_len;
    unsigned char importent_len;
}user_date_len[10];


int userFaceIn;


#endif

