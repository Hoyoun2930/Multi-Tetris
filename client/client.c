#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <strings.h>

#include "term_control.h"
#define ESC 27
#define MAXLINE 200
#define MSEC 1000

char brick[7][4][4][4]=
{
// ㅗ 회전 0
    0,1,0,0,
    1,1,1,0,
    0,0,0,0,
    0,0,0,0,
// ㅏ 회전 1
    0,1,0,0,
    0,1,1,0,
    0,1,0,0,
    0,0,0,0,
// ㅜ 회전 2
    0,0,0,0,
    1,1,1,0,
    0,1,0,0,
    0,0,0,0,
// ㅓ 회전 3
    0,1,0,0,
    1,1,0,0,
    0,1,0,0,
    0,0,0,0,
//Z 회전0
    0,1,1,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0,
//Z 회전1
    0,1,0,0,
    0,1,1,0,
    0,0,1,0,
    0,0,0,0,
//Z 회전2
    0,1,1,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0,
//Z 회전3
    0,1,0,0,
    0,1,1,0,
    0,0,1,0,
    0,0,0,0,
//Z 회전0
    1,1,0,0,
    0,1,1,0,
    0,0,0,0,
    0,0,0,0,
//Z 회전1
    0,1,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0,
//Z 회전2
    1,1,0,0,
    0,1,1,0,
    0,0,0,0,
    0,0,0,0,
//Z 회전3
    0,1,0,0,
    1,1,0,0,
    1,0,0,0,
    0,0,0,0,
//ㄱ회전0
    1,1,0,0,
    0,1,0,0,
    0,1,0,0,
    0,0,0,0,
//ㄱ회전1
    0,0,1,0,
    1,1,1,0,
    0,0,0,0,
    0,0,0,0,
//ㄱ회전2
    0,1,0,0,
    0,1,0,0,
    0,1,1,0,
    0,0,0,0,
//ㄱ회전3
    0,0,0,0,
    1,1,1,0,
    1,0,0,0,
    0,0,0,0,
//ㄴ회전0
    0,1,1,0,
    0,1,0,0,
    0,1,0,0,
    0,0,0,0,
//ㄴ회전1
    0,0,0,0,
    1,1,1,0,
    0,0,1,0,
    0,0,0,0,
//ㄴ회전2
    0,1,0,0,
    0,1,0,0,
    1,1,0,0,
    0,0,0,0,
//ㄴ회전3
    1,0,0,0,
    1,1,1,0,
    0,0,0,0,
    0,0,0,0,
//직선 회전0
    0,1,0,0,
    0,1,0,0,
    0,1,0,0,
    0,1,0,0,
//직선 회전1
    0,0,0,0,
    1,1,1,1,
    0,0,0,0,
    0,0,0,0,
//직선 회전2
    0,1,0,0,
    0,1,0,0,
    0,1,0,0,
    0,1,0,0,
//직선 회전3
    0,0,0,0,
    1,1,1,1,
    0,0,0,0,
    0,0,0,0,
//상자 회전0
    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0,
//상자 회전1
    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0,
//상자 회전2
    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0,
//상자 회전3
    1,1,0,0,
    1,1,0,0,
    0,0,0,0,
    0,0,0,0
};


char tetris[22][12] = { \
                            ' ','-','-','-','-','-','-','-','-','-','-',' ',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            '|','+','+','+','+','+','+','+','+','+','+','|',\
                            ' ','-','-','-','-','-','-','-','-','-','-',' '\
                          };

char tetris_tmp[5][22][12];
int shp_arr[3];
struct Info
{
    char id[100];
    int len_id,state,score;
}info[4];
typedef struct LOCK
{
    pthread_mutex_t lock1;
    pthread_mutex_t lock2;
    pthread_mutex_t lock3;
    pthread_cond_t give;
    pthread_cond_t take1;
    pthread_cond_t take2;
}Lock;
enum { KEY_CHANGED_NONE = 0, KEY_CHANGED_OK, KEY_CHANGED_QUIT };
enum{ SINGLE_PLAY=0,MULTI,PAUSE,READY,GAME_OVER,WAIT_GAME,NEW_CH};
static int posX;
static int posY;
static int key_changed = KEY_CHANGED_NONE;
static int shapeNo;
static int rotationNo;
static int save_sh;
static int cfd;
static int tmp_state;
static int tmp_score;
static int player_no;
struct hostent *h;
struct sockaddr_in saddr;
Lock *lock;
char id[1024];

