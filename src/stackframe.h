// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <string>
#include <iostream>

class StackFrame {
public:
    static const int kNoLineNumber      = 0;
    static const int kUnkownLocation    = -1;
    static const int kCcompiledMethod   = -2;
    static const int kNativeMethod      = -3;

    explicit StackFrame(long id, const std::string& method, const std::string& sig, const std::string& file, int serial, int line)
        : _id(id),
          _methodName(method),
          _signature(sig),
          _fileName(file),
          _serialNumber(serial),
          _lineNumber(line)
    {}

    long getId() const { return _id; }
    std::string getMethodName() const { return _methodName; }
    std::string getSignature() const { return _signature; }
    std::string getFileName() const { return _fileName; }
    int getSerialNumber() const { return _serialNumber; }

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
        os << stack.getMethodName() << stack.getSignature() << " - " << stack.getFileName() << ":" << stack.lineNumberString();
        return os;
    }

private:
    unsigned long   _id;
    std::string     _methodName;
    std::string     _signature;
    std::string     _fileName;
    int             _serialNumber;
    int             _lineNumber;
};

