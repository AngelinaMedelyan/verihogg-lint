#include "rules/undeclared_cell.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/ErrorReporting/ErrorContainer.h>
#include <Surelog/SourceCompile/SymbolTable.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <string_view>
#include <unordered_map>

#include "main/lint_rules.h"
#include "utils/design_utils.h"
#include "utils/location_utils.h"

namespace SL = SURELOG;

namespace {

struct ModuleInfo {
  std::string_view name;
  SL::NodeId nodeId;
  const SL::FileContent* fileContent;
};

using CellInfo = struct {
  SL::NodeId configNode;
  std::string_view cellName;
  std::string_view liblist;
  const SL::FileContent* fileContent;
};

auto CollectCellInfos(const SL::FileContent* fileContent)
    -> std::vector<CellInfo> {
  std::vector<CellInfo> result;

  SL::NodeId const kRoot = fileContent->getRootNode();
  if (!kRoot) {
    return result;
  }

  for (SL::NodeId const kConfigDecl : fileContent->sl_collect_all(
           kRoot, SL::VObjectType::paConfig_declaration)) {
    for (SL::NodeId const kConfigStatement : fileContent->sl_collect_all(
             kRoot, SL::VObjectType::paConfig_rule_statement)) {
      SL::NodeId const kCellClause = fileContent->Child(kConfigStatement);
      if (!kCellClause ||
          fileContent->Type(kCellClause) != SL::VObjectType::paCell_clause) {
        continue;
      }
      SL::NodeId const kLiblistClause = fileContent->Sibling(kCellClause);
      if (!kLiblistClause || fileContent->Type(kLiblistClause) !=
                                 SL::VObjectType::paLiblist_clause) {
        continue;
      }
      SL::NodeId const kCellStringConstNode =
          fileContent->sl_get(kCellClause, SL::VObjectType::slStringConst);
      if (!kCellStringConstNode) {
        continue;
      }
      SL::NodeId const kLiblistStringConstNode =
          fileContent->sl_get(kLiblistClause, SL::VObjectType::slStringConst);
      if (!kLiblistStringConstNode) {
        continue;
      }

      std::string_view kCellName = fileContent->SymName(kCellStringConstNode);
      std::string_view kLiblistName =
          fileContent->SymName(kLiblistStringConstNode);

      const CellInfo cellInfo = CellInfo{
          .configNode = kConfigDecl,
          .cellName = kCellName,
          .liblist = kLiblistName,
      };
      result.push_back(cellInfo);
    }
  }
  return result;
}

}  // namespace

