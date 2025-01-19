// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <utility>

#include "para.h"

class Hprof {
public:
    explicit Hprof()
        : _fd(-1),
          _fSize(-1),
          _data(nullptr)
    {}

    explicit Hprof(const Parameter& para)
        : _para(para),
          _fd(-1),
          _fSize(-1),
          _data(nullptr)
    {}

    explicit Hprof(Parameter&& para)
        : _para(std::move(para)),
          _fd(-1),
          _fSize(-1),
          _data(nullptr)
    {}

    // mmap file operations.
    void mmapOpen(const string& path);

    void mmapClose();

    bool isMmapOpen() const {
        return _fd == -1;
    }

    void initMmapMembers() {
        _fd = -1;
        _fSize = -1;
        _data = nullptr;
    }

private:
    Parameter   _para;
    int         _fd;
    long        _fSize;
    char       *_data;
};