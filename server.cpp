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
#include <string>
#include <sstream>
#include <vector>
#include <sqlite3.h>

#define VERSION 23
#define BUFSIZE 8096
#define ERROR      42
#define LOG        44
#define FORBIDDEN 403
#define NOTFOUND  404
#define FILENAME "config.conf"
#define MAXBUF 102
#define DELIM "="
static char buffer[BUFSIZE + 1]; /* static so zero filled */

using namespace std;

static int callback(void *data, int argc, char **argv, char **azColName);
int login_auth(char *un, char *pw);
void join_table(int);
void send_and_close_socket(int);
#define DB "database.db"

bool isOpenDB = false;
sqlite3 *dbfile;

int create_user(char *un, char *pw);
bool ConnectDB();
void DisonnectDB();

/* User structure */
struct user {
	string name;
	int id;
	float balance;
	user * next;
};

/* Structure for saving tables */
struct table {
	int id;
	int no_of_users;
	user * users;
	vector<int> table;
};

table * tables;

/* structure define for reading the configuration file*/
struct config {
	char port[MAXBUF];
	char getcmd[MAXBUF];
	//char path[MAXBUF];
};

/* medhod which reads the configuration file*/
struct config get_config(char *filename) {
	struct config configstruct;
	FILE *file = fopen(filename, "r");

	if (file != NULL) {
		char line[MAXBUF];
		int i = 0;

		while (fgets(line, sizeof(line), file) != NULL) {
			char *cfline;
			cfline = strstr((char *) line, DELIM);
			cfline = cfline + strlen(DELIM);

			if (i == 0) {
				memcpy(configstruct.port, cfline, strlen(cfline));
				printf("%s", configstruct.port);
			} else if (i == 1) {
				memcpy(configstruct.getcmd, cfline, strlen(cfline));
				//printf("%s",configstruct.ccserver);
			}
			//		else if (i == 2) {
//				memcpy(configstruct.path, cfline, strlen(cfline));
//			}

			i++;
		} // End while
	} // End if file

	fclose(file);

	return configstruct;

}
//////////////////////////////////////////////////////////////////////////////////

pthread_mutex_t mymutex = PTHREAD_MUTEX_INITIALIZER;

static int good_requests,
bad_requests;

static long total_filesize;
void handle_request(int socketfd);
void system_details();

void init_casino() {
	table * temp, *current = NULL;
	for (int i = 0; i < 3; i++) {

		temp = new table;
		if (tables == NULL) {
			tables = temp;
			tables->next = NULL;
			current = tables;
		} else {
			temp->next = NULL;
			current->next = temp;
			current = temp;
		}

	}
}

//load all users(struct user link list) and their info from text file
void load_user_data() {

}

//search for this id. Return pointer to that node if present otherwise create new node
user* search_or_create_user(int user_id) {

}
//search for available tables and return their ids in an array
int* search_available_tables() {

}

void join_table(int table_no) {
//search if already playing on any other table
// look if the requested table is free and add
}

void create_table() {

}

void hit(int user_id) {

}

void stand(int user_id) {

}

void leave_table() {

}

int main(int argc, char **argv) {
//pid_t pid;
	int pnum = 1, status;
	pthread_t threads[100];
	int i, port, pid, listenfd, socketfd, hit;
	socklen_t length;
	struct sockaddr_in cli_addr; /* static = initialised to zeros */
	struct sockaddr_in serv_addr; /* static = initialised to zeros */
	struct config configstruct;/*read the config file*/

	configstruct = get_config(FILENAME);
	port = atoi(configstruct.port);

	//for(i=0;i<32;i++)
	//	(void)close(i);		/* close open files */
	(void) setpgrp(); /* break away from process group */

	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	if (port < 0 || port > 60000)
		printf("Invalid port number (try 1->60000)");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	if (bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		printf("system call bind error\n");
//		cout<<"system call bind error"<<endl;
		exit(0);
	}

	if (listen(listenfd, 64) < 0) {
		printf("system call listen error");
		exit(0);
	}
	cout << "\n              Server is Listening" << endl;

	for (i = 1;; i++) {
		length = sizeof(cli_addr);
		if ((socketfd = accept(listenfd, (struct sockaddr *) &cli_addr, &length))
				< 0) {
			printf("system call accept error");
			exit(0);
		}

		if (pthread_create(&threads[i], NULL,
				(void * (*)(void *))handle_request, (void *)socketfd) < 0)
		printf("Error creating thread");

		sleep(1);
	}

}

