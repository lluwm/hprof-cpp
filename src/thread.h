// Copyright (c) 2025 Lei Lu. All rights reserved.

#pragma once

class Thread {
public:
    explicit Thread(unsigned long id, unsigned int serialNum) {
        _id = id;
        _stackSerialNum = serialNum;
    }

    unsigned int getStackSerialNum() const {
        return _stackSerialNum;
    }

private:
    unsigned long _id;
    unsigned int  _stackSerialNum;
};