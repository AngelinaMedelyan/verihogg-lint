#include <gtest/gtest.h>

#include <filesystem>

#include "main/lint_rules.h"
#include "rules/invalid_liblist.h"
#include "rules/undeclared_cell.h"
#include "rules/undeclared_configuration.h"
#include "rules/undeclared_design.h"
#include "utils.h"

namespace fs = std::filesystem;

namespace {
auto BasePath() -> const fs::path {
  return fs::current_path() / ".." / ".." / "tests" / "Configurations";
}

TEST(InvalidLiblistTest, NoError) {
  const fs::path tests_path{BasePath() / "InvalidLiblist" / "NoError"};

  test::CheckWithNoErrorsExpected(tests_path, CheckInvalidLiblist);
}

TEST(InvalidLiblistTest, RaiseError) {
  const fs::path tests_path{BasePath() / "InvalidLiblist" / "RaiseError"};

  std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{
      SURELOG::ErrorDefinition::COMP_UNDEFINED_BASE_CLASS};

  test::CheckWithErrorsExpected(tests_path, verihogg_lint::LINT_INVALID_LIBLIST,
                                ignoreList, CheckInvalidLiblist);
}

TEST(UndeclaredCellTest, NoError) {
  const fs::path tests_path{BasePath() / "UndeclaredCell" / "NoError"};

  global::CheckWithNoErrorsExpected(tests_path, CheckUndeclaredCell);
}

TEST(UndeclaredCellTest, RaiseError) {
  const fs::path tests_path{BasePath() / "UndeclaredCell" / "RaiseError"};

  std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{
      SURELOG::ErrorDefinition::COMP_UNDEFINED_BASE_CLASS};

  global::CheckWithErrorsExpected(tests_path,
                                  verihogg_lint::LINT_UNDECLARED_CELL,
                                  ignoreList, CheckUndeclaredCell);
}

TEST(UndeclaredDesignTest, NoError) {
  const fs::path tests_path{BasePath() / "UndeclaredDesign" / "NoError"};

  global::CheckWithNoErrorsExpected(tests_path, CheckUndeclaredDesign);
}

TEST(UndeclaredDesignTest, RaiseError) {
  const fs::path tests_path{BasePath() / "UndeclaredDesign" / "RaiseError"};

  std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{
      SURELOG::ErrorDefinition::COMP_UNDEFINED_BASE_CLASS,
      SURELOG::ErrorDefinition::PA_SYNTAX_ERROR};

  global::CheckWithErrorsExpected(tests_path,
                                  verihogg_lint::LINT_UNDECLARED_DESIGN,
                                  ignoreList, CheckUndeclaredDesign);
}

TEST(UndeclaredConfigurationTest, NoError) {
  const fs::path tests_path{BasePath() / "UndeclaredConfiguration" / "NoError"};

  global::CheckWithNoErrorsExpected(tests_path, CheckUndeclaredConfiguration);
}

TEST(UndeclaredConfigurationTest, RaiseError) {
  const fs::path tests_path{BasePath() / "UndeclaredConfiguration" /
                            "RaiseError"};

  std::unordered_set<SURELOG::ErrorDefinition::ErrorType> ignoreList{
      SURELOG::ErrorDefinition::COMP_UNDEFINED_BASE_CLASS,
      SURELOG::ErrorDefinition::PA_SYNTAX_ERROR};

  global::CheckWithErrorsExpected(tests_path,
                                  verihogg_lint::LINT_UNDECLARED_CONFIGURATION,
                                  ignoreList, CheckUndeclaredConfiguration);
}

}  // namespace

auto main(int argc, char** argv) -> int {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
