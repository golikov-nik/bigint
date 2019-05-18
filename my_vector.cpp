//  Copyright 2019 Nikita Golikov

#include <iostream>
#include "./my_vector.h"

template <typename T>
my_vector<T>::my_vector(T const& x) : _size(1), is_small(true), small(x) {
}

template <typename T>
T& my_vector<T>::operator[](size_t at) {
  assure_modifiable();
  return is_small ? small : (*vec)[at];
}

template <typename T>
T const& my_vector<T>::operator[](size_t at) const {
  return is_small ? small : (*vec)[at];
}

template <typename T>
T& my_vector<T>::back() {
  return (*this)[size() - 1];
}

template <typename T>
T const& my_vector<T>::back() const {
  return (*this)[size() - 1];
}

template <typename T>
bool my_vector<T>::empty() const {
  return size() == 0;
}

template <typename T>
void my_vector<T>::pop_back() {
  assure_modifiable();
  if (is_small) {
    small.~T();
  } else {
    vec->pop_back();
  }
  _size--;
}

template <typename T>
void my_vector<T>::resize(size_t n, T const& val) {
  if (n == size() || n == 0) {
    return;
  }
  assure_modifiable();
  if (is_small) {
    if (n == 1) {
      small = val;
    } else {
      make_big();
    }
  }
  if (!is_small) {
    vec->resize(n, val);
  }
  _size = n;
}

template <typename T>
bool my_vector<T>::operator==(my_vector<T> const& other) const {
  if (size() != other.size()) {
    return false;
  }
  if (empty()) {
    return true;
  }
  if (size() > 1) {
    return *vec == *other.vec;
  }
  return back() == other.back();
}

template <typename T>
size_t my_vector<T>::size() const {
  return _size;
}

template <typename T>
void my_vector<T>::swap(my_vector<T>& other) {
  assure_modifiable();
  other.assure_modifiable();

  using std::swap;

  swap(_size, other._size);
  swap(is_small, other.is_small);
  swap(small, other.small);
  swap(vec, other.vec);
}

template <typename T>
void my_vector<T>::push_front(T const& val, size_t n) {
  if (n == 0) {
    return;
  }
  assure_modifiable();

  size_t new_size = size() + n;
  if (is_small) {
    if (new_size == 1) {
      small = val;
    } else {
      make_big();
    }
  }
  if (!is_small) {
    vec->insert(vec->begin(), n, val);
  }
  _size += n;
}

template <typename T>
void my_vector<T>::push_back(T const& val) {
  assure_modifiable();

  if (is_small) {
    if (empty()) {
      small = val;
    } else {
      make_big();
    }
  }
  if (!is_small) {
    vec->push_back(val);
  }
  _size++;
}

template <typename T>
void my_vector<T>::pop_front(size_t n) {
  if (n == 0) {
    return;
  }
  assure_modifiable();

  if (is_small) {
    small.~T();
  } else {
    vec->erase(vec->begin(), vec->begin() + n);
  }
  _size -= n;
}

template <typename T>
void my_vector<T>::make_big() {
  if (!is_small) {
    return;
  }
  is_small = false;
  vec = std::make_shared<std::vector<T>>(empty() ? std::vector<T>() :
                         std::vector<T>{std::move(small)});
}

template <typename T>
void my_vector<T>::assure_modifiable() {
  if (is_small || vec.unique()) {
    return;
  }
  std::vector<T> copy = *vec;
  vec = std::make_shared<std::vector<T>>(std::move(copy));
}



