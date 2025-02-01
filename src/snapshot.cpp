// Copyright (c) 2025 Lei Lu. All rights reserved.

#include <iostream>
#include <memory>

#include "snapshot.h"

using std::cout;
using std::endl;
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

std::shared_ptr<Instance>
Snapshot::getInstance(unsigned long id) const
{
    auto search = _instances.find(id);
    return search != _instances.end() ? search->second : nullptr;
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


void
Snapshot::displayThreads()
{
    for (auto& item : _threads) {
        StackTrace *st = getStackTrace(item.first).get();
        if (st != nullptr) {
            cout << *st;
        }
    }
}