void *UI_Work(void *arg);
void *Key_Work(void *arg);
void *Net_Work(void *arg);
Lock *qinit();
void printScreen();
void cmdScreen();
void qdelete(Lock *q);

int main (int argc, char *argv[])
{
    int i,j,k,cnt;
    int port=10001;
    char host[100];
    char pars[3][MAXLINE];
    pthread_t ui_thread, key_thread,net_thread;

    printf("%s\n",argv[1]);

    pars[0][0]='\0';
    pars[1][0]='\0';
    pars[2][0]='\0';

    for(i=0,j=0,k=0,cnt=0; i<strlen(argv[1]); i++)
    {
        if(argv[1][i]=='.')cnt++;
        if(argv[1][i]==':')
        {
            pars[j][k]='\0';
            i++;
            k=0;
            j=1;
            pars[j][k]='\0';
        }
        if(argv[1][i]=='/')
        {
            pars[j][k]='\0';
            i++;
            k=0;
            j=2;
            pars[j][k]='\0';
        }
        pars[j][k++]=argv[1][i];
    }
    pars[j][k]='\0';
    if(cnt<3)
    {
        printf("Invalid Argument\n");
        return 0;
    }
    if(pars[2][0]!='\0')strcpy(id,pars[2]);
    else strcpy(id,"Anonymous");
    if(pars[1][0]!='\0')port=atoi(pars[1]);
    strcpy(host,pars[0]);
    printf("%s %d %s\n",host,port,id);

    if ((cfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("ERROR\n");
        return 0;
    }
    if ((h = gethostbyname(host)) == NULL)
    {
        printf("ERROR\n");
        return 0;
    }

    bzero((char *)&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    bcopy((char *)h->h_addr, (char *)&saddr.sin_addr.s_addr, h->h_length);
    saddr.sin_port = htons(port);
    if (connect(cfd,(struct sockaddr *)&saddr,sizeof(saddr)) < 0)
    {
        printf("ERROR\n");
    }

    //write(cfd, id,strlen(id));

/*
htonl(rt);
        read(cfd,(char *)&rt,4);
        ntohl(rt);
*/






    srand((unsigned)time(NULL));
    for(i=0;i<3;i++)
    {
        shp_arr[i]=rand()%7;
    }
    shapeNo = rand() % 7;
    rotationNo=0;
    posX=5;
    posY=1;
    save_sh=-1;
    player_no=1;


    strcpy(info[0].id,id);

    info[0].state=WAIT_GAME;
    info[0].score=0;

    clearScreen();

    tmp_state=info[0].state;
    tmp_score=info[0].score;

    write(cfd,info[0].id,100);

    htonl(tmp_state);
    write(cfd,(char *)&tmp_state,4);
    ntohl(tmp_state);
    htonl(tmp_score);
    write(cfd,(char *)&tmp_score,4);
    ntohl(tmp_score);

    for(k=0;k<5;k++)
    {
        for(i=0;i<22;i++)
        {
            for(j=0;j<12;j++)
                tetris_tmp[k][i][j]=tetris[i][j];
        }
    }
    //write(cfd,tetris_tmp[0],22*12);



    //write(cfd,tetris,22*12);


    //clearScreen();
    //printScreen();
    lock=qinit();


    pthread_create(&ui_thread, NULL, UI_Work, NULL);
    pthread_create(&key_thread, NULL, Key_Work, NULL);
    pthread_create(&net_thread, NULL, Net_Work, NULL);

    pthread_join(key_thread, NULL);
    pthread_join(ui_thread, NULL);
    pthread_join(net_thread, NULL);
    close(cfd);

    return 0;
}
void cmdScreen()
{
    int i,j;
    char key;
    for(i=0; i<22; i++)
    {
        for(j=0; j<12; j++)
        {
            tetris_tmp[0][i][j]=tetris[i][j];
        }
    }

    if(info[0].state==WAIT_GAME)
    {
        for(i=0; i<22; i++)
        {
            printf("  ");
            if(i==6)
            {
                printf("|<");
                setColor(NYX_COLOR_GREEEN);
                printf("Spacebar");
                setColor(NYX_COLOR_WHITE);
                printf(">|");
            }
            else if(i==7)
            {
                setColor(NYX_COLOR_WHITE);
                printf("|++Single++|");
            }
            else if(i==9)
            {
                printf("|+<");
                setColor(NYX_COLOR_GREEEN);
                printf("<Enter>");
                setColor(NYX_COLOR_WHITE);
                printf("+|");
            }
            else if(i==10)
            {
                setColor(NYX_COLOR_WHITE);
                printf("|+++Ready++|");
            }
            else
            {
                for(j=0; j<12; j++)
                {
                    setColor(NYX_COLOR_WHITE);
                    printf("%c",tetris_tmp[0][i][j]);
                }
            }
            printf("\n");
        }
    }
    else if(info[0].state==SINGLE_PLAY)
    {
    }
    else if(info[0].state==NEW_CH)
    {
        clearScreen();
        for(i=0; i<22; i++)
        {
            printf("  ");
            if(i==4)
            {
                printf("|+++");
                setColor(NYX_COLOR_GREEEN);
                printf("New");
                setColor(NYX_COLOR_WHITE);
                printf("++++|");
            }
            else if(i==5)
            {
                printf("|");
                setColor(NYX_COLOR_GREEEN);
                printf("Challenger");
                setColor(NYX_COLOR_WHITE);
                printf("|");
            }
            else if(i==6)
            {
                printf("|<");
                setColor(NYX_COLOR_GREEEN);
                printf("Spacebar");
                setColor(NYX_COLOR_WHITE);
                printf(">|");
            }
            else if(i==7)
            {
                setColor(NYX_COLOR_WHITE);
                printf("|++Resume++|");
            }
            else if(i==9)
            {
                printf("|+<");
                setColor(NYX_COLOR_GREEEN);
                printf("<Enter>");
                setColor(NYX_COLOR_WHITE);
                printf("+|");
            }
            else if(i==10)
            {
                setColor(NYX_COLOR_WHITE);
                printf("|+++Ready++|");
            }
            else
            {
                for(j=0; j<12; j++)
                {
                    setColor(NYX_COLOR_WHITE);
                    printf("%c",tetris_tmp[0][i][j]);
                }
            }
            printf("\n");
        }
        while(1)
        {
            scanf("%c",&key);
            if(key==' ')
            {
                info[0].state=SINGLE_PLAY;
                pthread_cond_signal(&lock->take1);
                pthread_cond_signal(&lock->take2);
                return;
            }
            else if(key=='\n')
            {
                info[0].state=READY;
                cmdScreen();
                return;
            }
        }
    }
    else if(info[0].state==PAUSE)
    {
        clearScreen();
        for(i=0; i<22; i++)
        {
            printf("  ");
            if(i==6)
            {
                printf("|<");
                setColor(NYX_COLOR_GREEEN);
                printf("Spacebar");
                setColor(NYX_COLOR_WHITE);
                printf(">|");
            }
            else if(i==7)
            {
                setColor(NYX_COLOR_WHITE);
                printf("|++Resume++|");
            }
            else if(i==9)
            {
                printf("|+<");
                setColor(NYX_COLOR_GREEEN);
                printf("<Enter>");
                setColor(NYX_COLOR_WHITE);
                printf("+|");
            }
            else if(i==10)
            {
                setColor(NYX_COLOR_WHITE);
                printf("|+++Ready++|");
            }
            else
            {
                for(j=0; j<12; j++)
                {
                    setColor(NYX_COLOR_WHITE);
                    printf("%c",tetris_tmp[0][i][j]);
                }
            }
            printf("\n");
        }
        while(1)
        {
            scanf("%c",&key);
            if(key==' ')
            {
                info[0].state=SINGLE_PLAY;
                pthread_cond_signal(&lock->take1);
                pthread_cond_signal(&lock->take2);
                return;
            }
            else if(key=='\n')
            {
                info[0].state=READY;
                cmdScreen();
                return;
            }
        }
    }
    else if(info[0].state==MULTI)
    {
    }
    else if(info[0].state==READY)
    {
        clearScreen();
        for(i=0; i<22; i++)
        {
            printf("  ");
            if(i==10)
            {
                setColor(NYX_COLOR_RED);
                printf("|+++Ready++|");
            }
            else
            {
                for(j=0; j<12; j++)
                {
                    setColor(NYX_COLOR_WHITE);
                    printf("%c",tetris_tmp[0][i][j]);
                }
            }
            printf("\n");
        }
        return;
    }
    else if(info[0].state==GAME_OVER)
    {
        for(i=0; i<22; i++)
        {
            printf("  ");
            if(i==7)
            {
                printf("|");
                setColor(NYX_COLOR_RED);
                printf("GAME  OVER");
                setColor(NYX_COLOR_WHITE);
                printf("|");
            }
            else
            {
                for(j=0; j<12; j++)
                {
                    setColor(NYX_COLOR_WHITE);
                    printf("%c",tetris_tmp[0][i][j]);
                }
            }
            printf("\n");
        }
        printf("  ID : %s\n",info[0].id);
        printf("  SCORE : %d\n",info[0].score);
    }

}
void printScreen()
{
    int i,j,k;
    int check[22][12];


    for(i=0; i<22; i++)
    {
        for(j=0; j<12; j++)
        {
            check[i][j]=0;
        }
    }
    clearScreen();

    for(i=posY; i<posY+4; i++)
    {
        for(j=posX; j<posX+4; j++)
        {
            if(brick[shapeNo][rotationNo][i-posY][j-posX]==1)
            {
                check[i][j]=1;
                if(tetris[i][j]=='O')
                {
                    key_changed=KEY_CHANGED_NONE;
                    info[0].state=GAME_OVER;
                    cmdScreen();
                    return;

                }
                for(k=i+1; k<21; k++)
                {
                    if(tetris[k][j]=='+')
                        check[k][j]=2;
                }
            }
        }
    }
    for(i=0;i<22;i++)
    {
        for(j=0;j<12;j++)
        {
            if(check[i][j]==1)
            {
                tetris_tmp[0][i][j]='O';
            }
            else tetris_tmp[0][i][j]=tetris[i][j];
        }
    }
    for(i=0; i<22; i++)
    {
        printf("  ");
        for(j=0; j<12; j++)
        {
            char s;
            if(check[i][j]==1)
            {
                setColor(NYX_COLOR_BROWN);
                printf("O");

            }
            else if(check[i][j]==2)
            {
                setColor(NYX_COLOR_TURQUOISE);
                printf("|");
            }
            else
            {
                if(tetris[i][j]=='O')setColor(NYX_COLOR_GREEEN);
                else if(tetris[i][j]=='|'||tetris[i][j]=='-')setColor(NYX_COLOR_RED);
                else setColor(NYX_COLOR_WHITE);
                s = tetris[i][j];
                printf("%c", s);
            }
        }
        setColor(NYX_COLOR_WHITE);

        if(i==0)printf(" NEXT           ");
        else if(i==1)printf(" ----           ");
        else if(i==2)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[0]][0][0][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==3)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[0]][0][1][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==4)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[0]][0][2][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==5)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[0]][0][3][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==6)printf(" ----           ");
        else if(i==7)printf(" ----           ");
        else if(i==8)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[1]][0][0][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==9)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[1]][0][1][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==10)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[1]][0][2][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==11)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[1]][0][3][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==12)printf(" ----           ");
        else if(i==13)printf(" ----           ");
        else if(i==14)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[2]][0][0][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==15)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[2]][0][1][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==16)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[2]][0][2][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==17)
        {
            printf("|");
            for(j=0;j<4;j++)
            {
                if(brick[shp_arr[2]][0][3][j]==1)printf("O");
                else printf(" ");
            }
            printf("|          ");
        }
        else if(i==18)printf(" ----           ");
        else printf("                ");

        for(k=1;k<player_no;k++)
        {
            for(j=0;j<12;j++)
            {
                if(tetris_tmp[k][i][j]=='-'||tetris_tmp[k][i][j]=='|')setColor(NYX_COLOR_GREEEN);
                else setColor(NYX_COLOR_WHITE);
                printf("%c",tetris_tmp[k][i][j]);
            }
        }
        printf("   \n");
    }

    printf("   ID : ");
    setColor(NYX_COLOR_GREEEN);
    printf("%s",info[0].id);
    setColor(NYX_COLOR_WHITE);
    printf("     Press E");
    for(i=1;i<player_no;i++)printf("      ID : %s",info[i].id);
    printf("\n");
    printf("   SCORE : %d   change block",info[0].score);
    for(i=1;i<player_no;i++)printf("   SCORE : %d",info[i].score);
    printf("\n");
    for(i=0;i<4;i++)
    {
        printf("               |");
        for(j=0;j<4;j++)
        {
            if(save_sh!=-1)
            {
                if(brick[save_sh][0][i][j]==1)
                {
                    setColor(NYX_COLOR_RED);
                    printf("O");
                    setColor(NYX_COLOR_WHITE);
                }
                else
                {

                    printf(" ");
                }

            }
            else
            {
                setColor(NYX_COLOR_WHITE);
                printf("+");
            }
        }
        printf("|\n");
    }
}
void *Net_Work(void *arg)
{
    int i,j,k;
    char key;
    //Lock *q=(Lock *)arg;
    cmdScreen();
    key = getchar();
    if(key==' ')
    {
        info[0].state=SINGLE_PLAY;
        tmp_state=info[0].state;
        tmp_score=info[0].score;

        write(cfd,info[0].id,100);

        htonl(tmp_state);
        write(cfd,(char *)&tmp_state,4);
        ntohl(tmp_state);

        htonl(tmp_score);
        write(cfd,(char *)&tmp_score,4);
        ntohl(tmp_score);

        pthread_cond_signal(&lock->take1);
        pthread_cond_signal(&lock->take2);
        clearScreen();
        printScreen();
    }
    else if(key=='\n')
    {
        info[0].state=READY;
        tmp_state=info[0].state;
        tmp_score=info[0].score;

        write(cfd,info[0].id,100);

        htonl(tmp_state);
        write(cfd,(char *)&tmp_state,4);
        ntohl(tmp_state);

        htonl(tmp_score);
        write(cfd,(char *)&tmp_score,4);
        ntohl(tmp_score);
    }
    while(1)
    {

        if(info[0].state==SINGLE_PLAY)
        {
            htonl(tmp_state);
            read(cfd,(char *)&tmp_state,4);
            ntohl(tmp_state);
            info[0].state=tmp_state;

            pthread_mutex_lock(&lock->lock3);
            pthread_cond_wait(&lock->give,&lock->lock3);
            pthread_mutex_unlock(&lock->lock3);
            cmdScreen();

            if(info[0].state==READY)
            {
                tmp_state=info[0].state;
                tmp_score=info[0].score;

                write(cfd,info[0].id,100);

                htonl(tmp_state);
                write(cfd,(char *)&tmp_state,4);
                ntohl(tmp_state);

                htonl(tmp_score);
                write(cfd,(char *)&tmp_score,4);
                ntohl(tmp_score);
            }
        }
        else if(info[0].state==PAUSE)
        {

            //cmdScreen();
            if(info[0].state==READY)
            {
                tmp_state=info[0].state;
                tmp_score=info[0].score;

                write(cfd,info[0].id,100);

                htonl(tmp_state);
                write(cfd,(char *)&tmp_state,4);
                ntohl(tmp_state);

                htonl(tmp_score);
                write(cfd,(char *)&tmp_score,4);
                ntohl(tmp_score);
            }
            pthread_cond_signal(&lock->take1);
            pthread_cond_signal(&lock->take2);
        }
        else if(info[0].state==READY)
        {

            cmdScreen();
            htonl(tmp_state);
            read(cfd,(char *)&tmp_state,4);
            ntohl(tmp_state);

            info[0].state=tmp_state;

            if(tmp_state==MULTI)
            {
                for(i=0;i<3;i++)shp_arr[i]=rand()%7;
                shapeNo = rand() % 7;
                rotationNo=0;
                posX=5;
                posY=1;
                save_sh=-1;
                player_no=1;
                for(i=1;i<21;i++)
                {
                    for(j=1;j<11;j++)tetris[i][j]='+';
                    for(j=1;j<11;j++)tetris_tmp[0][i][j]='+';
                }
                for(k=0; k<3; k++)
                {
                    clearScreen();

                    for(i=0; i<22; i++)
                    {
                        printf("  ");
                        if(i==6&&k==0)printf("|++++OO++++|");
                        else if(i==7&&k==0)printf("|+++O++O+++|");
                        else if(i==8&&k==0)printf("|+++++O++++|");
                        else if(i==9&&k==0)printf("|+++O++O+++|");
                        else if(i==10&&k==0)printf("|++++OO++++|");

                        else if(i==6&&k==1)printf("|++++OO++++|");
                        else if(i==7&&k==1)printf("|+++O++O+++|");
                        else if(i==8&&k==1)printf("|+++++O++++|");
                        else if(i==9&&k==1)printf("|++++O+++++|");
                        else if(i==10&&k==1)printf("|+++OOOO+++|");

                        else if(i==6&&k==2)printf("|+++++O++++|");
                        else if(i==7&&k==2)printf("|++++OO++++|");
                        else if(i==8&&k==2)printf("|+++++O++++|");
                        else if(i==9&&k==2)printf("|+++++O++++|");
                        else if(i==10&&k==2)printf("|++++OOO+++|");
                        else
                        {

                            for(j=0; j<12; j++)
                            {
                                setColor(NYX_COLOR_WHITE);
                                printf("%c",tetris_tmp[0][i][j]);
                            }

                        }
                        printf("\n");
                    }
                    sleep(1);

                }
                pthread_cond_signal(&lock->take1);
                pthread_cond_signal(&lock->take2);
            }
        }
        else if(info[0].state==MULTI)
        {
            pthread_mutex_lock(&lock->lock3);
            pthread_cond_wait(&lock->give,&lock->lock3);
            pthread_mutex_unlock(&lock->lock3);


            write(cfd,info[0].id,100);
            tmp_state=info[0].state;
            tmp_score=info[0].score;

            htonl(tmp_state);
            write(cfd,(char *)&tmp_state,4);
            ntohl(tmp_state);

            htonl(tmp_score);
            write(cfd,(char *)&tmp_score,4);
            ntohl(tmp_score);

            write(cfd,tetris_tmp[0],22*12);

            htonl(player_no);
            read(cfd,(char *)&player_no,4);
            ntohl(player_no);

            //printf("player no : %d\n",player_no);


            for(i=1;i<player_no;i++)
            {
                read(cfd,info[i].id,100);
                htonl(tmp_score);
                read(cfd,(char *)&tmp_score,4);
                ntohl(tmp_score);
                info[i].score=tmp_score;
            }
            for(i=1;i<player_no;i++)
            {
                read(cfd,tetris_tmp[i],22*12);
            }
            pthread_cond_signal(&lock->take1);
            pthread_cond_signal(&lock->take2);
        }
        else if(key_changed==KEY_CHANGED_QUIT)
        {
            //write ~~~~
            pthread_exit(0);
        }
        else if(info[0].state==GAME_OVER)
        {
            write(cfd,info[0].id,100);
            tmp_state=info[0].state;
            tmp_score=info[0].score;

            htonl(tmp_state);
            write(cfd,(char *)&tmp_state,4);
            ntohl(tmp_state);

            htonl(tmp_score);
            write(cfd,(char *)&tmp_score,4);
            ntohl(tmp_score);

            write(cfd,tetris_tmp[0],22*12);

            htonl(player_no);
            read(cfd,(char *)&player_no,4);
            ntohl(player_no);
            for(i=1;i<player_no;i++)
            {
                read(cfd,tetris_tmp[i],22*12);
            }
            printScreen();
            if(player_no==-1)
            {
                set_disp_mode(1);
                key_changed=KEY_CHANGED_QUIT;
            }
        }
    }
}
void *UI_Work(void *arg)
{
    //Lock *q=(Lock *)arg;
    int ml,mr,md,px,py,ps,pr;
    int i,j,k;
    int flag=0;
    int flag_line;
    int score_count;
    printf("\n");
    //if(info[0].state==WAIT_GAME||info[0].state==READY)
    pthread_mutex_lock(&lock->lock1);
    pthread_cond_wait(&lock->take1,&lock->lock1);
    pthread_mutex_unlock(&lock->lock1);
    while(1)
    {


        usleep(100*MSEC);



        if(key_changed == KEY_CHANGED_QUIT||info[0].state==GAME_OVER)
        {
            pthread_exit(0);
        }
        else if(key_changed == KEY_CHANGED_OK)
        {
            key_changed = KEY_CHANGED_NONE;
            printScreen();
        }
        else
        {
            usleep(100*MSEC);
            if(key_changed == KEY_CHANGED_QUIT)
            {
                pthread_exit(0);
            }
            if(md>=20||flag)
            {
                for(i=posY; i<posY+4; i++)
                {
                    for(j=posX; j<posX+4; j++)
                    {
                        if(brick[shapeNo][rotationNo][i-posY][j-posX]==1)
                        {
                            tetris[i][j]='O';
                        }
                    }
                }
                score_count=0;
                for(i=1;i<21;i++)
                {
                    flag_line=1;
                    for(j=1;j<11;j++)
                    {
                        if(tetris[i][j]!='O')flag_line=0;
                    }

                    if(flag_line)
                    {
                        for(k=i;k>1;k--)
                        {
                            for(j=1;j<11;j++)
                            {
                                tetris[k][j]=tetris[k-1][j];
                            }
                        }
                        for(j=1;j<11;j++)
                        {
                            tetris[1][j]='+';
                        }
                        score_count++;
                    }
                }
                j=score_count;
                for(i=0;i<j;i++)score_count*=2;
                info[0].score+=score_count*5;

                shapeNo=shp_arr[0];
                shp_arr[0]=shp_arr[1];
                shp_arr[1]=shp_arr[2];
                shp_arr[2]=rand()%7;
                rotationNo=0;
                posX=5;
                posY=0;
                flag=0;
                md=0;

            }
            else
            {
                md=1;
                mr=1;
                ml=21;

                for(i=posY; i<posY+4; i++)
                {
                    for(j=posX; j<posX+4; j++)
                    {
                        if(brick[shapeNo][rotationNo][i-posY][j-posX]==1)
                        {
                            if(i>md)md=i;
                            if(j>mr)mr=j;
                            if(j<ml)ml=j;
                            if(tetris[i+1][j]=='O')flag=1;
                        }

                    }
                }

                if(md < 20&&!flag)
                {
                    posY++;
                    md++;
                }
                key_changed = KEY_CHANGED_NONE;
                printScreen();
            }
        }
        if(info[0].state==PAUSE)
        {
            cmdScreen();
        }

        pthread_cond_signal(&lock->give);

        pthread_mutex_lock(&lock->lock1);
        if(info[0].state!=SINGLE_PLAY&&info[0].state!=GAME_OVER)pthread_cond_wait(&lock->take1,&lock->lock1);
        pthread_mutex_unlock(&lock->lock1);
    }
}

