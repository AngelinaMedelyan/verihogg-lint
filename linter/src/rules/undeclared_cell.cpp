#include "rules/undeclared_cell.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "main/lint_rules.h"
#include "utils/design_utils.h"
#include "utils/location_utils.h"
#include "utils/module_utils.h"

namespace SL = SURELOG;

namespace {

struct CellInfo {
  SL::NodeId configNode;
  std::string_view cellName;
  std::string_view liblist;
  DesignInfo designInfo;
  const SL::FileContent* fileContent;
};

auto ExtractCellNameFromClause(const SL::FileContent* fileContent,
                               SL::NodeId cellClause) -> std::string_view {
  const SL::NodeId nameNode =
      fileContent->sl_get(cellClause, SL::VObjectType::slStringConst);
  if (nameNode) {
    return fileContent->SymName(nameNode);
  }
  return "";
}

auto ExtractLiblistFromClause(const SL::FileContent* fileContent,
                              SL::NodeId liblistClause) -> std::string_view {
  std::string result;
  SL::NodeId current = fileContent->Child(liblistClause);
  while (current) {
    if (fileContent->Type(current) == SL::VObjectType::slStringConst) {
      if (!result.empty()) {
        result += " ";
      }
      result += fileContent->SymName(current);
    }
    current = fileContent->Sibling(current);
  }

  const SL::NodeId firstLib =
      fileContent->sl_get(liblistClause, SL::VObjectType::slStringConst);
  if (firstLib) {
    return fileContent->SymName(firstLib);
  }
  return {};
}

auto CollectAllCellInfos(SL::Design* design) -> std::vector<CellInfo> {
  std::vector<CellInfo> allCells;

  DesignUtils::ForEachFileContent(design, [&](const SL::FileContent* fileCont) {
    const SL::NodeId kRoot = fileCont->getRootNode();
    if (!kRoot) {
      return;
    }

    for (SL::NodeId const kConfigDecl : fileCont->sl_collect_all(
             kRoot, SL::VObjectType::paConfig_declaration)) {
      for (SL::NodeId const kConfigStatement : fileCont->sl_collect_all(
               kConfigDecl, SL::VObjectType::paConfig_rule_statement)) {
        const DesignInfo kDesignInfo =
            DesignUtils::ExtractDesignInfo(fileCont, kConfigDecl);

        SL::NodeId kCellClause = fileCont->Child(kConfigStatement);
        while (kCellClause) {
          if (fileCont->Type(kCellClause) == SL::VObjectType::paCell_clause) {
            const SL::NodeId kLiblistClause = fileCont->Sibling(kCellClause);
            if (kLiblistClause && fileCont->Type(kLiblistClause) ==
                                      SL::VObjectType::paLiblist_clause) {
              const std::string_view cellName =
                  ExtractCellNameFromClause(fileCont, kCellClause);
              const std::string_view liblist =
                  ExtractLiblistFromClause(fileCont, kLiblistClause);

              if (!cellName.empty()) {
                const CellInfo info{.configNode = kConfigDecl,
                                    .cellName = cellName,
                                    .liblist = liblist,
                                    .designInfo = kDesignInfo,
                                    .fileContent = fileCont};
                allCells.push_back(info);
              }
            }
          }
          kCellClause = fileCont->Sibling(kCellClause);
        }
      }
    }
  });

  return allCells;
}

auto CellIsInModule(
    const std::string_view cellName,
    const std::unordered_map<std::string, ModuleInfo>& moduleMap,
    const std::string& moduleName, const SL::FileContent* fileContent) -> bool {
  if (moduleMap.find(moduleName) == moduleMap.end()) {
    return false;
  }

  const auto& moduleInfo = moduleMap.at(moduleName);
  const std::vector<SL::NodeId> kAllInsts = fileContent->sl_collect_all(
      moduleInfo.nodeId, SL::VObjectType::paModule_instantiation);

  for (auto& instId : kAllInsts) {
    const SL::NodeId nameId =
        fileContent->sl_get(instId, SL::VObjectType::slStringConst);
    const std::string_view kInstName = fileContent->SymName(nameId);

    if (kInstName == cellName) {
      return true;
    }
  }
  return false;
}

}  // namespace

void CheckUndeclaredCell(SL::Design* design, SL::ErrorContainer* errors,
                         SL::SymbolTable* symbols) {
  if (design == nullptr || errors == nullptr || symbols == nullptr) {
    return;
  }

  const auto globalModuleMap = ModuleUtils::CollectAllModules(design);
  if (globalModuleMap.empty()) {
    return;
  }

  for (const auto& cellInfo : CollectAllCellInfos(design)) {
    const std::string kFullScopeName =
        cellInfo.designInfo.libName + "@" + cellInfo.designInfo.scopeName;
    const std::string kFullModuleName =
        cellInfo.designInfo.libName + "@" + std::string(cellInfo.cellName);
    const bool CellExists =
        CellIsInModule(cellInfo.cellName, globalModuleMap, kFullScopeName,
                       cellInfo.fileContent);
    if (CellExists &&
        globalModuleMap.find(kFullModuleName) != globalModuleMap.end()) {
      continue;
    }
    ReportError(cellInfo.fileContent, cellInfo.configNode, cellInfo.cellName,
                verihogg_lint::LINT_UNDECLARED_CELL, errors, symbols);
  }
}
