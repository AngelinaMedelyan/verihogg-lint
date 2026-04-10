#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <utils/ast_utils.h>
#include <utils/design_utils.h>
#include <utils/module_utils.h>

#include <string>
#include <unordered_map>

namespace SL = SURELOG;

namespace ModuleUtils {
auto CollectAllModules(SL::Design* design)
    -> std::unordered_map<std::string, ModuleInfo> {
  std::unordered_map<std::string, ModuleInfo> moduleMap;

  DesignUtils::ForEachFileContent(
      design, [&](const SL::FileContent* fileContent) {
        SL::NodeId const kRoot = fileContent->getRootNode();
        if (!kRoot) {
          return;
        }

        for (SL::NodeId const kModuleDecl : fileContent->sl_collect_all(
                 kRoot, SL::VObjectType::paModule_declaration)) {
          SL::NodeId kAnsiHeader = fileContent->Child(kModuleDecl);
          if (!kAnsiHeader || (fileContent->Type(kAnsiHeader) !=
                                   SL::VObjectType::paModule_ansi_header &&
                               fileContent->Type(kAnsiHeader) !=
                                   SL::VObjectType::paModule_nonansi_header)) {
            continue;
          }

          std::string const kPrefix = GetPrefix(fileContent, kAnsiHeader);
          std::string const kFullName = kPrefix.substr(0, kPrefix.size() - 2);

          ModuleInfo info{.fullName = kFullName,
                          .nodeId = kModuleDecl,
                          .fileContent = fileContent};

          moduleMap[kFullName] = info;
        }

        for (SL::NodeId const kInterfaceDecl : fileContent->sl_collect_all(
                 kRoot, SL::VObjectType::paInterface_declaration)) {
          SL::NodeId kHeader = fileContent->Child(kInterfaceDecl);
          if (!kHeader || fileContent->Type(kHeader) !=
                              SL::VObjectType::paInterface_ansi_header) {
            continue;
          }

          std::string const kPrefix = GetPrefix(fileContent, kHeader);
          std::string const kFullName = kPrefix.substr(0, kPrefix.size() - 2);

          ModuleInfo info{.fullName = kFullName,
                          .nodeId = kInterfaceDecl,
                          .fileContent = fileContent};

          moduleMap[kFullName] = info;
        }

        for (SL::NodeId const kPackageDecl : fileContent->sl_collect_all(
                 kRoot, SL::VObjectType::paPackage_declaration)) {
          std::string const kShortName =
              GetStringConst(fileContent, kPackageDecl);
          if (kShortName == "") {
            continue;
          }

          ModuleInfo info{.fullName = kShortName,
                          .nodeId = kPackageDecl,
                          .fileContent = fileContent};

          moduleMap[kShortName] = info;
        }
      });
  return moduleMap;
}
}  // namespace ModuleUtils
