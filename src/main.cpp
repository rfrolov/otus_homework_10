#include <iostream>
#include "CmdParser.h"

int main(int argc, char const *argv[]) {
    auto n{(argc == 2) ? static_cast<std::size_t>(strtoll(argv[1], nullptr, 0)) : 0};

    if(n == 0) {
        std::cout << "Wrong argument." << std::endl;
        return 1;
    }

    CmdParser bulk(n);
    bulk.process();
    return 0;
}
