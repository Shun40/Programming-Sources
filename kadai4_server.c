/* 課題4 */
/* サーバ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <netdb.h>

#define MSG_STOCK 10

/* Function prototype declare */
void Download(void);
void Upload(void);
int SearchMessageTime(int Year, int Month, int Day, int Hour, int Min, int HourOff, int MinOff);
void SendTimeAndName(int num);
int RequestCheck(char *msg1,char *msg2,char *msg3);
int StrComp(char str1[], char str2[]);
void InitialRcvBuf(char *Buf);
void Interrupt(int signalType);

struct MSG {
  char name[32];
  int year;
  int month;
  int day;
  int hour, min, sec;
  char week[8];
  char msg[512];
};

struct TIME {
  int year, month, day, hour, min, priority, max_flag;
};

struct MSG message[MSG_STOCK];
struct TIME para[MSG_STOCK];

int ssock;
int csock;
int MsgNotFlag = 1;
int MsgMaxFlag = 0;
int MsgNum = -1;

int main(int argc, char *argv[])
{ 
  struct sockaddr_in saddr;
  struct sockaddr_in caddr;
  struct sigaction handler;

  int BindCheck, ReqCheck, Mode;
  int alen = sizeof(caddr);
  int flag = 1;
  char RcvBuf[32];
  char SendMsg[32];
  char *GET, *FileName, *HTTP, *Ver;

  /* Message set */
  strcpy(SendMsg, "Server received HTTP request!\0\n");

  printf("Server set up!\n\n");

  handler.sa_handler=Interrupt;
  if(sigfillset(&handler.sa_mask) < 0)
      exit(1);
  handler.sa_flags=0;
  if(sigaction(SIGINT,&handler,0) < 0)
      exit(1);
 
  ssock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

  memset(&saddr,0,sizeof(saddr));
  saddr.sin_family=AF_INET;
  saddr.sin_addr.s_addr=htonl(INADDR_ANY);
  saddr.sin_port=htons(atoi(argv[1]));

  setsockopt(ssock,SOL_SOCKET,SO_REUSEADDR,&flag,sizeof(flag));

  BindCheck = bind(ssock,(struct sockaddr*)&saddr,sizeof(saddr));
  if(BindCheck == -1)
    {
      printf("This port number cannot use!\n");
      printf("Program exit.\n");
      exit(1);
    }

  listen(ssock,10);

  while(1)
    {
      csock = accept(ssock,(struct sockaddr*)&caddr,&alen);
      if(csock == -1)
	{
	  printf("Accept failed.\n");
	  printf("Program exit.\n\n");
	  exit(1);
	}

      printf("--------------------------------------------------\n\n");

      /* HTTP request receive */
      InitialRcvBuf(RcvBuf);
      recv(csock, RcvBuf, 32, 0);

      GET = strtok(RcvBuf, "/");
      FileName = strtok(NULL, " ");
      HTTP = strtok(NULL, "/");
      Ver = strtok(NULL, "\n");

      /* HTTP request check */
      ReqCheck = RequestCheck(GET, HTTP, Ver);
      if(ReqCheck == 0)
	{
	  printf("Request message:%s/%s %s/%s\n",GET, FileName, HTTP, Ver);
	  printf("File name:%s\n",FileName);
	}
 
      send(csock, SendMsg, strlen(SendMsg), 0);
      sleep(1);

      /* Download or Upload receive */
      recv(csock, &Mode, sizeof(int), 0);

      /* Download */
      if(Mode == 1)
	Download();
      /* Upload */
      else if(Mode == 2)
	Upload();

      close(csock);
    }
}

