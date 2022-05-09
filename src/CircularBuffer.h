#pragma once
#include <array>
#include <vector>

template <typename T, size_t N>
class CircularBuffer {
   public:
    auto& getBuffer() { return data; }
    void push(T const& value) {
        data[index++] = value;
        if (index == N) {
            index = 0;
        }
    }
    int count(T const& value) const {
        int found = 0;
        for (auto const& x : data) {
            if (x == value) {
                found++;
            }
        }
        return found;
    }
    void reset() {
        index = 0;
        data.fill({});
    }
    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

   private:
    std::array<T, N> data{};
    int index{};
};

template <typename T>
class DynamicCircularBuffer {
   public:
    DynamicCircularBuffer() {}

    DynamicCircularBuffer(size_t maxSize, int minIndex = 0)
        : maxSize(maxSize), minIndex(minIndex) {
        data.resize(maxSize);
    }
    auto const& getBuffer() { return data; }
    void setMaxSize(size_t maxSize) {
        this->maxSize = maxSize;
        data.resize(maxSize);
    }
    void setMinIndex(int i) {
        minIndex = i;
        if (index < minIndex) {
            index = minIndex;
        }
    }
    void push(T const& value) {
        data[index++] = value;
        if (index == maxSize) {
            index = minIndex;
        }
    }
    int count(T const& value) const {
        int found = 0;
        for (auto const& x : data) {
            if (x == value) {
                found++;
            }
        }
        return found;
    }
    int getCurrentIndex() const { return index; }
    void reset() {
        index = 0;
        data.fill({});
    }
    auto begin() { return data.begin(); }
    auto end() { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end() const { return data.end(); }

   private:
    std::vector<T> data{};
    size_t maxSize{};
    int index{};
    int minIndex{};
};