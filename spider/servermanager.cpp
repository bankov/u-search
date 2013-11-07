#include "servermanager.h"

ServerManager::ServerManager()
    : servers_file_() {

  servers_list_ = NULL;

  servers_list_ = new(std::nothrow) std::list<std::string>();
  if (UNLIKELY(servers_list_ == NULL)) {
    error_ = ENOMEM;
    MSS_FATAL("", error_);
    return;
  }

  error_ = 0;
}

ServerManager::ServerManager(const std::string &servers_file) : ServerManager() {
  if (UNLIKELY(error_))
    return;

  servers_file_ = servers_file;

  // Read servers_file_.
  if (UNLIKELY(ReadServersList())) {
    MSS_DEBUG_ERROR("ReadServersList", error_);
    delete servers_list_;
    servers_list_ = NULL;
    return;
  }
}

ServerManager::~ServerManager() {
  if (servers_list_)
    delete servers_list_;
}

int ServerManager::ReadServersList() {
  if (!servers_list_->empty()) {
    return 0;  // Servers file was read earlier.
  }
  FILE *fin;
  fin = fopen(servers_file_.c_str(), "r");
  if (UNLIKELY(fin == NULL)) {
    DetectError();
    MSS_ERROR("fopen", error_);
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
    if (UNLIKELY(std::find(servers_list_->begin(), servers_list_->end()
,
                           temp) != servers_list_->end())) {
      MSS_WARN_MESSAGE(("Duplicated server: " + temp).c_str());
      temp.clear();
      continue;  // Do nothing if current server in servers_list_ yet.
    }
    // Add current server to the servers_list_.
    servers_list_->push_back(temp);
    temp.clear();
  }
  free(buf);
  fclose(fin);
  return 0;
}

int ServerManager::AddServer(const std::string &name) {
  if (std::find(servers_list_->begin(), servers_list_->end(),
                name) != servers_list_->end()) {
    MSS_WARN_MESSAGE(("Server " + name + "is in servers_list_").c_str());
    return 0;
  }
  servers_list_->push_back(name);
  FILE *fout = fopen(servers_file_.c_str(), "a");
  if (UNLIKELY(!fout)) {
    DetectError();
    MSS_ERROR("fopen", error_);
    return -1;
  }
  if (UNLIKELY(fprintf(fout, "%s\n", name.c_str()) < 0)) {
    DetectError();
    MSS_ERROR("fprintf", error_);
    fclose(fout);
    return -1;
  }
  fclose(fout);
  return 0;
}

int ServerManager::DelServer(const std::string &name) {
  if (std::find(servers_list_->begin(), servers_list_->end(),
                name) == servers_list_->end()) {
    MSS_WARN_MESSAGE(("Server " + name + "not in servers_list_").c_str());
    return 0;
  }
  servers_list_->remove(name);
  FILE *fout = fopen(servers_file_.c_str(), "w");
  if (UNLIKELY(!fout)) {
    DetectError();
    MSS_ERROR("fopen", error_);
    return -1;
  }
  for (std::list<std::string>::iterator itr = servers_list_->begin();
       itr != servers_list_->end(); ++itr) {
    if (UNLIKELY(fprintf(fout, "%s\n", itr->c_str()) < 0)) {
      DetectError();
      MSS_ERROR("fprintf", error_);
      fclose(fout);
      return -1;
    }
  }
  fclose(fout);
  return 0;
}

std::string ServerManager::GetServer() {
  std::string tmp = servers_list_->front();

  servers_list_->emplace_back(tmp);
  servers_list_->pop_front();

  return servers_list_->back();
}
