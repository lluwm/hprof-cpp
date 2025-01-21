// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <utility>
#include <unordered_map>

#include "memory_mapped_file_buffer.h"
#include "para.h"

class Hprof {
public:
    static const int kStringInUtf8      = 0x01;
    static const int kLoadClass         = 0x02;
    static const int kUnloadClass       = 0x03;
    static const int kStackFrame        = 0x04;
    static const int kStackTrace        = 0x05;
    static const int kAllocSites        = 0x06;
    static const int kHeapSummary       = 0x07;
    static const int kStartThread       = 0x0a;
    static const int kEnedThread        = 0x0b;
    static const int kHeapDump          = 0x0c;
    static const int kHeapDumpSegment   = 0x1c;
    static const int kHeapDumpEnd       = 0x2c;
    static const int kCpuSamples        = 0x0d;
    static const int kControlSettings   = 0x0e;

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

    void parse();
private:
    // parsing related operations.
    std::string readNullTerminatedString();
    void loadString(int length);
    unsigned long readId();
    std::string readUTF8(int length);
    void loadClass();
    void loadStackFrame();

private:
    Parameter                                       _para;
    MemoryMappedFileBuffer                          _buffer;
    unsigned int                                    _idSizeInBytes;
    std::unordered_map<unsigned long, std::string>  _strings;
    std::unordered_map<unsigned long, std::string>  _classNamesById;
    std::unordered_map<unsigned int, std::string>   _classNamesBySerial;
};