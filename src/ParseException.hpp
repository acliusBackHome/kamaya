#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
#pragma ide diagnostic ignored "OCUnusedStructInspection"
#ifndef NKU_PRACTICE_PARSE_EXCEPTION_HPP
#define NKU_PRACTICE_PARSE_EXCEPTION_HPP

#include "ParseDef.hpp"
#include <exception>
#include <vector>

using namespace std;

class ParseException : public exception {
public:

    /**
     * 默认构造: code=EX_UNKNOWN, node_id=0, _msg=""
     */
    ParseException();

    /**
     * 一般构造:
     * _msg是在异常信息栈中的第一个信息
     * @param _code
     * @param _node_id
     * @param _msg
     */
    explicit ParseException(ExceptionCode _code = EX_UNKNOWN, const string &msg = "");

    ParseException(const ParseException &other) noexcept;

    /**
     * 获得异常代码
     * @return
     */
    ExceptionCode get_code() const;

    /**
     * 获得异常信息
     * @return
     */
    string get_info() const;

    /**
     * 往异常栈中压入信息
     * @param _trace
     */
    void push_trace(const string &_trace);

    const char *what() const noexcept override;

    ~ParseException() noexcept override = default;

    /**
     * 获取指定code对应的意义
     * @param _code
     * @return
     */
    static string get_code_msg(ExceptionCode _code);

private:
    ExceptionCode code;// 异常代码
    vector<string> trace;// 异常信息栈
    string what_str;// 用于存储抛出异常时的字符串
};


#endif //NKU_PRACTICE_PARSE_EXCEPTION_HPP

#pragma clang diagnostic pop