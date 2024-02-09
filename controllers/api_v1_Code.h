#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

namespace api
{
  namespace v1
  {
    class Code : public drogon::HttpController<Code>
    {
    public:
      METHOD_LIST_BEGIN
      // use METHOD_ADD to add your custom processing function here;
      // METHOD_ADD(Code::get, "/{2}/{1}", Get); // path is /api/v1/Code/{arg2}/{arg1}
      // METHOD_ADD(Code::your_method_name, "/{1}/{2}/list", Get); // path is /api/v1/Code/{arg1}/{arg2}/list
      // ADD_METHOD_TO(Code::your_method_name, "/absolute/path/{1}/{2}/list", Get); // path is /absolute/path/{arg1}/{arg2}/list
      METHOD_ADD(Code::getAllCodes, "", Get);
      METHOD_ADD(Code::getCodeById, "/{1}", Get);

      METHOD_ADD(Code::submitCode, "/submit", Post);
      METHOD_LIST_END
      // your declaration of processing function maybe like this:
      // void get(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, int p1, std::string p2);
      // void your_method_name(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback, double p1, int p2) const;

      void getAllCodes(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;

      void getCodeById(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback, std::string id) const;

      void submitCode(const drogon::HttpRequestPtr &req, std::function<void(const drogon::HttpResponsePtr &)> &&callback) const;
    };
  }
}
