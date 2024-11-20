// udp.hpp
#ifndef UDP_HPP
#define UDP_HPP

#include <string>

int send_UDP(const std::string& msg, std::string& response, const std::string& ip, const std::string& port);

#endif
