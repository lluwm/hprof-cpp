// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <memory>

#include "stacktrace.h"

enum class RootType {
    kUnknown,
    kNativeStatic,
    kNativeLocal,
    kJavaLocal,
    kNativeStack,
    kSystemClass,
    kThreadBlock,
    kBusyMonitor,
    kThreadObject
};

class RootObj {
public:
    explicit RootObj(RootType type, unsigned long id)
        : RootObj(type, id, 0, nullptr)
    {}

    explicit RootObj(RootType type, unsigned long id, unsigned int threadSerialNum, std::shared_ptr<StackTrace> stack)
        : _id(id),
          _type(type),
          _threadSerialNum(threadSerialNum),
          _stack(stack)
    {}

private:
    unsigned long               _id;
    RootType                    _type;
    unsigned int                _threadSerialNum;
    std::shared_ptr<StackTrace> _stack;
};