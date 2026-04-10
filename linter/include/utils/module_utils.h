#pragma once

#include <Surelog/Design/Design.h>
#include <Surelog/Design/FileContent.h>

#include <string>
#include <unordered_map>

struct ModuleInfo {
  std::string_view fullName;
  SURELOG::NodeId nodeId;
  const SURELOG::FileContent* fileContent;
};

namespace ModuleUtils {
auto CollectAllModules(SURELOG::Design* design)
    -> std::unordered_map<std::string, ModuleInfo>;
}
