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
#include <time.h> 
#include <fstream>

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

struct user {
	string name;
	string password;
	int id;
	float balance;
	int cards[13];
	int no_of_cards;
	user * next;
};

#define DB "database.db"

bool isOpenDB = false;
sqlite3 *dbfile;

int login_auth(char *un, char *pw);
void join_table(int);
int create_user(char *un, char *pw);
bool ConnectDB();
void DisonnectDB();
void send_and_close_socket(int, char *);
void shuffle_array(int *, int *);
int evaluate_hand(int);
void assign_table(user*);
void save_users();
void load_user_data();



struct deck{
	int card_img_no[52];
	int card_value[52];
	int index;
	int cards[13];
	int no_of_cards;
};

deck d1;
/* User structure */


user u1;
vector<user> user_data;
/* Structure for saving tables */
struct table {
	int id;
	int no_of_users;
	//user * users;
	vector<user> users;
	table * next;
	int cards[13];
	int no_of_cards;
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
	
	for (int i = 0; i < 1; i++) {

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
	
	// for(int i=0; i < 10; i++){
	// 	user u1;
	// 	u1.id = i+1;
	// 	u1.name = "varun";
	// 	u1.password = "pass";
	// 	u1.balance = 1000;
	// 	tables->users.push_back(u1);
	// }
	cout << "\nTable->user.size = " << tables->users.size();
	//save_users();
	load_user_data();
}

void load_user_data() {
	int i=0,	no_of_members=100;
	string filePath;
	ifstream  dimensionsInFile;

	dimensionsInFile.open ("users.dat");	
	cout << dimensionsInFile;											
	if (!dimensionsInFile)
	{
		cout<<"Cannot load file"<<endl;
		return ;
	}
	else
	{
		while(dimensionsInFile != NULL)
		{	// write struct data from file	
			dimensionsInFile>>
			user_data[i].id>>
			user_data[i].name>>
			user_data[i].password>>		
			user_data[i].balance;
			cout<<"\nMember no "<<i<<"loaded"<<endl;
			cout<< user_data[i].id<<
			user_data[i].name<<
			user_data[i].password<<		
			user_data[i].balance;
			i++;
			cout << "\n" << i<<endl;
			
		}
		cout <<"All members have been successfully loaded"<<endl;
		dimensionsInFile.close();
	}
}
	/*user {
	string name;
	int id;
	float balance;
	int cards[13];
	int no_of_cards;
	user * next;
	};*/


void save_users()
{
	int no,i_of_members=100;	
	string	filePath;
	ofstream  dimensionsOutfile;		
	dimensionsOutfile.open ("users.dat");												
	if (!dimensionsOutfile)
	{
		cout<<"Cannot load file"<<endl;
		return ;
	}
	else
	{
		for(int i = 0; i < user_data.size(); i++)
		{	// write struct data from file	

			dimensionsOutfile<<" "<<
			user_data[i].id<<" "<<
			user_data[i].name<<" "<<
			user_data[i].password<<	" "<<	
			user_data[i].balance<< "\n";
			cout<<"\n Member no "<<i<<"stored"<<endl;
		}
		cout <<"\nAll members have been successfully saved"<<endl;
	dimensionsOutfile.close();
}

}

//search for this id. Return pointer to that node if present otherwise create new node
user* search_or_create_user(int user_id) {

}
//search for available tables and return their ids in an array
int* search_available_tables() {

}

void assign_table(user* u1) {
//search if already playing on any other table
// look if the requested table is free and add

	for(int i = 0; i < tables->users.size(); i++){
		if (tables->users[i].id == u1->id) 
			return;
	}

	tables->users.push_back(*u1);

/*
	table* current = tables;  
	if(current == NULL){
		current = new table;
		current->next = NULL;
	}
	while(current->no_of_users > 5){
		if(current->next != NULL)
			current = current->next;
		else{
			current->next = new table;
			current = current->next;
			current->next = NULL;
			current->no_of_users = 0;
			break;
		}
	}


	current->no_of_users++;
	current->users.push_back(*u1);
		*/	



}

void create_table() {

}

void shuffleNewDeck(){
	cout << "\n ShufflingNewDeck............";
	string downCardFile = "images/b2fv.png";

	int value = 14;
	int j = 0;
	for (int i = 1; i <= 52; ++i)
	{
		d1.card_img_no[i]= i;
		d1.card_value[i] = value;
		j++;
		j = j % 4;
		if ( j == 0) 
			value--;
	}	
	d1.index = 51;
	d1.no_of_cards = 0;
	//initialize test user -- to be removed
	
	// u1.name = "Varun";
	// u1.id = 1;
	// u1.balance = 1000;
	// u1.no_of_cards = 0;	
	//for(int j=0; j < tables->users)


	shuffle_array( (d1.card_value), (d1.card_img_no));
}

void shuffle_array(int *card_value, int *card_img_no) {
	int tmp, current, top = 52;
	
	if(top) 
		while(--top) {

			current = rand()%52;
			tmp = card_value[current];
			card_value[current] = card_value[top];
			card_value[top] = tmp;

			tmp = card_img_no[current];
			card_img_no[current] = card_img_no[top];
			card_img_no[top] = tmp;
			
		}
	}

