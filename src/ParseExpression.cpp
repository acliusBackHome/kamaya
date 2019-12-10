#include "ParseExpression.hpp"

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
using namespace std;

string ParseExpression::get_info() const {
    return "to be continued";
}

ParseExpression::ParseExpression() {
    defined = false;
}

bool ParseExpression::is_defined() const {
    return defined;
}

#pragma clang diagnostic pop