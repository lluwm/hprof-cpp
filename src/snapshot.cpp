// Copyright (c) 2025 Lei Lu. All rights reserved.

#include "snapshot.h"

StackFrame *
Snapshot::getStackFrame(unsigned long id) const
{
    auto search = _frames.find(id);
    return search != _frames.end() ? search->second : nullptr;
}

StackTrace *
Snapshot::getStackTrace(unsigned int id) const
{
    auto search = _traces.find(id);
    return search != _traces.end() ? search->second : nullptr;
}