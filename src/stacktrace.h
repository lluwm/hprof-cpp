// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

#include <memory>
#include <utility>
#include <vector>

#include "stackframe.h"

class StackTrace {
public:
    explicit StackTrace(long serialNum, int threadSerialNum, const std::vector<std::shared_ptr<StackFrame>>& frames)
        : _serialNumber(serialNum),
          _threadSerialNumber(threadSerialNum),
          _frames(frames),
          _parent(nullptr),
          _offset(0)
    {}

    explicit StackTrace(long serialNum, int threadSerialNum, std::vector<std::shared_ptr<StackFrame>>&& frames)
        : _serialNumber(serialNum),
          _threadSerialNumber(threadSerialNum),
          _frames(std::move(frames)),
          _parent(nullptr),
          _offset(0)
    {}

    explicit StackTrace()
        : _serialNumber(0),
          _threadSerialNumber(0),
          _parent(nullptr),
          _offset(0)
    {}

    unsigned int getSerialNumber() const {
        return _serialNumber;
    }

    unsigned int getThreadSerialNumber() const {
        return _threadSerialNumber;
    }

    void setParent(StackTrace *parent) {
        _parent = parent;
    }

    void setOffset(unsigned int value) {
        _offset = value;
    }

    std::shared_ptr<StackTrace> fromDepth(int startingDepth) {
        std::shared_ptr<StackTrace> result = std::make_shared<StackTrace>();
        if (_parent != nullptr) {
            result->setParent(_parent);
        } else {
            result->setParent(this);
        }
        result->setOffset(startingDepth + _offset);
        return result;
    }

    friend std::ostream& operator <<(std::ostream& os, const StackTrace& trace) {
        for (const std::shared_ptr<StackFrame>& frame : trace._frames) {
            os << *frame;
        }
        return os;
    }

private:
    unsigned int                                _serialNumber;
    unsigned int                                _threadSerialNumber;
    std::vector<std::shared_ptr<StackFrame>>    _frames;
    
    /*
     * For subsets of the stack frame we'll reference the parent list of frames
     * but keep track of its offset into the parent's list of stack frame ids.
     * This alleviates the need to constantly be duplicating subsections of the
     * list of stack frame ids.
     */
    StackTrace                                  *_parent;
    unsigned int                                _offset;
};