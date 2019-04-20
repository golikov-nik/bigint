#include "big_integer.h"

template <typename T>
inline digit_t to_digit_t(T x) {
    return static_cast<digit_t>(x);
}

template <typename T>
inline overflow_t to_overflow_t(T x) {
    return static_cast<overflow_t>(x);
}

big_integer::big_integer(digit_t a) : sign(0), data{a} {
    strip();
}

big_integer::big_integer(int a) : sign(to_digit_t(-(a < 0))), data{to_digit_t(a)} {
    strip();
}

big_integer::big_integer() : big_integer(0) {}

big_integer::big_integer(std::string const& str) : big_integer() {
    if (!str.empty()) {
        int result_sign = 1;
        size_t pos = 0;
        if (str[0] == '-') {
            result_sign = -1;
            pos++;
        } else if (str[0] == '+') {
            pos++;
        }
        for (; pos < str.size(); pos++) {
            if (str[pos] < '0' || str[pos] > '9') {
                throw std::invalid_argument("invalid bigint representation");
            }
            (*this *= 10) += (str[pos] - '0') * result_sign;
        }
    }
}

big_integer& big_integer::operator+=(big_integer const& rhs) {
    assure_size(std::max(size(), rhs.size()) + 2);
    bool carry = false;
    for (size_t i = 0; i < size(); i++) {
        overflow_t sum = to_overflow_t(data[i]) + rhs.digit_at(i) + carry;
        data[i] = to_digit_t(sum);
        carry = sum > MAX_DIGIT;
    }
    sign = data.back();
    return strip();
}

big_integer& big_integer::strip() {
    while (!data.empty() && data.back() == sign) {
        data.pop_back();
    }
    return *this;
}

digit_t big_integer::digit_at(size_t i) const {
    return i < size() ? data[i] : sign;
}

void big_integer::assure_size(size_t sz) {
    data.resize(std::max(size(), sz), sign);
}

bool operator==(big_integer const& a, big_integer const& b) {
    return a.sign == b.sign && a.data == b.data;
}

big_integer& big_integer::operator-=(big_integer const& rhs) {
    return *this += -rhs;
}

big_integer big_integer::operator-() const {
    return ~*this + 1;
}

big_integer big_integer::operator+() const {
    return *this;
}

big_integer big_integer::operator~() const {
    big_integer copy = *this;
    copy.sign ^= MAX_DIGIT;
    for (digit_t& digit : copy.data) {
        digit ^= MAX_DIGIT;
    }
    return copy;
}

big_integer& big_integer::operator++() {
    return *this += 1;
}

big_integer operator+(big_integer a, big_integer const& b) {
    return a += b;
}

big_integer operator-(big_integer a, big_integer const& b) {
    return a -= b;
}

big_integer operator&(big_integer a, big_integer const& b) {
    return a &= b;
}

big_integer operator|(big_integer a, big_integer const& b) {
    return a |= b;
}

big_integer operator^(big_integer a, big_integer const& b) {
    return a ^= b;
}

big_integer& big_integer::operator*=(big_integer const& rhs) {
    return *this = *this * rhs;
}

big_integer& big_integer::apply_bitwise_op(big_integer const& rhs,
                                           std::function<digit_t(digit_t, digit_t)> const& op) {
    assure_size(rhs.size());
    for (size_t i = 0; i < size(); i++) {
        data[i] = op(data[i], rhs.digit_at(i));
    }
    sign = op(sign, rhs.sign);
    return strip();
}

size_t big_integer::size() const {
    return data.size();
}

big_integer& big_integer::operator&=(big_integer const& rhs) {
    return apply_bitwise_op(rhs, std::bit_and<>());
}

big_integer& big_integer::operator|=(big_integer const& rhs) {
    return apply_bitwise_op(rhs, std::bit_or<>());
}

big_integer& big_integer::operator^=(big_integer const& rhs) {
    return apply_bitwise_op(rhs, std::bit_xor<>());
}

bool operator!=(big_integer const& a, big_integer const& b) {
    return !(a == b);
}

big_integer big_integer::operator++(int) {
    big_integer result = *this;
    ++*this;
    return result;
}

big_integer& big_integer::operator--() {
    return *this -= 1;
}

big_integer big_integer::operator--(int) {
    big_integer result = *this;
    --*this;
    return result;
}

bool operator<(big_integer const& a, big_integer const& b) {
    if (a.sign != b.sign) {
        return a.is_negative();
    }
    if (a.size() != b.size()) {
        return (a.size() < b.size()) ^ a.is_negative();
    }
    return std::lexicographical_compare(
            a.data.rbegin(), a.data.rend(), b.data.rbegin(), b.data.rend()
    );
}