void Download(void)
{
  int Search, RcvData;
  char SendMsg_d1[32], SendMsg_d2[40], SendMsg_d3[40], SendMsg_d4[64], SendMsg_d5[64]; 
  char RcvBuf_Name[32];

  /* Message set */
  strcpy(SendMsg_d1, "Request mode is Download.\0\n");
  strcpy(SendMsg_d2, "Message search use messanger's name.\0\n");
  strcpy(SendMsg_d3, "Message search use upload time.\0\n");
  strcpy(SendMsg_d4, "Select the newest message.\0\n");
  strcpy(SendMsg_d5, "Select the oldest message.\0\n");

  printf("Mode:Download\n");
  send(csock, SendMsg_d1, strlen(SendMsg_d1), 0);
  sleep(1);

  /* Message stock check */
  if(MsgNotFlag == 1)
    {
      printf("Message stock is not!\n");
      send(csock, &MsgNotFlag, sizeof(int), 0);
      sleep(1);
      return;
    }
  send(csock, &MsgNotFlag, sizeof(int), 0);
  sleep(1);

  /* Search key receive */
  recv(csock, &Search, sizeof(int), 0);

  /* Name */
  if(Search == 1)
    {
      printf("Search:Name\n");
      send(csock, SendMsg_d2, strlen(SendMsg_d2), 0);
      sleep(1);
      
      printf("Search name:");
      /* Name receive */
      int i = 0;
      InitialRcvBuf(RcvBuf_Name);
      while(1)
	{
	  RcvData = recv(csock, &RcvBuf_Name[i], 1, 0);
	  if(RcvBuf_Name[i]=='\0')
	    break;
	  else
	    printf("%s",&RcvBuf_Name[i]);
	  i++;
	}
      
      int CompResult;
      int SearchFlag = 0;
      for(i=MsgNum; i>=0; i--)
	{
	  /* Name search */
	  CompResult = StrComp(message[i].name, RcvBuf_Name);
	  if(CompResult == 0)
	    {
	      /* Message send */
	      printf("\n");
	      printf("File found!\n");
	      SendTimeAndName(i);
	      send(csock, message[i].msg, strlen(message[i].msg), 0);
	      sleep(1);

	      SearchFlag = 1;
	      printf("--------------------------------------------------\n\n");
	      break;
	    }
	}
      if(SearchFlag == 0)
	{
	  printf("\n");
	  printf("Message not found.\n");
	  printf("--------------------------------------------------\n\n");
	}
    }

  /* Time */
  else if(Search == 2)
    {
      int Year, Month, Day, Hour, Min, HourOff, MinOff;
      int Num;
      
      printf("Search:Time\n");
      send(csock, SendMsg_d3, strlen(SendMsg_d3), 0);
      sleep(1);
      recv(csock, &Year, sizeof(int), 0);
      recv(csock, &Month, sizeof(int), 0);
      recv(csock, &Day, sizeof(int), 0);
      recv(csock, &Hour, sizeof(int), 0);
      recv(csock, &HourOff, sizeof(int), 0);
      recv(csock, &Min, sizeof(int), 0);
      recv(csock, &MinOff, sizeof(int), 0);
      printf("Request time: %d %d %d %d(+-%d):%d(+-%d)\n",Year, Month, Day,
	     Hour, HourOff, Min, MinOff);
      Num = SearchMessageTime(Year, Month, Day, Hour, Min, HourOff, MinOff);

      printf("\n");
      printf("File found!\n");
      SendTimeAndName(Num);
      send(csock, message[Num].msg, strlen(message[Num].msg), 0);
      sleep(1);

      printf("Send to client was finished!\n");
      printf("--------------------------------------------------\n\n");
    }

  /* The newest */
  else if(Search == 3)
    {
      send(csock, SendMsg_d4, strlen(SendMsg_d4), 0);
      sleep(1);

      printf("\n");
      printf("File found!\n");
      SendTimeAndName(MsgNum);
      send(csock, message[MsgNum].msg, strlen(message[MsgNum].msg), 0);
      sleep(1);

      printf("Send to client was finished!\n");
      printf("--------------------------------------------------\n\n");
    }

  /* The oldest */
  else if(Search == 4)
    {
      send(csock, SendMsg_d5, strlen(SendMsg_d5), 0);
      sleep(1);

      printf("\n");
      printf("File found!\n");
      SendTimeAndName(0);
      send(csock, message[0].msg, strlen(message[0].msg), 0);
      sleep(1);

      printf("Send to client was finished!\n");
      printf("--------------------------------------------------\n\n");
    }
}

