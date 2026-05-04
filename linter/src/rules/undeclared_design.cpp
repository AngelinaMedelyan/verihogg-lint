#include "rules/undeclared_design.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>

#include <string>
#include <unordered_map>
#include <unordered_set>

#include "main/lint_rules.h"
#include "utils/ast_utils.h"
#include "utils/config_utils.h"
#include "utils/design_utils.h"
#include "utils/location_utils.h"
#include "utils/module_utils.h"

namespace SL = SURELOG;

void CheckUndeclaredDesign(SL::Design* design, SL::ErrorContainer* errors,
                           SL::SymbolTable* symbols) {
  if (design == nullptr || errors == nullptr || symbols == nullptr) {
    return;
  }

  auto globalModuleMap = ModuleUtils::CollectAllModules(design);
  if (globalModuleMap.empty()) {
    return;
  }

  auto globalConfigMap = ConfigUtils::CollectAllConfig(design);
  if (globalConfigMap.empty()) {
    return;
  }

  for (auto& [k, kConfigInfo] : globalConfigMap) {
    auto designInfo = DesignUtils::ExtractDesignInfo(kConfigInfo.fileContent,
                                                     kConfigInfo.nodeid);
    auto kFullName =
        designInfo.libName + "@" + std::string(designInfo.moduleName);
    if (globalModuleMap.find(kFullName) != globalModuleMap.end() ||
        globalConfigMap.find(kFullName) != globalConfigMap.end()) {
      continue;
    }
    ReportError(kConfigInfo.fileContent, kConfigInfo.nodeid, kFullName,
                verihogg_lint::LINT_UNDECLARED_DESIGN, errors, symbols);
  }
}
