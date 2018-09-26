#pragma once

#include <sstream>
#include "Cmd.h"

/// Класс персера команд.
struct CmdParser {
    /**
     * Конструктор.
     * @param block_size Размер блока.
     * @param is Ссылка на поток ввода.
     */
    explicit CmdParser(const std::size_t block_size, std::istream &is = std::cin) :
            m_is{&is}
            , m_cmd{block_size}
            , m_print_console{std::make_shared<PrintConsole>()}
            , m_print_file{std::make_shared<PrintFile>()} {
        assert(m_is != nullptr);

        m_cmd.attach_printer(m_print_console);
        m_cmd.attach_printer(m_print_file);
    }

    /// Деструктор.
    ~CmdParser() {
        m_cmd.detach_printer(m_print_file);
        m_cmd.detach_printer(m_print_console);
    }

    /// Обработать поток ввода.
    void process() {
        for (std::string cmd; getline(*m_is, cmd);) {
            m_cmd.add_cmd(cmd);
        }
        m_cmd.end();
    }

private:
    std::istream                  *m_is{};
    Cmd                           m_cmd;
    std::shared_ptr<PrintConsole> m_print_console{};
    std::shared_ptr<PrintFile>    m_print_file{};
};