bool big_integer::is_negative() const {
    return sign != 0;
}

bool operator>(big_integer const& a, big_integer const& b) {
    return b < a;
}

bool operator<=(big_integer const& a, big_integer const& b) {
    return !(a > b);
}

bool operator>=(big_integer const& a, big_integer const& b) {
    return !(a < b);
}

big_integer big_integer::abs() const {
    return is_negative() ? -*this : *this;
}

//  both numbers are non-negative
big_integer multiply_by_digit(big_integer a, digit_t b) {
    digit_t carry = 0;
    for (size_t i = 0; i < a.size(); i++) {
        overflow_t product = to_overflow_t(a.data[i]) * b + carry;
        a.data[i] = to_digit_t(product);
        carry = to_digit_t(product >> DIGITS);
    }
    a.data.push_back(carry);
    return a.strip();
}

big_integer operator*(big_integer a, big_integer b) {
    digit_t result_sign = a.sign ^ b.sign;
    a = a.abs();
    b = b.abs();
    big_integer result;
    for (digit_t digit : b.data) {
        result += multiply_by_digit(a, digit);
        a.data.insert(a.data.begin(), 0);
    }
    return result.set_sign(result_sign);
}

big_integer& big_integer::operator/=(big_integer const& rhs) {
    return *this = *this / rhs;
}

big_integer& big_integer::operator%=(big_integer const& rhs) {
    return *this = *this % rhs;
}

big_integer operator/(big_integer const& a, big_integer const& b) {
    return divmod(a, b).first;
}

big_integer operator%(big_integer const& a, big_integer const& b) {
    return divmod(a, b).second;
}

std::pair<big_integer, big_integer> divmod(big_integer a, big_integer b) {
    if (b == 0) {
        throw std::invalid_argument("division by zero");
    }
    digit_t q_sign = a.sign ^ b.sign;
    digit_t r_sign = a.sign;
    a = a.abs();
    b = b.abs();
    int shift = 0;
    for (overflow_t b_first = b.data.back(); b_first < BASE / 2; shift++, b_first *= 2);
    a <<= shift;
    b <<= shift;
    big_integer q, r;
    q.assure_size(a.size());
    for (size_t i = a.size(); i-- > 0;) {
        r.data.insert(r.data.begin(), 0);
        r += a.data[i];
        overflow_t divi = (to_overflow_t(r.digit_at(b.data.size())) << DIGITS) | r.digit_at(b.data.size() - 1);
        digit_t quo = to_digit_t(divi / b.data.back());
        r -= b * quo;
        while (r.is_negative()) {
            r += b;
            quo--;
        }
        q.data[i] = quo;
    }
    r >>= shift;
    return {q.set_sign(q_sign).strip(), r.set_sign(r_sign)};
}

big_integer& big_integer::set_sign(digit_t new_sign) {
    return sign == new_sign ? strip() : *this = -*this;
}

big_integer& big_integer::operator<<=(int rhs) {
    if (rhs <= 0) {
        return *this >>= (-rhs);
    }
    data.insert(data.begin(), rhs / DIGITS, 0);
    data.push_back(sign);
    digit_t carry = 0;
    unsigned left = rhs % DIGITS;
    for (size_t i = 0; i < size(); i++) {
        std::tie(data[i], carry) = std::make_pair(
                (data[i] << left) | carry, data[i] >> (DIGITS - left)
        );
    }
    return strip();
}

big_integer operator<<(big_integer a, int b) {
    return a <<= b;
}

big_integer& big_integer::operator>>=(int rhs) {
    if (rhs < 0) {
        return *this <<= (-rhs);
    }
    if (rhs == 0) {
        return *this;
    }
    data.erase(data.begin(), data.begin() + std::min<size_t>(data.size(), rhs / DIGITS));
    digit_t carry = sign;
    unsigned left = rhs % DIGITS;
    for (size_t i = size(); i-- > 0;) {
        std::tie(data[i], carry) = std::make_pair(
                (carry << (DIGITS - left)) | (data[i] >> left), data[i] & ((1u << left) - 1)
        );
    }
    return strip();
}

big_integer operator>>(big_integer a, int b) {
    return a >>= b;
}

std::string to_string(big_integer a) {
    bool negative = a.is_negative();
    a = a.abs();
    std::string result;
    big_integer r;
    do {
        std::tie(a, r) = divmod(a, 10);
        result += static_cast<char>('0' + r.digit_at(0));
    } while (a != 0);
    if (negative) {
        result += '-';
    }
    return {result.rbegin(), result.rend()};
}