namespace {

auto FindScopeContainer(const SL::FileContent* fileContent, SL::NodeId node)
    -> SL::NodeId {
  SL::NodeId current = fileContent->Parent(node);
  while (current) {
    SL::VObjectType type = fileContent->Type(current);
    if (type == SL::VObjectType::paModule_declaration ||
        type == SL::VObjectType::paInterface_declaration ||
        type == SL::VObjectType::paPackage_declaration ||
        type == SL::VObjectType::paConfig_declaration) {
      return current;
    }
    current = fileContent->Parent(current);
  }
  return SL::InvalidNodeId;
}

auto GetScopeName(const SL::FileContent* fileContent, SL::NodeId scopeNode)
    -> std::string_view {
  if (!scopeNode) {
    return {};
  }

  SL::VObjectType type = fileContent->Type(scopeNode);
  if (type == SL::VObjectType::paModule_declaration ||
      type == SL::VObjectType::paInterface_declaration ||
      type == SL::VObjectType::paPackage_declaration) {
    SL::NodeId header = fileContent->Child(scopeNode);
    if (header &&
        (fileContent->Type(header) == SL::VObjectType::paModule_ansi_header ||
         fileContent->Type(header) ==
             SL::VObjectType::paModule_nonansi_header)) {
      SL::NodeId nameNode =
          fileContent->sl_get(header, SL::VObjectType::slStringConst);
      if (nameNode) {
        return fileContent->SymName(nameNode);
      }
    }
  } else if (type == SL::VObjectType::paConfig_declaration) {
    SL::NodeId nameNode =
        fileContent->sl_get(scopeNode, SL::VObjectType::slStringConst);
    if (nameNode) {
      return fileContent->SymName(nameNode);
    }
  }

  return {};
}

auto CollectAllModules(SL::Design* design)
    -> std::unordered_map<std::string_view, std::vector<ModuleInfo>> {
  std::unordered_map<std::string_view, std::vector<ModuleInfo>> moduleMap;

  DesignUtils::ForEachFileContent(design, [&](const SL::FileContent* fileCont) {
    SL::NodeId const kRoot = fileCont->getRootNode();
    if (!kRoot) {
      return;
    }

    for (SL::NodeId const kModuleDecl : fileCont->sl_collect_all(
             kRoot, SL::VObjectType::paModule_declaration)) {
      SL::NodeId kAnsiHeader = fileCont->Child(kModuleDecl);
      if (!kAnsiHeader || (fileCont->Type(kAnsiHeader) !=
                               SL::VObjectType::paModule_ansi_header &&
                           fileCont->Type(kAnsiHeader) !=
                               SL::VObjectType::paModule_nonansi_header)) {
        continue;
      }

      SL::NodeId kModuleNameNode =
          fileCont->sl_get(kAnsiHeader, SL::VObjectType::slStringConst);
      if (!kModuleNameNode) {
        continue;
      }

      std::string_view const kModuleName = fileCont->SymName(kModuleNameNode);

      ModuleInfo info{
          .name = kModuleName, .nodeId = kModuleDecl, .fileContent = fileCont};

      moduleMap[kModuleName].push_back(info);
    }

    for (SL::NodeId const kInterfaceDecl : fileCont->sl_collect_all(
             kRoot, SL::VObjectType::paInterface_declaration)) {
      SL::NodeId kHeader = fileCont->Child(kInterfaceDecl);
      if (!kHeader ||
          fileCont->Type(kHeader) != SL::VObjectType::paInterface_ansi_header) {
        continue;
      }

      SL::NodeId kNameNode =
          fileCont->sl_get(kHeader, SL::VObjectType::slStringConst);
      if (!kNameNode) {
        continue;
      }

      std::string_view const kInterfaceName = fileCont->SymName(kNameNode);

      ModuleInfo info{.name = kInterfaceName,
                      .nodeId = kInterfaceDecl,
                      .fileContent = fileCont};

      moduleMap[kInterfaceName].push_back(info);
    }

    for (SL::NodeId const kPackageDecl : fileCont->sl_collect_all(
             kRoot, SL::VObjectType::paPackage_declaration)) {
      SL::NodeId kNameNode =
          fileCont->sl_get(kPackageDecl, SL::VObjectType::slStringConst);
      if (!kNameNode) {
        continue;
      }

      std::string_view const kPackageName = fileCont->SymName(kNameNode);

      ModuleInfo info{.name = kPackageName,
                      .nodeId = kPackageDecl,
                      .fileContent = fileCont};

      moduleMap[kPackageName].push_back(info);
    }
  });

  return moduleMap;
}

auto ExtractCellNameFromClause(const SL::FileContent* fileContent,
                               SL::NodeId cellClause) -> std::string_view {
  SL::NodeId nameNode =
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

  SL::NodeId firstLib =
      fileContent->sl_get(liblistClause, SL::VObjectType::slStringConst);
  if (firstLib) {
    return fileContent->SymName(firstLib);
  }
  return {};
}

auto CollectAllCellInfos(SL::Design* design) -> std::vector<CellInfo> {
  std::vector<CellInfo> allCells;

  DesignUtils::ForEachFileContent(design, [&](const SL::FileContent* fileCont) {
    SL::NodeId const kRoot = fileCont->getRootNode();
    if (!kRoot) {
      return;
    }

    for (SL::NodeId const kConfigDecl : fileCont->sl_collect_all(
             kRoot, SL::VObjectType::paConfig_declaration)) {
      for (SL::NodeId const kConfigStatement : fileCont->sl_collect_all(
               kConfigDecl, SL::VObjectType::paConfig_rule_statement)) {
        SL::NodeId kCellClause = fileCont->Child(kConfigStatement);
        while (kCellClause) {
          if (fileCont->Type(kCellClause) == SL::VObjectType::paCell_clause) {
            SL::NodeId kLiblistClause = fileCont->Sibling(kCellClause);
            if (kLiblistClause && fileCont->Type(kLiblistClause) ==
                                      SL::VObjectType::paLiblist_clause) {
              std::string_view cellName =
                  ExtractCellNameFromClause(fileCont, kCellClause);
              std::string_view liblist =
                  ExtractLiblistFromClause(fileCont, kLiblistClause);

              if (!cellName.empty()) {
                CellInfo info{.configNode = kConfigDecl,
                              .cellName = cellName,
                              .liblist = liblist,
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

auto ModuleExistsInScope(
    std::string_view moduleName,
    const std::unordered_map<std::string_view, std::vector<ModuleInfo>>&
        moduleMap,
    std::string_view scopeName) -> bool {
  auto it = moduleMap.find(moduleName);
  if (it == moduleMap.end()) {
    return false;
  }

  if (scopeName.empty()) {
    return !it->second.empty();
  }

  std::string_view::size_type double_colon = moduleName.find("::");
  if (double_colon != std::string_view::npos) {
    std::string_view pkgName = moduleName.substr(0, double_colon);
    std::string_view modName = moduleName.substr(double_colon + 2);

    auto pkgIt = moduleMap.find(pkgName);
    if (pkgIt != moduleMap.end()) {
      return moduleMap.find(modName) != moduleMap.end();
    }
    return false;
  }

  return !it->second.empty();
}

}  // namespace

void CheckUndeclaredCell(SL::Design* design, SL::ErrorContainer* errors,
                         SL::SymbolTable* symbols) {
  if (design == nullptr || errors == nullptr || symbols == nullptr) {
    return;
  }

  auto globalModuleMap = CollectAllModules(design);
  if (globalModuleMap.empty()) {
    return;
  }

  std::vector<CellInfo> allCells = CollectAllCellInfos(design);
  if (allCells.empty()) {
    return;
  }

  for (const auto& cellInfo : allCells) {
    bool moduleExists =
        ModuleExistsInScope(cellInfo.cellName, globalModuleMap, "");
    if (moduleExists) {
      continue;
    }
    ReportError(cellInfo.fileContent, cellInfo.configNode, cellInfo.cellName,
                verihogg_lint::LINT_UNDECLARED_CELL, errors, symbols);
  }
}
