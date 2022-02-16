//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket server, synchronous
//
//------------------------------------------------------------------------------

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <boost/beast/ssl.hpp>

// #include <math>

#include "state.hpp"
#include "minimax.hpp"
#include "json.hpp"

using json = nlohmann::json;

// #include <boost/json.hpp>

namespace ssl = boost::asio::ssl;       // from <boost/asio/ssl.hpp>
namespace beast = boost::beast;         // from <boost/beast.hpp>
namespace http = beast::http;           // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio;            // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>



static void	set_winner(json &response, State s)
{
	// Sets response["winner"] to "black", "white" or "no"
	if (s.game_win)
	{
		if (s.winner == BLACK)
			response["winner"] = "black";
		
		if (s.winner == WHITE)
			response["winner"] = "white";
	}
	else
	{
		response["winner"] = "no";
	}
}


static void	set_player(json &response, State s)
{
	// Sets response["player"] to "black", "white"

	if (s.player == BLACK)
		response["player"] = "black";
	
	if (s.player == WHITE)
		response["player"] = "white";
}


static void	add_board_to_json(json &response, State s)
{
	std::stringstream ss;
	ss << s.b_board << std::endl;
	std::string out = ss.str();
	response["black_board"] = out;

	std::stringstream sss;
	sss << s.w_board << std::endl;
	std::string out2 = sss.str();
	response["white_board"] = out2;

	std::stringstream illegal_sstr;
	illegal_sstr << s.make_illegal_move_board() << std::endl;
	std::string illegal_str = illegal_sstr.str();
	response["illegal_board"] = illegal_str;
}


static void	add_game_state_to_json(json &response, State &s)
{
	set_winner(response, s);
	set_player(response, s);
	add_board_to_json(response, s);

	response["w_captures"] 	= s.w_captures;
	response["b_captures"] 	= s.b_captures;
	response["score"] 		= s.score;

}


float       norm_eval(int eval)
{
	// return eval;
	float out = eval;
	std::cout << "EVALLL: " << eval << std::endl;
	if (eval == 0)
		return 0;
	int sign = (eval < 0) ? -1 : 1;
	out = sign * out * 5;
	out = log10f32(out + 1) * sign;
	out = std::max<float>(out, -5.0);
	out = std::min<float>(out, 5.0);
	std::cout << "norm: " << out << std::endl;
	return out;
}

#define MSGLEN 2048
#define PRINT_STATE_ON_MOVE true

class game_handler
{
private:
	int 	sockfd;
	State	s;
	bool 	cpu;
	bool	game_over = false;
	int		depth;
	int		k_beam = false;
	bool	was_possible_capture = false;

public:
	game_handler();
	~game_handler();
	bool	waiting_on_AI = false;
	int		eval = 0;


	std::string	handle_message(std::string msg);
	std::string	handle_message_start(json json_msg);
	std::string	handle_message_move(json json_msg);
	std::string	play_received_move(json json_msg);
	std::string	AI_move_or_predict(void);
	bool		is_game_over(void);
};


game_handler::game_handler()
{
	this->s = State();
	this->s.coord_evaluation_function = eval_surround_square;
}


game_handler::~game_handler()
{
}


bool			game_handler::is_game_over(void)
{
	return (this->s.game_win);
}


// void	prep(State &s)
// {
// 	s = s.make_baby_from_coord();
// }

std::string 	game_handler::handle_message_start(json json_msg)
{
	this->s = State();
	this->s.coord_evaluation_function = eval_surround_square;
	this->waiting_on_AI = false;
	this->cpu           = json_msg["cpu"];
	this->depth         = json_msg["depth"];
	this->k_beam        = json_msg["k_beam"];
	potential_capture_value 	= POTENTIAL_CAPTURE_VALUE;

	json response;

	// if (json_msg["first_player"] == "black")
	if (this->s.player == BLACK)
	{
		response["player"] = "black";
	}
	else
	{
		response["player"] = "white";
	}

	response["type"]	= "game_start";
	response["cpu"]		= this->cpu;
	response["depth"]	= this->depth;
	
    return (response.dump().c_str());
}


std::string		game_handler::play_received_move(json json_msg)
{
	json response;
	response["type"]	= "game_state";
	response["type2"]	= "player_move";
	response["illegal"] = false;
	response["cpu"]		= this->cpu;

	State tmp;
	tmp = this->s;

	this->s = this->s.make_baby_from_coord(json_msg["move"]);
	if (is_illegal(this->s))
	{
		response["illegal"] = true;
		this->s = tmp;
		std::cout << "Illegal move, reverting" << std::endl;
	}
	add_game_state_to_json(response, this->s);
	response["score"] = norm_eval(this->eval);
	if (!was_anything_captured(this->s, tmp)  && tmp.is_possible_capture())
	{
		potential_capture_value = (potential_capture_value * 8) / 9;
	}

	this->waiting_on_AI = true;
    return (response.dump().c_str());
}


