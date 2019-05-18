//  Copyright 2019 Nikita Golikov

#ifndef MY_VECTOR_H_
#define MY_VECTOR_H_

#include <cstddef>
#include <vector>
#include <memory>
#include <utility>

template <typename T>
struct my_vector {
  explicit my_vector(T const& x);

  T* data();

  T const* data() const;

  T& operator[](size_t at);

  T const& operator[](size_t at) const;

  T& back();

  T const& back() const;

  bool empty() const;

  void pop_back();

  void resize(size_t n, T const& val);

  bool operator==(my_vector<T> const& other) const;

  size_t size() const;

  void swap(my_vector<T>& other);

  void push_front(T const& val, size_t n = 1);

  void push_back(T const& val);

  void pop_front(size_t n);

 private:
  size_t _size{};
  bool is_small;

  T small;
  std::shared_ptr<std::vector<T>> vec;

  void make_big();
  void assure_modifiable();
};

#endif  // MY_VECTOR_H_
