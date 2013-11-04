#include <sys/types.h>
#include <sys/socket.h>

#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#include "servermanager.h"

ServerManager::ServerManager(const std::string &server, const std::string &service) : keepalive_(0) {
  int e;
  struct addrinfo hints, *servinfo, *p;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;

  e = getaddrinfo(server.c_str(), TASKSERVICES, &hints, &servinfo);
  if (e != 0) {
    MSS_FATAL_MESSAGE(gai_strerror(e));
    return;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    sockfd_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
    if (sockfd_ == -1) {
      MSS_ERROR("socket", errno);
      continue;
    }
    break;
  }

  if (p == NULL) {
    MSS_FATAL_MESSAGE("failed to create socket");
    return;
  }

  if (connect(sockfd_, p->ai_addr, p->ai_addrlen) == -1)
      MSS_ERROR("connect", errno);

  freeaddrinfo(servinfo);

  // TODO: remove AF_INET, support IPv6
  sockaddr_in tgt;
  memset(&tgt, 0, sizeof tgt);
  tgt.sin_family = AF_INET;
  tgt.sin_port = htons(2050);
  tgt.sin_addr.s_addr = INADDR_ANY;
  bind(sockfd_, (const sockaddr *)&tgt, sizeof tgt);

  struct timeval timeout;
  timeout.tv_sec = 5;
  timeout.tv_usec = 0;
  if (setsockopt (sockfd_, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) == -1)
    MSS_FATAL_MESSAGE("setsockopt");
}

ServerManager::~ServerManager() {
  close(sockfd_);
}

std::string ServerManager::GetServer() {
  char buf[256];

  do {
    send(sockfd_, "G", 1, 0);
    memset(buf, 0, sizeof buf);
    // Timeout if there are no free servers.
    if (recv(sockfd_, buf, sizeof buf - 1, 0) == -1) {
      MSS_ERROR("recv", errno);;
      sleep(5);
    }
  } while (buf[0] == '\0');

  smbserver_ = buf;

  keepalivemutex_.lock();
  keepalive_ = 1;
  keepalivemutex_.unlock();

  // Send Keep Alive messages until server is released.
  keepalivethread_ = std::thread([this]() {
    while(1) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      std::string cmd = "G" + smbserver_;
      send(sockfd_, cmd.c_str(), cmd.size(), 0);

      {
        std::lock_guard<std::mutex> lock(keepalivemutex_);
        if(!keepalive_)
          return;
      }
    }
  });
  return smbserver_;
}

void ServerManager::ReleaseServer() {
  keepalivemutex_.lock();
  keepalive_ = 0;
  keepalivemutex_.unlock();
  keepalivethread_.join();

  std::string cmd = "R" + smbserver_;
  send(sockfd_, cmd.c_str(), cmd.size(), 0);
}