	int* get_card(int user_id){
		cout << "get_card";
		int arr[2];
	/*user* user_ptr = search_or_create_user(user_id);
	*/
		table* current_table = tables;
		int user_index = -1;
		user current_user;

	// while(current_table != NULL && user_index == -1){
	// 	for(int i=0; i < current_table->users.size() > 0 ; i++){

	// 		if(current_table->users[i].id == user_id){
	// 			user_index = i;
	// 			current_user = &(current_table->users[i]);
	// 			break;
	// 		}
	// 	}
	// 	current_table = current_table->next;
	// }
		int found = 0;
		if(tables==NULL)
			cout << "TABLE NULL";
		else{
			cout << "table size = " << tables->users.size();
		}
		for(int k=0; k < tables->users.size() ; k++ ){
			if(tables->users[k].id == user_id ){
				found = 1;
				break;
			}

		}

		current_user = tables->users[user_id%10];
		cout << "Current_user_id = " << u1.id;


	int card_num = u1.no_of_cards;//user_ptr->no_of_cards;
	int next_card = d1.card_img_no[d1.index];

	u1.cards[card_num] = d1.card_value[d1.index];

	d1.index--;

	arr[0] = card_num;
	arr[1] = next_card;
	u1.no_of_cards++;	
	
	/*
	int next_card = d1.card_value[d1.index];
	d1.index++;
	user_ptr->cards[user_ptr->no_of_cards] = next_card;
	user_ptr->no_of_cards++;
	*/
	return arr;
	// $("#container").append("<img id=\"card" + cardNum + "\" src=\"\" />");
	//    var left = 300 + (25 * cardNum); //72

	// var wrappedCard = $("#card" + cardNum);		
	//    wrappedCard.css({ 'position' : 'absolute', 'height' : '96', 'width': '72', 'left': left, 'top':50 });
	// animateCardToLocation(wrappedCard, nextCard.file); 
}

int* get_dealer_card(int table_id){
	int resp[2];
	int card_num = d1.no_of_cards;
	int next_card = d1.card_img_no[d1.index];

	d1.cards[card_num] = d1.card_value[d1.index];
	d1.index--;
	d1.no_of_cards++;
	resp[0] = card_num;
	resp[1] = next_card;
	return resp;
		// $("#dealerContainer").append("<img id=\"dealer-card" + cardNum + "\" src=\"\" >");
		// var left = 300 + (25 * cardNum);
		// var wrappedCard = $("#dealer-card" + cardNum);
		// wrappedCard.css( { 'position' : 'absolute', 'height' : '96', 'width': '72', 'left': left, 'top': 50});
		// animateCardToLocation(wrappedCard, cardNum == 0 ? downCardFile : nextCard.file);
}

int evaluate_hand(int user_id){
	int value = 0;
	int num_aces = 0;
	int no_of_cards = u1.no_of_cards;
	int *user_cards = u1.cards;
	for(int i = 0; i < no_of_cards; i++)
	{	
			//console.log(cardList[i].value);
		if( user_cards[i] >= 10 && user_cards[i] <= 13)
			value = value + 10;
		else if(user_cards[i] == 14)
		{
			value = value + 11;
			num_aces++;
		}
		else
			value = value + user_cards[i];
	}

	while ( value > 21 && num_aces > 0)
	{
		value = value - 10;
		num_aces--;
	}

	return value;
}

int evaluate_dealer_hand(int table_id){
	int value = 0;
	int num_aces = 0;
	int no_of_cards = d1.no_of_cards;
	int *dealer_cards = d1.cards;
	for(int i = 0; i <= no_of_cards; i++)
	{	
			//console.log(cardList[i].value);

		if( dealer_cards[i] >= 10 && dealer_cards[i] <= 13)
			value = value + 10;
		else if(dealer_cards[i] == 14)
		{
			value = value + 11;
			num_aces++;
		}
		else
			value = value + dealer_cards[i];
	}

	while ( value > 21 && num_aces > 0)
	{
		value = value - 10;
		num_aces--;
	}

	return value;
}

void hit(int user_id){
	//Just get_card is called from js
}

void stand(int user_id) {

}

void leave_table() {

}

int main(int argc, char **argv) {
	srand(time(NULL));
	init_casino();
	shuffleNewDeck();
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

	int j, file_fd, buflen, user_id;
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
		// exit(0);
	write(socketfd,
		"HTTP/1.1 404 Not Found\nContent-Length: 185\nConnection: close\nContent-Type: text/html\n\n<html><head>\n<title>404 Not Found</title>\n</head><body>\n<h1>Forbidden</h1>\nThe requested URL, file type or operation is not allowed on this simple static file webserver.\n</body></html>\n",
		280);
	send_and_close_socket(socketfd, buffer);
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

	 cout << "\nNew request" <<buffer;

	/* Check Login Details */
	 if (strncmp(&buffer[0], "GET /login_auth\0", 9) == 0
	 	|| strncmp(&buffer[0], "get /login_auth\0", 9) == 0) {
	 	cout<<"here1";
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
		cout << "\nPassword = " << password << "\n";
		user_id = login_auth(username, password);


		cout << "\nUser id=" << user_id;
		if(user_id > 0){
			cout << "632";
			(void) sprintf(buffer,
				"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"url\":\"http://127.0.0.1:8082/game.html/%d\"} \n",
					VERSION, 45, "text/html", user_id);
			send_and_close_socket(socketfd,buffer);
//			(void) sprintf(buffer, "GET /httpd/game.html");
		}else{
			(void) sprintf(buffer, "GET /\0");
		}

	}
	else if (strncmp(&buffer[0], "GET /get-dealer-card\0", 18) == 0
		|| strncmp(&buffer[0], "get /get-dealer-card\0", 18) == 0) {
		cout<<"here2";
	pthread_mutex_lock(&mymutex);
	good_requests += 1;
	pthread_mutex_unlock(&mymutex);
	int* resp = get_dealer_card(0); 
	(void) sprintf(buffer,
		"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"card_no\":%d, \"next_card\":%d} \n",
				VERSION, 35, "text/html", resp[0], resp[1]); /* Header + a blank line */
		
		send_and_close_socket(socketfd,buffer);
	} 
	else if (strncmp(&buffer[0], "GET /join-table\0", 9) == 0
		|| strncmp(&buffer[0], "get /join_table\0", 9) == 0) {
		cout<<"here3";
	pthread_mutex_lock(&mymutex);
	good_requests += 1;
	pthread_mutex_unlock(&mymutex);

	(void) sprintf(buffer,
		"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"player_score\":%d}  \n",
				VERSION, 20, "text/html", 6); /* Header + a blank line */
		
		send_and_close_socket(socketfd,buffer);
	}
	else if (strncmp(&buffer[0], "GET /evaluate-dealer-hand\0", 21) == 0 || strncmp(&buffer[0], "get /evaluate-dealer-hand\0", 21) == 0 ){
		pthread_mutex_lock(&mymutex);
		good_requests += 1;
		pthread_mutex_unlock(&mymutex);

		int dealer_score = evaluate_dealer_hand(1);

		(void) sprintf(buffer,
			"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"dealer_score\":%d}  \n",
				VERSION, 20, "text/html", dealer_score); /* Header + a blank line */
			send_and_close_socket(socketfd, buffer);
		} 
		else if (strncmp(&buffer[0], "GET /evaluate-hand\0", 12) == 0 || strncmp(&buffer[0], "get /evaluate-hand\0", 12) == 0 ){


		//(void) strcpy(buffer, "GET /httpd/game.html");

			pthread_mutex_lock(&mymutex);
			good_requests += 1;
			pthread_mutex_unlock(&mymutex);

			int player_score = evaluate_hand(1);

			(void) sprintf(buffer,
				"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"player_score\":%d}  \n",
				VERSION, 20, "text/html", player_score); /* Header + a blank line */
				send_and_close_socket(socketfd, buffer);
			}
			else if (strncmp(&buffer[0], "GET /get-card\0", 12) == 0 || strncmp(&buffer[0], "get /get-card\0", 9) == 0 ){


		//(void) strcpy(buffer, "GET /httpd/game.html");
				int user_id = (int)buffer[14] - 48;
				cout<<"User Id:"<<user_id<<endl;

				pthread_mutex_lock(&mymutex);
				good_requests += 1;
				pthread_mutex_unlock(&mymutex);

				int* arr = get_card(user_id);

				(void) sprintf(buffer,
					"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"card_no\":%d, \"next_card\":%d}  \n",
				VERSION, 30, "text/html", arr[0], arr[1]); /* Header + a blank line */
					send_and_close_socket(socketfd, buffer);
				}
				else if (strncmp(&buffer[0], "GET /shuffleNewDeck\0", 9) == 0 || strncmp(&buffer[0], "get /shuffleNewDeck\0", 9) == 0 ){


		//(void) strcpy(buffer, "GET /httpd/game.html");

					pthread_mutex_lock(&mymutex);
					good_requests += 1;
					pthread_mutex_unlock(&mymutex);

					shuffleNewDeck();
					(void) sprintf(buffer,
						"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"a\":1,\"b\":2}\n",
				VERSION, 14, "text/html"); /* Header + a blank line */
						send_and_close_socket(socketfd, buffer);
					}
					else if (strncmp(&buffer[0], "GET /game\0", 9) == 0 || strncmp(&buffer[0], "get /game\0", 9) == 0 ){

						(void) strcpy(buffer, "GET /httpd/game.html");
						pthread_mutex_lock(&mymutex);
						good_requests += 1;
						save_users();
						pthread_mutex_unlock(&mymutex);
					}
					else if (strncmp(&buffer[0], "GET /join-table\0", 9) == 0 || strncmp(&buffer[0], "get /join_table\0", 9) == 0 ){
						pthread_mutex_lock(&mymutex);
						good_requests += 1;
						pthread_mutex_unlock(&mymutex);
			//printf("Buffer %d", buffer[16]);
						int tableNo = (int)buffer[16] - 48;
						cout<<"Table No:"<<tableNo<<endl;

						(void) sprintf(buffer,
							"HTTP/1.1 200 OK\nServer: 207httpd/%d.0\nContent-Length: %ld\nConnection: close\nContent-Type: %s\n\n{\"a\":1,\"b\":2}\n",
				VERSION, 14, "text/html"); /* Header + a blank line */


							send_and_close_socket(socketfd, buffer);
						}
	else if (!strncmp(&buffer[0], "GET /\0", 6) || !strncmp(&buffer[0], "get /\0", 6)) /* convert no filename to index file */
						{
							(void) strcpy(buffer, "GET /httpd/index.html");
							pthread_mutex_lock(&mymutex);
							good_requests += 1;
							pthread_mutex_unlock(&mymutex);
						}

	buflen = strlen(buffer);
	fstr = (char *) 0;

	cout << "\nFinal buffer" << buffer;
	string extension = "html";

	len = strlen(extension.c_str());
	cout << "\nBuff len= " << buflen << "\nLen = " << len;
	cout << "\nBuff c = " << &buffer[buflen - len - 2] << "\nLen = " << len;
	cout << strncmp(&buffer[buflen - len - 2], extension.c_str(), len);
	if (!strncmp(&buffer[buflen - len ], extension.c_str(), len)) {
		cout << "\nText file";
		fstr = "text/html";
	}

	int file_folder_len = 0;	// = strlen(file_folder);
	extension = "jpg";
	len = strlen(extension.c_str());
	if (!strncmp(&buffer[buflen - len], extension.c_str(), len)) {
		fstr = "image/jpeg";
		file_folder = "httpd/images/";

		file_folder_len = strlen(file_folder);
		//break;
	}
	extension = "gif";
	len = strlen(extension.c_str());
	if (!strncmp(&buffer[buflen - len], extension.c_str(), len)) {
		fstr = "image/gif";
		file_folder = "httpd/images/";
		file_folder_len = strlen(file_folder);
	}

	extension = "png";
	len = strlen(extension.c_str());
	if (!strncmp(&buffer[buflen - len], extension.c_str(), len)) {
		cout<<"here4";
		fstr = "image/png";
		file_folder = "httpd/";
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


void send_and_close_socket(int socketfd, char *buffer){

	(void) write(socketfd, buffer, strlen(buffer));

	sleep(1); /* allow socket to drain before signalling the socket is closed */
	close(socketfd);
	pthread_exit(NULL);
}

int login_auth(char *un, char *pw) {

	string s;
	user *u1 = new user;
	int size, last_id = -1, i;
	size = user_data.size();
	
	if(size > 0){
		cout << "\n login_auth";

		for( i=0; i<size; i++ ){
			if(strncmp(user_data[i].name.c_str(), un, min(strlen(un), user_data[i].name.length())) == 0){
				if(strncmp(user_data[i].password.c_str(), pw, min(strlen(pw), user_data[i].password.length())) == 0){
					//authenticated
					assign_table(&user_data[i]);
					return user_data[i].id;
				}
				//invalid password
				return -1;
			} 
			//new user
			last_id = user_data[i].id;

		}
	}
	else{

		u1->name = un;
		u1->password = pw;
		u1->balance = 1000;
		u1->id = 1;
		user_data.push_back(*u1);
	}
	//assign_table(u1);
	
	
	return u1->id;

	 // vector<string> data;
	 // isOpenDB = ConnectDB();
	 // char buff[1024];
	 // sqlite3_stmt *statement;
	 // sprintf(buff,
	 // 		"SELECT id, balance from users where username = '%s' and password = '%s';",
	 // 		un, pw);
	 // cout << "\n query:" << buff;
	 // char *query = buff;

	 // if (sqlite3_prepare(dbfile, query, -1, &statement, 0) == SQLITE_OK) {
	 // 	int ctotal = sqlite3_column_count(statement);
	 // 	int res = 0;

	 // 	while (1)

	 // 	{
	 // 		res = sqlite3_step(statement);

	 // 		if (res == SQLITE_ROW) {
	 // 			for (int i = 0; i < ctotal; i++) {
	 // 				data.push_back((char*) sqlite3_column_text(statement, i));
	 // 			}

	 // 			cout << endl;
	 // 		}

	 // 		if (res == SQLITE_DONE) {
	 // 			//cout << "\nSize:" << data.size();
	 // 			for (size_t n = 0; n < data.size(); n++) {
	 // 				cout << "\ndata:" << data[n] << " ";
	 // 				cout << endl;
	 // 			}



	 // 		u1->id = 1;//data[0];
	 // 		u1->balance = 123;//data[1];
	 // 		u1->name = un;
	 // 		u1->no_of_cards = 0;

	 // 			if (data.size() == 0) {
	 // 				cout << "\nInvalid User Details, creating new user";
	 // 				//create_user(un, pw);
	 // 				return 0;
	 // 			}
	 // 			break;
	 // 		}

	 // 	}

	 // 	return u1;
	 // }
}

int create_user(char *un, char *pw) {
	 // isOpenDB = ConnectDB();
	 // int balance = 1000;
	 // std::stringstream strm;
	 // strm << "insert into users(username,password,balance) values(" << un << ",'"
	 // 		<< pw << "'," << balance << ")";

	 // string s = strm.str();
	 // char *str = &s[0];

	 // sqlite3_stmt *statement;
	 // int result;
	 // char *query = str;
	 // {
	 // 	if (sqlite3_prepare(dbfile, query, -1, &statement, 0) == SQLITE_OK) {
	 // 		int res = sqlite3_step(statement);
	 // 		result = res;
	 // 		sqlite3_finalize(statement);
	 // 	}
	 // 	return result;
	 // }

	return 0;
}

bool ConnectDB() {
	 // if (sqlite3_open(DB, &dbfile) == SQLITE_OK) {
	 // 	isOpenDB = true;
	 // 	cout << "connection done";
	 // 	return true;
	 // }

	return false;
}

void DisonnectDB() {
	 // if (isOpenDB == true) {
	 // 	sqlite3_close(dbfile);
	 // }
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