void handle_request(int socketfd) {

	int j, file_fd, buflen;
	long i, ret, len;
	char * fstr;
	char * file_folder;
	char folder_path[100];

	ret = read(socketfd, buffer, BUFSIZE); /* read Web request in one go */
	if (ret == 0 || ret == -1) { /* read failure stop now */
		printf("failed to read browser request");
		pthread_mutex_lock(&mymutex);
		bad_requests += 1;
		pthread_mutex_unlock(&mymutex);
		exit(0);
	}

	//printf("New thread called");
	if (ret > 0 && ret < BUFSIZE) /* return code is valid chars */
		buffer[ret] = 0; /* terminate the buffer */
	else
		buffer[0] = 0;
	for (i = 0; i < ret; i++) { /* remove CF and LF characters */

		if (buffer[i] == '\r' || buffer[i] == '\n')
			buffer[i] = '*';
	}
	if (strncmp(buffer, "GET ", 4) && strncmp(buffer, "get ", 4)) {
		printf("Only simple GET operation supported:%s", buffer);
	}

	for (i = 4; i < BUFSIZE; i++) { /* null terminate after the second space to ignore extra stuff */
		if (buffer[i] == ' ') { /* string is "GET URL " +lots of other stuff */
			buffer[i] = 0;
			break;
		}
	}

	/*
	 create various parser for different requests
	 if (strncmp(&buffer[0], "GET /join-table\0", 9) == 0 || strncmp(&buffer[0], "get /join_table\0", 9) == 0 ){

	 send_and_close_socket(socketfd);
	 }
	 */

	/* Check Login Details */
	if (strncmp(&buffer[0], "GET /login_auth\0", 9) == 0
			|| strncmp(&buffer[0], "get /login_auth\0", 9) == 0) {
		char *result = strtok(buffer, "?=&");
		/*		result = strtok(NULL, "?=");
		 result = strtok(NULL, "&=");*/
		char *username;	// = result;
		char *password;

		while (result) {

			if (strcmp(result, "un") == 0) {
				username = strtok(NULL, "?=&");
			} else if (strcmp(result, "pw") == 0) {
				password = strtok(NULL, "?=&");

			}
			result = strtok(NULL, "?=&");
		}

		cout << "\nUsername = " << username;
		cout << "\nPassword = " << password;
		int res = login_auth(username, password);
		if(res == 1){
			(void) strcpy(buffer, "GET /home.html");
		}
	}

	else if (strncmp(&buffer[0], "GET /join-table\0", 9) == 0
			|| strncmp(&buffer[0], "get /join_table\0", 9) == 0) {

		send_and_close_socket(socketfd);
	} else if (!strncmp(&buffer[0], "GET /\0", 6) || !strncmp(&buffer[0], "get /\0", 6)) /* convert no filename to index file */{
		(void) strcpy(buffer, "GET /index.html");
		pthread_mutex_lock(&mymutex);
		good_requests += 1;
		pthread_mutex_unlock(&mymutex);
	}
	buflen = strlen(buffer);
	fstr = (char *) 0;

	string extension = "html";

	len = strlen(extension.c_str());
	if (!strncmp(&buffer[buflen - len], extension.c_str(), len)) {
		fstr = "text/html";
		//break;
	}

	int file_folder_len = 0;	// = strlen(file_folder);
	extension = "jpg";
	len = strlen(extension.c_str());
	if (!strncmp(&buffer[buflen - len], extension.c_str(), len)) {
		fstr = "/httpd/images/jpeg";
		file_folder = "httpd/images/";

		file_folder_len = strlen(file_folder);
		//break;
	}
	extension = "gif";
	len = strlen(extension.c_str());
	if (!strncmp(&buffer[buflen - len], extension.c_str(), len)) {
		fstr = "/httpd/images/gif";
		file_folder = "httpd/images/";
		file_folder_len = strlen(file_folder);
	}

	extension = "png";
	len = strlen(extension.c_str());
	if (!strncmp(&buffer[buflen - len], extension.c_str(), len)) {
		fstr = "/httpd/images/png";
		file_folder = "httpd/images/";
		file_folder_len = strlen(file_folder);
	}

	if (file_folder_len != 0) {
		for (i = 0; i < file_folder_len; i++) {
			folder_path[i] = file_folder[i];
		}

		int k = 0;

		for (int j = file_folder_len, k = 5; j < file_folder_len + buflen - 5;
				j++, k++) {

			folder_path[j] = buffer[k];

		}
		cout << "\n";

	}
	if (fstr == 0) {
		pthread_mutex_lock(&mymutex);
		bad_requests += 1;
		pthread_mutex_unlock(&mymutex);
		printf("file extension type not supported:%s", buffer);
	}

	if (file_folder_len == 0) {

		if ((file_fd = open(&buffer[5], O_RDONLY)) == -1) {
			write(socketfd,
					"HTTP/1.1 404 Not Found\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",
					280);

		}
	} else {
		if ((file_fd = open(folder_path, O_RDONLY)) == -1) {
			write(socketfd,
					"HTTP/1.1 404 Not Found\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",
					280);

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

	len = (long) lseek(file_fd, (off_t) 0, SEEK_END); /* lseek to the file end to find the length */
	(void) lseek(file_fd, (off_t) 0, SEEK_SET); /* lseek back to the file start ready for reading */
	(void) sprintf(buffer,
			"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n",
			VERSION, len, fstr); /* Header + a blank line */

	(void) write(socketfd, buffer, strlen(buffer));

	/* send file in 8KB block - last block may be smaller */
	while ((ret = read(file_fd, buffer, BUFSIZE)) > 0) {
		(void) write(socketfd, buffer, ret);

	}
	sleep(1); /* allow socket to drain before signalling the socket is closed */
	close(socketfd);
	pthread_exit(NULL);
}

void send_and_close_socket(int socketfd) {

	//printf("Buffer %d", buffer[16]);
	int tableNo = (int) buffer[16] - 48;
	cout << "Table No:" << tableNo << endl;

	(void) sprintf(buffer,
			"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"a\":1,\"b\":2}\n",
			VERSION, 14, "text/html"); /* Header + a blank line */

	(void) write(socketfd, buffer, strlen(buffer));

	sleep(1); /* allow socket to drain before signalling the socket is closed */
	close(socketfd);
	pthread_exit(NULL);
}

int login_auth(char *un, char *pw) {

	string s;
	vector<string> data;
	isOpenDB = ConnectDB();
	char buff[1024];
	sqlite3_stmt *statement;
	sprintf(buff,
			"SELECT id, balance from users where username = '%s' and password = '%s';",
			un, pw);
	cout << "\n query:" << buff;
	char *query = buff;

	if (sqlite3_prepare(dbfile, query, -1, &statement, 0) == SQLITE_OK) {
		int ctotal = sqlite3_column_count(statement);
		int res = 0;

		while (1)

		{
			res = sqlite3_step(statement);

			if (res == SQLITE_ROW) {
				for (int i = 0; i < ctotal; i++) {
					data.push_back((char*) sqlite3_column_text(statement, i));
				}

				cout << endl;
			}

			if (res == SQLITE_DONE) {
				//cout << "\nSize:" << data.size();
				for (size_t n = 0; n < data.size(); n++) {
					cout << "\ndata:" << data[n] << " ";
					cout << endl;
				}

				if (data.size() == 0) {
					cout << "\nInvalid User Details, creating new user";
					//create_user(un, pw);
					return 0;
				}
				break;
			}

		}

		return 1;
	}
}

int create_user(char *un, char *pw) {
	isOpenDB = ConnectDB();
	int balance = 1000;
	std::stringstream strm;
	strm << "insert into users(username,password,balance) values(" << un << ",'"
			<< pw << "'," << balance << ")";

	string s = strm.str();
	char *str = &s[0];

	sqlite3_stmt *statement;
	int result;
	char *query = str;
	{
		if (sqlite3_prepare(dbfile, query, -1, &statement, 0) == SQLITE_OK) {
			int res = sqlite3_step(statement);
			result = res;
			sqlite3_finalize(statement);
		}
		return result;
	}

	return 0;
}

bool ConnectDB() {
	if (sqlite3_open(DB, &dbfile) == SQLITE_OK) {
		isOpenDB = true;
		cout << "connection done";
		return true;
	}

	return false;
}

void DisonnectDB() {
	if (isOpenDB == true) {
		sqlite3_close(dbfile);
	}
}
/*

 int login_auth(char *un, char *pw){

 sqlite3 *db;
 char *zErrMsg = 0;
 int rc;
 char *sql;
 char buff[1024];
 const char* data = "Callback function called";

 Open database
 rc = sqlite3_open("database.db", &db);
 if( rc ){
 fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
 exit(0);
 }else{
 cout << "\nasdfasd";
 fprintf(stderr, "Opened database successfully\n");
 Create SQL statement
 sprintf(buff, "SELECT id, balance from users where username = '%s' and password = '%s';", un, pw);

 Execute SQL statement
 rc = sqlite3_exec(db, buff, callback, (void*)data, &zErrMsg);

 if( rc != SQLITE_OK ){
 fprintf(stderr, "SQL error: %s\n", zErrMsg);
 sqlite3_free(zErrMsg);
 }else{

 fprintf(stdout, "Operation done successfully\n");

 }
 sqlite3_close(db);
 return 0;

 }
 }

 static int callback(void *data, int argc, char **argv, char **azColName){
 int i;
 fprintf(stderr, "%s: ", (const char*)data);
 for(i=0; i<argc; i++){
 printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
 }
 printf("\n");
 return 0;
 }
 */
