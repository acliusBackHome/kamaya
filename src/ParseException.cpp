#include <cstring>
#include "ParseException.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

ParseException::ParseException() : code(EX_UNKNOWN) {}

ParseException::ParseException(ExceptionCode _code, const string &msg) : code(_code) {
    if (msg.empty()) {
        return;
    }
    trace.emplace_back(msg);
}

ExceptionCode ParseException::get_code() const {
    return code;
}

string ParseException::get_info() const {
    string temp_what = "ParseException {\n  code:";
    temp_what += get_code_msg(code) + ",\n";
    for (const auto &each : trace) {
        temp_what += "  " + each + ",\n";
    }
    temp_what += "}";
    return temp_what;
}

string ParseException::get_code_msg(ExceptionCode _code) {
    switch (_code) {
        case EX_UNKNOWN:
            break;
        case EX_NODE_KEY_NOT_DEFINED:
            return "NodeKeyNotDefinedException(bug)";
        case EX_NODE_NO_SUCH_KEY:
            return "NodeHasNoSuckKeyException";
        case EX_NODE_KEY_DEFINED_NOT_FOUND:
            return "NodeKeyDefinedButNotFoundException(bug)";
        case EX_NODE_NOT_ALLOW_OP:
            return "NodeNotAllowedOperationException(bug)";
        case EX_TREE_NOT_INCOMPLETE:
            return "TreeIncompleteException(bug)";
        case EX_TREE_NO_SUCH_NODE:
            return "TreeHasNoSuchNodeException(bug)";
        case EX_NOT_DECLARED:
            return "NotDeclaredException";
        case EX_DECLARATION_NOT_A_VARIABLE:
            return "DeclarationIsNotVariableException";
        case EX_DECLARATION_NOT_A_FUNCTION:
            return "DeclarationIsNotFunctionException";
        case EX_DECLARATION_NOT_FOUND:
            return "DeclarationNotFoundException";
        case EX_EXPRESSION_NOT_CONST:
            return "ExpressionNotConstantException";
        case EX_EXPRESSION_DIVIDE_ZERO:
            return "DivideZeroException";
        case EX_TYPE_CAN_NOT_CONVERT:
            return "CanNotConvertTypeException";
        case EX_EXPRESSION_CAN_NOT_GENERATE:
            return "CanNotGenerateExpressionException(bug)";
        case EX_EXPRESSION_CAN_NOT_ACCESS:
            return "CanNotAccessExpression(bug)";
    }
    return "UnknownException";
}

void ParseException::push_trace(const string &_trace) {
    trace.emplace_back(_trace);
}

const char *ParseException::what() const noexcept {
    const string &info = get_info();
    char *temp = new char[get_info().length() + 1];
    strcpy(temp, get_info().c_str());
    return temp;
}

ParseException::ParseException(const ParseException &other) noexcept:
        code(other.code), trace(other.trace) {}


#pragma clang diagnostic pop