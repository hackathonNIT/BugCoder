#include "api_v1_User.h"

using namespace api::v1;

// Add definition of your processing function here

void User::getAllUsers(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT * FROM users");
  try
  {
    auto result = f.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;
    int i = 0;
    // for (auto row : result)
    // {
    //   std::cout << i++ << ": user name is " << row["user_name"].as<std::string>() << std::endl;
    // }
    for (const auto &row : result)
    {
      Json::Value entry;
      for (const auto &field : row)
      {
        if (((std::string) "password").compare(field.name()) == 0)
        {
          std::cout << field.name() << std::endl;
          continue;
        }
        entry[field.name()] = field.as<std::string>();
      }
      jsonData.append(entry);
    }
  }
  catch (...)
  {
    std::cerr << "error" << std::endl;
  }

  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}

void User::getUserById(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string id) const
{
  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT * FROM users WHERE user_id = " + id);
  try
  {
    auto result = f.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;

    for (const auto &field : result[0])
    {
      if (((std::string) "password").compare(field.name()) == 0)
      {
        std::cout << field.name() << std::endl;
        continue;
      }
      jsonData[field.name()] = field.as<std::string>();
    }
  }
  catch (...)
  {
    std::cerr << "error" << std::endl;
  }

  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}

void User::handleLogin(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  auto body = req->body();

  std::unordered_map<std::string, std::string> params;

  // '&' を区切り文字として、キーと値を分解する
  std::istringstream ss(body.data());
  std::string keyValue;
  while (std::getline(ss, keyValue, '&'))
  {
    // '=' を区切り文字として、キーと値を分解する
    size_t pos = keyValue.find('=');
    if (pos != std::string::npos)
    {
      std::string key = keyValue.substr(0, pos);
      std::string value = keyValue.substr(pos + 1);
      // URL デコード
      key = drogon::utils::urlDecode(key);
      value = drogon::utils::urlDecode(value);
      params[key] = value;
    }
  }

  // パラメータの内容を出力
  for (const auto &param : params)
  {
    std::cout << "Key: " << param.first << ", Value: " << param.second << std::endl;
  }

  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT * FROM users WHERE user_name = \'" + params["user_name"] + "\' AND password = \'" + params["password"] + "\'");
  try
  {
    auto result = f.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;
    if (result.size() == 0)
    {
      jsonData["is_exist"] = false;
    }
    else
    {
      jsonData["is_exist"] = true;
      for (const auto &field : result[0])
      {
        if (((std::string) "password").compare(field.name()) == 0)
        {
          std::cout << field.name() << std::endl;
          continue;
        }
        jsonData[field.name()] = field.as<std::string>();
      }
    }
  }
  catch (...)
  {
    std::cerr << "error" << std::endl;
  }

  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}