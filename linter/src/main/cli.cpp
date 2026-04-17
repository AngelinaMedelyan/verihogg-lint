#include "main/cli.h"

#include <yaml-cpp/node/convert.h>      // NOLINT(misc-include-cleaner)
#include <yaml-cpp/node/detail/impl.h>  // NOLINT(misc-include-cleaner)
#include <yaml-cpp/node/emit.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/node/parse.h>
#include <yaml-cpp/null.h>
#include <yaml-cpp/parser.h>

#include <cstring>
#include <filesystem>
#include <gsl/span>
#include <iostream>

#include "main/rule_dispatcher.h"

namespace cli {

constexpr size_t CONFIG_FLAG_LEN = 13;

auto ParseArgs(const gsl::span<const char*> args) -> Options {
  Options opts;

  opts.surelog_args.push_back(args[0]);

  const std::filesystem::path configFileName = DefaultConfigFileName;
  opts.config_file = std::filesystem::current_path() / configFileName;

  for (const auto arg : args.subspan(1)) {
    if (std::strcmp(arg, "--dump-config") == 0) {
      opts.dump_config = true;
      return opts;
    } else if (std::strcmp(arg, "--help") == 0 || std::strcmp(arg, "-h") == 0) {
      opts.show_help = true;
    } else if (std::strcmp(arg, "--version") == 0) {
      opts.show_version = true;
    } else if (std::strcmp(arg, "--list-rules") == 0) {
      opts.show_rules = true;
    } else if (std::strcmp(arg, "--surelog-help") == 0) {
      opts.show_surelog_help = true;
    }  // else if (std::strncmp(arg, "--config-file", CONFIG_FLAG_LEN) == 0) {
    //   const std::string strArg{arg};
    //   const std::string config_file =
    //       strArg.substr(CONFIG_FLAG_LEN + 1, strArg.size());
    //   opts.config_file = std::filesystem::path{config_file};
    // }
    else {
      opts.surelog_args.push_back(arg);
    }
  }

  return opts;
}

// auto GetYamlConfig(const std::filesystem::path& configFile) -> YAML::Node {
//   if (!configFile.empty() && std::filesystem::exists(configFile)) {
//     try {
//       return YAML::LoadFile(configFile);
//     } catch (const std::exception& e) {
//       std::cerr << "Bad config file" << "\n";
//       return YAML::Node{};
//     }
//   }

//   const std::filesystem::path configPath = DefaultConfigFileName;
//   std::filesystem::path currentDir = std::filesystem::current_path();

//   while (!std::filesystem::exists(currentDir / configPath)) {
//     if (currentDir.parent_path() == currentDir) {
//       std::cerr << "No config file" << "\n";
//       return YAML::Node{};
//     }
//     currentDir = currentDir.parent_path();
//   }

//   try {
//     return YAML::LoadFile(currentDir / configPath);
//   } catch (const std::exception& e) {
//     std::cerr << "Bad config file" << "\n";
//     return YAML::Node{};
//   }
// }

// template <typename RuleType>
// void FilterSpecificRule(RuleType& rule, const YAML::Node& tree) {
//   const YAML::Node node = tree[rule.idName];
//   if (node) {
//     const auto value = node.as<std::string>();

//     if (value == "yes" || value == "true") {
//       rule.enabled = true;
//     } else if (value == "false" || value == "no") {
//       rule.enabled = false;
//     } else {
//       std::cerr << "Expected boolean, got: "
//                 << std::string(value.begin(), value.end()) << "\n";
//     }
//   }
// }

// void FilterRules(const std::filesystem::path& configFile) {
//   const auto yaml = GetYamlConfig(configFile);
//   if (yaml.IsNull()) {
//     return;
//   }

//   for (auto& rule : RuleInfo::allRules) {
//     FilterSpecificRule(rule, yaml);
//   }
//   for (auto& rule : RuleInfo::globalRules) {
//     FilterSpecificRule(rule, yaml);
//   }
// }

void DumpConfig() {
  std::cout << "Checks:\n";
  for (auto& rule : RuleInfo::allRules) {
    std::cout << "  " << rule.idName << "\n";
  }
  for (auto& rule : RuleInfo::globalRules) {
    std::cout << "  " << rule.idName << "\n";
  }
}

void PrintVersion() { std::cout << "verihogg-lint " << kVersion << "\n"; }

void PrintHelp(const char* programName) {
  // clang-format off
  std::cout
    << "Usage: " << programName << " [OPTIONS] <file.sv> [<file.sv>...]\n"
    << "       " << programName << " [OPTIONS] -f <filelist>\n"
    << "\n"
    << "A SystemVerilog linter with static analysis rules built on Surelog.\n"
    << "\n"
    << "OPTIONS:\n"
    << "  -h, --help          Show this help and exit\n"
    << "  --version           Show version and exit\n"
    << "  --list-rules        List all available lint rules\n"
    << "  --surelog-help      Show full Surelog parser/elaboration options\n"
    << "\n"
    << "INPUT:\n"
    << "  <file>.sv           SystemVerilog source file\n"
    << "  -f <file>           Filelist (sources, includes, defines)\n"
    << "  +incdir+<dir>       Add include directory (repeatable)\n"
    << "  +define+<name>[=<value>]\n"
    << "                      Define a preprocessor macro (repeatable)\n"
    << "  -nobuiltin          Skip built-in SV classes (recommended)\n"
    << "\n"
    << "FILELIST FORMAT (.f file):\n"
    << "  Lines starting with // or # are comments.\n"
    << "  Example:\n"
    << "    +incdir+rtl/include\n"
    << "    +define+SYNTHESIS\n"
    << "    rtl/pkg/common_pkg.sv\n"
    << "    rtl/core/top.sv\n"
    << "\n"
    << "EXAMPLES:\n"
    << "  " << programName << " file.sv -nobuiltin\n"
    << "  " << programName << " -f files.f -nobuiltin\n"
    << "  " << programName << " --list-rules\n"
    << "\n"
    << "All other flags are forwarded to Surelog (parser/elaboration).\n"
    << "Run '" << programName << " --surelog-help' for the full list.\n";
  // clang-format on
}

void PrintRules() {
  std::cout << "Available lint rules (" << RuleInfo::TotalRuleCount << "):\n";

  for (const auto& rule : RuleInfo::allRules) {
    std::cout << "\n  " << rule.idName << "\n"
              << "      " << rule.description << "\n";
  }

  for (const auto& rule : RuleInfo::globalRules) {
    std::cout << "\n  " << rule.idName << "\n"
              << "      " << rule.description << "\n";
  }
}
}  // namespace cli
