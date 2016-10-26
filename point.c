#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define DELIM_STR " |<>"

struct cmd_t{
    char name[256];
    char para[10][256];
    char ppipe[10];
    char redir_s;        // redirection sign '>, >>, <'
    char redirection[10];
};

int pp_cmd(struct cmd_t *cmd){
    int i = 0;
    //printf("cmd=%s", cmd->para)
    printf("cmd.name=%s\n", (cmd->para)[0]);
    printf("cmd.para:\n");
    for(i=1; i<10; i++)
        printf("%s\n", (cmd->para)[i]);

    return 0;
}

//int pp_buf(int bufc, char **pbuf){
int pp_buf(int bufc, char *pbuf[4]){
    int i;

    for(i=0; i<bufc; i++){
        printf("buf[%d][0]=%c\n", i, pbuf[i][0]);
    }

    return 0;
}

int split_str(const char *src, struct cmd_t *result, char *delim){
    int r, c;
    char t;

    r = 0, c = 0;
    memset(result, 0, sizeof(struct cmd_t));
    t = *src;
    while(*src){
        if(NULL == strchr(delim, *src)){
            (result->para)[r][c++] = *src;
        }
        else{
            if(' '==*src && t==*src);
            //(result->para)[r++][c] = 0;
            //else if(*src=='<' or *src=='>')
            else c = 0, r++;
            //(result->para)[r][c++] = *src++;
        }
        t = *src++;
    }

    return 0;
}

int deal_redirct(char sign, char *file, char *cmd, char **parav){
    int access = -1;
    int oldfd, newfd;
    pid_t pid;

    if(sign == '>'){
        access = O_WRONLY|O_CREAT;
        newfd = STDOUT_FILENO;
    }
    else if(sign == '<'){
        access = O_RDONLY;
        newfd = STDIN_FILENO;
    }
    else
        ;
    pid = fork();
    if(pid < 0) exit(1);
    else if(pid == 0){
        //execv(cmd, parav);
        if(0 > (oldfd=open(file, access))){
            printf("open file %s faild\n", file);
            exit(1);
        }
        dup2(oldfd, newfd);

        execl("/bin/ls","ls","-l",NULL);
        printf("execv error!\n");
    }
    else{
        wait(NULL);
        printf("wait child over!\n");

    }

    return 0;
}

int deal_pipe(char *fcmd, char **fparav, char *bcmd, char **bparav){
    int fd[2];
    int pipe_s = 2;
    int i = 0;
    pid_t pid;

    if(0>pipe(fd)){
        printf("pipe is error!\n");
        exit(1);
    }
    //for(i=0; i<pipe_s; i++){ }
    pid = fork();
    if(pid==0){
        close(fd[0]);
        dup2(fd[1], STDOUT_FILENO);
        execl("/bin/ls","ls","-l",NULL);
        printf("child error!\n");
    }
    close(fd[1]);
    pid = fork();
    if(pid==0){
        close(fd[1]);
        dup2(fd[0], STDIN_FILENO);
        execl("/usr/bin/wc","wc","-l",NULL);
        printf("child error!\n");
    }
    close(fd[0]);
    while((pid = wait(NULL)) > 0 ){
        printf("pid = %d\n", pid);
    }

    return 0;
}


int main(int argc, char *argv[]){
    //int i, ret;
    //char buf[3][4] = {{'a', 'b', 'c'},
    //                  {'d', 'd', 'e'},
    //                  {'f', 'g', 'h'}};
    //char *sbuf[3] = {"abc", "cd", "ef"};
    //char (*pbuf)[4] = &buf[0];
    //char *p_buf[3];
    //p_buf[0]    = &buf[0];
    //p_buf[1]    = &buf[1];
    //p_buf[2]    = &buf[2];

    // for(i=0; i<argc; i++){
    //     printf("argv[%d][0]=%c\n", i, argv[i][0]);
    // }
    //ret = pp_buf(3, (char **)buf);
    //ret = pp_buf(3, p_buf);
    char buf[256] = {0};
    struct cmd_t cmd = {0};
    char *parav[] = {"-l", "-a"};

    while(1){
        if(0>fputs("my@shell# ", stdout)) exit(1);
        if(NULL != fgets(buf, 255, stdin)){
            split_str(buf, &cmd, DELIM_STR);
            //pp_cmd(&cmd);
            //deal_redirct('>', "test.sl", "ls", parav);
            deal_pipe("ls", NULL, "wc", NULL);
        }
    }

    return 0;
}
