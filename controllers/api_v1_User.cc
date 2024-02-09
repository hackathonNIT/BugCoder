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