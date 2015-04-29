#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <sys/stat.h>
#include <pthread.h>
#define VERSION 23
#define BUFSIZE 8096
#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404
#define FILENAME "config.conf"
#define MAXBUF 102
#define DELIM "="

using namespace std;


/* structure define for reading the configuration file*/
 struct config
    {
    char port[MAXBUF];
    char getcmd[MAXBUF];
    };

/* medhod which reads the configuration file*/
struct config get_config(char *filename)
    {
    struct config configstruct;
    FILE *file = fopen (filename, "r");
     
    if (file != NULL)
    {
    char line[MAXBUF];
    int i = 0;
     
    while(fgets(line, sizeof(line), file) != NULL)
    {
    char *cfline;
    cfline = strstr((char *)line,DELIM);
    cfline = cfline + strlen(DELIM);
     
    if (i == 0){
    memcpy(configstruct.port,cfline,strlen(cfline));
    printf("%s",configstruct.port);
    } else if (i == 1){
    memcpy(configstruct.getcmd,cfline,strlen(cfline));
    //printf("%s",configstruct.ccserver);
    }
     
    i++;
    } // End while
    } // End if file
     
    fclose(file);
     
    return configstruct;
     
    }
 //////////////////////////////////////////////////////////////////////////////////   

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

static int good_requests,bad_requests;

static long total_filesize;
void handle_request(int socketfd);
void system_details();



int main(int argc, char **argv)
{
//pid_t pid;
	int pnum=1, status;
	pthread_t threads[100];
	int i, port , pid, listenfd, socketfd, hit;
	socklen_t length;
	 struct sockaddr_in cli_addr; /* static = initialised to zeros */
	 struct sockaddr_in serv_addr; /* static = initialised to zeros */
 	struct config configstruct;/*read the config file*/
     
         configstruct = get_config(FILENAME);	
	 port = atoi(configstruct.port);
	
	//for(i=0;i<32;i++)
	//	(void)close(i);		/* close open files */
	(void)setpgrp();		/* break away from process group */

	listenfd = socket(AF_INET, SOCK_STREAM,0);

	if(port < 0 || port >60000)
		printf("Invalid port number (try 1->60000)");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);



	if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
		{
		printf("system call bind error\n");
//		cout<<"system call bind error"<<endl;
		exit(0);
		}
	
	if( listen(listenfd,64) <0){
		printf("system call listen error");
		exit(0);
		}
	cout<<"\n              Server is Listening"                << endl;
	if(pthread_create(&threads[0], NULL, (void * (*)(void *))system_details,NULL) < 0)
	printf("Could not call system datils function");

	for(i=1; ;i++) {
		length = sizeof(cli_addr);
		if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0)
			{
			printf("system call accept error");
			exit(0);
			}


			if (pthread_create(&threads[i], NULL, (void * (*)(void *))handle_request, (void *)socketfd) < 0)
				printf("Error creating thread");

			sleep(1);
}

}


