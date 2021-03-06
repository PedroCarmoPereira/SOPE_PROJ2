    #include <stdio.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <stdlib.h>
    #include <pthread.h>
    #include <limits.h>
    #include <errno.h>
    #include <sys/wait.h>
    #include "string.h"
    #include "constants.h"
    #include "types.h"
    #include "sope.h"
    #include "hashsum.h"

    #define READ 0
    #define WRITE 1
    #define MAXLINE 1000

    void err_sys(char *msg);
    void err_msg(char *msg);

void hashGenerator(char *salt, char *password, char *hash){

        int fd1[2], fd2[2];
        pid_t pid;
        if (pipe(fd1) < 0 || pipe(fd2) < 0)
            err_sys("pipe error");
        if ((pid = fork()) < 0)
            err_sys("fork error");

        if (pid == 0) /* child 1 */{
            close(fd2[WRITE]);
            close(fd1[READ]);
            dup2(fd2[READ], STDIN_FILENO);
            dup2(fd1[WRITE], STDOUT_FILENO);
            execlp("sha256sum", "sha256sum", NULL);
            exit(0);                
        }
        
        else /*parent*/{
            char temp_pass[HASH_LEN + MAX_PASSWORD_LEN + 1];
            strcpy(temp_pass, password);
            strcat(temp_pass, salt);
            close(fd1[WRITE]);
            close(fd2[READ]);
            write(fd2[WRITE], temp_pass, HASH_LEN);
            close(fd2[WRITE]);
            wait(NULL);
            read(fd1[READ], hash, HASH_LEN);
            hash[HASH_LEN] = '\0';
        }
}
            
void err_sys(char *msg){
    fprintf(stderr, "%s\n", msg);
    exit(1);
}

void err_msg(char *msg){
    printf("%s\n", msg);
    return;
}
