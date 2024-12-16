#ifndef TCP_HPP
#define TCP_HPP
#include <string>


int send_TCP(const std::string& msg, std::string& response, const std::string& ip, const std::string& port);
void writeFile(std::istringstream& response);
#endif