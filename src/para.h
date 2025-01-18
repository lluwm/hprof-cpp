// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <string>

using std::string;

class Parameter {
public:
    explicit Parameter(const string& path, bool dumpStackTrace) {
        _filePath = path;
        _dumpStackTrace = dumpStackTrace;
    }

    explicit Parameter() {}

    bool dumpStackTrace() const {
        return _dumpStackTrace;
    }

    string getFilePath() const {
        return _filePath;
    }

    void setFilePath(const string& path) {
        _filePath = path;
    }

    void setDumpStackTrace() {
        _dumpStackTrace = true;
    }
private:
    string  _filePath;
    bool    _dumpStackTrace;
};