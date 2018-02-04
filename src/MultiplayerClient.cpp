#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include <fstream>
#include "Command.hpp"
#include <boost/asio.hpp>
#include "ApplicationData.h"
#include "ApplicationMode.h"
#include "MapSelectionMode.h"


using boost::asio::ip::tcp;

/*
 * @Class Handler
 *
 * @brief Handles input from incoming server messages
 *
 * @author Devon Johnson
 * 
 * @date 11/5/2017
 *
 *
 *
 */



/*
 * Handle handles input using input function object
 *
 * @param[in] function Function object of type void(bool) that determines calls
 * required for specific input
 *
 * @param[in] input Input String from server for handling
 *
 */

//maybe simplify handler, add true or false boolean,
// do string check in client function for more concise code

void Handler::handle(std::function<void(bool)> function, bool input){
	function(input);
}


void Handler::handle(std::string command, std::string expected, std::deque<std::string> inqueue){
	while(command.compare(expected) != 0 && !inqueue.empty()){
		inqueue.pop_front();
	}
}



CMultiplayerClient::CMultiplayerClient(std::shared_ptr<CApplicationData> context, std::string hostname, std::string port)
	:io_service_(),
	socket_(io_service_),
	handler()
{
	
	tcp::resolver resolver(io_service_);
	auto endpoint_iterator = resolver.resolve({hostname.c_str(), port.c_str()});
	do_connect(endpoint_iterator);
	players = {"Players"};
	games = {};
}

void CMultiplayerClient::CMultiplayerClient::start_client_thread() {
	std::thread io_thread([this](){ io_service_.run();});
	service_thread.swap(io_thread);
}

void CMultiplayerClient::CMultiplayerClient::start_client_chat() {
	std::thread io_thread([this](){ io_service_.run();});
	char line[512];
	while(std::cin.getline(line, 512))
	{
		send_message(std::string(line));
	}
	io_thread.join();
}

void CMultiplayerClient::CMultiplayerClient::send_message(std::string message) {
	message += "\n";
	this->write(message);
}

// login returns true for authorization, false for incorrect authorization
bool CMultiplayerClient::login( std::function<void(bool)> function, std::string username, std::string password) {
	std::string login = std::string("LOGIN ") + username + " " + password;

	bool result;
	send_message(login);
	;
	while(inqueue.empty()){std::this_thread::sleep_for(std::chrono::milliseconds(50));}
	auto msg = inqueue.front();
	auto code = msg.substr(0,msg.find(" "));
	inqueue.pop_front();
	if(code.compare("WELCOM") == 0){
		handler.handle(function, true);
	}
	else if(code.compare("BADATH") == 0)
		handler.handle(function, false);

}

bool CMultiplayerClient::joinGame(std::function<void(bool)> function, int ID, std::string password) {
	std::string joinRequest = std::string("JGAME ") + std::to_string(ID) + " " + password;
	send_message(joinRequest);
	while(inqueue.empty()){std::this_thread::sleep_for(std::chrono::milliseconds(50));}
	auto msg = inqueue.front();
	auto code = msg.substr(0,msg.find(" "));
	inqueue.pop_front();
	if(code.compare("INGAME") == 0)
		handler.handle(function, true);
	else if(code.compare("NOJOIN") == 0)
		return false;
	//TODO add appropriate Error handline for NOEXIS, BADPAS, and FULL	
}

bool CMultiplayerClient::hostGame(std::function<void(bool)> function, std::string map, int numPlayers, std::string password, int isPrivate, std::string name) {
	Command hostRequest = Command(std::string("HGAMEP"));
	hostRequest.add(map);
	hostRequest.add( std::to_string(numPlayers));
	hostRequest.add(password);
	hostRequest.add(std::to_string(isPrivate));
	hostRequest.add(name);
	// std::cout << hostRequest.toString() << std::endl;
	send_message(hostRequest.toString());
	while(inqueue.empty()){std::this_thread::sleep_for(std::chrono::milliseconds(50));}
	auto msg = inqueue.front();
	auto code = msg.substr(0,msg.find(" "));
	inqueue.pop_front();
	if(code.compare("INHOST") == 0)
		handler.handle(function, true);
	else if(code.compare("NOHOST") == 0)
		handler.handle(function, false);
}


