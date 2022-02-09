#pragma once
#include <fstream>
#include <string>

#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace mpt::test {

  /// Status of running a collector
  enum class status_code { success = false, failure = true };

  /// Check if a status code is success
  bool is_success(status_code sc) { return sc == status_code::success; }

  /// Container of errors
  using errors = std::vector<std::string>;

  /* \brief Object handling a collection of test functions
   *
   */
  class collector {

  public:
    /// Functions must return a list with the error messages
    using function_type = std::function<errors(void)>;

    /// Constructor from a name
    collector(std::string const &name) : m_name{name} {}
    /// Default destructor
    ~collector() = default;

    // Invalidate other constructors
    collector(collector const &) = delete;
    collector(collector &&) = delete;
    collector &operator=(collector const &) = delete;

    /// Add a new test function
    void add_test_function(std::string name, function_type const &function) {
      m_functions.push_back(std::make_pair(std::move(name), function));
    }

    /// Run the stored tests and return the status
    status_code run() const {

      std::map<std::size_t, std::vector<std::string>> error_map;

      for (auto i = 0u; i < m_functions.size(); ++i) {
        auto v = m_functions[i].second();
        if (v.size() != 0)
          error_map[i] = std::move(v);
      }

      std::cout << "Results for collector \"" << m_name << '\"' << std::endl;
      for (auto i = 0u; i < m_functions.size(); ++i)
        std::cout << "- "
                  << (error_map.find(i) == error_map.cend() ? "(success) "
                                                            : "(failed) ")
                  << m_functions[i].first << std::endl;

      if (error_map.size() != 0) {
        std::cerr << "summary of errors:" << std::endl;
        for (auto const &p : error_map) {
          std::cerr << "* " << m_functions[p.first].first << ':' << std::endl;
          for (auto const &what : p.second)
            std::cerr << " - " << what << std::endl;
        }
        return status_code::failure;
      }

      return status_code::success;
    }

  protected:
    /// Names and functions
    std::vector<std::pair<std::string, function_type>> m_functions;
    /// Name of the collector
    std::string m_name;
  };

  /// Check if two numbers are close given the
  bool is_close(double a, double b, double rtol = 1e-5, double atol = 1e-8) {

    return std::abs(a - b) < std::abs(rtol * b) + std::abs(atol);
  }
} // namespace mpt::test

#define MPT_TEST_UTILS_ADD_TEST(collector, function)                           \
  collector.add_test_function(#function, function)

#define MPT_TEST_UTILS_CATCH_EXCEPTIONS(errors)                                \
  catch (...) {                                                                \
    errors.push_back("Unknown error detected");                                \
  }
