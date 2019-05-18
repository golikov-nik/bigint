//  Copyright 2019 Nikita Golikov

#ifndef BIG_INTEGER_H_
#define BIG_INTEGER_H_

#include <string>
#include <functional>
#include <limits>
#include <utility>

#include "./my_vector.cpp"

using digit_t = uint32_t;
using overflow_t = uint64_t;

digit_t const MAX_DIGIT = std::numeric_limits<digit_t>::max();
unsigned const DIGITS = std::numeric_limits<digit_t>::digits;
overflow_t const BASE = static_cast<overflow_t>(MAX_DIGIT) + 1;

struct big_integer {
  big_integer();

  big_integer(big_integer const& other) = default;

  big_integer(digit_t a);

  big_integer(int a);

  explicit big_integer(std::string const& str);

  ~big_integer() = default;

  big_integer& operator=(big_integer const& other) = default;

  big_integer& operator+=(big_integer const& rhs);

  big_integer& operator-=(big_integer const& rhs);

  big_integer& add_sub(big_integer const& rhs, bool add);

  big_integer& operator*=(big_integer const& rhs);

  big_integer& operator/=(big_integer const& rhs);

  big_integer& operator%=(big_integer const& rhs);

  big_integer& apply_bitwise_op(big_integer const& rhs,
                                std::function<digit_t(digit_t,
                                                      digit_t)> const& op);

  big_integer& operator&=(big_integer const& rhs);

  big_integer& operator|=(big_integer const& rhs);

  big_integer& operator^=(big_integer const& rhs);

  big_integer& operator<<=(int rhs);

  big_integer& operator>>=(int rhs);

  big_integer operator+() const;

  big_integer operator-() const;

  big_integer abs() const;

  big_integer operator~() const;

  big_integer& operator++();

  big_integer operator++(int);

  big_integer& operator--();

  big_integer operator--(int);

  friend bool operator==(big_integer const& a, big_integer const& b);

  friend bool operator!=(big_integer const& a, big_integer const& b);

  friend bool operator<(big_integer const& a, big_integer const& b);

  friend bool operator>(big_integer const& a, big_integer const& b);

  friend bool operator<=(big_integer const& a, big_integer const& b);

  friend bool operator>=(big_integer const& a, big_integer const& b);

  friend big_integer operator+(big_integer a, big_integer const& b);

  friend big_integer operator-(big_integer a, big_integer const& b);

  friend big_integer operator*(big_integer a, big_integer b);

  friend std::pair<big_integer, big_integer>
  divmod(big_integer a, big_integer b);

  friend big_integer operator/(big_integer const& a, big_integer const& b);

  friend big_integer operator%(big_integer const& a, big_integer const& b);

  friend big_integer operator&(big_integer a, big_integer const& b);

  friend big_integer operator|(big_integer a, big_integer const& b);

  friend big_integer operator^(big_integer a, big_integer const& b);

  friend big_integer operator<<(big_integer a, int b);

  friend big_integer operator>>(big_integer a, int b);

  friend std::string to_string(big_integer a);

  friend std::ostream& operator<<(std::ostream& out, big_integer const& a) {
    return out << to_string(a);
  }

 private:
  digit_t sign;
  my_vector<digit_t> data;

  big_integer& strip();

  big_integer& set_sign(digit_t sign);

  size_t size() const;

  digit_t digit_at(size_t i) const;

  void assure_size(size_t sz);

  bool is_negative() const;

  friend big_integer multiply_by_digit(big_integer const& a, digit_t b);
};

#endif  // BIG_INTEGER_H_
