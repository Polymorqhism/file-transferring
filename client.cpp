#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <fstream>

// WARNING: THIS FUNCTION IS SKIDDED WITH CHATGPT TOO
std::string get_file_name(const std::string &data) {
  std::string delimiter = "!FILE_NAME=";
  size_t pos = data.find(delimiter);
  if (pos != std::string::npos) {
    return data.substr(pos + delimiter.length());
  }
  return "";
}


#define PORT 12609

void create_final_file(std::string file_name, std::string file_data) {
  std::ofstream file(file_name, std::ios::out);
  std::string suffix = "!FILE_NAME=" + file_name;
  size_t pos = file_data.find(suffix);
  if (pos != std::string::npos) {
      file_data.erase(pos, suffix.length());
  }
  file << file_data;
  file.close();
  std::cout << "SUCCESSFULLY WRITTEN TO FILE" << std::endl;
}

int main () {
  int server_socket = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in server_address;
  server_address.sin_port = htons(PORT);
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_family = AF_INET;

  int connecting = connect(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
  if(connecting < 0) { 
    std::cout << "error connecting" << std::endl; 
    return 1; 
  }

  std::vector<char> buffer; 
  char rcv_buffer[1024];

  while (true) {
    ssize_t bytes_recved = recv(server_socket, rcv_buffer, sizeof(rcv_buffer), 0);
    if (bytes_recved < 0) {
      std::cout << "recv error" << std::endl; 
      break; 
    }
    if (bytes_recved == 0) { 
      break; 
    }
    buffer.insert(buffer.end(), rcv_buffer, rcv_buffer + bytes_recved);
  }

  std::string file_data(buffer.data(), buffer.size());
  std::cout << "raw file data: '" << file_data << "'" << std::endl;

  std::cout << "\n\nfetched data\n";
  std::string file_name = get_file_name(file_data);
  
  if (file_name.empty()) {
    std::cout << "no file name" << std::endl;
  } else {
    std::cout << "file name: " << file_name << std::endl;
  }

  
  create_final_file(file_name, file_data);

  close(server_socket);
  return 0;
}