std::vector<std::string> CMultiplayerClient::getInfo() {
	std::string infoRequest = "MYINFO";
	send_message(infoRequest);
	while(inqueue.empty()){std::this_thread::sleep_for(std::chrono::milliseconds(50));}
	auto msg = inqueue.front();

	Command cmd(msg,true);
	std::vector<std::string> info = cmd.split(msg);

	auto code = msg.substr(0,msg.find(" "));
	inqueue.pop_front();

	if(code.compare("YRINFO") == 0) {
		return info;
	}
}

void CMultiplayerClient::logout(std::function<void(bool)> function) {
	std::string logoutRequest = "LOGOUT";
	send_message(logoutRequest);
	handler.handle(function, true);
}

std::string CMultiplayerClient::getCommand() {
	//TODO add parsing and handlind based on Linux Version Protocol
	if(inqueue.empty()) return std::string("");
	// std::cout << inqueue.size();
	auto commandStr = inqueue.front();
	auto code = commandStr.substr(0,7);
	if(code.compare("COMMAND") != 0) return std::string("");
	Command command(commandStr,true);
	auto outstr = command.arg();
	while(command.next()){
		outstr += command.arg();	
	}
	inqueue.pop_front();

	return outstr;
}

void CMultiplayerClient::sendCommand(std::string input){
	Command command(std::string("COMMAND"));

	auto i = input.find("\n");
	while(std::string::npos != i){
		i++;
		command.add(input.substr(0,i));
		input = input.substr(i);
		i = input.find("\n");	
	}
	send_message(command.toString());
}

std::string CMultiplayerClient::getMap() {
	//TODO add parsing and handlind based on Linux Version Protocol
	if(inqueue.empty()) return std::string("");
	auto commandStr = inqueue.front();
	Command command(commandStr,true);
	auto outstr = command.arg();
	if(inqueue.empty()) return outstr;

	while(command.next()){
		outstr += command.arg();	
	}
	inqueue.pop_front();
	// std::cout << "'" << outstr << "'";
	return outstr;
}

void CMultiplayerClient::getGameInfo(){
	auto listGames = std::string("LGAMES");
	send_message(listGames);
	while(inqueue.empty()){std::this_thread::sleep_for(std::chrono::milliseconds(50));}
	auto msg = inqueue.front();
	auto code = msg.substr(0,6);
	inqueue.pop_front();
	std::string tmp;
	Command command(msg, true);
	//std::cout << "# rows = " << command.rows() << std::endl;
	if(code.compare("RGAMEP") == 0){
		games.clear();
		for(unsigned i = 0; i < command.rows(); i++) {
			games.emplace_back(command.arg(1) + ". " + command.arg(0));
			command.nextRow();
		}
	}
	else if(code.compare("NOHOST") == 0){
		return;
	}
}

void CMultiplayerClient::getJoinedPlayerInfo(){
	if(inqueue.empty()) return;
	auto msg = inqueue.front();
	auto code = msg.substr(0,msg.find(" "));
	if(code.compare("PLYINF") == 0){
		inqueue.pop_front();
		Command command(msg, true);
		players.emplace_back(command.arg());
	} 
}

void CMultiplayerClient::getPlayerInfo(){
	auto listPlayers = std::string("GETINF");
	send_message(listPlayers);
	while(inqueue.empty()){std::this_thread::sleep_for(std::chrono::milliseconds(50));}
	auto msg = inqueue.front();
	inqueue.pop_front();
	std::string tmp;
	Command command(msg, true);
	auto code = msg.substr(0,5);
	if(code.compare("RGAME") == 0){
		tmp = command.arg();
		// std::cout << "ID: " + command.arg() << std::endl;
		//gameID = std::stoi(command.arg());
		command.next();//pop game ID
		// std::cout << "NumPlyrs: " + command.arg() << std::endl;
		team = std::stoi(command.arg());
		
		command.next();//pop numPlayers
		mapName = command.arg();
		// std::cout << "Map: " + command.arg() << std::endl;
		command.next();//pop numPlayers
		// std::cout << "Room: " + command.arg() << std::endl;

		players.clear();
		players.emplace_back("["+tmp+"]" + ". " + command.arg());
		command.next();//pop gameName
		do{
			players.emplace_back(command.arg());
			command.next();//pop ready
		// std::cout << "Ready: " + command.arg() << std::endl;
			command.next();//pop team
		// std::cout << "TEAM: " + command.arg() << std::endl;
		}while(command.next());
	}
	else if(code.compare("NOHOST") == 0){
		return;
	}
}



