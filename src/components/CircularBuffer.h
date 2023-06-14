#pragma once

#include <array>
#include <cstring>

#include "pico/stdlib.h"


using namespace std;


template<typename item_t, size_t  buffer_capacity>
class CircularBuffer {
private:
    array<item_t, buffer_capacity> buffer_{};
    size_t write_index_ = 0;
    size_t read_index_ = 0;
    size_t size_ = 0;

public:

    auto max_size() { return buffer_.size(); }

    auto size() { return size_; }

    bool empty() { return size_ == 0; }

    bool full() { return size_ == buffer_.size(); }

    bool add_element(item_t &el) {
        if (size_ == buffer_.size()) {
            return false;
        } else {
            memcpy(&buffer_[write_index_], &el, sizeof(el));
            write_index_ = (write_index_ + 1) % buffer_.size();
            size_++;
            return true;
        }
    }

    bool remove_element(item_t &el) {
        if (size_ > 0) {
            memcpy(&el, &buffer_[read_index_], sizeof(el));
            read_index_ = (read_index_ + 1) % buffer_.size();
            --size_;
            return true;
        } else {
            return false;
        }
    }
};

