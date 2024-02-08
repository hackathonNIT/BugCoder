#include <drogon/drogon.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>

int main()
{
    // Set HTTP listener address and port
    drogon::app().addListener("0.0.0.0", 5555);
    // Load config file
    drogon::app().loadConfigFile("../config.json");

    // Load env file
    std::ifstream file("../.env");
    std::map<std::string, std::string> envMap;
    if (file.is_open())
    {
        std::string line;
        while (std::getline(file, line))
        {
            // 行を "=" で分割して、キーと値を取得
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos)
            {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                envMap[key] = value;
            }
        }
        file.close();
    }
    else
    {
        std::cerr << "Failed to open .env file." << std::endl;
    }
    std::cout << envMap["DB_USERNAME"] << std::endl;
    auto &dbClients = drogon::app().getCustomConfig()["db_clients"];
    if (dbClients.isArray() && !dbClients.empty())
    {
        // auto &firstClient = dbClients[0];
        // if (firstClient["user"].isString())
        // {
        //     firstClient["user"] = envMap["DB_USERNAME"];
        // }
        // if (firstClient["passwd"].isString())
        // {
        //     firstClient["passwd"] = envMap["DB_PASSWORD"];
        // }
        // if (firstClient["dbname"].isString())
        // {
        //     firstClient["dbname"] = envMap["DB_NAME"];
        // }
    }
    // drogon::app().loadConfigFile("../config.yaml");
    //  Run HTTP framework,the method will block in the internal event loop
    drogon::app().run();
    return 0;
}
