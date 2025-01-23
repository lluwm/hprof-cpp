// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <unordered_map>

#include "stackframe.h"
#include "stacktrace.h"

class Snapshot {
public:
    void addStackFrame(StackFrame *frame) {
        _frames[frame->getId()] = frame;
    }

    StackFrame *getStackFrame(unsigned long id) const;

    void addStackTrace(StackTrace* trace) {
        _traces[trace->getSerialNumber()] = trace;
    }

    StackTrace *getStackTrace(unsigned int id) const;

private:
    std::unordered_map<unsigned long, StackFrame *> _frames; // TODO: change to use shared_prt later.
    std::unordered_map<unsigned int, StackTrace *>  _traces;
};