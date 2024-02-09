#include "api_v1_Code.h"

using namespace api::v1;

// Add definition of your processing function here

void Code::getAllCodes(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT * FROM codes");
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
  // catch (const DrogonDbException &e)
  // {
  //   std::cerr << "error:" << e.what() << std::endl;
  // }

  // client->execSqlAsync(
  //     "SELECT * FROM codes",
  //     [](const drogon::orm::Result &result)
  //     {
  //       std::cout << result.size() << " rows selected!" << std::endl;
  //       int i = 0;
  //       for (auto row : result)
  //       {
  //         std::cout << i++ << ": user name is " << row["user_name"].as<std::string>() << std::endl;
  //       }
  //       for (const auto &row : result)
  //       {
  //         Json::Value entry;
  //         for (const auto &field : row)
  //         {
  //           entry[field.name()] = field.as<std::string>();
  //         }
  //         jsonData.append(entry);
  //       }
  //     },
  //     [](const DrogonDbException &e)
  //     {
  //       std::cerr << "error:" << e.base().what() << std::endl;
  //     },
  //     "default");
  // [](const Result &r)
  // {
  //   for (const auto &row : r)
  //   {
  //     Json::Value entry;
  //     for (const auto &field : row)
  //     {
  //       entry[field.name()] = field.as<std::string>();
  //     }
  //     jsonData.append(entry);
  //   }
  // },
  // [](const DrogonDbException &e)
  // {
  //   LOG_ERROR << "Database error: " << e.what();
  //   auto response = HttpResponse::newHttpResponse();
  //   response->setStatusCode(HttpStatusCode::k500InternalServerError);
  //   callback(response);
  // });
}

void Code::getCodeById(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string id) const
{
  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT * FROM codes WHERE code_id = " + id);
  try
  {
    auto result = f.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;

    for (const auto &field : result[0])
    {
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