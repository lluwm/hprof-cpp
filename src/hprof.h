// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <utility>

#include "para.h"

class Hprof {
public:
    explicit Hprof()
    {}

    explicit Hprof(const Parameter& para) : _para(para)
    {}

    explicit Hprof(Parameter&& para) : _para(std::move(para))
    {}

private:
    Parameter _para;
};