void CMultiplayerClient::sendMap(std::string input){
	Command command(std::string("SENDMAP"));

	auto i = input.find("\n");
	while(std::string::npos != i){
		i++;
		command.add(input.substr(0,i));
		input = input.substr(i);
		i = input.find("\n");	
	}
	send_message(command.toString());
}

void CMultiplayerClient::sendChat(std::string input){
	std::string chat = "CHAT " + input;
	send_message(chat);
}

void CMultiplayerClient::sendReady(std::string input){
	std::string ready = "READY " + input;
	send_message(ready);
}

void CMultiplayerClient::sendStart(){
	std::string start = "START";
	send_message(start);
}

void CMultiplayerClient::sendQuit(){
	std::string quit = "QUIT";
	send_message(quit);
}

void CMultiplayerClient::getQuit() {
	if(inqueue.empty()) return;
	auto quit = inqueue.front();
	auto code = quit.substr(0,quit.find(" "));
	if(code.compare("LEAVE") == 0){
		inqueue.pop_front();
		getPlayerInfo();
//		std::cout << quit << "---pop front\n" ;
//		Command command(quit, true);
//		auto it = find(players.begin(), players.end(), 
//				command.arg());
//		//std::cout << it	<< "---erase " << command.arg() << std::endl;
//		players.erase(it);
	}
}

void CMultiplayerClient::getChat(std::list<std::string> &messages,int &start){
	if(inqueue.empty()) return;
	auto chat = inqueue.front();
	auto code = chat.substr(0,chat.find(" "));;
	if(code.compare("CHAT") == 0){
		inqueue.pop_front();

		messages.push_back(chat.substr(chat.find(" ")));
	} 

	else if(code.compare("ISRDY") == 0 || code.compare("UNRDY") == 0) {
		inqueue.pop_front();
	}

	else if(code.compare("GSTART") == 0){
		inqueue.pop_front();
		start = 1;
	}

}

void CMultiplayerClient::CMultiplayerClient::close()
{
	io_service_.post([this]() { socket_.close(); });
	service_thread.join();
}

void CMultiplayerClient::do_connect(tcp::resolver::iterator endpoint_iterator)
{
	boost::asio::async_connect(socket_, endpoint_iterator,
			[this](boost::system::error_code ec, tcp::resolver::iterator)
			{
			if (!ec)
			{
			boost::asio::ip::tcp::no_delay option(true);
			socket_.set_option(option);

			do_read();
			}
			});
}

void CMultiplayerClient::do_read()
{
	boost::asio::async_read_until(socket_,inbuf,'\n',
			[this](boost::system::error_code ec, std::size_t /*length*/)
			{
			if (!ec)
			{
			do_read_line();
			}
			else
			{
			std::cout << "ERROR " << ec << std::endl ;
			socket_.close();
			}
			});
}

//reads body of sent message, pushes to queue of actions to handle
void CMultiplayerClient::do_read_line()
{
	//if(history.size() == HISTORY_SIZE ) history.pop_back();
	std::string line;
	std::istream in(&inbuf);
	std::getline(in, line);
	//history.push_front(line);
	inqueue.push_front(std::string(line));
	// std::cout << line << std::endl;
	do_read();
}

void CMultiplayerClient::CMultiplayerClient::write(const std::string msg)
{
	io_service_.post(
			[this, msg]()
			{
			bool write_in_progress = !outqueue.empty();
			outqueue.push_back(msg);
			if (!write_in_progress)
			{
			do_write();
			}
			});
}

void CMultiplayerClient::do_write(){
	std::string msg = outqueue.back();
	boost::asio::async_write(socket_,boost::asio::buffer(msg),
			[this](boost::system::error_code ec, std::size_t /*length*/)
			{
			if (!ec)
			{
			outqueue.pop_front();
			if (!outqueue.empty())
			{
			do_write();
			}
			}
			else
			{
			socket_.close();
			}
			});
}

