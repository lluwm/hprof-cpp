// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "instance.h"
#include "classobj.h"
#include "rootobj.h"
#include "stackframe.h"
#include "stacktrace.h"
#include "thread.h"


static std::unordered_map<Type, int> kTypeSize = {
    { Type::kObject,    -1 },
    { Type::kBoolean,   1 },
    { Type::kChar,      2 },
    { Type::kFloat,     4 },
    { Type::kDouble,    8 },
    { Type::kByte,      1 },
    { Type::kShort,     2 },
    { Type::kInt,       4 },
    { Type::kLong,      8 }
};

class Snapshot {
public:
    void addStackFrame(std::shared_ptr<StackFrame> frame) {
        _frames[frame->getId()] = frame;
    }

    std::shared_ptr<StackFrame> getStackFrame(unsigned long serialNum) const;

    void addStackTrace(std::shared_ptr<StackTrace> trace) {
        _traces[trace->getSerialNumber()] = trace;
    }

    std::shared_ptr<StackTrace> getStackTrace(unsigned int serialNum) const;

    void addThread(unsigned int serialNumber, std::shared_ptr<Thread> thread) {
        _threads[serialNumber] = thread;
    }

    std::shared_ptr<Thread> getThread(unsigned int serialNum) const;

    void addRoot(std::shared_ptr<RootObj> root) {
        _roots.push_back(root);
    }

    std::shared_ptr<StackTrace> getStackTraceAtDepth(int traceSerialNumber, int depth) const;

    void addClass(unsigned long id, const std::shared_ptr<ClassObj> theClass) {
        _classesById[id] = theClass;
        _classesByName.insert({theClass->getClassName(), theClass});
    }

    void addInstance(unsigned long id, std::shared_ptr<Instance> instance) {
        _instances[id] = instance;
    }

    std::shared_ptr<Instance> getInstance(unsigned long id) const;

    void displayThreads();

private:
    std::unordered_map<unsigned long, std::shared_ptr<StackFrame>>  _frames;
    std::unordered_map<unsigned int, std::shared_ptr<StackTrace>>   _traces;
    std::unordered_map<unsigned int, std::shared_ptr<Thread>>       _threads; // Serial number to Thread *.
    std::vector<std::shared_ptr<RootObj>>                           _roots;
    std::unordered_map<unsigned long, std::shared_ptr<ClassObj>>    _classesById;
    std::multimap<std::string, std::shared_ptr<ClassObj>>           _classesByName;
    std::unordered_map<unsigned long, std::shared_ptr<Instance>>    _instances;
};