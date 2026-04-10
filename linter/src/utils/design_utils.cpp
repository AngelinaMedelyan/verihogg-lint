#include "utils/design_utils.h"

#include <functional>

#include "Surelog/Design/Design.h"
#include "Surelog/Design/FileContent.h"

namespace SL = SURELOG;

namespace DesignUtils {

void ForEachFileContent(
    SL::Design* design,
    const std::function<void(const SL::FileContent*)>& func) {
  if (design == nullptr) {
    return;
  }
  for (auto& [name, fileContent] : design->getAllFileContents()) {
    if (fileContent == nullptr) {
      continue;
    }
    func(fileContent);
  }
}

auto ExtractDesignInfo(const SL::FileContent* fileContent,
                       SL::NodeId configDecl) -> DesignInfo {
  DesignInfo info;

  SL::NodeId designStmt =
      fileContent->sl_get(configDecl, SL::VObjectType::paDesign_statement);
  if (!designStmt) {
    return info;
  }

  std::vector<std::string_view> identifiers;
  SL::NodeId current = fileContent->Child(designStmt);
  while (current) {
    if (fileContent->Type(current) == SL::VObjectType::slStringConst) {
      identifiers.push_back(fileContent->SymName(current));
    }
    current = fileContent->Sibling(current);
  }

  if (identifiers.empty()) {
    return info;
  }

  if (identifiers.size() == 1) {
    info.moduleName = identifiers[0];
    info.libName = "";
    info.scopeName = info.moduleName;
  } else {
    info.libName = identifiers[0];
    info.moduleName = identifiers.back();
    info.scopeName = identifiers[1];
    for (size_t i = 2; i < identifiers.size(); ++i) {
      info.scopeName = info.scopeName + "::" + std::string(identifiers[i]);
    }
  }

  return info;
}

}  // namespace DesignUtils
