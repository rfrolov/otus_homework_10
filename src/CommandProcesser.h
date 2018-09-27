#pragma once

#include <iostream>
#include <sstream>
#include "Cmd.h"
#include "ThreadPool.h"

std::ostream &operator<<(std::ostream &os, const Cmd::Statistic &statistic) {
    return os << "строк: " << statistic.string_num
              << ", блоков: " << statistic.bulk
              << ", команд: " << statistic.cmd_num;
}

std::ostream &operator<<(std::ostream &os, const ThreadPool::Statistic &statistic) {
    return os << "блоков: " << statistic.bulk << ", команд: " << statistic.cmd_num;
}


/// Класс персера команд.
struct CommandProcesser {
    /**
     * Конструктор.
     * @param block_size Размер блока.
     * @param is Ссылка на поток ввода.
     */
    explicit CommandProcesser(const std::size_t block_size,
                              std::istream &is = std::cin,
                              std::ostream &os = std::cout) :
            m_is{is}
            , m_os{os}
            , m_cmd{block_size}
            , m_log_pool{1}
            , m_file_pool{2} {

        m_print_log_id = m_cmd.add_printer([this](std::time_t, Cmd::cmd_pull_t cmd_pull) {
            m_log_pool.enqueue([cmd_pull] { return print_log(cmd_pull); });
        });

        m_print_file_id = m_cmd.add_printer([this](std::time_t time, Cmd::cmd_pull_t cmd_pull) {
            static size_t rnd{};
            m_file_pool.enqueue([time, cmd_pull] { return print_file(time, rnd++, cmd_pull); });
        });
    }

    /// Деструктор.
    ~CommandProcesser() {
        m_cmd.del_printer(m_print_file_id);
        m_cmd.del_printer(m_print_log_id);
    }

    /// Обработать поток ввода.
    void process() {
        for (std::string cmd; getline(m_is, cmd);) {
            m_cmd.add_cmd(cmd);
        }

        auto cmd_statistic  = m_cmd.finish();
        auto log_statistic  = m_log_pool.finish();
        auto file_statistic = m_file_pool.finish();

        m_os << "main поток: " << cmd_statistic << std::endl;

        for (const auto &statistic: log_statistic) {
            m_os << "log  поток(" << (&statistic - &log_statistic[0] + 1)
                 << "), " << statistic << std::endl;
        }

        for (const auto &statistic: file_statistic) {
            m_os << "file поток(" << (&statistic - &file_statistic[0] + 1)
                 << "), " << statistic << std::endl;
        }

    }

private:
    std::istream &m_is;
    std::ostream &m_os;
    Cmd          m_cmd;
    size_t       m_print_log_id;
    size_t       m_print_file_id;
    ThreadPool   m_log_pool;
    ThreadPool   m_file_pool;
};