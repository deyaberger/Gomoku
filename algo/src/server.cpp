/* The port number is passed as an argument */
// #define _BSD_SOURCE

// ! UNUSED FILE KEPT FOR FUTURE TCP SOCKET REFERENCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "state.hpp"
#include "minimax.hpp"
#include "json.hpp"
using json = nlohmann::json;


void	error(const char *msg)
{
    perror(msg);
    exit(1);
}


State	run_server(int portno, State s)
{
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	// create a socket
	// socket(int domain, int type, int protocol)
	sockfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	error("ERROR opening socket");

	// clear address structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
		int enable = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			error("setsockopt(SO_REUSEADDR) failed");

	/* setup the host_addr structure for use in bind call */
	// server byte order
	serv_addr.sin_family = AF_INET;  

	// automatically be filled with current host's IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;  

	// convert short integer value for port must be converted into network byte order
	serv_addr.sin_port = htons(portno);

	// bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
	// bind() passes file descriptor, the address structure, 
	// and the length of the address structure
	// This bind() call will bind  the socket to the current IP address on port, portno
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	// This listen() call tells the socket to listen to the incoming connections.
	// The listen() function places all incoming connection into a backlog queue
	// until accept() call accepts the connection.
	// Here, we set the maximum size for the backlog queue to 5.
	listen(sockfd,5);

	// The accept() call actually accepts an incoming connection
	clilen = sizeof(cli_addr);

	// This accept() function will write the connecting client's address info 
	// into the the address structure and the size of that structure is clilen.
	// The accept() returns a new socket file descriptor for the accepted connection.
	// So, the original socket file descriptor can continue to be used 
	// for accepting new connections while the new socker file descriptor is used for
	// communicating with the connected client.
	newsockfd = accept(sockfd, 
	(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
	error("ERROR on accept");
		
	printf("server: got connection from %s port %d\n",
	inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));


		while (true)
		{
			n = read(newsockfd,buffer,255);
			if (n > 0)
			{
				auto j2 = json::parse(buffer);
				std::cout << "Move: " << j2["move"] << std::endl;
				send(newsockfd, "Hello, world!\n", 13, 0);
			}
			bzero(buffer,256);
		}
		//  n = read(newsockfd,buffer,255);
		//  json j = n + _json;
		//  if (n < 0) error("ERROR reading from socket");

		//  printf("Here is the message: %s\n",buffer);

	// This send() function sends the 13 bytes of the string to the new socket




	close(newsockfd);
	close(sockfd);
	return s; 
}


static void	add_board_to_json(json &response, State s)
{
	std::stringstream ss;
	ss << s.b_board << std::endl;
	std::string out = ss.str();
	response["black"] = out;
	std::stringstream sss;
	sss << s.w_board << std::endl;
	std::string out2 = sss.str();
	response["white"] = out2;
}


void	run_server_perma(State s)
{
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	bzero(buffer,256);
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	// create a socket
	// socket(int domain, int type, int protocol)
	sockfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	error("ERROR opening socket");

	// clear address structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
		int enable = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			error("setsockopt(SO_REUSEADDR) failed");

	/* setup the host_addr structure for use in bind call */
	// server byte order
	serv_addr.sin_family = AF_INET;  

	// automatically be filled with current host's IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;  

	// convert short integer value for port must be converted into network byte order
	serv_addr.sin_port = htons(PORTNO);

	// bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
	// bind() passes file descriptor, the address structure, 
	// and the length of the address structure
	// This bind() call will bind  the socket to the current IP address on port, portno
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	// This listen() call tells the socket to listen to the incoming connections.
	// The listen() function places all incoming connection into a backlog queue
	// until accept() call accepts the connection.
	// Here, we set the maximum size for the backlog queue to 5.
	listen(sockfd,5);

	// The accept() call actually accepts an incoming connection
	clilen = sizeof(cli_addr);

	// This accept() function will write the connecting client's address info 
	// into the the address structure and the size of that structure is clilen.
	// The accept() returns a new socket file descriptor for the accepted connection.
	// So, the original socket file descriptor can continue to be used 
	// for accepting new connections while the new socker file descriptor is used for
	// communicating with the connected client.
	newsockfd = accept(sockfd, 
	(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0)
		error("ERROR on accept");
		
	printf("server: got connection from %s port %d\n",
	inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

	while(not s.game_win)
	{
		n = read(newsockfd,buffer,255);
		if (n > 0)
		{
			int next_move 		= -1;
			json response;
			response["move"] 	= -1;
			response["illegal"] = false;
			response["win"]		= "no";
			State tmp;
			auto j2 = json::parse(buffer);
			tmp = s;
			std::cout << "Recieved Move: " << j2["move"] << std::endl;
			if (j2["stop"] != true)
			{
				s = s.make_baby_from_coord(j2["move"]);
				if (s.free_threes == 2)
				{
					response["illegal"] = true;
					s = tmp;
				}
				else if (s.game_win)
				{
					response["win"] = "white";
				}
				else
				{
					next_move = minimax(s, 7);
					s = s.make_baby_from_coord(next_move);
				}
				if (s.game_win)
				{
					response["win"] = "black";
				}
				response["move"] = next_move;
			}
			else
			{
				s = State();
				s.coord_evaluation_function = eval_surround_square;
			}
			add_board_to_json(response, s);
			response["w_captures"] = s.w_captures;
			response["b_captures"] = s.b_captures;
			std::cout << "Sending" << response.dump() << std::endl;
			send(newsockfd, response.dump().c_str(), response.dump().length(), 0);
			bzero(buffer,256);
			s.print();
		}
	}
	close(newsockfd);
	close(sockfd);
}


State	run_server_once(State s)
{
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	bzero(buffer,256);
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	// create a socket
	// socket(int domain, int type, int protocol)
	sockfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	error("ERROR opening socket");

	// clear address structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
		int enable = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			error("setsockopt(SO_REUSEADDR) failed");

	/* setup the host_addr structure for use in bind call */
	// server byte order
	serv_addr.sin_family = AF_INET;  

	// automatically be filled with current host's IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;  

	// convert short integer value for port must be converted into network byte order
	serv_addr.sin_port = htons(PORTNO);

	// bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
	// bind() passes file descriptor, the address structure, 
	// and the length of the address structure
	// This bind() call will bind  the socket to the current IP address on port, portno
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	// This listen() call tells the socket to listen to the incoming connections.
	// The listen() function places all incoming connection into a backlog queue
	// until accept() call accepts the connection.
	// Here, we set the maximum size for the backlog queue to 5.
	listen(sockfd,5);

	// The accept() call actually accepts an incoming connection
	clilen = sizeof(cli_addr);

	// This accept() function will write the connecting client's address info 
	// into the the address structure and the size of that structure is clilen.
	// The accept() returns a new socket file descriptor for the accepted connection.
	// So, the original socket file descriptor can continue to be used 
	// for accepting new connections while the new socker file descriptor is used for
	// communicating with the connected client.
	newsockfd = accept(sockfd, 
	(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
	error("ERROR on accept");
		
	printf("server: got connection from %s port %d\n",
	inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

	n = read(newsockfd,buffer,255);
	int next_move 		= -1;
	json response;
	response["move"] 	= -1;
	response["illegal"] = false;
	response["win"]		= "no";
	State tmp;
	if (n > 0)
	{
		auto j2 = json::parse(buffer);
		tmp = s;
		std::cout << "Recieved Move: " << j2["move"] << std::endl;
		if (j2["stop"] != true)
		{
			s = s.make_baby_from_coord(j2["move"]);
			if (s.free_threes == 2)
			{
				response["illegal"] = true;
				s = tmp;
			}
			else if (s.game_win)
			{
				response["win"] = "white";
			}
			else
			{
				next_move = minimax(s, 7);
				s = s.make_baby_from_coord(next_move);
			}
			if (s.game_win)
			{
				response["win"] = "black";
			}
			response["move"] = next_move;
		}
		else
		{
			s = State();
			s.coord_evaluation_function = eval_surround_square;
		}
		add_board_to_json(response, s);
		response["w_captures"] = s.w_captures;
		response["b_captures"] = s.b_captures;
		std::cout << "Sending" << response.dump() << std::endl;
		send(newsockfd, response.dump().c_str(), response.dump().length(), 0);
	}
	bzero(buffer,256);

	close(newsockfd);
	close(sockfd);
	return s;
}


State	run_server_once_human(State s)
{
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	bzero(buffer,256);
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	// create a socket
	// socket(int domain, int type, int protocol)
	sockfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
	error("ERROR opening socket");

	// clear address structure
	bzero((char *) &serv_addr, sizeof(serv_addr));
		int enable = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			error("setsockopt(SO_REUSEADDR) failed");

	/* setup the host_addr structure for use in bind call */
	// server byte order
	serv_addr.sin_family = AF_INET;  

	// automatically be filled with current host's IP address
	serv_addr.sin_addr.s_addr = INADDR_ANY;  

	// convert short integer value for port must be converted into network byte order
	serv_addr.sin_port = htons(PORTNO);

	// bind(int fd, struct sockaddr *local_addr, socklen_t addr_length)
	// bind() passes file descriptor, the address structure, 
	// and the length of the address structure
	// This bind() call will bind  the socket to the current IP address on port, portno
	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	// This listen() call tells the socket to listen to the incoming connections.
	// The listen() function places all incoming connection into a backlog queue
	// until accept() call accepts the connection.
	// Here, we set the maximum size for the backlog queue to 5.
	listen(sockfd,5);

	// The accept() call actually accepts an incoming connection
	clilen = sizeof(cli_addr);

	// This accept() function will write the connecting client's address info 
	// into the the address structure and the size of that structure is clilen.
	// The accept() returns a new socket file descriptor for the accepted connection.
	// So, the original socket file descriptor can continue to be used 
	// for accepting new connections while the new socker file descriptor is used for
	// communicating with the connected client.
	newsockfd = accept(sockfd, 
	(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
	error("ERROR on accept");
		
	printf("server: got connection from %s port %d\n",
	inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

	n = read(newsockfd,buffer,255);
	State	tmp;
	json	response;
	int		next_move	= -1;
	response["move"] 	= -1;
	response["illegal"] = false;
	response["win"]		= "no";
	if (n > 0)
	{
		auto j2 = json::parse(buffer);
		tmp = s;
		std::cout << "Recieved Move: " << j2["move"] << std::endl;
		if (j2["move"] != 1234)
		{
			s = s.make_baby_from_coord(j2["move"]);
			if (s.free_threes == 2)
			{
				response["illegal"] = true;
				s = tmp;
			}
			else if (s.game_win)
			{
				if (s.player == BLACK)
				{
					response["win"] = "white";
				}
				else
				{
					response["win"] = "black";
				}
			}
			else
			{
				next_move = minimax(s, 7);
			}
			response["suggest_move"] = next_move;
		}
		else
		{
			s = State();
			s.coord_evaluation_function = eval_surround_square;
		}
		add_board_to_json(response, s);
		response["w_captures"] = s.w_captures;
		response["b_captures"] = s.b_captures;
		std::cout << "Sending" << response.dump() << std::endl;
		send(newsockfd, response.dump().c_str(), response.dump().length(), 0);
	}
	bzero(buffer,256);

	close(newsockfd);
	close(sockfd);
	return s;
}


void	play_vs_cpu()
{
	State s = State();
	s.coord_evaluation_function = eval_surround_square;
	while (not s.game_win)
	{
		s = run_server_once(s);
		s.print();
	}
}


void	play_hotseat()
{
	State s = State();
	s.coord_evaluation_function = eval_surround_square;
	while (not s.game_win)
	{
		s = run_server_once_human(s);
		s.print();
	}
}


void	await_game_request()
{
	int sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	bzero(buffer,256);
	struct sockaddr_in serv_addr, cli_addr;
	int n;

	sockfd =  socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
		int enable = 1;
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
			error("setsockopt(SO_REUSEADDR) failed");

	serv_addr.sin_family = AF_INET;  
	serv_addr.sin_addr.s_addr = INADDR_ANY;  
	serv_addr.sin_port = htons(PORTNO);

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
		sizeof(serv_addr)) < 0) 
		error("ERROR on binding");

	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	newsockfd = accept(sockfd, 
	(struct sockaddr *) &cli_addr, &clilen);
	if (newsockfd < 0) 
	error("ERROR on accept");
		
	printf("server: got connection from %s port %d\n",
	inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));

	n = read(newsockfd,buffer,255);

	if (n > 0)
	{
		auto j2 = json::parse(buffer);

	}
	int next_move 		= -1;
	json response;
	response["move"] 	= -1;
	response["illegal"] = false;
	response["win"]		= "no";
	State tmp;
}