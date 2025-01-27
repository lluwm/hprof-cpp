// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <memory>

#include "instance.h"
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

class RootObj : public Instance {
public:
    explicit RootObj(RootType type, unsigned long id)
        : RootObj(type, id, 0, nullptr)
    {}

    explicit RootObj(RootType type, unsigned long id, unsigned int threadSerialNum, std::shared_ptr<StackTrace> stack)
        : Instance(id, stack),
          _type(type),
          _threadSerialNum(threadSerialNum)
    {}

private:
    RootType                    _type;
    unsigned int                _threadSerialNum;
};