#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "servermanager.h"

ServerManager::ServerManager(const std::string &server, const std::string &service) {
  int e;
  struct addrinfo hints, *servinfo, *p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  e = getaddrinfo(server.c_str(), "2050", &hints, &servinfo);
  if (e != 0) {
    MSS_FATAL_MESSAGE(gai_strerror(e));
    return;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    wsockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (wsockfd == -1) {
      MSS_ERROR("socket", errno);
      continue;
    }
    break;
  }

  if (p == NULL) {
    MSS_FATAL_MESSAGE("failed to create socket");
    return;
  }

  if (connect(wsockfd, p->ai_addr, p->ai_addrlen) == -1)
      MSS_ERROR("connect", errno);

  freeaddrinfo(servinfo);

  hints.ai_flags = AI_PASSIVE;

  e = getaddrinfo(0, service.c_str(), &hints, &servinfo);
  if (e != 0) {
    MSS_FATAL_MESSAGE(gai_strerror(e));
    return;
  }

  for(p = servinfo; p != NULL; p = p->ai_next) {
    rsockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (rsockfd == -1) {
      MSS_ERROR("socket", errno);
      continue;
    }

    if (bind(rsockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(rsockfd);
      MSS_ERROR("bind", errno);
      continue;
    }

    break;
  }

  if (p == NULL)
    MSS_FATAL_MESSAGE("failed to bind socket");

  freeaddrinfo(servinfo);
}

ServerManager::~ServerManager() {
  close(wsockfd);
}

std::string ServerManager::GetServer() {
  char buf[256];
  int n;

  n = send(wsockfd, "G\n", 2, 0);
  if (n == -1)
    return "";

  memset(buf, 0, sizeof buf);
  recv(rsockfd, buf, sizeof buf - 1, 0);

  return buf;
}
