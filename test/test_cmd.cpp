#include <chrono>
#include "gtest/gtest.h"
#include "Cmd.h"

struct TestPrinter : public IPrinter {

    ~TestPrinter() override = default;

    void print_cmd_pool(const std::vector<std::string> &cmd_pool) override {
        m_cmd_pool = cmd_pool;
    }

    void set_first_cmd_time(std::time_t time) override {
        m_first_time = time;
    }

    std::vector<std::string> get_pool() {
        return m_cmd_pool;
    }

    auto get_first_time() {
        return m_first_time;
    }

    void clear_cmd_pool() {
        m_cmd_pool.clear();
    }

private:
    std::time_t m_first_time{};
    std::vector<std::string> m_cmd_pool{};
};


TEST(cmd, attach) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test");
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{"test"});
}

TEST(cmd, detach) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.detach_printer(printer);
    cmd.add_cmd("test");
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{});
}

TEST(cmd, first_time_true) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test");
    EXPECT_NE(printer->get_first_time(), std::time_t{});
}

TEST(cmd, first_time_false) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    EXPECT_EQ(printer->get_first_time(), std::time_t{});
}

TEST(cmd, second_time_true) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test_1");
    std::time_t first_time = printer->get_first_time();

    for(std::time_t time = std::time(nullptr); std::time(nullptr) == time;);

    cmd.add_cmd("test_2");
    EXPECT_NE(printer->get_first_time(), first_time);
}

TEST(cmd, second_time_false) {
    Cmd cmd{3};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test_1");
    std::time_t first_time = printer->get_first_time();
    cmd.add_cmd("test_2");
    EXPECT_EQ(printer->get_first_time(), first_time);
}

TEST(cmd, add_cmd_true_block_size) {
    Cmd cmd{3};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test_1");
    cmd.add_cmd("test_2");
    cmd.add_cmd("test_3");
    std::vector<std::string> v{"test_1", "test_2", "test_3"};
    EXPECT_EQ(printer->get_pool(), v);
}

TEST(cmd, add_cmd_false_block_size) {
    Cmd cmd{3};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test_1");
    cmd.add_cmd("test_2");
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{});
}

TEST(cmd, add_cmd_true_block) {
    Cmd cmd{10};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("{");
    cmd.add_cmd("test");
    cmd.add_cmd("}");
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{"test"});
}

TEST(cmd, add_cmd_true_block_block) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("{");
    cmd.add_cmd("test_1");
    cmd.add_cmd("{");
    cmd.add_cmd("test_2");
    cmd.add_cmd("test_3");
    cmd.add_cmd("}");
    cmd.add_cmd("}");
    std::vector<std::string> v{"test_1", "test_2", "test_3"};
    EXPECT_EQ(printer->get_pool(), v);
}

TEST(cmd, add_cmd_false_block) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("{");
    cmd.add_cmd("test_1");
    cmd.add_cmd("test_2");
    cmd.add_cmd("test_3");
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{});
}

TEST(cmd, add_cmd_false_block_block) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("{");
    cmd.add_cmd("test_1");
    cmd.add_cmd("{");
    cmd.add_cmd("test_2");
    cmd.add_cmd("test_3");
    cmd.add_cmd("}");
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{});
}

TEST(cmd, end_true) {
    Cmd cmd{2};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test");
    cmd.end();
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{"test"});
}

TEST(cmd, end_false_empty) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("test");
    printer->clear_cmd_pool();
    cmd.end();
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{});
}

TEST(cmd, end_false_braces) {
    Cmd cmd{1};
    auto printer = std::make_shared<TestPrinter>();
    cmd.attach_printer(printer);
    cmd.add_cmd("{");
    cmd.add_cmd("test");
    printer->clear_cmd_pool();
    cmd.end();
    EXPECT_EQ(printer->get_pool(), std::vector<std::string>{});
}