#include <stdexcept>

#include "byte_stream.hh"

using namespace std;

// que() is implemented by deque
// que2() is implemented by string
ByteStream::ByteStream(uint64_t capacity)
    : capacity_(capacity), closed_(false), err_(false), total_push_num(0),
      total_pop_num(0), // que(), // Method(deque)
      que2()            // Method(string)
{}

void Writer::push(string data) {
  if (is_closed() || data.empty() || err_)
    return;

  int len = min(available_capacity(), data.length());

  // Method(deque):
  // for (int i = 0; i < len; i++) {
  //   que.push_back(data[i]);
  // }

  // Method(string):
  que2 += data.substr(0, len);

  total_push_num += len;
}

void Writer::close() { closed_ = true; }

void Writer::set_error() { err_ = true; }

bool Writer::is_closed() const { return closed_; }

uint64_t Writer::available_capacity() const {
  // return capacity_ - que.size(); // Method(deque)
  return capacity_ - que2.size(); // Method(string)
}

uint64_t Writer::bytes_pushed() const { return total_push_num; }

string_view Reader::peek() const {
  // Method (deque): not work
  // if (!que.empty()) {
  //   std::string res;
  //   res = que.front();
  //   return res;
  // } else
  //   return "";

  // Method (deque):  why
  // return {&que.front(), 1};

  // Method (String)
  return que2;
}

bool Reader::is_finished() const {
  return closed_ && que2.empty(); // Method(string)
  // return closed_ && que.empty(); // Method(deque)
}

bool Reader::has_error() const { return err_; }

void Reader::pop(uint64_t len) {
  // int lengthh = min(len, que.size()); // Method(deque)
  int lengthh = min(len, que2.size()); // Method(string)

  // Method (deque)
  // string tmp = "";
  // for (int i = 0; i < lengthh; i++) {
  //   tmp += que.front();
  //   que.pop_front();
  // }

  // Method (string)
  que2 = que2.substr(lengthh);

  total_pop_num += lengthh;
}

uint64_t Reader::bytes_buffered() const {
  // return que.size(); // Method(deque)
  return que2.size(); // Method(string)
}

uint64_t Reader::bytes_popped() const { return total_pop_num; }
