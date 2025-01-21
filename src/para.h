// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <string>


class Parameter {
public:
    explicit Parameter(const std::string& path, bool dumpStackTrace) {
        _filePath = path;
        _dumpStackTrace = dumpStackTrace;
    }

    explicit Parameter() {}

    bool dumpStackTrace() const {
        return _dumpStackTrace;
    }

    std::string getFilePath() const {
        return _filePath;
    }

    void setFilePath(const std::string& path) {
        _filePath = path;
    }

    void setDumpStackTrace() {
        _dumpStackTrace = true;
    }
private:
    std::string  _filePath;
    bool    _dumpStackTrace;
};