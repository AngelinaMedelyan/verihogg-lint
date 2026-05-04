#include <Surelog/Design/Design.h>

#include <string>
#include <unordered_map>

struct ConfigInfo {
  SURELOG::NodeId nodeid;
  const SURELOG::FileContent* fileContent = nullptr;
};

namespace ConfigUtils {

auto CollectAllConfig(SURELOG::Design* design)
    -> std::unordered_map<std::string, ConfigInfo>;

}  // namespace ConfigUtils
