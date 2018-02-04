#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#ifndef BOOST_ASIO_HPP
#define BOOST_ASIO_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <fstream>
#include <boost/asio.hpp>
#include "ApplicationData.h"
#include "ApplicationMode.h"
#include "MapSelectionMode.h"


using boost::asio::ip::tcp;
#define HISTORY_SIZE 256

class Handler
{
	private:
		std::shared_ptr<CApplicationData> context;
	public:

		void handle(std::function<void(bool)> function, bool input);
		void handle(std::string command, std::string expected, std::deque<std::string> buffer);
};

class CMultiplayerClient
{
	private:
		Handler handler;
		boost::asio::io_service io_service_;
		tcp::socket socket_;
		std::deque<std::string> outqueue;
		std::thread service_thread;
		boost::asio::streambuf inbuf, outbuf;
		std::deque<std::string> inqueue;
		std::deque<std::string> history;
	public:
		std::vector<std::string> games;
		std::vector<std::string> players;
		int player, team, gameID;
		std::string mapName;

		CMultiplayerClient(std::shared_ptr<CApplicationData> context, std::string hostname, std::string port);

		void start_client_thread();
		
		void start_client_chat();

		void send_message(std::string message);

		void sendChat(std::string input);

		void getChat(std::list<std::string> &messages, int &start );
		/*
		 * Note that for the function callbacks, use  the form:
		 * [any variables you want to use](bool someBoolean) -> returnType { function code; }
		 *
		 */ 
		 // login returns true for authorization, false for incorrect authorization
		bool login(std::function<void(bool)> function, std::string username, std::string password);

		bool joinGame(std::function<void(bool)> function, int ID, std::string password);

		bool hostGame(std::function<void(bool)> function, std::string map, int numPlayers, std::string password, int isPrivate, std::string name);

		std::vector<std::string> getInfo();

		void sendReady(std::string input);

		void sendStart();

		void logout(std::function<void(bool)> function);

		void sendCommand(std::string command);

		std::string getCommand();
	
		void sendMap(std::string command);

		void sendQuit();

		void getQuit();

		std::string getMap();
			
		void getJoinedPlayerInfo();

		void getPlayerInfo();

		void getGameInfo();
		
		void close();

	private:
		void do_connect(tcp::resolver::iterator endpoint_iterator);

		void do_read();

		//reads body of sent message, pushes to queue of actions to handle
		void do_read_line();

		void write(const std::string msg);

		void do_write();
};

#endif
