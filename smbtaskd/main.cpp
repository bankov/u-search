#include <sys/socket.h>
#include <sys/types.h>

#include <err.h>
#include <iostream>
#include <netdb.h>
#include <string.h>
#include <string.h>
#include <unistd.h>

#include <list>

#include "serverqueue.h"

enum {
  BACKLOG = 10
};

class SchedulerServer {
private:
  int sockfd;
  ServerQueue queue;
public:
  SchedulerServer(const std::string service, const std::string serversfile);
  void Run();
};

SchedulerServer::SchedulerServer(const std::string service, const std::string serversfile) : queue(serversfile) {
  struct addrinfo hints, *p;
  memset(&hints, 0, sizeof hints);
  hints.ai_flags = AI_PASSIVE;
  hints.ai_family = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  struct addrinfo *res;
  int error = getaddrinfo(NULL, service.c_str(), &hints, &res);
  if (error)
    errx(1, "getaddrinfo: %s\n", gai_strerror(error));

  for (p = res; p != NULL; p = p->ai_next) {
      sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd == -1) {
        warn("socket");
        continue;
      }

      int val = 1;
      setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof val);

      if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
        close(sockfd);
        warn("bind");
        continue;
      }

      break;
  }

  if (p == NULL)
    errx(1, "can not bind socket");
  freeaddrinfo(res);
}

void SchedulerServer::Run() {
  struct sockaddr_storage theiraddr;
  socklen_t salen;
  char buf[256];

  while (1) {
    memset(buf, '\0', sizeof buf);

    int ret = recvfrom(sockfd, buf, sizeof buf - 1, 0, (struct sockaddr *)&theiraddr, &salen);
    if (ret == -1)
      err(1, "recvfrom");

    std::string cmd(buf);

    std::size_t pos = cmd.find("\n");
    if (pos != std::string::npos)
      cmd.erase(pos);

    if (cmd.empty())
      continue;

    std::string server = cmd.substr(1);
    switch (cmd[0]) {
    case 'G':
      if (server.empty()) {
          server = queue.cmdGet();
          if (!server.empty())
              sendto(sockfd, server.c_str(), server.size(), 0, (struct sockaddr *)&theiraddr, salen);
      } else
          queue.cmdGet(server);
      break;
    case 'R':
      server = cmd.substr(1);
      queue.cmdRelease(server);
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fputs("usage: smbtaskd serversfile\n", stderr);
    return 1;
  }
  SchedulerServer serv(TASKSERVICE, argv[1]);
  serv.Run();
}
