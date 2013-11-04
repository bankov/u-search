#include "serverqueue.h"

ServerQueue::ServerQueue() : servers_file_() {

  servers_list_ = NULL;

  servers_list_ = new(std::nothrow) std::list<Server>();
  if (UNLIKELY(servers_list_ == NULL)) {
    MSS_FATAL("", ENOMEM);
    return;
  }

  ilast_server_ = servers_list_->begin();
}

ServerQueue::ServerQueue(const std::string &servers_file) : ServerQueue() {
  if (UNLIKELY(errno))
    return;

  servers_file_ = servers_file;

  // Read servers_file_.
  if (UNLIKELY(ReadServersList())) {
    MSS_DEBUG_ERROR("ReadServersList", errno);
    delete servers_list_;
    servers_list_ = NULL;
    return;
  }
}

ServerQueue::~ServerQueue() {
  if (servers_list_)
    delete servers_list_;
}

void ServerQueue::AddServer(std::string server_name) {
  if (UNLIKELY(server_name.empty())) {
    return;  // Do nothing if an empty server name is passed.
  }
 
  if (UNLIKELY(std::find(servers_list_->begin(), servers_list_->end(), server_name) != servers_list_->end())) {
    MSS_WARN_MESSAGE(("Duplicated server: " + server_name).c_str());
    return;  // Do nothing if server name is in servers_list_ yet.
  }

  ilast_server_ = servers_list_->insert(ilast_server_, server_name); //Add a new server before the head of the queue so that it will be returned by cmdGet() the next time
}

int ServerQueue::ReadServersList() {
  if (!servers_list_->empty()) {
    return 0;  // Servers file was read earlier.
  }
  FILE *fin;
  fin = fopen(servers_file_.c_str(), "r");
  if (UNLIKELY(fin == NULL)) {
    MSS_ERROR("fopen", errno);
    return -1;
  }
  std::string temp;
  char *buf = NULL;
  size_t size = 0;
  // Read lines from servers_file_ and put names servers names in servers_list_
  while (getline(&buf, &size, fin) != -1) {
    temp.insert(0, buf);
    // Deleting '\n' symbols from the end of the string
    temp.erase(temp.end() - 1);
    if (UNLIKELY(temp.empty())) {
      continue;  // Do nothing if find an empthy string.
    }
    AddServer(temp);
    temp.clear();
  }
  free(buf);
  fclose(fin);
  return 0;
}

std::string ServerQueue::cmdGet() {
  if (UNLIKELY(servers_list_ == NULL || servers_list_->empty())) {
    MSS_FATAL("", ENOMEM);
    return ""; //list of servers is empty
  }

  time_t current = time(NULL);
  std::list<ServerQueue::Server>::iterator iprev_head = ilast_server_; //iterator pointing to the current head of the queue

  bool found = false;
  do {
    found = current - ilast_server_->get_timestamp() > MAX_WAIT;
    if (found)
      break;
    if (UNLIKELY(++ilast_server_ == servers_list_->end())) //if the last server is in the end of the list,
        ilast_server_ = servers_list_->begin();              //the next server to choose is from the beginning of the list
  } while(ilast_server_ != iprev_head);

  if (!found)
    return "";

  ilast_server_->refresh(); //the timestamp of the returned server is set to the current time

  std::string last_server_name = ilast_server_->get_name();

  if (UNLIKELY(++ilast_server_ == servers_list_->end()))
    ilast_server_ = servers_list_->begin();
  
  return last_server_name;
}

void ServerQueue::cmdGet(const std::string address) {
  if (UNLIKELY(servers_list_ == NULL || servers_list_->empty())) {
    MSS_FATAL("", ENOMEM);
    return; //list of servers is empty
  }

  std::list<ServerQueue::Server>::iterator it = std::find(servers_list_->begin(), servers_list_->end(), address);
  if (UNLIKELY(it == servers_list_->end()))
    return; //server with name address hasn't been found
 
  it->refresh(); //the timestamp of the server is set to the current time
}

void ServerQueue::cmdRelease(const std::string address) {
  if (UNLIKELY(servers_list_ == NULL || servers_list_->empty())) {
    MSS_FATAL("", ENOMEM);
    return; //list of servers is empty
  }

  std::list<Server>::iterator it = std::find(servers_list_->begin(), servers_list_->end(), address);
  if (UNLIKELY(it == servers_list_->end()))
    return; //server with name address hasn't been found
 
  it->set_timestamp(0); //the timestamp of the server is set to 0 (year 1970)
}
