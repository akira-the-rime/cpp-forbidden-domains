#include <algorithm>
#include <concepts>
#include <cstddef> 
#include <functional>
#include <iostream>
#include <iterator>
#include <ranges>
#include <span>
#include <string>
#include <sstream>
#include <string_view>
#include <utility>
#include <vector>

class Domain {
public:
    Domain() = default;
    Domain(std::string data) noexcept
        : data_(std::make_move_iterator(data.rbegin()), std::make_move_iterator(data.rend())) {
    }

    bool operator==(const Domain& other) const noexcept {
        return this->data_ == other.data_;
    }

    bool operator<(const Domain& other) const noexcept {
        return this->data_ < other.data_;
    }

    bool IsSubdomain(const Domain& other) const noexcept {
        return this->data_.starts_with(other.data_);
    }

private:
    std::string data_;
};

class DomainChecker {
public:
    template <std::random_access_iterator RandomIt>
    DomainChecker(RandomIt begin, RandomIt end) noexcept
        : to_check_(begin, end) {
    }

    bool IsForbidden(const Domain& other) const noexcept {
        auto what = std::ranges::upper_bound(to_check_, other, std::less{});

        if (what != to_check_.begin()) {
            return other.IsSubdomain(*--what);
        }

        return false;
    }

private:
    std::span<const Domain> to_check_;
};

std::vector<Domain> ReadDomains(std::istream& is, const std::size_t count, bool be_sorted = false) {
    using namespace std::literals;

    std::vector<Domain> to_return(count);
    for (auto index : std::views::iota(0ULL, count)) {
        std::string read;
        std::getline(is, read);
        to_return[index] = std::move("."s + read);
    }

    if (be_sorted) {
        std::ranges::sort(to_return, std::less{});
        auto [begin, end] = std::ranges::unique(to_return, [](const Domain& what, const Domain& value) {
            return value.IsSubdomain(what);
        });

        to_return.erase(begin, end);
    }

    return to_return;
}

template <typename Number>
Number ReadNumberOnLine(std::istream& input) {
    std::string line;
    std::getline(input, line);

    Number num;
    std::istringstream(line) >> num;

    return num;
}

int main() {
    const std::vector<Domain> forbidden_domains = ReadDomains(std::cin, ReadNumberOnLine<std::size_t>(std::cin), true);
    DomainChecker checker(forbidden_domains.begin(), forbidden_domains.end());

    const std::vector<Domain> test_domains = ReadDomains(std::cin, ReadNumberOnLine<std::size_t>(std::cin));
    for (const Domain& domain : test_domains) {
        using namespace std::literals;

        std::cout << (checker.IsForbidden(domain) ? "Bad"sv : "Good"sv) << std::endl;
    }
}