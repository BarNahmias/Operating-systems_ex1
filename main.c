#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
//#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

// #include <sys/types.h> 







char* input() {
    printf("%s","enter input: ");
    int count = 0;
    char c;
    char* str = (char*)malloc((count+1)*sizeof(char));
    char* cpy = str;
    while(scanf("%c",&c) && c != '\n'){
    count++;
    str = realloc(str, (count+1)*sizeof(char));
    if(!str){
    printf("Too long input");
    free(cpy);
    return NULL;
    }
    cpy = str;
    str[count-1] = c;
    }
    str[count] = '\0';
    return str;
}





int main(){

    int tcpOrLocal = 0;
    int sock = 0;
    char* str = "";
   



    /* part 2 */

    char cwd[256];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("getcwd() error");
        return 1;
    }

    while (1)
    {
        
        str = input();

                // boons (sort the string)

                int i = 0;
                char *p = strtok (str, " ");
                char *array[3];

                while (p != NULL)
                {
                    array[i++] = p;
                    p = strtok (NULL, " ");
                }

                for (i = 0; i < 3; ++i) 
                    printf("%s\n", array[i]);



        /* part 3*/

        char subbuff[10];
        memcpy( subbuff, &str[0], 4);
        subbuff[4] = '\0';

        char subbuffOpen[10];
        memcpy( subbuffOpen, &str[0], 2);
        subbuffOpen[2] = '\0';

        char subbufdel[10];
        memcpy( subbufdel, &str[0], 6);
        subbufdel[6] = '\0';

        if(!strcmp(subbuff, "ECHO")){
            if (tcpOrLocal == 0){
            printf("%s\n", &str[5]);
            }
            else{
                int messageLen = strlen(&str[5]) + 1;

                int bytesSent = send(sock, &str[5], messageLen, 0);
                if (bytesSent == -1)
                {
                    perror("failed to sending File\n");
                    exit(0);
                }
            }
            free(str);
        }
            
        else if(!strcmp(str, "TCP PORT")){
            tcpOrLocal = 1;

            /*open socket*/ 
            sock = socket(AF_INET, SOCK_STREAM, 0);
            if(sock == -1)
            {
                printf("Could not create socket");
            }

            struct sockaddr_in serverAddress;
            memset(&serverAddress, 0, sizeof(serverAddress));

            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(5000);
            serverAddress.sin_addr.s_addr = INADDR_ANY;

            if (connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) == -1)
            {
                printf("connect() failed with error code");
                exit(0);
            }


            free(str);
        } 
        

        else if(!strcmp(str, "LOCAL")){
            tcpOrLocal = 0;
 
            close(sock);
            free(str);
        } 
        

        
    

        /*part 6 print list of file form folder */

        else if(!strcmp(str, "DIR")){

            DIR *folder;
            struct dirent *entry;
            int files = 0;

            folder = opendir(".");
            if(folder == NULL)
            {
                perror("Unable to read directory");
                return(1);
            }

            while((entry=readdir(folder)) )
            {
                if((strcmp(entry->d_name,".")==0 || strcmp(entry->d_name,"..")==0 || (*entry->d_name) == '.' ))
                {

                }else{
                files++;
                printf("File %3d: %s\n",
                        files,
                        entry->d_name
                    );
                }
            }

            closedir(folder);
            free(str);

        }

        /*part 7 open folder  */
            
    
        else if(!strcmp(subbuffOpen, "CD")){
            
            if (chdir(&str[3]) != 0) {
                perror("chdir() to /usr failed");
            }
            free(str);
        }

        else if(!strcmp(str, "EXIT")){
            break;
        }
        
        /*part 10 copy file src dst 
         // This is a C library function !*/

        else if(!strcmp(str, "COPY")){

            FILE *fptr1, *fptr2;
            char filename[100], c;

            printf("Enter the filename to open for reading \n");
            scanf("%s", filename);

            // Open one file for reading
            fptr1 = fopen(filename, "r");
            if (fptr1 == NULL)
            {
                printf("Cannot open file %s \n", filename);
                free(str);
                continue;
            }

            printf("Enter the filename to open for writing \n");
            scanf("%s", filename);

            // Open another file for writing
            fptr2 = fopen(filename, "w");
            if (fptr2 == NULL)
            {
                printf("Cannot open file %s \n", filename);
                free(str);
                continue;
            }

            // Read contents from file
            c = fgetc(fptr1);
            while (c != EOF)
            {
                fputc(c, fptr2);
                c = fgetc(fptr1);
            }

            printf("\nContents copied to %s\n", filename);

            fclose(fptr1);
            fclose(fptr2);
            free(str);
        }
        

        /*part 11 delete file  */

        else if(!strcmp(subbufdel, "DELETE")){

            if (unlink(&str[7]) == 0){
                    printf("\nDeleted successfully\n");
            }
            else{
                    printf("\nUnable to delete the file\n");
            }
            free(str);
        }
        
   


              /* boons */
            int pipe_fd[1];

           else if(!strcmp(array[1], "|"))
            {
                if(pipe(pipe_fd)){
                  perror("pipe");
                  return -1;
                }
                switch(fork()){
                    case -1:
                        perror("fork");
                        return -1;
                    case 0:
                        // child
                        close(pipe_fd[1]);
                        dup2(pipe_fd[0], STDIN_FILENO);
                        close(pipe_fd[0]);
                        execl("./log", NULL);
                    default:
                        // parent
                        close(pipe_fd[0]);
                        write(pipe_fd[1], str, sizeof(str));
                        close(pipe_fd[1]);
                        wait(NULL);
                }
                printf("END~\n");
                free(str);
            }



    

        /*part 8 use in system method */  
        // This is a C library function !
        // else{   
        //     system(str);
        //     free(str);
        // }
        
        /*part 9 use in fork, exec ,wait  */
        
        else{
            
            char command;
            char name[32];
            char *pointer;
            char *argv[]={str,NULL};


            if(&str[0]!=NULL){
                pointer = strtok(str, " ");
            }

            if(pointer != NULL){
                strcpy(str, pointer);
            }

            pid_t pid;

            int  status;

            if ((pid = fork()) < 0) {     
                printf("*** ERROR: forking child process failed\n");
                exit(1);
            }
            else if (pid == 0) {          
                if (execvp(str, argv) < 0) {     
                    printf("*** ERROR: exec failed\n");
                    exit(1);
                }
            }
            else                                  
            while (wait(&status) != pid)  ;
            free(str);
            
        } 

    }





            return 0;
            }