void handle_request(int socketfd)
{				

		int j, file_fd, buflen;
		long i, ret, len;
		char * fstr;
		char * file_folder;
		char folder_path[100];
		static char buffer[BUFSIZE+1]; /* static so zero filled */

		ret =read(socketfd,buffer,BUFSIZE); 	/* read Web request in one go */
		if(ret == 0 || ret == -1) {	/* read failure stop now */
			printf("failed to read browser request");
			pthread_mutex_lock(&mymutex); 
			bad_requests += 1;
			pthread_mutex_unlock(&mymutex);
			exit(0);
		}
		
		//printf("New thread called");
		if(ret > 0 && ret < BUFSIZE)	/* return code is valid chars */
		buffer[ret]=0;		/* terminate the buffer */
		else buffer[0]=0;
		for(i=0;i<ret;i++){	/* remove CF and LF characters */
			
			if(buffer[i] == '\r' || buffer[i] == '\n')
				buffer[i]='*';
		}
		if( strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4) ) {
		printf("Only simple GET operation supported:%s",buffer);
		}

		for(i=4;i<BUFSIZE;i++) { /* null terminate after the second space to ignore extra stuff */
		if(buffer[i] == ' ') { /* string is "GET URL " +lots of other stuff */
			buffer[i] = 0;
			break;
		}
		}


		if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6) ) /* convert no filename to index file */
		{				
			(void)strcpy(buffer,"GET /httpd/index.html");
			pthread_mutex_lock(&mymutex); 
			good_requests += 1;
			pthread_mutex_unlock(&mymutex);		
		}
		buflen=strlen(buffer);
		fstr = (char *)0;
		
		string extension = "html";
		//for(i=0;extensions[i].ext != 0;i++) {
		len = strlen(extension.c_str());
		if( !strncmp(&buffer[buflen-len], extension.c_str(), len)) {
			fstr = "text/html";
			//break;
		}

		int file_folder_len = 0;// = strlen(file_folder);
		extension = "jpg";
		len = strlen(extension.c_str());
		if( !strncmp(&buffer[buflen-len], extension.c_str(), len)) {
			fstr = "/httpd/image/jpeg";
			file_folder = "httpd/images/";
			
			file_folder_len = strlen(file_folder);


			for(i=0;i<file_folder_len;i++){	
				
				printf("%c", file_folder[i]);
				folder_path[i] = file_folder[i];
			}
			printf("buffer len = %d\n", buflen);
			int k = 0;
			cout << "\n buffer = ";
			for(int j=file_folder_len,k = 5; j<file_folder_len+buflen-5; j++,k++){	
			
				
				folder_path[j] = buffer[k];
				//printf("%c", buffer[k] );
				printf("%c", folder_path[j] );
			}
			cout << "\n";
			//break;
		}
		cout << "\n";
		for(i=0;i<25;i++){	
		
			printf("%c", folder_path[i] );
		}

		cout << "\n";
		for(i=0;i<25;i++){	
		
			printf("%c", buffer[i] );
		}
		extension = "gif";
		len = strlen(extension.c_str());
		if( !strncmp(&buffer[buflen-len], extension.c_str(), len)) {
			fstr = "/httpd/image/gif";
			//break;
		}
		//}
		
		if(fstr == 0){
			pthread_mutex_lock(&mymutex); 
			bad_requests += 1;
			pthread_mutex_unlock(&mymutex);		
			printf("file extension type not supported:%s",buffer);
		}
		

		if(file_folder_len == 0){

			if(( file_fd = open(&buffer[5],O_RDONLY)) == -1) {  
		         write(socketfd, "HTTP/1.1 404 Not Found\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",280);

			}
		}else{
			if(( file_fd = open(folder_path,O_RDONLY)) == -1) {  
		         write(socketfd, "HTTP/1.1 404 Not Found\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",280);

			}
		}
		
		struct stat filestatus;

		int rc = fstat(file_fd, &filestatus);
        if (rc < 0)
        	printf("error opening the file\n");
        fflush(stdout);
        pthread_mutex_lock(&mymutex);
        total_filesize += filestatus.st_size;
        pthread_mutex_unlock(&mymutex); 

		len = (long)lseek(file_fd, (off_t)0, SEEK_END); /* lseek to the file end to find the length */
	 	(void)lseek(file_fd, (off_t)0, SEEK_SET); /* lseek back to the file start ready for reading */
        (void)sprintf(buffer,"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n", VERSION,len, fstr); /* Header + a blank line */
	
		//logger(LOG,"Header",buffer,hit);
		(void)write(socketfd,buffer,strlen(buffer));

		/* send file in 8KB block - last block may be smaller */
		while (	(ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
		(void)write(socketfd,buffer,ret);
		
		}
		sleep(1);	/* allow socket to drain before signalling the socket is closed */
		close(socketfd);
		pthread_exit(NULL);
}

void system_details()
{
	while(1)
	{
		sleep(10);
		cout<<"\n======================================================\n";
		cout<<"Number of successful Requests:"<<good_requests<<endl;
		cout<<"Number of Un-successful Requests:"<<bad_requests<<endl;
		cout<<"Tolal size of the file accessed:"<<total_filesize<<endl;

		fflush(stdout);
	}
}

