#ifndef ASSERT_FAIL_HPP
#define ASSERT_FAIL_HPP
#include <exception>
#include <string>

class AssertFail : public std::exception
{
  public:
    AssertFail(const std::string& context = "", const std::string& expected = "", const std::string& actual = "");

    const char* what() const noexcept override;

  private:
    std::string _message;
    std::string _expected;
    std::string _actual;
};
#endif
