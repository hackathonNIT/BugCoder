#include "User.h"

// Add definition of your processing function here
void User::login(const HttpRequestPtr &req,
                 std::function<void (const HttpResponsePtr &)> &&callback,
                 std::string &&userId,
                 const std::string &password)
{
    LOG_DEBUG<<"User "<<userId<<" login";
    //Authentication algorithm, read database, verify, identify, etc...
    //...
    Json::Value ret;
    ret["result"]="ok";
    ret["token"]=drogon::utils::getUuid();
    auto resp=HttpResponse::newHttpJsonResponse(ret);
    callback(resp);
}

void User::getInfo(const HttpRequestPtr &req,std::function<void(const HttpResponsePtr &)> &&callback,std::string userId,const std::string &token)const
{
       // LOG_DEBUGでコンソール上に、ログが表示される
       LOG_DEBUG << "User " << userId << " get his information";

       // ここでトークンを参照したり、データを取得する処理が入ったりする

       Json::Value ret;

       // JSONにデータを格納
       ret["result"] = "ok";
       ret["user_name"] = "Jack";
       ret["user_id"] = userId;
       ret["gender"] = 1;

       auto resp = HttpResponse::newHttpJsonResponse(ret);
       callback(resp);
   }