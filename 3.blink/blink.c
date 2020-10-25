#include <stdio.h>

#include <unistd.h>     ///sleep

   
#include <string.h>  ///shell

#define MAX_FILE_LEN  1024*10

#define bool _Bool
#define false 0
#define true 1

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
    char result[MAX_FILE_LEN] = {0};
    char s_cmd[64] = {0};
    bool led_state = false;
    while (1) 
    {
        memset(s_cmd, 0, sizeof(s_cmd));
        sprintf(s_cmd,"gpioset 0 3=%d",led_state);

        executeCMD(s_cmd, result);
        printf("led state[%d]\r\n",led_state);
       // printf("%s\r\n", result);
       // printf("==========================================================\r\n"); 
        led_state = !led_state;
        sleep(1); 
    }

}

