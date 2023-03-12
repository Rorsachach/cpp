#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>


void str_echo(int fd) {
  ssize_t n;
  char buf[1024];

again:
  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    printf("%s", buf);
    write(fd, buf, n);
  }
  
  if (n < 0 && errno == EINTR) 
    goto again;
  else
    exit(-1);
}

void sig_chld(int signo) {
  pid_t pid;
  int state;
  pid = wait(&state);
  printf("child %d closed.", pid);
  return;
}

int main() {
  pid_t child_pid;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_len;

  int listen_fd, conn_fd;

  if ((listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket failed!");
    exit(1);
  }

  bzero(&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(8000);
  server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  if (bind(listen_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed!");
    exit(1);
  }

  if (listen(listen_fd, 5) < 0) {
    perror("listen failed!");
    exit(1);
  }

  while (1) {
    client_len = sizeof(client_addr);
    conn_fd = accept(listen_fd, (struct sockaddr*) &client_addr, &client_len);
    if ((child_pid = fork()) == 0) {
      close(listen_fd);

      str_echo(conn_fd);

      close(conn_fd);
      exit(0);
    }

    close(conn_fd);
  }

}