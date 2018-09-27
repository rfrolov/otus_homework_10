#pragma once

#include <fstream>
#include <iostream>
#include "ThreadPool.h"

ThreadPool::Statistic print_log(const std::vector<std::string> &cmd_pool, std::ostream &os = std::cout) {
    os << "Bulk: ";
    for (const auto &cmd : cmd_pool) {
        os << cmd << (&cmd != &cmd_pool.back() ? ", " : "");
    }
    os << std::endl;
    return ThreadPool::Statistic{1, cmd_pool.size()};
}

ThreadPool::Statistic print_file(std::time_t time, size_t rnd, const std::vector<std::string> &cmd_pool) {
    std::fstream fs{};
    std::string  file_name = "bulk" + std::to_string(time) + "." + std::to_string(rnd) + ".log";
    fs.open(file_name, std::ios::out);
    if (!fs.is_open()) { return ThreadPool::Statistic{0, 0}; }

    fs << "Bulk: ";
    for (const auto &cmd : cmd_pool) {
        fs << cmd << (&cmd != &cmd_pool.back() ? ", " : "");
    }
    fs << std::endl;
    return ThreadPool::Statistic{1, cmd_pool.size()};
}