#include <iostream>
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <unordered_map>
#include <unordered_set>
#include <chrono>
#include <vector>
#include <string>
#include <ranges>
#include <string_view>
#include <set>

#include <ctre.hpp>


#include "Input.hpp"

void day1() {
    auto [column1, column2] = Input::Get2ColumnData<std::uint64_t>("infile.txt");

    auto begin = std::chrono::steady_clock::now();
    std::sort(column1.begin(), column1.end());
    std::sort(column2.begin(), column2.end());

    std::uint64_t total{0}; // Part 1
    std::unordered_map<std::uint64_t, std::uint64_t> occurrence_map; // Part 2
    for (std::size_t i{0}; i < column1.size(); i++) {
        total += std::abs(static_cast<int64_t>(column1[i]) - static_cast<int64_t>(column2[i]));
        occurrence_map[column2[i]]++;
    }

    std::uint64_t total2{0};
    for (std::size_t i{0}; i < column1.size(); i++) {
        auto it = occurrence_map.find(column1[i]);
        if (it != occurrence_map.end()) {
            total2 +=  column1[i] * it->second;
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << total << ' ' << total2 << " took " << duration <<"\n";
}

bool safe_report(std::vector<int> report)
{
    auto asc = std::is_sorted(report.begin(), report.end());
    auto desc = std::is_sorted(report.rbegin(), report.rend());
    if (!asc && !desc)
    {
        return false;
    }
    bool safe{true};
    constexpr int MIN{1}, MAX{3};
    for (std::size_t i{0}; i < report.size() - 1; i++) {
        if (!safe) continue;
        auto curr = report[i];
        auto next = report[i + 1];
        if (std::abs(curr - next) < MIN || std::abs(next - curr) > MAX) {
            safe = false;
        }
    }
    return safe;
}

void day2() { // TODO add Part 1 back
    auto reports = Input::GetStringData("infile.txt");
    std::uint64_t safe_reports{0};
    auto begin = std::chrono::steady_clock::now();
    for (const auto& string_report : reports) {
        std::istringstream is(string_report);
        std::vector<int> report;
        int number;
        while (is >> number) {
            report.push_back(number);
        }
        bool safe = safe_report(report);
        if (!safe)
        {
            for (std::size_t i{0}; i < report.size(); i++) {
                auto temp = report;
                temp.erase(temp.begin() + i);
                if (safe_report(temp)) {
                    safe = true;
                    break;
                }
            }
        }

        if (safe)
        {
            safe_reports++;
        }
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << safe_reports << " after " << duration;
}

std::uint64_t day3_1(const std::string& s) {
    using namespace ctre::literals;
    std::uint64_t result{0};
    auto start = s.cbegin();
    while (auto m = ctre::search<"mul\\(([0-9]+),([0-9]+)\\)">(start, s.cend())) {
        result += m.get<1>().to_number() * m.get<2>().to_number();
        start = m.get<0>().end();
    }
    return result;
}

std::uint64_t day3_2(const std::string& s) {
    bool enabled{true};
    std::uint64_t result{0};
    for (auto match : ctre::search_all<"do\\(\\)|don't\\(\\)|mul\\(([0-9]+),([0-9]+)\\)">(s)) {
        std::string_view view = match.get<0>();
        if (view.starts_with("mul")) {
            if (enabled) {
                result += match.get<1>().to_number() * match.get<2>().to_number();
            }
        }
        if (view.starts_with("don'")) {
            enabled = false;
        }
        if (view.starts_with("do(")) {
            enabled = true;
        }
    }
    return result;
}

std::uint64_t day4_1(const std::vector<std::string>& grid) {
    const auto rows = grid.size();
    const auto cols = grid[0].size();
    const std::string XMAS = "XMAS";

    const std::vector<std::pair<int, int>> directions = {
        {0, 1}, {0, -1},  // right, left
        {1, 0}, {-1, 0},  // up, down
        {1, 1}, {-1, -1}, // diagonal down-right, diagonal up-left
        {1, -1}, {-1, 1} // diagonal down-left, diagonal up-right
    };

    auto isXMASInDirection = [&](const std::size_t r, const std::size_t c, const int dr, const int dc) -> bool {
        for (std::size_t i = 0; i < XMAS.size(); ++i) {
            const auto next_r = r + i * dr;
            const auto next_c = c + i * dc;
            if (next_r >= rows || next_c >= cols || grid[next_r][next_c] != XMAS[i]) {
                return false;
            }
        }
        return true;
    };

    std::size_t count = 0;
    for (std::size_t r = 0; r < rows; ++r) {
        for (std::size_t c = 0; c < cols; ++c) {
            if (grid[r][c] == XMAS[0]) {
                for (const auto& [dr, dc] : directions) {
                    count += isXMASInDirection(r, c, dr, dc);
                }
            }
        }
    }
    return count;
}

std::uint64_t day4_2(const std::vector<std::string>& grid) {
    const auto rows = grid.size();
    const auto cols = grid[0].size();

    auto isMASCross = [&](const std::size_t r, const std::size_t c) -> bool {
        constexpr std::array<std::pair<int, int>, 3> DIAG1 = {{{-1, -1}, {0, 0}, {1, 1}}}; // Left, Center, Right
        constexpr std::array<std::pair<int, int>, 3> DIAG2 = {{{-1, 1}, {0, 0}, {1, -1}}}; // Right, Center, Left
        constexpr std::string_view MAS = "MAS";
        constexpr std::string_view SAM = "SAM";

        auto matchesDiagonal = [&](const std::array<std::pair<int, int>, 3>& diagonal, std::string_view pattern) -> bool
        {
            for (std::size_t i = 0; i < pattern.size(); ++i) {
                const int next_r = r + diagonal[i].first;
                const int next_c = c + diagonal[i].second;
                if (next_r < 0 || next_r >= static_cast<int>(rows) ||
                    next_c < 0 || next_c >= static_cast<int>(cols) ||
                    grid[next_r][next_c] != pattern[i])
                    {
                        return false;
                    }
            }
            return true;
        };

        return
            (matchesDiagonal(DIAG1, MAS) && matchesDiagonal(DIAG2, SAM)) ||
            (matchesDiagonal(DIAG1, MAS) && matchesDiagonal(DIAG2, MAS)) ||
            (matchesDiagonal(DIAG1, SAM) && matchesDiagonal(DIAG2, SAM)) ||
            (matchesDiagonal(DIAG1, SAM) && matchesDiagonal(DIAG2, MAS));
    };

    std::size_t count = 0;
    for (std::size_t r = 1; r < rows - 1; ++r) {
        for (std::size_t c = 1; c < cols - 1; ++c) {
            count += isMASCross(r,c);
        }
    }
    return count;
}

std::uint64_t day5(const std::vector<std::string>& pages) { // Todo split Part 1 and 2
    auto begin = std::chrono::steady_clock::now();
    // First, parse the rules
    std::unordered_map<std::uint32_t, std::set<std::uint32_t>> rules;
    std::size_t page_offset{0};
    for (const auto& page : pages) {
        if (page.empty()) {
            break; // Finished reading the rules
        }
        std::string_view sv(page);
        std::uint32_t page_number, page_rule;
        std::from_chars(sv.data(), sv.data() + 2, page_number);
        std::from_chars(sv.data() + 3, sv.data() + sv.size(), page_rule);
        if (rules.contains(page_number)) {
            rules[page_number].insert(page_rule);
        } else {
            rules[page_number] = {page_rule};
        }
        ++page_offset;
    }

    auto parse_line = [](const std::string& input) -> std::vector<int> {
        std::vector<int> result;
        std::stringstream ss(input);
        std::string token;
        while (std::getline(ss, token, ',')) {
            result.push_back(std::stoi(token));
        }
        return result;
    };

    auto sorting = [&](auto a, auto b){ return rules.at(a).contains(b); };

    // Now, check the updates
    std::uint64_t total{0};
    std::uint64_t total2{0};
    for (std::size_t i = page_offset + 1; i < pages.size(); ++i) {
        auto split = parse_line(pages[i]);
        if (!std::is_sorted(split.begin(), split.end(), sorting)) {
            total += split[split.size()/2]; // Part 1
        } else {
            std::sort(split.begin(), split.end(), sorting);
            total2 += split[split.size()/2]; // Part 2
        }
    }

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
    std::cout << total << ' ' << total << " took " << duration <<"\n";
    return total;
}

std::uint64_t day6(std::vector<std::string> map) {

    constexpr int directions_sz{4};
    std::array<std::pair<int, int>, directions_sz> directions = {{{-1, 0},  // Up
                                                                {0, 1},        // Right
                                                                {1, 0},        // Down
                                                                {0, -1}}};     // Left

    std::set<std::pair<int, int>> obstacles;
    std::pair<int, int> position;
    int compass{0}; // 0 - Up, 1 - Right, etc...
    std::pair<int, int> direction = directions[compass];
    for (std::size_t i = 0; i < map.size(); ++i) {
        for (std::size_t j = 0; j < map[0].size(); ++j) {
            if (map[i][j] == '#') { obstacles.insert(std::make_pair(i, j)); }
            if (map[i][j] == '^') { position = std::make_pair(i, j); }
        }
    }

    auto pathBlocked = [&](const auto& pos, const auto& dir) -> bool
    {
        return obstacles.contains({pos.first + dir.first, pos.second + dir.second});
    };

    auto outOfBounds = [&](const auto& pos) -> bool
    {
        return  pos.first < 0 || pos.second < 0 ||
                pos.first >= map.size() || pos.second >= map[0].size();
    };

    std::set<std::pair<int, int>> positions;

    using State = std::tuple<int, int, int, int>;
    std::uint64_t loopSum{0};
    auto run_sim = [&]() -> bool
    {
        std::set<State> location_states;
        while (true) {
            State state = std::make_tuple(position.first, position.second, direction.first, direction.second);
            if (location_states.contains(state)) {
                return true;
            }
            location_states.insert(state);

            // positions.insert(position); for part 1

            if (pathBlocked(position, direction)) {
                direction = directions[++compass % directions_sz];
            } else {
                position.first += direction.first;
                position.second += direction.second;
            }
            if (outOfBounds(position)) {
                return false;
            }
        }
    };

    auto start = position;
    auto init_direction = direction;
    auto init_compass = compass;
    for (std::size_t i = 0; i < map.size(); ++i) {
        for (std::size_t j = 0; j < map[0].size(); ++j) {
            if (map[i][j] == '.') {
                obstacles.insert({i, j});
                loopSum += run_sim();
                position = start;
                direction = init_direction;
                compass = init_compass;
                obstacles.erase({i, j});
            }
        }
    }

    return loopSum;
}

auto concat(std::uint64_t a, std::uint64_t b) -> std::uint64_t
{
    if (b == 0) return a;
    int pow = std::pow(10, std::floor(std::log10(b) + 1));
    return a * pow + b;
};

bool can_reach_target(std::uint64_t target, const std::vector<std::uint64_t>& numbers, size_t index) {
    if (index == 0) return target == numbers[0];
    if (can_reach_target(target - numbers[index], numbers, index - 1)) return true;
    if (target % numbers[index] == 0 && can_reach_target(target / numbers[index], numbers, index - 1))
    {
        return true;
    }
    return false;
}

bool can_reach_with_concat(std::uint64_t target, const std::vector<std::uint64_t>& numbers, size_t index, std::uint64_t curr) {
    if (index == numbers.size()) return curr == target;
    if (can_reach_with_concat(target, numbers, index + 1, curr + numbers[index])) return true;
    if (can_reach_with_concat(target, numbers, index + 1, curr * numbers[index])) return true;
    if (can_reach_with_concat(target, numbers, index + 1, concat(curr, numbers[index]))) return true;
    return false;
}

std::uint64_t day7(const std::vector<std::string>& input) {
    auto parse_line = [](const std::string& input, char delim) -> std::pair<std::uint64_t, std::vector<std::uint64_t>> {
        size_t colon_pos = input.find(':');
        std::uint64_t target = std::stoull(input.substr(0, colon_pos));
        std::vector<std::uint64_t> operands;
        std::istringstream operand_stream(input.substr(colon_pos + 1));
        std::uint64_t num;
        while (operand_stream >> num) {
            operands.push_back(num);
        }
        return {target, operands};
    };

    std::uint64_t total1{0};
    for (auto line : input) {
        auto equation = parse_line(line, ' ');
        if (can_reach_target(equation.first, equation.second, equation.second.size() - 1)) {
            total1 += equation.first;
        }
    }

    std::uint64_t total{0};
    for (auto line : input) {
        auto equation = parse_line(line, ' ');
        if (can_reach_with_concat(equation.first, equation.second, 1, equation.second.front())) {
            total += equation.first;
        }
    }

    return total;
}

int main() {
    auto input = Input::GetStringData("infile.txt");
    std::cout << day7(input);
}
