#pragma once

#include <array>
#include <atomic>
#include <cstddef>

template<typename T, std::size_t N>
class RingBuffer {
public:
    RingBuffer() : head_(0), tail_(0) {}

    // push returns false if buffer is full
    bool push(const T &value) {
        std::size_t head = head_.load(std::memory_order_relaxed);
        std::size_t next = (head + 1) % N;
        if (next == tail_.load(std::memory_order_acquire)) {
            return false; // full
        }
        buffer_[head] = value;
        head_.store(next, std::memory_order_release);
        return true;
    }

    // pop returns false if buffer is empty
    bool pop(T &out) {
        std::size_t tail = tail_.load(std::memory_order_relaxed);
        if (tail == head_.load(std::memory_order_acquire)) {
            return false; // empty
        }
        out = buffer_[tail];
        std::size_t next = (tail + 1) % N;
        tail_.store(next, std::memory_order_release);
        return true;
    }

private:
    std::array<T, N> buffer_;
    std::atomic<std::size_t> head_;
    std::atomic<std::size_t> tail_;
};