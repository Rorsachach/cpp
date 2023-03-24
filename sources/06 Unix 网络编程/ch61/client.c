#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>


void str_cli(FILE* fp, int fd) {
  char send_buf[1024], recv_buf[1024];
  int n;
  while (fgets(send_buf, 1024, fp) != NULL)
    write(fd, send_buf, strlen(send_buf));
  while ((n = read(fd, recv_buf, 1024)) > 0) {
    fputs(recv_buf, stdout);
  }
}

int main(int argc, char** argv) {
  struct sockaddr_in server_addr;
  int fd;

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed !");
    exit(1);
  }

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8000);
  inet_pton(AF_INET, argv[1], &server_addr.sin_addr);
  connect(fd, (struct sockaddr*) &server_addr, sizeof(server_addr));
  str_cli(stdin, fd);

  exit(0);
}