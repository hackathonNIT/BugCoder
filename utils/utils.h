#pragma once
#include <map>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <string>
#include <tuple>

inline std::unordered_map<std::string, std::string> getPostParams(const HttpRequestPtr &req)
{
  // body type is St17basic_string_viewIcSt11char_traitsIcEE
  std::unordered_map<std::string, std::string> params;
  std::string s = req->body().data();
  if (s[0] == '"')
  {
    s.erase(s.begin());
    s.pop_back();
  }
  std::istringstream ss(s);
  std::string keyValue;
  while (std::getline(ss, keyValue, '&'))
  {
    size_t pos = keyValue.find('=');
    if (pos != std::string::npos)
    {
      std::string key = keyValue.substr(0, pos);
      std::string value = keyValue.substr(pos + 1);
      key = drogon::utils::urlDecode(key);
      value = drogon::utils::urlDecode(value);
      params[key] = value;
    }
  }
  return params;
}

inline bool checkAnswer(std::string a, std::string b)
{
  // body type is St17basic_string_viewIcSt11char_traitsIcEE

  for (size_t i = 0; i < a.size();)
  {
    if (a[i] == ' ')
    {
      a.erase(i, 1);
    }
    else if (a[i] == '\n')
    {
      a.erase(i, 1);
    }
    else
    {
      ++i;
    }
  }
  for (size_t i = 0; i < b.size();)
  {
    if (b[i] == ' ')
    {
      b.erase(i, 1);
    }
    else if (b[i] == '\n')
    {
      b.erase(i, 1);
    }
    else
    {
      ++i;
    }
  }
  std::cout << "check:" << a << "," << b << "\n";
  return a.compare(b) == 0;

  // std::istringstream sa(a);
  // std::istringstream sb(b);
  // std::string keyValue;
  // std::vector<std::string> answer;
  // while (std::getline(sa, keyValue, ' '))
  // {
  //   std::string ss;
  //   std::istringstream sa2(keyValue);
  //   while (std::getline(sa2, ss, '\n'))
  //   {
  //     answer.push_back(ss);
  //   }
  // }
  // bool ret = true;
  // int i = 0;
  // while (std::getline(sb, keyValue, ' '))
  // {
  //   std::string ss;
  //   std::istringstream sa2(keyValue);
  //   while (std::getline(sa2, ss, '\n'))
  //   {

  //     if (answer[i + 1].compare(ss) == 0)
  //     {
  //     }
  //     else
  //     {
  //       return false;
  //     }
  //   }
  // }
  // return true;
}

inline void timeoutHandler(int signal, pid_t pid, FILE *fp)
{
  // kill(pid, SIGKILL);
  kill(pclose(fp), SIGKILL);
};

inline std::vector<std::pair<int, std::string>> myers(const std::string &text1, const std::string &text2);

inline std::vector<std::pair<int, std::string>> binary_search_middle_snake(const std::string &text1, const std::string &text2, int x, int y)
{
  std::string text1a = text1.substr(0, x);
  std::string text2a = text2.substr(0, y);
  std::string text1b = text1.substr(x);
  std::string text2b = text2.substr(y);

  // Compute both diffs serially.
  auto diffs = myers(text1a, text2a);
  auto diffsb = myers(text1b, text2b);

  diffs.insert(diffs.end(), diffsb.begin(), diffsb.end());

  return diffs;
}

