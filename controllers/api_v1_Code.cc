#include "api_v1_Code.h"

using namespace api::v1;

// Add definition of your processing function here

void Code::getAllCodes(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback) const
{

  Json::Value ret;
  ret["result"] = "ok";
  ret["codes"] = "codes";

  auto resp = HttpResponse::newHttpJsonResponse(ret);
  callback(resp);
}

void Code::getCodeById(
    const HttpRequestPtr &req,
    std::function<void(const HttpResponsePtr &)> &&callback,
    std::string id) const
{
  LOG_DEBUG << "Code " << id << " get his information";

  Json::Value ret;

  ret["result"] = "ok";
  ret["code"] = "print(\"hello\")";
  ret["code_id"] = id;
  ret["user_name"] = "user_name";

  auto resp = HttpResponse::newHttpJsonResponse(ret);
  callback(resp);
}