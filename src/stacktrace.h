// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include "stackframe.h"

class StackTrace {
public:
    explicit StackTrace(long serialNum, int threadSerialNum, StackFrame **frames)
        : _serialNumber(serialNum),
          _threadSerialNumber(threadSerialNum),
          _frames(frames)
    {}

private:
    unsigned int    _serialNumber;
    unsigned int    _threadSerialNumber;
    StackFrame      **_frames;
};