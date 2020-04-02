#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <arpa/inet.h>
#define MAXLINE 200
#define MAXUSER 4
struct Info
{
    char id[100];
    int state,score;
}info[4];
enum{ SINGLE_PLAY=0,MULTI,PAUSE,READY,GAME_OVER,WAIT_GAME,NEW_CH};
char map[4][22][12];
char *id[4];
static int uiUser=0;
static int tmp_state;
static int tmp_score;
int main (int argc, char *argv[])
{
    int n, listenfd, connfd, caddrlen,i,j,k;
    int nch=1;
    int servSock,clntSock[MAXUSER];
    fd_set fs_status;
    struct timeval timeout;
    int fr;
    int rd_count=0,pla_count=0;
    int pla=0;
    struct hostent *h;
    struct sockaddr_in saddr, caddr;
    char buf[MAXLINE],*output;
    char tmp[1024];
    char integer[4];
    int port =10001;
    int player_no;
    int iCnt,iCnt2,iRet;
    int tempSock,iMaxSock;
    int rt;
    pid_t pid;
    if(argv[1])
    {
        for(i=0,port=0; i<strlen(argv[1]); i++)
        {
            port*=10;
            port+=argv[1][i]-'0';
        }
    }

    if ((servSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return -1;
    }
    bzero((char *)&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = htonl(INADDR_ANY);
    saddr.sin_port = htons(port);

    if (bind(servSock, (struct sockaddr *)&saddr,sizeof(saddr)) < 0)
    {
        printf("bind error\n");
        return -1;
    }
    if (listen(servSock, 5) < 0)
    {
        return -1;
    }

    caddrlen = sizeof(caddr);
    iMaxSock=servSock+1;
    uiUser=0;


    while(1)
    {
        timeout.tv_sec=0;
        timeout.tv_usec=2000;
        FD_ZERO(&fs_status);
        FD_SET(0,&fs_status);
        FD_SET(servSock,&fs_status);

        for(iCnt2=uiUser-1;iCnt2>=0;iCnt2--)
        {
            FD_SET(clntSock[iCnt2],&fs_status);
            if(clntSock[iCnt2]>=iMaxSock)
            {
                iMaxSock=clntSock[iCnt2]+1;
            }
        }
        //printf("wait\n");
        iRet=select(iMaxSock,&fs_status,0,0,&timeout);
        //printf("pass\n");
        if(pla_count==uiUser&&uiUser>1)
        {
            player_no=uiUser;
            pla_count=0;
            for(iCnt=uiUser-1; iCnt>=0; iCnt--)
            {

                //printf("----player no : %d\n",player_no);
                htonl(player_no);
                write(clntSock[iCnt],(char *)&player_no,4);
                ntohl(player_no);
                for(iCnt2=0;iCnt2<uiUser;iCnt2++)
                {
                    if(iCnt2!=iCnt)
                    {
                        write(clntSock[iCnt],info[iCnt2].id,100);
                        htonl(tmp_score);
                        write(clntSock[iCnt],&info[iCnt2].score,4);
                        ntohl(tmp_score);
                    }

                }
                for(iCnt2=0;iCnt2<uiUser;iCnt2++)
                {
                    if(iCnt2!=iCnt)
                    {
                        write(clntSock[iCnt],map[iCnt2],22*12);
                    }

                }

            }
        }


        if(FD_ISSET(servSock,&fs_status)==1)
        {
            tempSock=accept(servSock,(struct sockaddr *)&caddr,&caddrlen);
            if(tempSock<0)
            {
                printf("accept() failed\n");
                continue;
            }

            //printf("cli ip %s\n",inet_ntoa(caddr.sin_addr));
            //printf("cli port %d\n",ntohs(caddr.sin_port));
            //printf("socket number %d\n",tempSock);

            if(uiUser>=MAXUSER)
            {
                close(tempSock);
                continue;
            }

            clntSock[uiUser]=tempSock;
            uiUser++;
            //printf("num of people %d \n",uiUser);
        }
        else if(1==FD_ISSET(0,&fs_status))
        {
            continue;
        }
        else
        {
            for(iCnt=uiUser-1;iCnt>=0;iCnt--)
            {
                if(info[iCnt].state==GAME_OVER)pla_count++;
                else if(1==FD_ISSET(clntSock[iCnt],&fs_status))
                {
                    if((iRet=read(clntSock[iCnt],tmp,100))==0)
                    {
                        info[iCnt].state=GAME_OVER;
                        pla_count++;
                        break;
                    }

                    strcpy(info[iCnt].id,tmp);

                    htonl(tmp_state);
                    read(clntSock[iCnt],(char *)&tmp_state,4);
                    ntohl(tmp_state);

                    htonl(tmp_score);
                    read(clntSock[iCnt],(char *)&tmp_score,4);
                    ntohl(tmp_score);

                    info[iCnt].state=tmp_state;
                    info[iCnt].score=tmp_score;

                    if(info[iCnt].state==READY)rd_count++;
                    if(info[iCnt].state==MULTI||info[iCnt].state==GAME_OVER)
                    {
                        read(clntSock[iCnt],map[iCnt],22*12);
                        pla_count++;
                    }


                    //read(clntSock[iCnt],map[iCnt],22*12);
                    //printf("iCnt : %d\n",iCnt);
                    //printf("state : %d score : %d \nclient %d : %s\nready player : %d\nnum of peo %d\n",tmp_state,tmp_score,iCnt,tmp,rd_count,uiUser);
                     /*for(i=0;i<22;i++)
                     {
                        for(j=0;j<12;j++)
                        {
                            printf("%c",map[iCnt][i][j]);
                        }
                        printf("\n");
                     }*/

                     //printf("-----------------------------------------------\n");
                    //fflush(stdout);
                    //printf("%s\n",tmp);
                }
            }

            for(iCnt=0;iCnt<uiUser;iCnt++)
            {
                if(info[iCnt].state==SINGLE_PLAY&&rd_count==1&&nch)
                {
                    tmp_state=NEW_CH;
                    htonl(tmp_state);
                    write(clntSock[iCnt],(char *)&tmp_state,4);
                    ntohl(tmp_state);
                    nch=0;

                    break;
                }
            }
            if(rd_count==uiUser&&rd_count>1)
            {

                for(iCnt=uiUser-1;iCnt>=0;iCnt--)
                {
                    //printf("!!!!\n");
                    tmp_state=MULTI;
                    htonl(tmp_state);
                    write(clntSock[iCnt],(char *)&tmp_state,4);
                    ntohl(tmp_state);
                }
                rd_count=0;
            }

        }


    }

    return 0;
}
