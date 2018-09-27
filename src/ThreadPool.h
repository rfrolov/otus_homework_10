#pragma once


#include <iostream>

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

struct Statistic {
    size_t a;
    size_t b;

    Statistic& operator += (const Statistic& other) {
      a += other.a;
      b += other.b;
      return *this;
    }
};

std::ostream &operator<<(std::ostream &os, const Statistic &statistic) {
    return os << "a: " << statistic.a << "\n" << "b: " << statistic.b << "\n";
}

struct ThreadPool {
    using func_t = std::function<Statistic()>;

    ThreadPool(size_t threads) { //TODO: Добавить максимальный размер очереди.
      statistics.resize(threads);
      workers.reserve(threads);

      for(size_t i = 0;i<threads;++i)
        workers.emplace_back([this, i]
        {
          for(;;)
          {
            func_t task;
            {
              std::unique_lock<std::mutex> lock(queue_mutex);
              condition.wait(lock, [this, i]{ return stop || !task_queue.empty(); });
              if(stop && task_queue.empty()) {
                return;
              }
              task = std::move(task_queue.front());
              task_queue.pop();
            }
            auto statistic = task();
            {
              std::unique_lock<std::mutex> lock(statistic_mutex);
              statistics[i] += statistic;
            }
          }
        }
        );
    }

    template<typename F, typename... Args>
    void enqueue(F&& f, Args&&... args)
    {
      {
        std::unique_lock<std::mutex> lock(queue_mutex);

        if(stop){
          throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        task_queue.emplace(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
      }
      condition.notify_one();
    }


    const auto& Finish() {
      {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
      }
      condition.notify_all();
      for(std::thread &worker: workers){
        if(worker.joinable()){
          worker.join();
        }
      }

      // Все потоки остановлены, поэтому мьютекс здесь не нужен.
      return statistics;
    }

    ~ThreadPool() {
      Finish();
    }


  private:
    std::vector<std::thread> workers{};

    std::mutex queue_mutex{};
    std::condition_variable condition{};
    std::queue<func_t> task_queue{};
    bool stop{};

    std::mutex statistic_mutex{};
    std::vector<Statistic> statistics{};

};

