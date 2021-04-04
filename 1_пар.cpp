#include <thread>
#include <iostream>
#include <random>
#include <mutex>
#include <functional>

std::mutex m_mutex;

auto s_func(int& N, int& S, int thread_num) {

    std::random_device rd;
    std::mt19937 mersenne(rd());
    std::uniform_real_distribution<> real(0.0, 1.0);


    for (int count = 0; count < N / thread_num; ++count)
    {

        auto x = real(mersenne);
        auto y = real(mersenne);

        m_mutex.lock();

        if (x * x + y * y < 1) { S++; };

        m_mutex.unlock();
    }

}

int main() {


    int S = 0;
    int N = 10000;

    int thread_num = (std::thread::hardware_concurrency() != 0) ? std::thread::hardware_concurrency() : 8;

    std::vector < std::thread > threads;

    for (std::size_t i = 0; i < thread_num; ++i)
    {
        threads.push_back(std::thread(s_func, std::ref(N), std::ref(S), std::ref(thread_num)));
    }

    std::for_each(threads.begin(), threads.end(),
        std::mem_fn(&std::thread::join));


    std::cout << "pi: " << 4 * (float)S / (float)N << std::endl;

    return 0;

}
