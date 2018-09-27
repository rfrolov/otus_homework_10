#include <iostream>
#include "CommandProcesser.h"

int main(int argc, char const *argv[]) {
    auto n{(argc == 2) ? static_cast<std::size_t>(strtoll(argv[1], nullptr, 0)) : 0};

    if(n == 0) {
        std::cout << "Wrong argument." << std::endl;
        return 1;
    }

    CommandProcesser app(n);
    app.process();
    return 0;
}
