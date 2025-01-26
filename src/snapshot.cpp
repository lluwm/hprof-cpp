// Copyright (c) 2025 Lei Lu. All rights reserved.

#include <memory>

#include "snapshot.h"

using std::shared_ptr;

shared_ptr<StackFrame>
Snapshot::getStackFrame(unsigned long serialNum) const
{
    auto search = _frames.find(serialNum);
    return search != _frames.end() ? search->second : nullptr;
}

shared_ptr<StackTrace>
Snapshot::getStackTrace(unsigned int serialNum) const
{
    auto search = _traces.find(serialNum);
    return search != _traces.end() ? search->second : nullptr;
}

shared_ptr<Thread>
Snapshot::getThread(unsigned int serialNum) const
{
    auto search = _threads.find(serialNum);
    return search != _threads.end() ? search->second : nullptr;
}

shared_ptr<StackTrace>
Snapshot::getStackTraceAtDepth(int traceSerialNumber, int depth) const
{
    shared_ptr<StackTrace> trace = getStackTrace(traceSerialNumber);
    if (trace != nullptr) {
        trace = trace->fromDepth(depth);
    }
    return trace;
}