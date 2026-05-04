#include "utils/config_utils.h"

#include <Surelog/Common/NodeId.h>
#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>
#include <Surelog/SourceCompile/VObjectTypes.h>

#include <string>
#include <unordered_map>

#include "utils/ast_utils.h"
#include "utils/design_utils.h"

namespace ConfigUtils {

auto CollectAllConfig(SL::Design* design)
    -> std::unordered_map<std::string, ConfigInfo> {
  std::unordered_map<std::string, ConfigInfo> allConfigs;

  DesignUtils::ForEachFileContent(design, [&](const SL::FileContent* fileCont) {
    SL::NodeId const kRoot = fileCont->getRootNode();
    if (!kRoot) {
      return;
    }

    for (SL::NodeId const kConfigDecl : fileCont->sl_collect_all(
             kRoot, SL::VObjectType::paConfig_declaration)) {
      std::string const kFullName = GetFullName(fileCont, kConfigDecl);
      if (kFullName == "") {
        continue;
      }
      allConfigs[kFullName] = ConfigInfo{
          .nodeid = kConfigDecl,
          .fileContent = fileCont,
      };
    }
  });

  return allConfigs;
}
}  // namespace ConfigUtils
