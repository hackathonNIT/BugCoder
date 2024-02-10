#pragma once
#include <map>
#include <sys/wait.h>

inline std::unordered_map<std::string, std::string> getPostParams(const HttpRequestPtr &req)
{
  // body type is St17basic_string_viewIcSt11char_traitsIcEE
  std::unordered_map<std::string, std::string> params;
  std::string s = req->body().data();
  if (s[0] == '"')
  {
    s.erase(s.begin());
    s.pop_back();
  }
  std::istringstream ss(s);
  std::string keyValue;
  while (std::getline(ss, keyValue, '&'))
  {
    size_t pos = keyValue.find('=');
    if (pos != std::string::npos)
    {
      std::string key = keyValue.substr(0, pos);
      std::string value = keyValue.substr(pos + 1);
      key = drogon::utils::urlDecode(key);
      value = drogon::utils::urlDecode(value);
      params[key] = value;
    }
  }
  return params;
}

inline void timeoutHandler(int signal, pid_t pid, FILE *fp)
{
  // kill(pid, SIGKILL);
  kill(pclose(fp), SIGKILL);
};