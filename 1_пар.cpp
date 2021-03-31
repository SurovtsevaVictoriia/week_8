#include <thread>
#include <iostream>
#include <random>
#include <mutex>

std::mutex m_mutex;

auto s_func(int & N, int & S) {

    std::random_device rd;
    std::mt19937 mersenne(rd());
    std::uniform_real_distribution<> real(0.0, 1.0);


    for (int count = 0; count < N / 2; ++count)
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
    int N = 10000000;

    
    std::thread thread1(s_func, std::ref(N), std::ref(S));
    std::thread thread2(s_func, std::ref(N),std::ref( S));

    thread1.join();
    thread2.join();

    std::cout << "pi: " << 4 * (float)S / (float)N << std::endl;


}