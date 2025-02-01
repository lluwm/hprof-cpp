// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <iostream>
#include <regex>
#include <string>
#include <utility>

class StackFrame {
public:
    static const int kNoLineNumber      = 0;
    static const int kUnkownLocation    = -1;
    static const int kCcompiledMethod   = -2;
    static const int kNativeMethod      = -3;

    explicit StackFrame(long id, const std::string& method, const std::string& sig, const std::string& file, int line, const std::string& className, int serial)
        : _id(id),
          _methodName(method),
          _signature(sig),
          _fileName(file),
          _lineNumber(line),
          _className(className),
          _classSerialNumber(serial)
    {}

    explicit StackFrame(long id, const std::string&& method, std::string&& sig, const std::string&& file, int line, std::string&& className, int serial)
        : _id(id),
          _methodName(std::move(method)),
          _signature(std::move(sig)),
          _fileName(file),
          _lineNumber(line),
          _className(std::move(className)),
          _classSerialNumber(serial)
    {}

    long getId() const { return _id; }
    std::string getMethodName() const { return _methodName; }
    std::string getSignature() const { return _signature; }
    std::string getFileName() const { return _fileName; }
    int getClassSerialNumber() const { return _classSerialNumber; }
    std::string getClassName() const { return _className; }

    std::string lineNumberString() const {
        switch (_lineNumber) {
        case kNoLineNumber:
            return "No line number";
        case kUnkownLocation:
            return "Unknown line number";
        case kCcompiledMethod:
            return "Compiled Method";
        case kNativeMethod:
            return "Native Method";
        default:
            return std::to_string(_lineNumber);
        }
    }

    friend std::ostream& operator <<(std::ostream& os, const StackFrame& stack) {
        std::string cName = stack.getClassName();
        os << "at " << std::regex_replace(cName, std::regex("/"), ".") << "." << stack.getMethodName() << "(" << stack.getFileName() << ":" << stack.lineNumberString() << ")" << std::endl;
        return os;
    }

private:
    unsigned long   _id;
    std::string     _methodName;
    std::string     _signature;
    std::string     _fileName;
    int             _lineNumber;
    std::string     _className;
    int             _classSerialNumber;
};

