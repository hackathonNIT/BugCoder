#include "ind.h"

void ind::asyncHandleHttpRequest(const HttpRequestPtr& req, std::function<void (const HttpResponsePtr &)> &&callback)
{
    // write your application logic here
    auto para = req->getParameters();
    HttpViewData data;
    data.insert("title", "Bag Coder");
    data.insert("parameters", para);
    data.insert("css_path",std::string("/css/style.css"));
    data.insert("logo_path",std::string("/img/logo2.png"));

    // ListParaCsp.cspにデータを渡す
    auto res =
        drogon::HttpResponse::newHttpViewResponse("defind.csp", data);
    callback(res);
}
