#include "api_v1_Code.h"
#include "../utils/utils.h"
#include <sys/wait.h>

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
  callback(response);
  callback(response);
}

void Code::submitCode(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  std::unordered_map<std::string, std::string> params = getPostParams(req);
  Json::Value jsonData;
  const char *source_code = R"(
        #include <iostream>
        int main() {
            // コンパイルエラーを引き起こす行
            //invalid_code;
            return 0;
        }
    )";

  // コンパイル(ここら辺要修正)
  const char *compiler_cmd = "g++ -x c++ -o my_program -";
  const char *code = params["code"].c_str();

  FILE *pipe = popen(compiler_cmd, "w");
  if (!pipe)
  {
    std::cerr << "popen failed\n";
    return;
  }

  // ソースコードを書き込む
  // fwrite(params["code"].c_str(), sizeof(std::string), params["code"].size(), pipe);
  fwrite(code, sizeof(char), strlen(code), pipe);

  // fwrite(source_code, sizeof(char), strlen(source_code), pipe);
  int status = pclose(pipe);

  if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
  {
    std::cerr << "Compilation failed\n";
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), stderr) != NULL)
    {
      std::cerr << buffer;
    }
    jsonData["status"] = "CE";
  }
  else
  {
    std::cout << "Compilation successful\n";
    jsonData["status"] = "AC";
  }

  // std::cout << params["code"].c_str() << std::endl;
  // std::cout << params["code"] << std::endl;

  // 実行

  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}