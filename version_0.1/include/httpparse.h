/*
 * Copyright (C) 2019 CSGuide(https://csguide.cn)
 * Author: xiaobei (https://github.com/imarvinle)
 */

 //写在前面的话：这个头文件意思是处理发过来的http请求，然后对http的请求做解析。

#ifndef WEBSERVER_HTTPPARSE_H
#define WEBSERVER_HTTPPARSE_H

#include <iostream>
#include <string>
#include <unordered_map>

#define CR '\r'
#define LF '\n'
#define LINE_END '\0'
#define PASS

namespace http {

class HttpRequest;

std::ostream &operator<<(std::ostream &, const HttpRequest &);

class HttpRequestParser {
public:

  //主状态机两个情况，一个正在分析请求行，一个正在分析头部字段

  //这是一个简单的状态机思想
  enum LINE_STATE { LINE_OK = 0, LINE_BAD, LINE_MORE };    //行的读取状态：一个完整的 行出错 行读取不完整
  enum PARSE_STATE { PARSE_REQUESTLINE = 0, PARSE_HEADER, PARSE_BODY }; //解析状态
  enum HTTP_CODE { NO_REQUEST, GET_REQUEST, BAD_REQUEST, FORBIDDEN_REQUEST, INTERNAL_ERROR, CLOSED_CONNECTION }; 
                  //服务器解析HTTP请求的结果

  static LINE_STATE parse_line(char *buffer, int &checked_index, int &read_index);//从状态机解析出一行内容
  static HTTP_CODE parse_requestline(char *line, PARSE_STATE &parse_state, HttpRequest &request);//分析请求行
  static HTTP_CODE parse_headers(char *line, PARSE_STATE &parse_state, HttpRequest &request);//分析头部字段
  static HTTP_CODE parse_body(char *body, HttpRequest &request);
  static HTTP_CODE parse_content(char *buffer, int &check_index, int &read_index, PARSE_STATE &parse_state,
                                 int &start_line, HttpRequest &request);//分析HTTP请求入口的函数
};

struct HttpRequest {
  friend std::ostream &operator<<(std::ostream &, const HttpRequest &);

  enum HTTP_VERSION { HTTP_10 = 0, HTTP_11, VERSION_NOT_SUPPORT };
  enum HTTP_METHOD { GET = 0, POST, PUT, DELETE, METHOD_NOT_SUPPORT };
  enum HTTP_HEADER {
    Host = 0,
    User_Agent,
    Connection,
    Accept_Encoding,
    Accept_Language,
    Accept,
    Cache_Control,
    Upgrade_Insecure_Requests
  };
  struct EnumClassHash {

    //这个模板函数??，见下面的map里面
    template <typename T>
    std::size_t operator()(T t) const {
      return static_cast<std::size_t>(t);
    }
  };

  //这是对不定长报头做处理的map
  static std::unordered_map<std::string, HTTP_HEADER> header_map;

  HttpRequest(std::string url = std::string(""), HTTP_METHOD method = METHOD_NOT_SUPPORT,
              HTTP_VERSION version = VERSION_NOT_SUPPORT)
      : mMethod(method),
        mVersion(version),
        mUri(url),
        //请求行
        mContent(nullptr),//很长的一个请求体
        mHeaders(std::unordered_map<HTTP_HEADER, std::string, EnumClassHash>()){};//请求头

  HTTP_METHOD mMethod;
  HTTP_VERSION mVersion;
  std::string mUri;
  char *mContent; 
  std::unordered_map<HTTP_HEADER, std::string, EnumClassHash> mHeaders;
};

}  // namespace http

#endif  // WEBSERVER_HTTPPARSE_H
