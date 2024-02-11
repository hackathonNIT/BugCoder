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
  auto f = client->execSqlAsyncFuture("SELECT * FROM codes WHERE parent_id IS NULL");
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
}

void Code::getCodeById(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string id) const
{
  auto client = app().getDbClient();
  Json::Value jsonData;
  auto f = client->execSqlAsyncFuture("SELECT codes.*, users.user_name FROM codes JOIN users ON codes.user_id = users.user_id WHERE code_id = " + id);
  // auto f = client->execSqlAsyncFuture("SELECT * FROM codes WHERE code_id = " + id);
  auto children = client->execSqlAsyncFuture("SELECT codes.*, users.user_name FROM codes JOIN users ON codes.user_id = users.user_id WHERE parent_id = " + id);
  auto answer = client->execSqlAsyncFuture("SELECT * FROM answers WHERE code_id = " + id);
  try
  {
    auto result = f.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;

    for (const auto &field : result[0])
    {
      jsonData[field.name()] = field.as<std::string>();
    }

    auto f2 = client->execSqlAsyncFuture("SELECT * FROM results WHERE code_id = " + id);
    try
    {
      auto result2 = f2.get(); // Block until we get the result or catch the exception;
      std::cout << result2.size() << " rows selected!" << std::endl;

      Json::Value childrenJson;
      for (const auto &r : result2)
      {
        Json::Value childJson;
        for (const auto &field2 : r)
        {
          childJson[field2.name()] = field2.as<std::string>();
        }
        childrenJson.append(childJson);
      }
      jsonData["results"] = childrenJson;
    }
    catch (...)
    {
      std::cerr << "error" << std::endl;
    }
  }
  catch (...)
  {
    std::cerr << "error" << std::endl;
  }

  try
  {
    auto result = children.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;
    Json::Value childrenJson;
    for (const auto &r : result)
    {
      Json::Value childJson;
      for (const auto &field : r)
      {
        childJson[field.name()] = field.as<std::string>();
      }
      std::string sss = childJson["code_id"].asString();
      auto f2 = client->execSqlAsyncFuture("SELECT * FROM results WHERE code_id = " + sss);
      try
      {
        auto result2 = f2.get(); // Block until we get the result or catch the exception;
        std::cout << result2.size() << " rows selected!" << std::endl;

        Json::Value childrenJson2;
        for (const auto &r : result2)
        {
          Json::Value childJson;
          for (const auto &field2 : r)
          {
            childJson[field2.name()] = field2.as<std::string>();
          }
          childrenJson2.append(childJson);
        }
        childJson["results"] = childrenJson2;
      }
      catch (...)
      {
        std::cerr << "error" << std::endl;
      }
      childrenJson.append(childJson);
    }
    jsonData["children"] = childrenJson;
  }
  catch (...)
  {
    std::cerr << "error" << std::endl;
  }
  try
  {
    auto result = answer.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;
    Json::Value childrenJson;
    for (const auto &r : result)
    {
      Json::Value childJson;
      for (const auto &field : r)
      {
        childJson[field.name()] = field.as<std::string>();
      }
      childrenJson.append(childJson);
    }
    jsonData["answer"] = childrenJson;
  }
  catch (...)
  {
    std::cerr << "error" << std::endl;
  }

  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}