int SearchMessageTime(int Year, int Month, int Day, int Hour, int Min, int HourOff, int MinOff)
{
  int i, DecidedNum;
  int HourMinus, HourPlus, MinMinus, MinPlus;
  int PriorityMax = 0;
  int MaxCount = 0;

  /* Min offset */
  MinMinus = Min - MinOff;
  if(MinMinus < 0)
    MinMinus = 0;
  MinPlus = Min + MinOff;
  if(MinPlus > 59)
    MinPlus = 59;

  /* Hour offset */
  HourMinus = Hour - HourOff;
  if(HourMinus < 0)
    HourMinus = 0;
  HourPlus = Hour + HourOff;
  if(HourPlus > 23)
    HourPlus = 23;

  /* Time check */
  for(i=0; i<=MsgNum; i++)
    {
      if(Year == message[MsgNum].year)
	para[i].year = 1;
      if(Month == message[MsgNum].month)
	para[i].month = 1;
      if(Day == message[MsgNum].day)
	para[i].day = 1;
      if(Hour == message[MsgNum].hour)
	para[i].hour = 1;
      else if((HourMinus <= Hour)&&(Hour <= HourPlus))
	para[i].hour = 1;
      if(Min == message[MsgNum].min)
	para[i].min = 1;
      else if((MinMinus <= Min)&&(Min <= MinPlus))
	para[i].min = 1;
      para[i].priority = para[i].year + para[i].month + para[i].day + para[i].hour + para[i].min;
      if(para[i].priority >= PriorityMax)
	PriorityMax = para[i].priority;
    }

  for(i=0; i<=MsgNum; i++)
    {
      if(para[i].priority == PriorityMax)
	{
	  para[i].max_flag = 1;
	  MaxCount++;
	}
    }

  for(i=MsgNum; i>=0; i--)
    {
      if(para[i].max_flag == 1)
	{
	  DecidedNum = i;
	  break;
	}
    }

  return DecidedNum;
}

void SendTimeAndName(int num)
{
  send(csock, message[num].name, strlen(message[num].name), 0);
  sleep(1);
  send(csock, &message[num].year, sizeof(int), 0);
  send(csock, &message[num].month, sizeof(int), 0);
  send(csock, &message[num].day, sizeof(int), 0);
  send(csock, &message[num].hour, sizeof(int), 0);
  send(csock, &message[num].min, sizeof(int), 0);
  send(csock, &message[num].sec, sizeof(int), 0);
  send(csock, message[num].week, strlen(message[num].week), 0);
  sleep(1);
}


