// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <string>

#include "type.h"

class Field {
public:
    explicit Field(Type type, const std::string& name)
        : _type(type),
          _name(name)
    {}

private:
    Type        _type;
    std::string _name;
};