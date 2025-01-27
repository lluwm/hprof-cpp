// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <string>
#include <memory>
#include <utility>

#include "field.h"
#include "stacktrace.h"

class ClassObj {
public:
    explicit ClassObj(long id, std::shared_ptr<StackTrace> stack, std::string className, long staticFieldsOffset)
        : _id(id),
          _stack(stack),
          _className(className),
          _staticFieldsOffset(staticFieldsOffset),
          _superClassId(0),
          _classLoaderId(0)
    {}

    void setSuperClassId(unsigned long superClass) {
        _superClassId = superClass;
    }

    void setClassLoaderId(unsigned long classLoader) {
        _classLoaderId = classLoader;
    }

    void setStaticFields(const std::vector<std::shared_ptr<Field>>& staticFields) {
        _staticFields = staticFields;
    }

    void setStaticFields(std::vector<std::shared_ptr<Field>>&& staticFields) {
        _staticFields = std::move(staticFields);
    }

    void setFields(const std::vector<std::shared_ptr<Field>>& fields) {
        _fields = fields;
    }

    void setFields(std::vector<std::shared_ptr<Field>>&& fields) {
        _fields = std::move(fields);
    }

    void setInstanceSize(unsigned int val) {
        _instanceSize = val;
    }
    std::string getClassName() const {
        return _className;
    }
private:
    unsigned long                       _id;
    std::shared_ptr<StackTrace>         _stack;
    std::string                         _className;
    long                                _staticFieldsOffset;
    unsigned long                       _superClassId;
    unsigned long                       _classLoaderId;
    std::vector<std::shared_ptr<Field>> _staticFields;
    std::vector<std::shared_ptr<Field>> _fields;
    unsigned int                        _instanceSize;
};