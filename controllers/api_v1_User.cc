#include "api_v1_User.h"

#include "../utils/utils.h"

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

  auto resp = HttpResponse::newHttpJsonResponse(jsonData);
  auto &origin = req->getHeader("Origin");
  resp->addHeader("Access-Control-Allow-Origin", origin);
  resp->addHeader("Access-Control-Allow-Methods", "OPTIONS,POST");
  resp->addHeader("Access-Control-Allow-Headers",
                  "x-requested-with,content-type");
  resp->addHeader("Access-Control-Allow-Credentials", "true");
  callback(resp);
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

  auto resp = HttpResponse::newHttpJsonResponse(jsonData);
  auto &origin = req->getHeader("Origin");
  resp->addHeader("Access-Control-Allow-Origin", origin);
  resp->addHeader("Access-Control-Allow-Methods", "OPTIONS,POST");
  resp->addHeader("Access-Control-Allow-Headers",
                  "x-requested-with,content-type");
  resp->addHeader("Access-Control-Allow-Credentials", "true");
  callback(resp);
}

void User::handleLogin(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  std::unordered_map<std::string, std::string> params = getPostParams(req);
  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT * FROM users WHERE user_name = \'" + params["user_name"] + "\' AND password = \'" + params["password"] + "\'");
  std::cout << "FAFA" << std::endl;
  for (auto pair : params)
  {
    std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
  }
  try
  {
    auto result = f.get();
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

  auto resp = HttpResponse::newHttpJsonResponse(jsonData);
  auto &origin = req->getHeader("Origin");
  resp->addHeader("Access-Control-Allow-Origin", origin);
  resp->addHeader("Access-Control-Allow-Methods", "OPTIONS,POST");
  resp->addHeader("Access-Control-Allow-Headers",
                  "x-requested-with,content-type");
  resp->addHeader("Access-Control-Allow-Credentials", "true");
  callback(resp);
}

void User::handleSignin(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  std::unordered_map<std::string, std::string> params = getPostParams(req);
  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT * FROM users WHERE user_name = \'" + params["user_name"] + "\'");
  std::cout << "FAFA" << std::endl;
  for (auto pair : params)
  {
    std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
  }
  try
  {
    auto result = f.get();
    std::cout << result.size() << " rows selected!" << std::endl;
    if (result.size() == 0)
    {
      client->execSqlAsyncFuture("INSERT INTO users (user_name, password, introduction) VALUES (\'" + params["user_name"] + "\',\'" + params["password"] + "\',\'hello!\')");
      jsonData["is_exist"] = false;
    }
    else
    {
      jsonData["is_exist"] = true;
    }
  }
  catch (...)
  {
    std::cerr << "error" << std::endl;
  }

  auto resp = HttpResponse::newHttpJsonResponse(jsonData);
  auto &origin = req->getHeader("Origin");
  resp->addHeader("Access-Control-Allow-Origin", origin);
  resp->addHeader("Access-Control-Allow-Methods", "OPTIONS,POST");
  resp->addHeader("Access-Control-Allow-Headers",
                  "x-requested-with,content-type");
  resp->addHeader("Access-Control-Allow-Credentials", "true");
  callback(resp);
}