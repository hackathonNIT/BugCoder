#include "api_v1_Code.h"
#include "../utils/utils.h"
#include <sys/wait.h>
#include <functional>
#include <csignal>
#include <iostream>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

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
  // const char *source_code = R"(
  //       #include <iostream>
  //       int main() {
  //           return 0;
  //       }
  //   )";

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

  std::cout << params["code"].c_str() << std::endl;
  std::cout << params["code"] << std::endl;

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
    auto response = HttpResponse::newHttpJsonResponse(jsonData);
    callback(response);
    return;
  }
  else
  {
    std::cout << "Compilation successful\n";
  }

  // 実行

  const int timeoutSeconds = 5000;
  jsonData["status"] = "AC";
  int result;
  pid_t pid = fork();
  if (pid == 0)
  {
    // 入力渡すとき、ここいじくる
    // execlp("sh", "sh", "-c", command.c_str(), nullptr);
    result = execlp("./my_program", "./my_program", nullptr);
    exit(EXIT_FAILURE); // If execlp fails
  }
  else if (pid > 0)
  {
    bool timeoutOccurred = false;
    auto start = std::chrono::steady_clock::now();
    while (true)
    {
      int status;
      pid_t result = waitpid(pid, &status, WNOHANG);
      if (result == pid)
      {
        if (WIFEXITED(status))
        {
          int exitStatus = WEXITSTATUS(status);
          std::cout << "Command exited with status: " << exitStatus << std::endl;
          // Handle output/error processing here if needed
        }
        break;
      }
      else if (result == 0)
      { // Child process still running
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        if (duration > timeoutSeconds)
        {
          // Timeout
          kill(pid, SIGKILL);
          timeoutOccurred = true;
          std::cout << "Command timed out after " << timeoutSeconds << " milliseconds." << std::endl;
          jsonData["status"] = "TLE";
          auto response = HttpResponse::newHttpJsonResponse(jsonData);
          callback(response);
          return;
        }
        // Sleep 1 millisecond
        usleep(1000);
      }
      else
      {
        std::cerr << "Error in waitpid." << std::endl;
        break;
      }
    }
    if (!timeoutOccurred)
    {
      //
      waitpid(pid, nullptr, 0);
    }
  }
  else
  {
    std::cerr << "Error in fork." << std::endl;
  }

  std::cout << "result:" << result << std::endl;

  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}