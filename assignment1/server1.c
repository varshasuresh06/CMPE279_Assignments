// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pwd.h>

#define PORT 80
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Show ASLR
    printf("execve=0x%p\n", execve);

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

   // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

// forking to create child process
    pid_t child = fork();

    if (child == 0) {
    // get nobody user id
      if(getpwnam("nobody") == NULL){
        printf("cannot find the nobody user");
      } else {
        uid_t nobody_uid = getpwnam("nobody")->pw_uid;
        printf("Nobody user Id for this system: %ld\n", (long int)nobody_uid);
        // dropping privileges
        setuid(nobody_uid);

        valread = read(new_socket, buffer, 1024);
        printf("%s\n",buffer );
        send(new_socket , hello , strlen(hello) , 0 );
        printf("Hello message sent\n");
        exit(0);
      }
    } else {
        int returnStatus;    
        waitpid(child, &returnStatus, 0);
        // Verify child process terminated without error. 
        printf("Returned to parent process after waiting for child to terminate.\n"); 
        if (returnStatus == 0){
        printf("The child process terminated normally.\n");    
        }
        if (returnStatus == 1){
       printf("The child process terminated with an error!.\n");    
        }
    }


    
    return 0;
}
