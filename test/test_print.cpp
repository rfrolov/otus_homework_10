#include "gtest/gtest.h"
#include "print.h"

TEST(print, PrintConsole) {
    std::stringstream is{};
    PrintConsole print{is};

    std::vector<std::string> cmd_pool{"first", "second", "third"};
    print.print_cmd_pool(cmd_pool);

    EXPECT_EQ(is.str(), "Bulk: first, second, third\n");
}

TEST(print, PrintFile) {
    PrintFile print{};
    std::time_t time = std::time(nullptr);
    print.set_first_cmd_time(time);

    std::vector<std::string> cmd_pool{"first", "second", "third"};
    print.print_cmd_pool(cmd_pool);

    std::fstream fs{};
    std::string file_name = "bulk" + std::to_string(time) + ".log";
    fs.open(file_name, std::ios::in);
    EXPECT_TRUE(fs.is_open());

    std::string str{};
    std::getline(fs, str);

    EXPECT_EQ(str, "Bulk: first, second, third");
}
