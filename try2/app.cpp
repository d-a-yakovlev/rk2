#include <atomic>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <semaphore>

using binary_semaphore = std::counting_semaphore<1>;

class PingPong
{
public:
    PingPong(size_t max_count)
    {
        max_count_.store(max_count);
    }

    void start()
    {
        from_start2ping.release();
    }

    void ping()
    {
        from_start2ping.acquire();
        while (count_.load() < max_count_.load())
        {
            if (count_.load() != 0) from_pong2ping.acquire();
            if (count_.load() >= max_count_.load()) continue;

            std::cout << "Ping " << std::endl;
            count_++;

            from_ping2pong.release();
        }
    }

    void pong()
    {
        while (count_.load() < max_count_.load())
        {
            from_ping2pong.acquire();
            if (count_.load() >= max_count_.load()) continue;

            std::cout << "Pong " << std::endl;
            count_++;

            from_pong2ping.release();
        }
    }

private:
    std::atomic<std::size_t> max_count_;
    std::atomic<std::size_t> count_{0};
    binary_semaphore from_ping2pong{0};
    binary_semaphore from_pong2ping{0};
    binary_semaphore from_start2ping{0};
};


int main(int argc, char* argv[])
{
    size_t N = std::stoi(argv[1]);
    PingPong p(N);

    std::thread pingThread(&PingPong::ping, &p);
    std::thread pongThread(&PingPong::pong, &p);

    p.start();

    pingThread.join();
    pongThread.join();
}