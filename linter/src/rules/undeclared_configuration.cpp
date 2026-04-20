#include "rules/undeclared_configuration.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/Design/ModuleInstance.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "main/lint_rules.h"
#include "utils/ast_utils.h"
#include "utils/design_utils.h"
#include "utils/location_utils.h"

namespace SL = SURELOG;

struct ConfigInfo {
  SL::NodeId nodeid;
  std::string name;
  const SL::FileContent* fileContent = nullptr;
};

namespace {

auto BuildSetOfAllConfigs(SL::Design* design)
    -> std::unordered_map<std::string_view, ConfigInfo> {
  std::unordered_map<std::string_view, ConfigInfo> kResultMap;

  DesignUtils::ForEachFileContent(
      design, [&](const SL::FileContent* fileContent) {
        SL::NodeId const kRoot = fileContent->getRootNode();
        if (!kRoot) {
          return;
        }

        for (SL::NodeId const kConfigDecl : fileContent->sl_collect_all(
                 kRoot, SL::VObjectType::paConfig_declaration)) {
          std::string const kFullName = GetFullName(fileContent, kConfigDecl);
          kResultMap[kFullName] = ConfigInfo{
              .nodeid = kConfigDecl,
              .name = kFullName,
              .fileContent = fileContent,
          };
        }
      });

  return kResultMap;
}
}  // namespace

void CheckUndeclaredConfiguration(SL::Design* design,
                                  SL::ErrorContainer* errors,
                                  SL::SymbolTable* symbols) {
  if (design == nullptr || errors == nullptr || symbols == nullptr) {
    return;
  }

  const auto kConfigMap = BuildSetOfAllConfigs(design);
  if (kConfigMap.empty()) {
    return;
  }

  const auto& kTopInstances = design->getTopLevelModuleInstances();
  for (auto& kModuleInst : kTopInstances) {
    const auto kFileContent = kModuleInst->getFileContent();
    const auto kNodeId = kModuleInst->getNodeId();
    const std::vector<SL::NodeId> kConfigs = kFileContent->sl_collect_all(
        kNodeId, SL::VObjectType::paConfig_declaration);
    for (auto& kConfig : kConfigs) {
      std::string const kFullName = GetFullName(kFileContent, kConfig);
      if (kConfigMap.find(kFullName) == kConfigMap.end()) {
        ReportError(kFileContent, kNodeId, kFullName,
                    verihogg_lint::LINT_UNDECLARED_DESIGN, errors, symbols);
      }
    }
  }
}
