#include <stdio.h>
#include <stdlib.h>
#include <string.h>//for building HTTP response from the server
#include <netinet/in.h>//imports the structs

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

int port = 0;//will store the port number
char* path;//store the path

void load_Config()
{
    FILE *confg;//loasds the config.txt file
    char *line = NULL;
    size_t len = 0;
    ssize_t read = 0;
    confg = fopen("config.txt", "r");//read only

    if(confg == NULL)
        exit(EXIT_FAILURE);
    
    while((read = getline(&line, &len, confg)) != -1)//readling line by line
    {
        printf("line =  %s", line);
        if(strstr(line, "PORT = ") != NULL)//strstr returns pointer tp the first character of "PORT="
        {
            //printf("Port number has been read\n");//debugging to check if condition is entered
            port = atoi(line + 7);//string to integer
        }
        if(strstr(line, "PATH = ") != NULL)//strstr returns pointer tp the first character of "PATH
        {
            //printf("Path number has been read\n");//debugging to check if condition is entered
            path = line + 7;
        }
    }
    fclose(confg);
        printf("config.txt has been read from successfully\n PORT NUMBER: %d\n PATH: %s\n", port, path);

}


int main()
{
    load_Config();
    FILE *html_data;//holds the data
    path[strlen(path) - 1] = '\0';
    html_data = fopen(strcat(path, "index.html"), "r");//since we are only reading from the file
    //printf("html data: %s\n", html_data);
    if(html_data == NULL)
    {
        printf("Couldn't open index.html\n");
        exit(EXIT_FAILURE);
    }

    char buffer[128];
    char response_data[2048] = {'\0'};//stores the HTML of the response, '\0' ensures no garbage value
    while(fgets(buffer, sizeof(buffer), html_data))//reads the html_data file to response_data
    {
        strcat(response_data, buffer);
    }
    
    printf("Respone data: %s\n", response_data);

    char http_header[4096] = "HTTP/1.1 200 OK\r\n\n";//Lets the client know that response for the request is OK.
    strcat(http_header, response_data);

    printf("Header: %s\n", http_header);

    //creating a server socket
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, 0);//same as TCP since HTTP is built on TCP.

    //defining the address
    struct sockaddr_in server_address;//defines where are server is going to server the data
    int server_address_len = sizeof(server_address);
    server_address.sin_family = AF_INET;//IPv4
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY;

    memset(server_address.sin_zero, '\0', sizeof(server_address.sin_zero));

    //bind the socket to the port
    if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Bind Error");
        exit(EXIT_FAILURE);
    }

    //listen for connections on the socket
    if(listen(server_socket, 5) < 0)//5 max at a time
    {
        perror("Listen Error");
        exit(EXIT_FAILURE);
    }
    //placeholder for the client socket
    int client_socket;
    while(1)
    {
        printf("***LOOKING FOR A CONNECTION***\n");//being used for debugging(checks if code is entering while loop)
        if((client_socket = accept(server_socket, (struct sockaddr *)&server_address, (socklen_t *)&server_address_len)<0))
        {
            perror("Accept error");
            exit(EXIT_FAILURE);
        }//we're not storing th address/port number
        send(client_socket, http_header, sizeof(http_header), 0);//sending the data to the client
        printf("Data sent\n");//used for debugging(confirms that data was sent)
        close(client_socket);
        printf("Socket closed\n");//used for debugging(confirms that socket was closed)
    }
    return 0;
}

