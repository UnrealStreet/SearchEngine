#include <iostream>
#include <wfrest/json.hpp>

using namespace std;

using Json = nlohmann::json;

int main() {
    Json sites1 =
            {{"sites", {
                    {{"name", "王道论坛"}, {"url", "www.cskaoyan.com"}},
                    {{"name", "google"}, {"url", "www.google.com"}},
                    {{"name", "微博"}, {"url", "www.weibo.com"}}
            }}};
    Json sites2;
    sites2["sites"].push_back({{"name", "王道论坛"},
                               {"url",  "www.cskaoyan.com"}});
    sites2["sites"].push_back({{"name", "google"},
                               {"url",  "www.google.com"}});
    sites2["sites"].push_back({{"name", "微博"},
                               {"url",  "www.weibo.com"}});
    Json object;
    object["key1"] = "value1";
    object["key2"] = 2;
    Json mess;
//    mess["error"] ={{"message","密码错误"}};
    mess["error"] = {"message", "密码错误"};
    // 使用[]运算符可以增加键值对
    // 使用push_back方法可以增加数组元素
    string apiRes = R"(
    {
  "id": "<string>",
  "choices": [
    {
      "message": {
        "role": "assistant",
        "content": "<测试开始>",
        "reasoning_content": "<string>"
      },
      "finish_reason": "stop"
    }
  ],
  "tool_calls": [
    {
      "id": "<string>",
      "type": "function",
      "function": {
        "name": "<string>",
        "arguments": "<string>"
      }
    }
  ],
  "usage": {
    "prompt_tokens": 123,
    "completion_tokens": 123,
    "total_tokens": 123
  },
  "created": 123,
  "model": "<string>",
  "object": "chat.completion"
}
    )";
//    string test1 = R"(
//    {
//    "name": "王道论坛",
//    "url": "www.cskaoyan.com"
//    }
//    )";
    Json parse = Json::parse(apiRes);
//    Json test = Json::parse(test1);
    std::cout << sites1.dump() << "\n";
    std::cout << sites2.dump() << "\n";
    std::cout << object.dump() << "\n";
    std::cout << mess.dump() << "\n";
//    cout << parse.dump() << "\n";
//    cout << test.dump() << "\n";
    cout << parse.contains("choices") << "\n";
    auto it = parse.find("choices");
    cout <<it->empty() << "\n";

    cout << parse["choices"].size() << "\n";
    cout << parse["choices"][0]["message"].size() << "\n";
    string content = parse["choices"][0]["message"]["content"];
    cout <<"content: " << content << "\n";
}
