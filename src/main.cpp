// Copyright (c) 2025 Lei Lu. All rights reserved.

#include <iostream>
#include <utility>

#include "cxxopts.hpp"
#include "hprof.h"

using std::cout;
using std::endl;
using std::string;

using cxxopts::Options;
using cxxopts::ParseResult;
using cxxopts::value;
using cxxopts::exceptions::exception;

int
main(int argc, char *argv[])
{
    Parameter para;
    Options options("hprof_parser", " - Hprof parser program");
    options.add_options()
        ("d,debug", "Enable debugging") // a bool parameter
        ("s,dump-stack-trace", "Dump thread stack trace") // a bool parameter
        ("h,help", "Print help")
        ("file", "The file to process", value<string>())
        ;
    options.parse_positional({"file"});
    options.positional_help("file"); // do not show 'file' in help.

    ParseResult result;
    try {
        result = options.parse(argc, argv);
    } catch (const exception &e) {
        cout << "Error in parsing options: " << e.what() << endl;
        cout << options.help() << endl;
        exit(0);
    }

    if (result.count("help")) {
        cout << options.help() << endl;
        exit(0);
    }

    if (result.count("s")) {
        para.setDumpStackTrace();
    }

    // Must have 'file' input from command line.
    if (!result.count("file")) {
        cout << "Missing 'file' input"  << endl;
        cout << options.help() << endl;
        exit(0);
    }

    para.setFilePath(result["file"].as<string>());

    // para is not valid after this point.
    Hprof hprof(std::move(para));
    hprof.parse();

    hprof.displayThreads();

    return 0;
}