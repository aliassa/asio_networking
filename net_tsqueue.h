#pragma once
#include "net_common.h"

namespace net
{
    template <typename T>
    class tsqueue
    {
    public:
        tsqueue() = default;
        tsqueue(const tsqueue<T>&) = delete;
        virtual ~tsqueue() { clear(); }
    public:
        const T& front()
        {
            std::unique_lock lock(muxQueue);
            return deqQueue.front();
        }

        const T& back()
        {
            std::unique_lock lock(muxQueue);
            return deqQueue.back();
        }

        void push_back(const T& item)
        {
            std::scoped_lock lock(muxQueue);
            deqQueue.emplace_back(std::move(item));

            std::unique_lock<std::mutex> ul(muxBlocking);
            cvBlocking.notify_one();
        }

        void push_front(const T& item)
        {
            std::scoped_lock lock(muxQueue);
            deqQueue.emplace_front(std::move(item));

            std::unique_lock<std::mutex> ul(muxBlocking);
            cvBlocking.notify_one();
        }

        bool empty()
        {
            std::unique_lock lock(muxQueue);
            return deqQueue.empty();
        }

        size_t count()
        {
            std::unique_lock lock(muxQueue);
            return deqQueue.size();
        }

        void clear()
        {
            std::unique_lock lock(muxQueue);
            deqQueue.clear();
        }

        T pop_front()
        {
            std::unique_lock lock(muxQueue);
            auto t = std::move(deqQueue.front());
            deqQueue.pop_front();
            return t;
        }

        T pop_back()
        {
            std::unique_lock lock(muxQueue);
            auto t = std::move(deqQueue.back());
            deqQueue.pop_back();
            return t;
        }
        void wait()
			{
				while (empty())
				{
					std::unique_lock ul(muxBlocking);
					cvBlocking.wait(ul);
				}
			}
    protected:
        std::mutex muxQueue;
        std::deque<T> deqQueue;
        std::condition_variable cvBlocking;
        std::mutex muxBlocking;

    };

}