void Upload(void)
{
  int RcvData;
  char SendMsg_u1[32], SendMsg_u2[128]="", SendMsg_u3[64], SendMsg_u4[32];

  /* Message set */
  strcpy(SendMsg_u1, "Request mode is Upload.\0\n");
  strcpy(SendMsg_u2, "Time.\0\n");
  strcpy(SendMsg_u3, "Message stock is max, so you cannot upload!\0\n");
  strcpy(SendMsg_u4, "You can upload!\0\n");

  printf("Mode:Upload\n\n");
  send(csock, SendMsg_u1, strlen(SendMsg_u1), 0);
  sleep(1);

  /* Message stock check */
  if(MsgMaxFlag == 1)
    {
      printf("Message stock is max!\n");
      send(csock, &MsgMaxFlag, sizeof(int), 0);
      sleep(1);
      return;
    }
  send(csock, &MsgMaxFlag, sizeof(int), 0);
  sleep(1);

  /* Messgae stock count increment */
  MsgNum++;
  
  /* Upload name receive */
  InitialRcvBuf(message[MsgNum].name);
  recv(csock, message[MsgNum].name, 32, 0);
  printf("Upload name:%s\n",message[MsgNum].name);
  /* Upload time receive */
  recv(csock, &message[MsgNum].year, sizeof(int), 0);
  recv(csock, &message[MsgNum].month, sizeof(int), 0);
  recv(csock, &message[MsgNum].day, sizeof(int), 0);
  recv(csock, &message[MsgNum].hour, sizeof(int), 0);
  recv(csock, &message[MsgNum].min, sizeof(int), 0);
  recv(csock, &message[MsgNum].sec, sizeof(int), 0);
  recv(csock, message[MsgNum].week, 8, 0);
  sprintf(SendMsg_u2, "Upload time: %d %d %d(%s) %d:%d:%d\n",
	  message[MsgNum].year,  message[MsgNum].month, 
	  message[MsgNum].day, message[MsgNum].week, 
	  message[MsgNum].hour, message[MsgNum].min, message[MsgNum].sec);
  send(csock, SendMsg_u2, strlen(SendMsg_u2), 0);
  sleep(1);
  puts(SendMsg_u2);
  
  /* Upload message receive */
  printf("Upload message:\n");
  char MSGBUF[32];
  int i = 0;
  InitialRcvBuf(MSGBUF);
  while(1)
    {
      RcvData = recv(csock, MSGBUF, 1, 0);
      if(RcvData == 0)
	break;
      printf("%s",MSGBUF);
      strcpy(&message[MsgNum].msg[i], MSGBUF);
      i++;
    }
  printf("\n");
  printf("--------------------------------------------------\n\n");
  
  /* Message stock check */
  if(MsgNum == MSG_STOCK-1)
    {
      printf("Message stock max!\n");
      MsgMaxFlag = 1;
    }
  MsgNotFlag = 0;
}


int RequestCheck(char *msg1,char *msg2,char *msg3)
{
  char Err_Msg[28];

  strcpy(Err_Msg,"Request message is not true.");

  /* "GET" check */
  if((msg1[0]!='G')||(msg1[1]!='E')||(msg1[2]!='T')||(msg1[3]!=' '))
    {
      send(csock,Err_Msg,strlen(Err_Msg),0);
      printf("\n%s\n",Err_Msg);
      printf("%s:bad message.\n",msg1);
      printf("------------------------------\n\n");
      return -1;
    }

  /* "HTTP" check */
  if((msg2[0]!='H')||(msg2[1]!='T')||(msg2[2]!='T')||(msg2[3]!='P'))
    {
      send(csock,Err_Msg,strlen(Err_Msg),0);
      printf("\n%s\n",Err_Msg);
      printf("%s:bad message.\n",msg2);
      printf("------------------------------\n\n");
      return -1;
    }

  /* "Ver" check */
  if((msg3[0]!='1')||(msg3[1]!='.')||(msg3[2]!='1'))
    {
      send(csock,Err_Msg,strlen(Err_Msg),0);
      printf("\n%s\n",Err_Msg);
      printf("%s:bad message.\n",msg3);
      printf("------------------------------\n\n");
      return -1;
    }

  return 0;
}

int StrComp(char str1[], char str2[])
{
  int i;

  for(i=0; str1[i] == str2[i]; i++)
    {
      if(str1[i] == '\0')
	{
	  return 0;
	}
    }
  return str1[i] - str2[i];
}

void InitialRcvBuf(char *Buf)
{
  int i = 0;
  while(1)
    {
      if(Buf[i] != 0)
	Buf[i]=0;
      else
	break;
      i++;
    }
}

void Interrupt(int signalType)
{
  printf("\nInterrupt!\n");
  printf("Signal:%d\n\n",signalType);
  close(ssock);
  exit(1);
}
