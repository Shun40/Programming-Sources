/* 4I 36番 山下峻 */
/* 課題4 */
/* クライアント */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>

/* Function prototype declare */
void ReceiveTimeAndName(int csock);
void InitialRcvBuf(char *Buf);

/* DNS system */
unsigned long ResolveName(char name[])
{
  struct hostent *host;

  if((host = gethostbyname(name)) == NULL)
    {
      fprintf(stderr,"\n入力されたURLは不正です。\nプログラムを終了します。\n\n");
      exit(1);
    }

  return *((unsigned long *)host->h_addr_list[0]);
}

int main(int argc, char *argv[])
{
  int csock;
  int Mode, Search;
  int ReConnect, RcvData;
  time_t Now;
  char RcvBuf_1[32], RcvBuf_2[32], RcvBuf_3[40], RcvBuf_4[128];
  char HTTP[64], FileName[64], HTTP_req[256];
  char Name[32];
  char FileBuf[128], *pp;
  char MsgBuf[1024];
  char week[][6] = {"Sun\0", "Mon\0", "Tue\0", "Wed\0", "Thu\0", "Fri\0", "Sat\0"};
  FILE *fp;
  struct sockaddr_in addr;

  printf("--------------------------------------------------\n");

  printf("Start to commnuication with server.\n");
  printf("Please input text file name.\n");
  printf("Download:Download message write to the text file.\n");
  printf("Upload:The text file's message send to server.\n");
  printf(">>");
  gets(FileName);
  sprintf(HTTP, "GET /%s HTTP/1.1\n\r\n\r",FileName);

  fp = fopen(FileName, "rb");
  if(fp = NULL)
    {
      printf("File not found.\n");
      printf("Program exit.\n\n");
      exit(1);
    }

  csock = socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);

  memset(&addr,0,sizeof(addr));
  addr.sin_family=AF_INET;
  addr.sin_addr.s_addr=ResolveName(argv[1]);
  addr.sin_port=htons(atoi(argv[2]));

  ReConnect = connect(csock,(struct sockaddr*)&addr,sizeof(addr));
  if(ReConnect == -1)
    {
      printf("Server send not ACK.\n");
      printf("Program exit.\n\n");
      printf("--------------------------------------------------\n\n");
      exit(1);
    }

  send(csock, HTTP, strlen(HTTP), 0);
  sleep(1);
  InitialRcvBuf(RcvBuf_1);
  recv(csock, RcvBuf_1, 32, 0);
  printf("\n");
  printf("Server< ");
  puts(RcvBuf_1);

  printf("Communication mode ?\n");
  printf("Download:1\n");
  printf("Upload:2\n");
  printf(">>");
  scanf("%d",&Mode);

  send(csock, &Mode, sizeof(int), 0);
  sleep(1);
  InitialRcvBuf(RcvBuf_2);
  recv(csock, RcvBuf_2, 32, 0);
  printf("\n");
  printf("Server< ");
  puts(RcvBuf_2);

  /* Download */
  if(Mode == 1)
    {
      int NotDown;

      recv(csock, &NotDown, sizeof(int), 0);
      if(NotDown == 1)
	{
	  printf("Message stock is 0, so you cannot download.\n");
	  printf("--------------------------------------------------\n\n");
	  exit(1);
	}
      else if(NotDown == 0)
	printf("You can download.\n");

      printf("How do you want to search the message ?\n");
      printf("Name:1\n");
      printf("Time:2\n");
      printf("The newest:3\n");
      printf("The oldest:4\n");
      printf(">>");
      scanf("%d",&Search);

      send(csock, &Search, sizeof(int), 0);
      sleep(1);
      InitialRcvBuf(RcvBuf_3);
      recv(csock, RcvBuf_3, 40, 0);
      printf("\n");
      printf("Server< ");
      puts(RcvBuf_3);
      
      /* Name */
      if(Search == 1)
	{
	  char SearchName[32];
	  printf("Please input search name.\n");
	  printf(">>");
	  scanf("%s",SearchName);
	  printf("\n");
	  /* Name send */
	  int i = 0;
	  while(1)
	    {
	      send(csock, &SearchName[i], 1, 0);
	      if(SearchName[i]=='\0')
		break;
	      i++;
	    }

	  ReceiveTimeAndName(csock);
	  fp = fopen(FileName, "wb");
	  InitialRcvBuf(MsgBuf);
	  RcvData = recv(csock, MsgBuf, 1024, 0);
	  printf("Download message:\n");
	  printf("%s",MsgBuf);
	  fputs(MsgBuf, fp);
	  fclose(fp);

	  printf("\n");
	  printf("--------------------------------------------------\n\n");
	}

      /* Time */
      else if(Search == 2)
	{
	  int Year, Month, Day, Hour, Min, HourOff, MinOff;

	  printf("Please inout seach time.\n");
	  printf("Year:"); scanf("%d",&Year); send(csock, &Year, sizeof(int), 0);
	  printf("Month:"); scanf("%d",&Month); send(csock, &Month, sizeof(int), 0);
	  printf("Day:"); scanf("%d",&Day); send(csock, &Day, sizeof(int), 0);
	  printf("Hour:");  scanf("%d",&Hour); send(csock, &Hour, sizeof(int), 0);
	  printf("Hour offset?\n");
	  printf(">>+-");
	  scanf("%d",&HourOff);
	  send(csock, &HourOff, sizeof(int), 0);
	  printf("Min:"); scanf("%d",&Min); send(csock, &Min, sizeof(int), 0);
	  printf("Min offset?\n");
	  printf(">>+-");
	  scanf("%d",&MinOff);
	  send(csock, &MinOff, sizeof(int), 0);

	  printf("\n");

	  ReceiveTimeAndName(csock);
	  fp = fopen(FileName, "wb");
	  InitialRcvBuf(MsgBuf);
	  RcvData = recv(csock, MsgBuf, 1024, 0);
	  printf("Download message:\n");
	  printf("%s",MsgBuf);
	  fputs(MsgBuf, fp);
	  fclose(fp);

	  printf("\n");
	  printf("--------------------------------------------------\n\n");
	}

      /* The newest or the oldest */
      else if((Search == 3)||(Search == 4))
	{
	  printf("\n");

	  ReceiveTimeAndName(csock);
	  fp = fopen(FileName, "wb");
	  InitialRcvBuf(MsgBuf);
	  RcvData = recv(csock, MsgBuf, 1024, 0);
	  printf("Download message:\n");
	  printf("%s",MsgBuf);
	  fputs(MsgBuf, fp);
	  fclose(fp);

	  printf("\n");
	  printf("--------------------------------------------------\n\n");
	}
    }

  /* Upload */
  else if(Mode == 2)
    {
      int NotUp;

      recv(csock, &NotUp, sizeof(int), 0);
      if(NotUp == 1)
	{
	  printf("Message stock is max, so you cannot upload.\n");
	  printf("--------------------------------------------------\n\n");
	  exit(1);
	}
      else if(NotUp == 0)
	printf("You can upload.\n");

      printf("Please input your name.\n");
      printf(">>");
      scanf("%s",Name);
      /* Upload name send */
      send(csock, Name, strlen(Name), 0);
      sleep(1);

      /* Now time get */
      Now = time(NULL);
      struct tm *pnow = localtime(&Now);
      int YEAR = pnow->tm_year+1900;
      int MONTH = pnow->tm_mon+1;

      send(csock, &YEAR, sizeof(int), 0);
      send(csock, &MONTH, sizeof(int), 0);
      send(csock, &(pnow->tm_mday), sizeof(int), 0);
      send(csock, &(pnow->tm_hour), sizeof(int), 0);
      send(csock, &(pnow->tm_min), sizeof(int), 0);
      send(csock, &(pnow->tm_sec), sizeof(int), 0);
      send(csock, week[pnow->tm_wday], strlen(week[pnow->tm_wday]), 0);
      InitialRcvBuf(RcvBuf_4);
      recv(csock, RcvBuf_4, 128, 0);
      printf("\n");
      printf("Server< ");
      puts(RcvBuf_4);

      printf("\n");
      printf("Text file's message send to server.\n");

      fp = fopen(FileName, "rb");
      while(1)
	{
	  pp = fgets(FileBuf, 256, fp);
	  if(pp==NULL)
	      break;
	  send(csock, FileBuf, strlen(FileBuf), 0);
	  sleep(1);
	}
      fclose(fp);

      printf("Send message finished.\n\n");
      printf("--------------------------------------------------\n\n");
    }

  close(csock);
  exit(0);
}

void ReceiveTimeAndName(int csock)
{
  int Year, Month, Day, Hour, Min, Sec;
  char Name[32], Week[8];

  recv(csock, Name, 32, 0);
  recv(csock, &Year, sizeof(int), 0);
  recv(csock, &Month, sizeof(int), 0);
  recv(csock, &Day, sizeof(int), 0);
  recv(csock, &Hour, sizeof(int), 0);
  recv(csock, &Min, sizeof(int), 0);
  recv(csock, &Sec, sizeof(int), 0);
  recv(csock, Week, 8, 0);

  printf("Upload name:%s\n",Name);
  printf("Upload time:%d %d %d(%s) %d:%d:%d\n",Year, Month, Day, Week, Hour, Min, Sec);
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
