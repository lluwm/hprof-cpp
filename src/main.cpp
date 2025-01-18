#include <iostream>

#include "cxxopts.hpp"

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
    Options options(argv[0], " - Hprof parser program");
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
        cout << "Dump thread stack trace." << endl;
    }

    if (result.count("file")) {
        cout << "Hprof file." << endl;
    }

    return 0;
}