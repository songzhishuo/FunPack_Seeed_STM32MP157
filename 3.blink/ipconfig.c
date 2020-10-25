
/*
函数功能：popen（）会调用fork（）产生子进程，然后从子进程中调用/bin/sh -c来执行参数command的指令。
    参数type可使用“r”代表读取，“w”代表写入。
    依照此type值，popen（）会建立管道连到子进程的标准输出设备或标准输入设备，然后返回一个文件指针。
    随后进程便可利用此文件指针来读取子进程的输出设备或是写入到子进程的标准输入设备中
返回值：若成功则返回文件指针，否则返回NULL，错误原因存于errno中
*/


/*
函数功能：pclose（）用来关闭由popen所建立的管道及文件指针。参数stream为先前由popen（）所返回的文件指针
返回值：若成功返回shell的终止状态(也即子进程的终止状态)，若出错返回-1，错误原因存于errno中
*/
#include <stdio.h>
#include <string.h>


#define MAX_FILE_LEN  1024*10

void executeCMD(const char *cmd, char *result)
{
    char buf_ps[MAX_FILE_LEN];                          
    char ps[MAX_FILE_LEN]={0};
    FILE *ptr;
    strcpy(ps, cmd);
    if((ptr=popen(ps, "r"))!=NULL)              //打开管道
    {
        while(fgets(buf_ps, MAX_FILE_LEN, ptr)!=NULL)
        {
           strcat(result, buf_ps);
           if(strlen(result)>MAX_FILE_LEN)
               break;
        }
        pclose(ptr);                            //关闭管道
        ptr = NULL;
    }
    else
    {
        printf("popen %s error\n", ps);
    }
}

int main()
{
        char result[MAX_FILE_LEN];
        executeCMD("ifconfig", result);
        printf("===================shell[\"ifconfig\"]====================\r\n");
        printf("%s\r\n", result);
        printf("==========================================================\r\n");
        return 0;
}


