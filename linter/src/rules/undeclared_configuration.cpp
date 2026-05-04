#include "rules/undeclared_configuration.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Config/Config.h>
#include <Surelog/Config/ConfigSet.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/Design/ModuleDefinition.h>
#include <Surelog/Design/ModuleInstance.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/Library/Library.h>
#include <Surelog/Library/LibrarySet.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "main/lint_rules.h"
#include "utils/config_utils.h"
#include "utils/design_utils.h"
#include "utils/location_utils.h"
#include "utils/module_utils.h"

namespace SL = SURELOG;

namespace {

void CheckDesignStatement(
    const SL::FileContent* kFileContent, const SL::NodeId kNode,
    std::unordered_map<std::string, ConfigInfo> globalConfigMap,
    std::unordered_map<std::string, ModuleInfo> globalModuleMap,
    SL::ErrorContainer* errors, SL::SymbolTable* symbols) {
  const auto kDesignNode =
      kFileContent->sl_get(kNode, SL::VObjectType::paDesign_statement);
  if (!kDesignNode) {
    return;
  }
  std::vector<std::string_view> identifiers;
  SL::NodeId current = kFileContent->Child(kDesignNode);
  while (current) {
    if (kFileContent->Type(current) == SL::VObjectType::slStringConst) {
      identifiers.push_back(kFileContent->SymName(current));
    }
    current = kFileContent->Sibling(current);
  }

  if (identifiers.empty()) {
    return;
  }

  std::string kFullName;
  std::string_view designName;
  if (identifiers.size() == 1) {
    designName = identifiers.at(0);
    kFullName = std::string(designName);
  } else {
    const std::string_view libName = identifiers.at(0);
    designName = identifiers.back();
    kFullName = std::string(libName) + "@" + std::string(designName);
  }
  if (globalModuleMap.find(kFullName) != globalModuleMap.end()) {
    return;
  }

  if (globalConfigMap.find(kFullName) == globalConfigMap.end()) {
    ReportError(kFileContent, kDesignNode, designName,
                verihogg_lint::LINT_UNDECLARED_CONFIGURATION, errors, symbols);
  }
}

void CheckInstanceStatement(
    const SL::FileContent* kFileContent, SL::NodeId kNode,
    std::unordered_map<std::string, ConfigInfo> configMap,
    std::unordered_map<std::string, ModuleInfo> moduleMap,
    SL::ErrorContainer* errors, SL::SymbolTable* symbols) {
  SL::NodeId current = kFileContent->Child(kNode);

  while (current) {
    if (kFileContent->Type(current) == SL::VObjectType::paInst_clause) {
      const SL::NodeId kUseClause = kFileContent->Sibling(current);

      if (!kUseClause ||
          kFileContent->Type(kUseClause) != SL::VObjectType::paUse_clause) {
        current = kFileContent->Sibling(current);
        continue;
      }

      std::vector<std::string_view> identifiers;
      SL::NodeId kUseChild = kFileContent->Child(kUseClause);
      while (kUseChild) {
        if (kFileContent->Type(kUseChild) == SL::VObjectType::slStringConst) {
          identifiers.push_back(kFileContent->SymName(kUseChild));
        }
        kUseChild = kFileContent->Sibling(kUseChild);
      }

      if (identifiers.empty()) {
        continue;
      }

      std::string kFullName;
      std::string_view configName;
      if (identifiers.size() == 1) {
        configName = identifiers.at(0);
        kFullName = std::string(configName);
      } else {
        const std::string_view libName = identifiers.at(0);
        configName = identifiers.back();
        kFullName = std::string(libName) + "@" + std::string(configName);
      }
      if (moduleMap.find(kFullName) != moduleMap.end()) {
        continue;
      }

      if (configMap.find(kFullName) == configMap.end()) {
        ReportError(kFileContent, kUseClause, configName,
                    verihogg_lint::LINT_UNDECLARED_CONFIGURATION, errors,
                    symbols);
      }
    }
    current = kFileContent->Sibling(current);
  }
}

}  // namespace

void CheckUndeclaredConfiguration(SL::Design* design,
                                  SL::ErrorContainer* errors,
                                  SL::SymbolTable* symbols) {
  if (design == nullptr || errors == nullptr || symbols == nullptr) {
    return;
  }

  const auto kConfigMap = ConfigUtils::CollectAllConfig(design);
  const auto kModuleMap = ModuleUtils::CollectAllModules(design);

  DesignUtils::ForEachFileContent(
      design, [&](const SL::FileContent* kFileContent) {
        const SL::NodeId kRoot = kFileContent->getRootNode();

        if (!kRoot) {
          return;
        }

        for (auto& kConfig : kFileContent->sl_collect_all(
                 kRoot, SL::VObjectType::paConfig_rule_statement)) {
          CheckDesignStatement(kFileContent, kConfig, kConfigMap, kModuleMap,
                               errors, symbols);
          CheckInstanceStatement(kFileContent, kConfig, kConfigMap, kModuleMap,
                                 errors, symbols);
        }
        for (auto& kConfig : kFileContent->sl_collect_all(
                 kRoot, SL::VObjectType::paConfig_declaration)) {
          CheckDesignStatement(kFileContent, kConfig, kConfigMap, kModuleMap,
                               errors, symbols);
          CheckInstanceStatement(kFileContent, kConfig, kConfigMap, kModuleMap,
                                 errors, symbols);
        }
      });
}
