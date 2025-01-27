// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include "instance.h"
#include "stacktrace.h"
#include "type.h"

class ArrayInstance : public Instance {
public:
    explicit ArrayInstance(unsigned long id, std::shared_ptr<StackTrace> stack, Type type, int length, long valuesOffset)
        : Instance(id, stack),
          _type(type),
          _length(length),
          _valuesOffset(valuesOffset)
    {}

private:
    Type    _type;
    int     _length;
    long    _valuesOffset;
};