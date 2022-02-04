#ifndef SERVER_H
#define SERVER_H

State	run_server(int portno, State s = State());
State	run_server_once(State s);
void	play_vs_cpu();
void	play_hotseat();
void	run_server_perma(State s);
void 	run_websocket_server(std::string adress, int port);

#endif