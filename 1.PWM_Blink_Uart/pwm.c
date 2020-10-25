#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
//#include <linux/ioctl.h>

#include <sys/select.h>

#define dbmsg(fmt, args ...) printf("%s[%d]: "fmt"\n", __FUNCTION__, __LINE__,##args)

#define PERIOD            "1000"		//频率
//#define DUTYCYCLE         "5"		//占空比
#define LENGTH            100

int pwm_init()
{
  int fd = 0;
  int ret;
#if 1 
  fd = open("/sys/class/pwm/pwmchip0/export", O_WRONLY);
  if(fd < 0)
  {
      dbmsg("open export error\n");
      return -1;
  }
  ret = write(fd, "3", strlen("3"));
  if(ret < 0)
  {
      dbmsg("creat pwm3 error\n");
      return -1;
  }else
    dbmsg("export pwm3 ok\n");
  close(fd);
#endif	
}

int pwm_setup(char* period, int per_length, char* dutycycle, int duty_length)
{
  int ret;
  int fd_period = 0,fd_duty = 0,fd_enable = 0,duty_m = 0;
 

					
  fd_period	= open("/sys/class/pwm/pwmchip0/pwm3/period", O_RDWR);
  fd_duty	= open("/sys/class/pwm/pwmchip0/pwm3/duty_cycle", O_RDWR);
  fd_enable = open("/sys/class/pwm/pwmchip0/pwm3/enable", O_RDWR);
   
  if((fd_period < 0)||(fd_duty < 0)||(fd_enable < 0))
  {
      dbmsg("open error\n");
      return -1;
  }

  
  ret = write(fd_period, period,strlen(PERIOD));
  //ret = write(fd_period, 1000,sizeof(int));		//频率
  if(ret < 0)
  {
      dbmsg("change period error\n");
      return -1;
  }else
  {
    dbmsg("change period ok\n");
	  close (fd_period);
	  
  }


  ret = write(fd_duty, dutycycle, duty_length);	//占空比
  if(ret < 0)
  {
      dbmsg("change duty_cycle error\n");
      return -1;
  }else
  {
    dbmsg("change duty_cycle ok\n");
	close (fd_duty);	
 
  }

#if 1
  ret = write(fd_enable, "1", strlen("1"));			//使能开关
  if(ret < 0)
  {
      dbmsg("enable pwm3 error\n");
      return -1;
  }else
    dbmsg("enable pwm3 ok\n");
	close (fd_enable); 
 #endif
 //duty_m = atoi(DUTYCYCLE)/2;
 // printf("duty_m: %d \n",duty_m);
  return 0;
}

void sleep_ms(unsigned int secs)

{
    struct timeval tval;

    tval.tv_sec=secs/1000;

    tval.tv_usec=(secs*1000)%1000000;

    select(0,NULL,NULL,NULL,&tval);

}

int main ()
{
  int ret;
  int duty_num = atoi(PERIOD);
  char c_buf[16] = {0};
  unsigned int flag = 1;
  printf("================\r\n");
  
  pwm_init();
  
  while(1)
  {
	  
	if(duty_num >= atoi(PERIOD))
	{
		printf("ture \r\n");
		flag = 0;
	}		
	else if(duty_num <= (atoi(PERIOD))/2)
	{
		printf("ture \r\n");
		flag = 1;
	}		
	  
	  
	if(flag == 0)		//--
	{
		duty_num -= 10;
	}	
	else if(flag == 1)	//++
	{
		duty_num += 10;
	}
	
	memset(c_buf, 0, sizeof(c_buf));
	sprintf(c_buf,"%d",duty_num);

	printf("duty_num : [%s] flag [%d]\r\n",c_buf, flag);
	pwm_setup(PERIOD, sizeof(PERIOD), c_buf, sizeof(c_buf));
	sleep_ms(50);	    
  }	  

	

  return 0;
}
