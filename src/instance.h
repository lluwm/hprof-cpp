// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <memory>

#include "stacktrace.h"

class Instance {
public:
    explicit Instance(unsigned long id, std::shared_ptr<StackTrace> stack)
        : _id(id),
          _stack(stack)
    {}
    
    void setClassId(unsigned long classId) {
        _classId = classId;
    }
private:
    unsigned long               _id;
    std::shared_ptr<StackTrace> _stack;
    //  Id of the ClassObj of which this object is an instance
    unsigned long               _classId;
};