void Code::submitCode(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  std::unordered_map<std::string, std::string> params = getPostParams(req);
  Json::Value jsonData;
  auto client = app().getDbClient();

  size_t pos = params["code"].find("\\n");
  while (pos != std::string::npos)
  {
    params["code"].replace(pos, 2, "\n");
    pos = params["code"].find("\\n", pos + 1);
  }
  pos = params["code"].find("\\\"");
  while (pos != std::string::npos)
  {
    params["code"].erase(pos, 1);
    pos = params["code"].find("\\\"", pos + 1);
  }
  for (auto pair : params)
  {
    std::cout << "Key: " << pair.first << ", Value: " << pair.second << std::endl;
  }

  // char *input[10];
  std::vector<std::string> input;
  std::vector<std::string> answer;
  std::string filename = generateRandomString(12);
  std::string input_str = "./" + filename;
  // input[0] = const_cast<char *>(input_str.c_str());
  input.push_back(input_str);
  auto f = client->execSqlAsyncFuture("SELECT * FROM answers WHERE code_id = " + params["code_id"]);
  int answer_cnt;
  std::vector<Json::Value> dattta;
  try
  {
    auto result = f.get(); // Block until we get the result or catch the exception;
    std::cout << result.size() << " rows selected!" << std::endl;
    int i = 0;
    answer_cnt = result.size();
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
      dattta.push_back(entry);
      answer.push_back(entry["outdata"].asString());
      i++;
      input.push_back(entry["indata"].asString());
      std::cout << "an;" << answer[i - 1] << "\n";
    }
    // input[i + 1] = nullptr;
  }

  catch (...)
  {
    std::cerr << "error" << std::endl;
  }

  // const char *source_code = R"(
  //       #include <iostream>
  //       int main() {
  //           return 0;
  //       }
  //   )";

  // コンパイル(ここら辺要修正)
  std::string cmd_str = "g++ -x c++ -o " + filename + " -";
  const char *compiler_cmd = cmd_str.c_str();
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
    jsonData[0]["status"] = "CE";
    auto response = HttpResponse::newHttpJsonResponse(jsonData);
    callback(response);
    return;
  }
  else
  {
    std::cout << "Compilation successful\n";
  }

  // 実行
  for (int i = 0; i < answer_cnt; i++)
  {

    const int timeoutSeconds = 5000;
    // jsonData["status"] = "AC";
    int result;
    pid_t pid = fork();
    if (pid == 0)
    {
      // 入力渡すとき、ここいじくる
      // execlp("sh", "sh", "-c", command.c_str(), nullptr);
      result = execlp(input[0].c_str(), input[0].c_str(), input[i + 1].c_str(), nullptr);
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

    FILE *pp = popen(input_str.c_str(), "r"); // コマンドを実行し、読み取りモードでパイプを開く
    if (!pp)
    {
      std::cerr << "popen() failed!" << std::endl;
      return;
    }

    constexpr int buffer_size = 128;
    char buffer[buffer_size];

    std::string result_p = "";

    // パイプからデータを読み取り、文字列に追加する
    while (!feof(pp))
    {
      if (fgets(buffer, buffer_size, pp) != nullptr)
      {
        result_p += buffer;
      }
    }

    pclose(pp); // パイプを閉じる

    // コマンドの出力を表示する
    std::cout << "Command output:\n"
              << result_p << std::endl;

    // std::string an(answer[i]);
    bool fans;

    std::cout << "answer:" << answer[i] << "\n";
    // fans = an.compare(result_p) == 0 ? true : false;

    fans = checkAnswer(result_p, answer[i]);
    //  Json::Value &addedData = jsonData[i];
    //  bool fans = true;
    dattta[i]["status"] = (fans ? "AC" : "WA");
    jsonData.append(dattta[i]);
    // addedData["status"] = fans ? "AC" : "WA";
    //  jsonData[i]["status"] = fans ? "AC" : "WA";
  }
  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}

void Code::submitIssue(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{
  std::unordered_map<std::string, std::string> params = getPostParams(req);
  Json::Value jsonData;
  auto client = app().getDbClient();

  // auto f = client->execSqlAsyncFuture("INSERT INTO results (user_id, code_id,answer_id,result) VALUES (1, 1, 1,'CE') RETURNING result_id");

  // // std::cout << f << std::endl;
  // const auto &result2 = f.get();
  // try
  // {
  //   std::string iid;
  //   for (const auto &r : result2)
  //   {
  //     Json::Value childJson;
  //     for (const auto &field2 : r)
  //     {
  //       std::cout << "ffff:" << field2.name() << "," << field2.as<std::string>() << "\n";
  //       childJson[field2.name()] = field2.as<std::string>();
  //       if (field2.name().compare("result_id") == 0)
  //       {
  //         iid = field2.as<std::string>();
  //       }
  //     }
  //     std::cout << "iddd:" << iid << "\n";
  //   }
  // }
  // catch (...)
  // {
  // }

  auto f = client->execSqlAsyncFuture("INSERT INTO codes (user_id, code_data, title, detail, lang_id) VALUES (" + params["user_id"] + ",'" + params["code_data"] + "','" + params["title"] + "','" + params["detail"] + "'," + params["lang_id"] + ") RETURNING code_id");
  const auto &result2 = f.get();
  try
  {
    std::string iid;
    for (const auto &r : result2)
    {
      Json::Value childJson;
      for (const auto &field2 : r)
      {
        std::cout << "ffff:" << field2.name() << "," << field2.as<std::string>() << "\n";
        childJson[field2.name()] = field2.as<std::string>();
        // if (field2.name().compare("code_id") == 0)
        // {
        client->execSqlAsyncFuture("INSERT INTO answers (user_id, code_id, indata,outdata) VALUES(" + params["user_id"] + "," + field2.as<std::string>() + ",'" + params["indata"] + "','" + params["outdata"] + "')");
      }
      std::cout << "iddd:" << iid << "\n";
    }
  }
  catch (...)
  {
  }
  // auto f2 =client->execSqlAsyncFuture("SELECT * FROM codes WHERE ");

  //              .then([client, params](const auto &userResult)
  //                    {
  //   if (!userResult.empty())
  //   {
  //     // userテーブルに挿入されたユーザーのIDを取得
  //     const auto &userRow = userResult.front();
  //     int userId = userRow["id"].as<int>();

  //     // codeテーブルにコードを挿入し、userIdを使用して関連付ける
  //     return client->execSqlAsyncFuture("INSERT INTO code (user_id, code_text) VALUES ($1, $2) RETURNING id", userId, codeText);
  //   }
  //   else
  //   {
  //     std::cout << "Failed to insert user" << std::endl;
  //     return drogon::Future<drogon::orm::Result>();
  //   } });

  // auto f2 = client->execSqlAsyncFuture("INSERT INTO codes (user_id, code_id, indata,outdata) VALUES";

  auto response = HttpResponse::newHttpJsonResponse(jsonData);
  callback(response);
}