inline std::vector<std::pair<int, std::string>> myers_middle_snake(const std::string &text1, const std::string &text2)
{
  int text1_length = text1.length();
  int text2_length = text2.length();
  int max_d = (text1_length + text2_length + 1) / 2;
  int v_offset = max_d;
  int v_length = 2 * max_d;
  std::vector<int> v1(v_length, -1);
  v1[v_offset + 1] = 0;
  std::vector<int> v2(v1);
  int delta = text1_length - text2_length;
  bool front = (delta % 2 != 0);
  int k1start = 0;
  int k1end = 0;
  int k2start = 0;
  int k2end = 0;
  std::vector<std::pair<int, std::string>> diffs;

  for (int d = 0; d < max_d; ++d)
  {
    for (int k1 = -d + k1start; k1 < d + 1 - k1end; k1 += 2)
    {
      int k1_offset = v_offset + k1;
      int x1, y1;
      if (k1 == -d || (k1 != d && v1[k1_offset - 1] < v1[k1_offset + 1]))
      {
        x1 = v1[k1_offset + 1];
      }
      else
      {
        x1 = v1[k1_offset - 1] + 1;
      }
      y1 = x1 - k1;
      while (x1 < text1_length && y1 < text2_length && text1[x1] == text2[y1])
      {
        ++x1;
        ++y1;
      }
      v1[k1_offset] = x1;
      if (x1 > text1_length)
      {
        k1end += 2;
      }
      else if (y1 > text2_length)
      {
        k1start += 2;
      }
      else if (front)
      {
        int k2_offset = v_offset + delta - k1;
        if (k2_offset >= 0 && k2_offset < v_length && v2[k2_offset] != -1)
        {
          int x2 = text1_length - v2[k2_offset];
          if (x1 >= x2)
          {
            return binary_search_middle_snake(text1, text2, x1, y1);
          }
        }
      }
    }

    for (int k2 = -d + k2start; k2 < d + 1 - k2end; k2 += 2)
    {
      int k2_offset = v_offset + k2;
      int x2, y2;
      if (k2 == -d || (k2 != d && v2[k2_offset - 1] < v2[k2_offset + 1]))
      {
        x2 = v2[k2_offset + 1];
      }
      else
      {
        x2 = v2[k2_offset - 1] + 1;
      }
      y2 = x2 - k2;
      while (x2 < text1_length && y2 < text2_length && text1[text1_length - x2 - 1] == text2[text2_length - y2 - 1])
      {
        ++x2;
        ++y2;
      }
      v2[k2_offset] = x2;
      if (x2 > text1_length)
      {
        k2end += 2;
      }
      else if (y2 > text2_length)
      {
        k2start += 2;
      }
      else if (!front)
      {
        int k1_offset = v_offset + delta - k2;
        if (k1_offset >= 0 && k1_offset < v_length && v1[k1_offset] != -1)
        {
          int x1 = v1[k1_offset];
          int y1 = v_offset + x1 - k1_offset;
          int x2 = text1_length - x2;
          if (x1 >= x2)
          {
            return binary_search_middle_snake(text1, text2, x1, y1);
          }
        }
      }
    }
  }

  diffs.push_back(std::make_pair(-1, text1));
  diffs.push_back(std::make_pair(1, text2));

  return diffs;
}

inline std::vector<std::pair<int, std::string>> myers(const std::string &text1, const std::string &text2)
{
  std::vector<std::pair<int, std::string>> diffs;
  if (text1.empty() && text2.empty())
  {
    return diffs;
  }
  if (text1.empty())
  {
    diffs.push_back(std::make_pair(1, text2));
    return diffs;
  }
  if (text2.empty())
  {
    diffs.push_back(std::make_pair(-1, text1));
    return diffs;
  }
  if (text1.length() == 1 && text2.length() == 1)
  {
    if (text1 != text2)
    {
      diffs.push_back(std::make_pair(1, text2));
      diffs.push_back(std::make_pair(-1, text1));
    }
    else
    {
      diffs.push_back(std::make_pair(0, text1));
    }
    return diffs;
  }

  std::string longtext, shorttext;
  if (text1.length() > text2.length())
  {
    longtext = text1;
    shorttext = text2;
  }
  else
  {
    longtext = text2;
    shorttext = text1;
  }

  size_t i = longtext.find(shorttext);
  if (i != std::string::npos)
  {
    diffs.push_back(std::make_pair(1, longtext.substr(0, i)));
    diffs.push_back(std::make_pair(0, shorttext));
    diffs.push_back(std::make_pair(1, longtext.substr(i + shorttext.length())));
    if (text1.length() > text2.length())
    {
      diffs[0].first = -1;
      diffs[2].first = -1;
    }
    if (diffs[2].second.empty())
    {
      diffs.pop_back();
    }
    return diffs;
  }

  if (shorttext.length() == 1)
  {
    diffs.push_back(std::make_pair(-1, text1));
    diffs.push_back(std::make_pair(1, text2));
    return diffs;
  }

  return myers_middle_snake(text1, text2);
}

inline std::tuple<int, std::string, std::vector<std::pair<int, std::string>>> compute_diff(const std::string &text1, const std::string &text2)
{
  auto ses = myers(text1, text2);
  int ld = 0;
  std::string lcs;
  for (const auto &diff : ses)
  {
    int action = diff.first;
    const std::string &s = diff.second;
    if (action == 0)
    {
      lcs += s;
    }
    else
    {
      ld++;
    }
  }
  return std::make_tuple(ld, lcs, ses);
}

