// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <memory>

#include "instance.h"
#include "stacktrace.h"

class ClassInstance : public Instance {
public:
    explicit ClassInstance(unsigned long id, std::shared_ptr<StackTrace> stack, long valuesOffset)
        : Instance(id, stack),
          _valuesOffset(valuesOffset)
    {}

    void setClassId(unsigned long classId) {
        _classId = classId;
    }
private:
    long                        _valuesOffset;

    //  Id of the ClassObj of which this object is an instance
    unsigned long               _classId;
};