void *Key_Work(void *arg)
{
    char key;
    int ml,mr,md,px,py,ps,pr;
    int i,j;
    int tmp;
    int flagd=0,flagl=0,flagr=0;
    //Lock *q=(Lock *)arg;

    set_disp_mode(0);
    //if(info[0].state==WAIT_GAME||info[0].state==READY)
    pthread_mutex_lock(&lock->lock2);
    pthread_cond_wait(&lock->take2,&lock->lock2);
    pthread_mutex_unlock(&lock->lock2);
    while(1)
    {


        usleep(20*MSEC);



        if(info[0].state==GAME_OVER||key_changed==KEY_CHANGED_QUIT)
        {
            pthread_exit(0);
        }
        scanf("%c",&key);
        if( key == 'q' )
        {
            key_changed = KEY_CHANGED_QUIT;
            set_disp_mode(1);
            pthread_exit(0);
        }
        else
        {
            md=1;
            mr=1;
            ml=21;
            flagd=0;
            flagr=0;
            flagl=0;

            for(i=posY; i<posY+4; i++)
            {
                for(j=posX; j<posX+4; j++)
                {
                    if(brick[shapeNo][rotationNo][i-posY][j-posX]==1)
                    {
                        if(i>md)md=i;
                        if(j>mr)mr=j;
                        if(j<ml)ml=j;
                        if(tetris[i+1][j]=='O')flagd=1;
                        if(tetris[i][j+1]=='O')flagr=1;
                        if(tetris[i][j-1]=='O')flagl=1;
                    }
                }
            }
            if(flagd)continue;
            px=posX;
            py=posY;
            ps=shapeNo;
            pr=rotationNo;

            if(key == 'w')
            {
                key_changed = KEY_CHANGED_OK;
                if(rotationNo==3)rotationNo=0;
                else rotationNo++;

                for(i=posY; i<posY+4; i++)
                {
                    for(j=posX; j<posX+4; j++)
                    {
                        if(brick[shapeNo][rotationNo][i-posY][j-posX]==1&&tetris[i][j]=='O')
                        {
                            if(rotationNo==0)rotationNo=3;
                            else rotationNo--;
                            KEY_CHANGED_NONE;
                            break;
                        }
                    }
                }


                md=1;
                mr=1;
                ml=21;

                for(i=posY; i<posY+4; i++)
                {
                    for(j=posX; j<posX+4; j++)
                    {
                        if(brick[shapeNo][rotationNo][i-posY][j-posX]==1)
                        {
                            if(i>md)md=i;
                            if(j>mr)mr=j;
                            if(j<ml)ml=j;
                        }
                    }
                }

                if(ml<1)posX++;
                if(mr>10)posX--;
                if(md>19)posY--;


            }
            else if(key == 'a')
            {
                key_changed = KEY_CHANGED_OK;
                if(ml > 1&&!flagl)
                {
                    posX--;
                }
            }
            else if(key == 's')
            {
                key_changed = KEY_CHANGED_OK;
                if(md < 19&&!flagd)
                {
                    posY++;
                    md++;
                }
            }
            else if(key == 'd')
            {
                key_changed = KEY_CHANGED_OK;
                if(mr < 10&&!flagr)
                {
                    posX++;
                }
            }
            else if(key == ' ')
            {
                key_changed = KEY_CHANGED_OK;
                while(1)
                {
                    md=1;
                    mr=1;
                    ml=21;

                    for(i=posY; i<posY+4; i++)
                    {
                        for(j=posX; j<posX+4; j++)
                        {
                            if(brick[shapeNo][rotationNo][i-posY][j-posX]==1)
                            {
                                if(i>md)md=i;
                                if(j>mr)mr=j;
                                if(j<ml)ml=j;
                                if(tetris[i+1][j]=='O')flagd=1;
                            }
                        }
                    }
                    if(flagd)break;
                    if(md>19)break;
                    posY++;


                }

            }
            else if(key == '\n')
            {
                key_changed = KEY_CHANGED_OK;
                if(shapeNo == 6)
                    shapeNo = 0;
                else
                    shapeNo++;
            }
            else if(key=='e')
            {
                key_changed = KEY_CHANGED_OK;
                if(save_sh==-1)
                {
                    save_sh=shapeNo;
                    shapeNo=shp_arr[0];
                    shp_arr[0]=shp_arr[1];
                    shp_arr[1]=shp_arr[2];
                    shp_arr[2]=rand()%7;
                }
                else
                {
                    tmp=save_sh;
                    save_sh=shapeNo;
                    shapeNo=tmp;
                }
                posX=5;
                posY=1;
                rotationNo=0;

            }
            else if(key==ESC)
            {
                info[0].state=PAUSE;
            }
            else
                continue;




        }
        pthread_mutex_lock(&lock->lock2);
        if(info[0].state!=SINGLE_PLAY&&info[0].state!=GAME_OVER)pthread_cond_wait(&lock->take2,&lock->lock2);
        pthread_mutex_unlock(&lock->lock2);
    }
}
Lock *qinit()
{
    Lock *q;
    int i;
    q = (Lock *) malloc(sizeof(Lock));
    pthread_mutex_init(&q->lock1, NULL);
    pthread_mutex_init(&q->lock2, NULL);
    pthread_mutex_init(&q->lock3, NULL);
    pthread_cond_init(&q->give, NULL);
    pthread_cond_init(&q->take1, NULL);
    pthread_cond_init(&q->take2, NULL);
    return q;
}
void qdelete(Lock *q)
{
    pthread_mutex_destroy(&q->lock1);
    pthread_mutex_destroy(&q->lock2);
    pthread_mutex_destroy(&q->lock3);

    pthread_cond_destroy(&q->take1);
    pthread_cond_destroy(&q->take2);
    pthread_cond_destroy(&q->give);

    free(q);
}

