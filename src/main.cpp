#include <iostream>
#include "CmdParser.h"

#include "ThreadPool.h"

int main(/*int argc, char const *argv[]*/) {

  ThreadPool log{1};
  ThreadPool file{2};

  for(int i = 0; i < 15; ++i) {
    log.enqueue([] {
      Statistic statistic {2,1};
      std::cout << "log:" << std::this_thread::get_id() << std::endl;
      return statistic;
    });

    file.enqueue([] {
      Statistic statistic {1,2};
      std::cout << "file:" << std::this_thread::get_id() << std::endl;
      return statistic;
    });
  }

  auto log_statistic = log.Finish();
  std::cout << "log:\n";
  for(const auto& statistic : log_statistic) {
    std::cout << statistic;
  }
  std::cout << std::endl;

  std::cout << "file:\n";
  auto file_statistic = file.Finish();
  for(const auto& statistic : file_statistic) {
    std::cout << "thread " << (&statistic - &file_statistic[0]) << ":\n" << statistic;
  }



//    auto n{(argc == 2) ? static_cast<std::size_t>(strtoll(argv[1], nullptr, 0)) : 0};

//    if(n == 0) {
//        std::cout << "Wrong argument." << std::endl;
//        return 1;
//    }

//    CmdParser bulk(n);
//    bulk.process();
    return 0;
}