std::string		game_handler::AI_move_or_predict(void)
{
	json response;
	response["type"]	= "game_state";
	response["illegal"] = false;
	response["cpu"]		= this->cpu;

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	if (this->cpu)
	{
		if (this->k_beam)
		{
			#ifdef SINGLE_THREAD
				this->s = this->s.make_baby_from_coord(minimax_fred_start_brother_k_beam(s, this->depth, &this->eval));
			#else
				this->s = this->s.make_baby_from_coord(minimax_fred_start_brother_k_beam(s, this->depth, &this->eval));
			#endif
		}
		else
		{
			#ifdef SINGLE_THREAD
				this->s = this->s.make_baby_from_coord(minimax_single_fred(s, this->depth, std::deque<int>(), 0, BLACK_WIN, WHITE_WIN, &this->eval));
			#else
				this->s = this->s.make_baby_from_coord(minimax_fred_start_brother(s, this->depth, &this->eval));
			#endif
		}
		response["type2"] = "AI_move";

		if (PRINT_STATE_ON_MOVE)
			this->s.print();
	}
	else
	{
		if (this->k_beam)
		{
			#ifdef SINGLE_THREAD
				response["suggested_move"] = minimax_fred_start_brother_k_beam(s, this->depth, &this->eval);
			#else
				response["suggested_move"] = minimax_fred_start_brother_k_beam(s, this->depth, &this->eval);
			#endif
		}
		else
		{
			#ifdef SINGLE_THREAD
				response["suggested_move"] = minimax_single_fred(s, this->depth, std::deque<int>(), 0, BLACK_WIN, WHITE_WIN, &this->eval);
			#else
				response["suggested_move"] = minimax_fred_start_brother(s, this->depth, &this->eval);
			#endif
		}
		response["type2"] = "AI_move_suggestion";

	}
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    response["thinking_time"] = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

	add_game_state_to_json(response, this->s);
	response["score"] = norm_eval(this->eval);
	this->waiting_on_AI = false;
    return (response.dump().c_str());
}


std::string		game_handler::handle_message_move(json json_msg)
{
    return (this->play_received_move(json_msg));
}


std::string		game_handler::handle_message(std::string msg)
{
	json json_msg = json::parse(msg);

	if (json_msg["type"] == "start")
	{
		return (this->handle_message_start(json_msg));
	}
	else if (json_msg["type"] == "move")
	{
		return (this->handle_message_move(json_msg));
	}
    return ("Please send a valid JSON");
}

//------------------------------------------------------------------------------

void do_session(tcp::socket& socket, ssl::context& ctx)
{
    try
    {
        websocket::stream<beast::ssl_stream<tcp::socket&>> ws{socket, ctx};

        // Perform the SSL handshake
        ws.next_layer().handshake(ssl::stream_base::server);

        // Set a decorator to change the Server of the handshake
        ws.set_option(websocket::stream_base::decorator(
            [](websocket::response_type& res)
            {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING) +
                        " websocket-server-gomoku");
            }));

        // Accept the websocket handshake
        ws.accept();

		std::cout << "Websocket Connection accepted" << std::endl;
        game_handler game = game_handler();
        for(;;)
        {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;
            beast::flat_buffer reply_buffer;
            beast::flat_buffer reply_buffer2;

            std::string msg;

            // Read a message
            ws.read(buffer);

            // Echo the message back
            ws.text(ws.got_text());

            msg = beast::buffers_to_string(buffer.data());
            std::cout << "Recieved message:\n ------------\n" << msg << "\n------------" << std::endl;

            ostream(reply_buffer) << game.handle_message(msg);
            ws.write(reply_buffer.data());

			if (game.waiting_on_AI and not game.is_game_over())
			{
				ostream(reply_buffer2) << game.AI_move_or_predict();
				ws.write(reply_buffer2.data());
			}

            // ws.write(response);
        }
    }
    catch(beast::system_error const& se)
    {
        // This indicates that the session was closed
        if(se.code() != websocket::error::closed)
            std::cerr << "Error: " << se.code().message() << std::endl;
		std::cout << "Websocket Connection closed" << std::endl;
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}

//------------------------------------------------------------------------------

