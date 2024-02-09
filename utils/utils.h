#pragma once
#include <map>

inline std::unordered_map<std::string, std::string> getPostParams(const HttpRequestPtr &req)
{
  // body type is St17basic_string_viewIcSt11char_traitsIcEE
  std::unordered_map<std::string, std::string> params;

  std::istringstream ss(req->body().data());
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