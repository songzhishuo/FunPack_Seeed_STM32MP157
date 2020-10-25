/*
 * @Author: jiejie
 * @Github: https://github.com/jiejieTop
 * @Date: 2020-04-18 12:37:34
 * @LastEditTime: 2020-06-08 20:32:33
 * @Description: the code belongs to jiejie, please keep the author information and source code according to the license.
 */
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <pthread.h>
#include "mqttclient.h"

//IIC
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>

/* I2C控制器的设备节点 */ 
#define DEVICE_NAME    	"/dev/i2c-2"	

/* I2C设备地址 */
#define DEVICE_ADDR    0x38

int m_random = 0; 
float temp = 0,humidity = 0;

extern const char *test_ca_get();



static void interceptor_handler(void* client, message_data_t* msg)
{
    (void) client;
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
    MQTT_LOG_I("%s:%d %s()...\ntopic: %s\nmessage:%s", __FILE__, __LINE__, __FUNCTION__, msg->topic_name, (char*)msg->message->payload);
    MQTT_LOG_I("-----------------------------------------------------------------------------------");
}

void *mqtt_publish_thread(void *arg)
{
    mqtt_client_t *client = (mqtt_client_t *)arg;

    char buf[100] = { 0 };
    mqtt_message_t msg;
    memset(&msg, 0, sizeof(msg));
    sprintf(buf, "{\"id\":123,\"dp\":{\"temperatrue\":[{\"v\": 666,\"t\":1552289676}]}}");
    
    msg.qos = 0;
    msg.payload = (void *) buf;
    while(1) {
        printf("--->send message\r\n");
        printf("--->thread--random = [%d]\r\n",temp);
        //sprintf(buf, "{\"id\":123,\"dp\":{\"temperatrue\":[{\"v\": %d,\"t\":1552289676}]}}", temp);
        sprintf(buf, "{\"id\":123,\"dp\":{\"temperatrue\":[{\"v\":%.1f}],\"humidity\":[{\"v\":%.1f}]}}", temp, humidity);
        mqtt_publish(client, "$sys/362031/linux_c/dp/post/json", &msg);
        sleep(5);
    }
}




void CPU_temp()
{
 FILE *fd;
    int temp;
    char buff[256];

    fd = fopen("/sys/class/thermal/thermal_zone0/temp","r");
    fgets(buff,sizeof(buff),fd);
    sscanf(buff, "%d", &temp);

    fclose(fd);

    return temp/1000;


}

int init_AHT10()
{
    int fd;
	
	/* 要写的数据buf，第0个元素是要操作eeprom的寄存器地址*/
	unsigned char wr_buf[13] = {0};     

	printf("hello,this is read_write i2c test \n");
	
	/* 打开eeprom对应的I2C控制器文件 */
	fd =open(DEVICE_NAME, O_RDWR);
	if (fd< 0) 
	{
		printf("open"DEVICE_NAME"failed \n");
	}

	/*设置eeprom的I2C设备地址*/
	if (ioctl(fd,I2C_SLAVE_FORCE, DEVICE_ADDR) < 0) 
	{            
		printf("set slave address failed \n");
	}
	
	/* 将要操作的寄存器首地址赋给wr_buf[0] */
	//初始化//		
	wr_buf[0] = 0xe1;
	wr_buf[1] = 0x08;
	wr_buf[2] = 0x00;	
	/* 通过write函数完成向eeprom写数据的功能 */
	write(fd, wr_buf, 3);
    return fd;
}

int getVal_AHT10(int fd, float *temperature, float *humidity)
{
    unsigned char wr_buf[13] = {0}; 
    /* 将要读取的数据buf*/
	unsigned char rd_buf[13] = {0x10}; 
    int ret = 0,i=0;

    ///开始数据采集//
    wr_buf[0] = 0xac;       //commond
    wr_buf[1] = 0x33;       //data
    wr_buf[2] = 0x00;	
    /* 通过write函数完成向eeprom写数据的功能 */
    write(fd, wr_buf, 3);

    /* 延迟一段时间 */
    sleep(1);
    
    /* 从wr_buf[0] = w_add的寄存器地址开始读取12个字节的数据 */
    ret=read(fd, rd_buf, 6);
    printf("ret is %d \r\n",ret);

    for(i=0;i<6;i++)
    {
        printf("rd_buf is :%d\n",rd_buf[i]);
    }
    int h1 = 0 ,t1 = 0;
    h1 = rd_buf[1];
    h1 = (h1<<8) | rd_buf[2];
    h1 = (h1<<8) | rd_buf[3];
    h1 = h1>>4;

    h1 = (h1*1000)/1024/1024;

    t1 = rd_buf[3];
    t1 = t1 & 0x0000000F;
    t1 = (t1<<8) | rd_buf[4];
    t1 = (t1<<8) | rd_buf[5];

    t1 = (t1*2000)/1024/1024 - 500;
    *temperature 	= (float)t1/10;
	*humidity		= (float)h1/10;

    return 1;
   // t1 = (t1*2000)/1024/1024 - 500;
   // printf("温度:%d%d.%d",t1/100,(t1/10)%10,t1%10);
   // printf("湿度:%d%d.%d\r\n",h1/100,(h1/10)%10,h1%10);
   // printf("\r\n");
}

int main(void)
{
  // int m_random = 0;  
    int res;
    
    pthread_t thread1;
    mqtt_client_t *client = NULL;
    
    printf("\nwelcome to mqttclient test...\n");

    mqtt_log_init();

    client = mqtt_lease();

    mqtt_set_port(client, "1883");
    mqtt_set_host(client, "183.230.40.96");
    mqtt_set_client_id(client, "linux_c");
    mqtt_set_user_name(client, "362031");
    mqtt_set_password(client, "version=2018-10-31&res=products%2F362031%2Fdevices%2Flinux_c&et=1879077446&method=md5&sign=6In4kp8Rs2opcLIHvgCNcw%3D%3D");
    mqtt_set_clean_session(client, 1);
    
    mqtt_connect(client);
    
    //mqtt_subscribe(client, "topic1", QOS0, NULL);
    mqtt_subscribe(client, "$sys/362031/linux_c/dp/post/json", QOS0, NULL);             //

    mqtt_set_interceptor_handler(client, interceptor_handler);     // set interceptor handler
    
    res = pthread_create(&thread1, NULL, mqtt_publish_thread, client);
    if(res != 0) {
        MQTT_LOG_E("create mqtt publish thread fail");
        exit(res);
    }

    int fd_aht10 = init_AHT10();

    while (1) {

        getVal_AHT10(fd_aht10, &temp, &humidity);
        printf("main_Temp:%.1f,hum:%.1f\r\n",temp,humidity);
        //m_random = random_number();
        //printf("random = [%d]\r\n",m_random);
        sleep(3);
    }
}
