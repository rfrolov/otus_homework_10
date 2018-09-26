#pragma once

#include <vector>
#include <fstream>
#include <ctime>

/// Интерфейс принтера.
struct IPrinter {
    /// Деструктор.
    virtual ~IPrinter() = default;

    /**
     * Выводит пулл команд.
     * @param time Время первой команды.
     * @param cmd_pool Пулл команд.
     */
    virtual void print_cmd_pool(std::time_t time, const std::vector<std::string> &cmd_pool) = 0;
};

/// Класс печати пула команд в консоль.
struct PrintConsole : public IPrinter {

    /**
     * Конструктор.
     * @param os Ссылка на поток вывода.
     */
    explicit PrintConsole(std::ostream &os = std::cout) : m_os{&os} {}

    /// Деструктор.
    ~PrintConsole() override = default;

    /**
     * Выводит пулл команд.
     * @param time Время первой команды.
     * @param cmd_pool Пулл команд.
     */
    void print_cmd_pool(std::time_t time, const std::vector<std::string> &cmd_pool) override {
        *m_os << "Bulk: ";
        for (const auto &cmd : cmd_pool) {
            *m_os << cmd << (&cmd != &cmd_pool.back() ? ", " : "");
        }
        *m_os << std::endl;
    }

private:
    std::ostream *m_os{};
};


/// Класс печати пула команд в файл.
struct PrintFile : public IPrinter {

    /// Деструктор.
    ~PrintFile() override {
        fs_.close();
    }

    /**
     * Выводит пулл команд.
     * @param time Время первой команды.
     * @param cmd_pool Пулл команд.
     */
    void print_cmd_pool(std::time_t time, const std::vector<std::string> &cmd_pool) override {
        std::string file_name = "bulk" + std::to_string(time) + ".log";
        if (fs_.is_open()) { fs_.close(); }
        fs_.open(file_name, std::ios::out);
        if (!fs_.is_open()) { return; }

        fs_ << "Bulk: ";
        for (const auto &cmd : cmd_pool) {
            fs_ << cmd << (&cmd != &cmd_pool.back() ? ", " : "");
        }
        fs_ << std::endl;
    }

private:
    std::fstream fs_{};
};