#include "main/rule_dispatcher.h"

#include <Surelog/Common/FileSystem.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <uhdm/vpi_user.h>

#include <filesystem>

#include "rules/fatal_rule.h"

namespace SL = SURELOG;

void RunAllRules(const SL::FileContent* fileContent, SL::ErrorContainer* errors,
                 SL::SymbolTable* symbols) {
  for (const auto& rule : RuleInfo::allRules) {
    // if (!rule.enabled) {
    //   continue;
    // }
    rule.check(fileContent, errors, symbols);
  }
}

void RunAllRulesOnDesign(SL::Design* design, const vpiHandle& uhdmDesign,
                         SL::ErrorContainer* errors, SL::SymbolTable* symbols,
                         const std::filesystem::path& configFile) {
  if (design == nullptr) {
    return;
  }

  // FilterRules(configFile, RuleInfo::allRules, RuleInfo::globalRules);

  for (auto& [name, fileContent] : design->getAllFileContents()) {
    if (fileContent == nullptr) {
      continue;
    }

    RunAllRules(fileContent, errors, symbols);
    FatalListener listener(errors, symbols);
    listener.Listen(uhdmDesign);
  }

  for (const auto& rule : RuleInfo::globalRules) {
    // if (!rule.enabled) {
    //   continue;
    // }
    rule.check(design, errors, symbols);
  }
}