void run_websocket_server(std::string adress, int porto)
{
    try
    {
        std::cout << "porto: " << porto << std::endl;
        auto const address = net::ip::make_address(adress);
        unsigned short const port = static_cast<unsigned short>(porto);
        std::cout << "Starting WebSocket Server on adrress: " << address << ":" << port << std::endl;

      // The io_context is required for all I/O
        net::io_context ioc{1};

        // The SSL context is required, and holds certificates
        ssl::context ctx{ssl::context::tlsv12};

		ctx.set_password_callback(
			[](std::size_t,
				boost::asio::ssl::context_base::password_purpose)
			{
				return "test";
			});

		ctx.set_options(
			boost::asio::ssl::context::default_workarounds |
			boost::asio::ssl::context::no_sslv2 |
			boost::asio::ssl::context::single_dh_use);

		ctx.use_certificate_chain_file("/etc/letsencrypt/live/gomoku.deyaberger.fr/cert.pem");

		ctx.use_private_key_file("/etc/letsencrypt/live/gomoku.deyaberger.fr/privkey.pem",boost::asio::ssl::context::file_format::pem);

		// ctx.use_tmp_dh_file("dh512.pem");

        tcp::acceptor acceptor{ioc, {address, port}};
        for(;;)
        {
            // This will receive the new connection
            tcp::socket socket{ioc};

            // Block until we get a connection
            acceptor.accept(socket);
		    std::cout << "Websocket Connection attempt from: " << socket.remote_endpoint() << " to: " << socket.local_endpoint() << std::endl;
            

            // Launch the session, transferring ownership of the socket
            std::thread{std::bind(
                &do_session,
                std::move(socket),
                std::ref(ctx))}.detach();
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return;
    }
}

// void
// do_session(tcp::socket socket, ssl::context& ctx)
// {
//     try
//     {
//         // Construct the websocket stream around the socket
//         websocket::stream<beast::ssl_stream<tcp::socket&>> ws{socket, ctx};

//         // Perform the SSL handshake
//         ws.next_layer().handshake(ssl::stream_base::server);

//         // Set a decorator to change the Server of the handshake
//         ws.set_option(websocket::stream_base::decorator(
//             [](websocket::response_type& res)
//             {
//                 res.set(http::field::server,
//                     std::string(BOOST_BEAST_VERSION_STRING) +
//                         " websocket-server-sync-ssl");
//             }));

//         // Accept the websocket handshake
//         ws.accept();

//         for(;;)
//         {
//             // This buffer will hold the incoming message
//             beast::flat_buffer buffer;

//             // Read a message
//             ws.read(buffer);

//             // Echo the message back
//             ws.text(ws.got_text());
//             ws.write(buffer.data());
//         }
//     }
//     catch(beast::system_error const& se)
//     {
//         // This indicates that the session was closed
//         if(se.code() != websocket::error::closed)
//             std::cerr << "Error: " << se.code().message() << std::endl;
//     }
//     catch(std::exception const& e)
//     {
//         std::cerr << "Error: " << e.what() << std::endl;
//     }
// }

// //------------------------------------------------------------------------------

// int runino()
// {
//     try
//     {
//         // Check command line arguments.
//         auto const address = net::ip::make_address("0.0.0.0");
//         auto const port = static_cast<unsigned short>(16784);

//         // The io_context is required for all I/O
//         net::io_context ioc{1};

//         // The SSL context is required, and holds certificates
//         ssl::context ctx{ssl::context::tlsv12};

// 		ctx.set_password_callback(
// 			[](std::size_t,
// 				boost::asio::ssl::context_base::password_purpose)
// 			{
// 				return "test";
// 			});

// 		ctx.set_options(
// 			boost::asio::ssl::context::default_workarounds |
// 			boost::asio::ssl::context::no_sslv2 |
// 			boost::asio::ssl::context::single_dh_use);

// 		ctx.use_certificate_chain_file("newcert.pem");

// 		ctx.use_private_key_file("privkey.pem",boost::asio::ssl::context::file_format::pem);

// 		ctx.use_tmp_dh_file("dh512.pem");

//         tcp::acceptor acceptor{ioc, {address, port}};
//         for(;;)
//         {
//             // This will receive the new connection
//             tcp::socket socket{ioc};

//             // Block until we get a connection
//             acceptor.accept(socket);

//             // Launch the session, transferring ownership of the socket
//             std::thread(
//                 &do_session,
//                 std::move(socket),
//                 std::ref(ctx)).detach();
//         }
//     }
//     catch (const std::exception& e)
//     {
//         std::cerr << "Error: " << e.what() << std::endl;
//         return EXIT_FAILURE;
//     }
// }
