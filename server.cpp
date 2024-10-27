#include <ios>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <fstream>

#define PORT 12609

std::string trim_trailing_whitespace(const std::string &str) {
    size_t end = str.find_last_not_of(" \n\r\t\f\v");
    if (end == std::string::npos) {
        return ""; 
    }
    return str.substr(0, end + 1); 
}

std::string get_bin_data(char file_name[300]) {

  std::ifstream file;
  file.open(file_name, std::ios_base::binary);

  if(!file) { std::cout << "no such file exists or had an error while opening it" << std::endl; };

  file.seekg(0, std::ios::end);
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  char *buffer = new char[size];
  if(file.read(buffer, size)) {
    std::string contents = static_cast<std::string>(buffer);
    return trim_trailing_whitespace(contents);
  }

  return NULL;

}

int main (int argc, char *argv[]) {
  if(argc != 2) {
    std::cout << "improper usage\n\ncorrect usage:\n" << argv[0] << " [file_name]" << std::endl;
    return 0;
  }
  std::string file_name = argv[1];

  std::string binary_contents = get_bin_data(argv[1]);
  
  binary_contents += "!FILE_NAME=transferred_" + file_name ;

  // sockets part :D 

  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in server_address;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_family = AF_INET;
  int opt = 1;
  setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

  int binding = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

  if(binding < 0) { std::cout << "error binding" << std::endl; return 1; }

  listen(server_socket, 1);

  int client_socket = accept(server_socket, nullptr, nullptr);

  if(client_socket < 0) { std::cout << "error accepting client" << std::endl; }

  ssize_t bytes_sent = send(client_socket, binary_contents.data(), binary_contents.size(), 0);
  close(server_socket);

  return 0;
}
