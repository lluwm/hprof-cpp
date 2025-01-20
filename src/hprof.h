// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <utility>

#include "memory_mapped_file_buffer.h"
#include "para.h"

class Hprof {
public:
    explicit Hprof()
    {}

    explicit Hprof(const Parameter& para)
        : _para(para),
          _buffer(_para.getFilePath())
    {}

    explicit Hprof(Parameter&& para)
        : _para(std::move(para)),
          _buffer(_para.getFilePath())
    {}

private:

    // parsing related operations.
    string readNullTerminatedString();

private:
    Parameter               _para;
    MemoryMappedFileBuffer  _buffer;
};