#pragma once

#include <functional>
#include "print.h"

struct Cmd {
    using cmd_pull_t   = std::vector<std::string>;
    using fn_printer_t = std::function<void(std::time_t, const cmd_pull_t &)>;

    struct Statistic {
        size_t string_num;
        size_t bulk;
        size_t cmd_num;

        Statistic &operator+=(const Statistic &other) {
            string_num += other.string_num;
            bulk += other.bulk;
            cmd_num += other.cmd_num;
            return *this;
        }
    };

    /**
     * Конструктор.
     * @param block_size Размер блока.
     */
    explicit Cmd(const std::size_t block_size) : m_block_size{block_size} {
        if(block_size == 0) {
            throw std::runtime_error("block_size must be greater than zero");
        }
    }

    /**
     * Добавить принтер.
     * @param printer Указатель на принтер.
     */
    size_t add_printer(const fn_printer_t &printer) {
        m_printers.emplace_back(printer);
        return m_printers.size();
    }

    /**
     * Удалить принтер
     * @param printer Указатель на принтер.
     */
    void del_printer(size_t printer_id) {
        if (printer_id < m_printers.size()) {
            m_printers.erase(m_printers.begin() + printer_id);
        }
    }

    /**
     * Обрабатывает команду.
     * @param cmd Команда.
     */
    void add_cmd(const std::string &cmd) {
        ++m_statistic.string_num;
        if (cmd == "{") {
            print_pool();
            ++m_braces_num;
        } else if (cmd == "}") {
            if (m_braces_num) {
                --m_braces_num;
                print_pool();
            }
        } else {
            if (m_cmd_pool.empty()) {
                m_first_cmd_time = std::time(nullptr);
            }
            m_cmd_pool.emplace_back(cmd);
            if (m_cmd_pool.size() == m_block_size) {
                print_pool();
            }
        }
    }

    /// Выводит оставшиеся команды.
    const auto& finish() {
        print_pool();
        return m_statistic;
    }

private:
    void print_pool() {
        if (m_cmd_pool.empty() || m_braces_num) {
            return;
        }

        for (const auto &printer:m_printers) {
            printer(m_first_cmd_time, m_cmd_pool);
        }

        ++m_statistic.bulk;
        m_statistic.cmd_num += m_cmd_pool.size();
        m_cmd_pool.clear();
    }

private:
    size_t                    m_block_size{};
    size_t                    m_braces_num{};
    cmd_pull_t                m_cmd_pool{};
    std::vector<fn_printer_t> m_printers{};
    std::time_t               m_first_cmd_time{};
    Statistic                 m_statistic{};
};

