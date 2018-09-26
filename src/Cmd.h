#pragma once

#include <cassert>
#include <memory>
#include <algorithm>
#include "print.h"

struct Cmd {
    using printer_t = std::shared_ptr<IPrinter>;

    /**
     * Конструктор.
     * @param block_size Размер блока.
     */
    explicit Cmd(const std::size_t block_size) : m_block_size{block_size} {
        assert(block_size != 0);
    }

    /**
     * Добавить принтер.
     * @param printer Указатель на принтер.
     */
    void attach_printer(const printer_t &printer) {
        m_printers.emplace_back(printer);
    }

    /**
     * Удалить принтер
     * @param printer Указатель на принтер.
     */
    void detach_printer(const printer_t &printer) {
        auto it = std::find(m_printers.cbegin(), m_printers.cend(), printer);
        if (it != m_printers.cend()) {
            m_printers.erase(it);
        }
    }

    /**
     * Обрабатывает команду.
     * @param cmd Команда.
     */
    void add_cmd(const std::string &cmd) {
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
    void end() {
        print_pool();
    }

private:
    void print_pool() {
        if (m_cmd_pool.empty() || m_braces_num) {
            return;
        }

        for (const auto &printer:m_printers) {
            printer->print_cmd_pool(m_first_cmd_time, m_cmd_pool);
        }

        m_cmd_pool.clear();
    }

private:
    size_t m_block_size{};
    size_t m_braces_num{};
    std::vector<std::string> m_cmd_pool{};
    std::vector<printer_t> m_printers{};
    std::time_t m_first_cmd_time{};
};