#pragma once


enum class ArithOp { ADD, SUB, MUL };
enum class RelOp   { EQ, NEQ, LT, GT };
enum class BoolOp  { AND, OR };
enum class UnaryBoolOp { NOT };

inline const char* toString(ArithOp op) {
    switch (op) {
        case ArithOp::ADD: return "+";
        case ArithOp::SUB: return "-";
        case ArithOp::MUL: return "*";
        default:           return "?";
    }
}

inline const char* toString(RelOp op) {
    switch (op) {
        case RelOp::EQ:  return "==";
        case RelOp::NEQ: return "!=";
        case RelOp::LT:  return "<";
        case RelOp::GT:  return ">";
        default:         return "?";
    }
}

inline const char* toString(BoolOp op) {
    switch (op) {
        case BoolOp::AND: return "and";
        case BoolOp::OR:  return "or";
        default:          return "?";
    }
}

inline const char* toString(UnaryBoolOp op) {
    switch (op) {
        case UnaryBoolOp::NOT: return "not";
        default:               return "?";
    }
}