inline Json::Value calculateDiff(std::string text1, std::string text2)
{
  size_t pos = text1.find("\\n");
  while (pos != std::string::npos)
  {
    text1.replace(pos, 2, "\n");
    pos = text1.find("\\n", pos + 1);
  }
  pos = text1.find("\\\"");
  while (pos != std::string::npos)
  {
    text1.erase(pos, 1);
    pos = text1.find("\\\"", pos + 1);
  }

  pos = text2.find("\\n");
  while (pos != std::string::npos)
  {
    text2.replace(pos, 2, "\n");
    pos = text2.find("\\n", pos + 1);
  }
  pos = text2.find("\\\"");
  while (pos != std::string::npos)
  {
    text2.erase(pos, 1);
    pos = text2.find("\\\"", pos + 1);
  }

  // Compute diff.
  auto result = compute_diff(text1, text2);
  int ld = std::get<0>(result);
  std::string lcs = std::get<1>(result);
  auto ses = std::get<2>(result);

  std::cout << "LevenshteinDistance = " << ld << std::endl;
  std::cout << "LCS = " << lcs << std::endl;
  std::cout << "SES = ";
  std::string addstr = "", diffstr = "", samestr = "";
  bool addf = 0, samef = 0, difff = 0;
  Json::Value ret;
  for (const auto &pair : ses)
  {
    std::cout << "(" << pair.first << ", " << pair.second << ") ";

    if (pair.first == -1)
    {

      for (int i = 0; i < pair.second.size(); i++)
      {
        diffstr = 1;
        diffstr.push_back(pair.second[i]);
        if (pair.second[i] == '\n')
        {
          std::cout << "FFFAAA"
                    << "\n";
          if ((difff || addf) && samef)
          {
            Json::Value x;
            x["code"] = diffstr;
            x["status"] = -1;
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 1;
            ret.append(x);
            ret.append(y);
          }
          else if (samef)
          {
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 0;
            ret.append(y);
          }
          else if (addf)
          {
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 1;
            ret.append(y);
          }
          else if (difff)
          {
            Json::Value y;
            y["code"] = diffstr;
            y["status"] = -1;
            ret.append(y);
          }
          else
          {
          }
          addf = 0;
          difff = 0;
          samef = 0;
          addstr = "";
          diffstr = "";
        }
      }
    }
    else if (pair.first == 0)
    {

      for (int i = 0; i < pair.second.size(); i++)
      {
        samef = 1;
        addstr.push_back(pair.second[i]);
        diffstr.push_back(pair.second[i]);
        if (pair.second[i] == '\n')
        {
          std::cout << "FFFAAA"
                    << "\n";
          if ((difff || addf) && samef)
          {
            Json::Value x;
            x["code"] = diffstr;
            x["status"] = -1;
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 1;
            ret.append(x);
            ret.append(y);
          }
          else if (samef)
          {
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 0;
            ret.append(y);
          }
          else if (addf)
          {
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 1;
            ret.append(y);
          }
          else if (difff)
          {
            Json::Value y;
            y["code"] = diffstr;
            y["status"] = -1;
            ret.append(y);
          }
          else
          {
          }
          addf = 0;
          difff = 0;
          samef = 0;
          addstr = "";
          diffstr = "";
        }
      }
    }
    else
    {
      for (int i = 0; i < pair.second.size(); i++)
      {
        addf = 1;
        addstr.push_back(pair.second[i]);
        if (pair.second[i] == '\n')
        {
          std::cout << "FFFAAA"
                    << "\n";
          if ((difff || addf) && samef)
          {
            Json::Value x;
            x["code"] = diffstr;
            x["status"] = -1;
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 1;
            ret.append(x);
            ret.append(y);
          }
          else if (samef)
          {
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 0;
            ret.append(y);
          }
          else if (addf)
          {
            Json::Value y;
            y["code"] = addstr;
            y["status"] = 1;
            ret.append(y);
          }
          else if (difff)
          {
            Json::Value y;
            y["code"] = diffstr;
            y["status"] = -1;
            ret.append(y);
          }
          else
          {
          }
          addf = 0;
          difff = 0;
          samef = 0;
          addstr = "";
          diffstr = "";
        }
      }
    }
  }

  if ((difff || addf) && samef)
  {
    Json::Value x;
    x["code"] = diffstr;
    x["status"] = -1;
    Json::Value y;
    y["code"] = addstr;
    y["status"] = 1;
    ret.append(x);
    ret.append(y);
  }
  else if (samef)
  {
    Json::Value y;
    y["code"] = addstr;
    y["status"] = 0;
    ret.append(y);
  }
  else if (addf)
  {
    Json::Value y;
    y["code"] = addstr;
    y["status"] = 1;
    ret.append(y);
  }
  else if (difff)
  {
    Json::Value y;
    y["code"] = diffstr;
    y["status"] = -1;
    ret.append(y);
  }

  std::cout << std::endl;
  return ret;
}

inline std::string generateRandomString(int length)
{
  const char charset[] =
      "abcdefghijklmnopqrstuvwxyz"
      "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
      "0123456789";

  std::string result;
  result.reserve(length);

  srand(time(nullptr)); // シードを設定

  for (int i = 0; i < length; ++i)
  {
    result += charset[rand() % (sizeof(charset) - 1)];
  }

  return result;
}