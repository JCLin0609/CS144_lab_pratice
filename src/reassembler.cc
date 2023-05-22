#include "reassembler.hh"
#include <iostream>
using namespace std;

void Reassembler::insert(uint64_t first_index, string data,
                         bool is_last_substring, Writer &output) {

  // output可用空間已滿
  if (output.available_capacity() == 0)
    return;

  //最後一個子字串
  if (is_last_substring) {
    is_end_received_ = true;
    end_bytes_index = first_index + data.size();
  }

  if (capacity_ == 0) {
    capacity_ = output.available_capacity();
    cout << "Init cap: " << capacity_ << endl;
  }

  first_unacceptable_ = first_unpopped_ + capacity_;
  first_unassembled_ = first_unpopped_ + bytes_pending();
  cout << "first unpopped: " << first_unpopped_ << endl;
  cout << "first unassembled: " << first_unassembled_ << endl;
  cout << "first unacceptable: " << first_unacceptable_ << endl;

  //超出邊界的資料直接丟棄
  if (first_index + data.size() < first_unpopped_ ||
      first_index >= first_unacceptable_)
    return;

  //將超出右邊界的資料切掉
  if (first_index + data.size() > first_unacceptable_)
    data = data.substr(0, first_unacceptable_ - first_index);
  //將低於左邊界的資料切掉
  if (first_index < first_unpopped_) {
    int data_end_byte = first_index + data.size();
    data = data.substr(first_unpopped_ - first_index,
                       data_end_byte - first_unpopped_);
    first_index = first_unpopped_;
  }

  //處理不同data重疊的問題
  auto iter = buffer_.lower_bound(first_index);
  if (iter != buffer_.begin()) {
    auto pre_iter = std::prev(iter);
    uint64_t pre_iter_end_pos = pre_iter->first + pre_iter->second.size();
    if (pre_iter_end_pos >= first_index) {
      if (pre_iter_end_pos >= first_index + data.size())
        data = pre_iter->second;
      else {
        std::string str;
        str = pre_iter->second.substr(0, first_index - pre_iter->first);
        data = str + data;
      }
      first_index = pre_iter->first;
      iter = buffer_.erase(pre_iter);
    }
  }
  while (iter != buffer_.end() && first_index + data.size() >= iter->first) {
    if (iter->first + iter->second.size() > first_index + data.size())
      data = data.substr(0, iter->first - first_index) + iter->second; // revise
    iter = buffer_.erase(iter);
  }
  buffer_[first_index] = data;

  auto iter_push = buffer_.begin();
  while (iter_push != buffer_.end() && iter_push->first == first_unpopped_) {
    output.push(iter_push->second);
    first_unpopped_ += iter_push->second.size();
    iter_push = buffer_.erase(iter_push);
  }

  if (first_unpopped_ == end_bytes_index && is_end_received_ && buffer_.empty())
    output.close();
}

uint64_t Reassembler::bytes_pending() const {
  // Your code here.
  uint64_t total = 0;
  for (const auto &tmp : buffer_)
    total += tmp.second.size();
  return total;
}
