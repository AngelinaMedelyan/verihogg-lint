#include "rules/undeclared_design.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <unordered_map>

#include "main/lint_rules.h"
#include "utils/ast_utils.h"
#include "utils/design_utils.h"
#include "utils/location_utils.h"
#include "utils/module_utils.h"

namespace SL = SURELOG;

struct ConfigInfo {
  SL::NodeId nodeid;
  std::string name;
  const SL::FileContent* fileContent;
};

namespace {
auto CollectAllConfig(SL::Design* design) -> std::vector<ConfigInfo> {
  std::vector<ConfigInfo> allConfigs;

  DesignUtils::ForEachFileContent(design, [&](const SL::FileContent* fileCont) {
    SL::NodeId const kRoot = fileCont->getRootNode();
    if (!kRoot) {
      return;
    }

    for (SL::NodeId const kConfigDecl : fileCont->sl_collect_all(
             kRoot, SL::VObjectType::paConfig_declaration)) {
      std::string const kFullName = GetFullName(fileCont, kConfigDecl);
      allConfigs.push_back(ConfigInfo{
          .nodeid = kConfigDecl,
          .name = kFullName,
          .fileContent = fileCont,
      });
    }
  });

  return allConfigs;
}
}  // namespace

void CheckUndeclaredDesign(SL::Design* design, SL::ErrorContainer* errors,
                           SL::SymbolTable* symbols) {
  if (design == nullptr || errors == nullptr || symbols == nullptr) {
    return;
  }

  auto globalModuleMap = ModuleUtils::CollectAllModules(design);
  if (globalModuleMap.empty()) {
    return;
  }

  for (auto& kConfigInfo : CollectAllConfig(design)) {
    auto designInfo = DesignUtils::ExtractDesignInfo(kConfigInfo.fileContent,
                                                     kConfigInfo.nodeid);
    auto kFullName =
        designInfo.libName + "@" + std::string(designInfo.moduleName);
    if (globalModuleMap.find(kFullName) != globalModuleMap.end()) {
      continue;
    }
    ReportError(kConfigInfo.fileContent, kConfigInfo.nodeid, kConfigInfo.name,
                verihogg_lint::LINT_UNDECLARED_DESIGN, errors, symbols);
  }
}
