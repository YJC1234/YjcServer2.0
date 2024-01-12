#include <Config/Config.h>
#include <Config/util.h>

namespace YServer {

//查找配置参数，返回配置参数基类
ConfigVarBase::ptr Config::LookupBase(std::string& name) {
    auto it = GetDatas().find(name);
    return it == GetDatas().end() ? nullptr : it->second;
}

static void
listAllMember(const std::string& prefix, const YAML::Node& node,
              std::list<std::pair<std::string, const YAML::Node>>& output) {
    if (prefix.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._012345678") !=
        std::string::npos) {
        spdlog::get("system_logger")
            ->error("Config::listAllMember():Config name invalid:{} ", prefix);
        return;
    }
    output.push_back({prefix, node});
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            listAllMember(prefix.empty() ? it->first.Scalar()
                                         : prefix + "." + it->first.Scalar(),
                          it->second, output);
        }
    }
}

//从Node初始化配置模块,参数名为"xxx.xxx"
void Config::LoadFromYaml(YAML::Node& root) {
    std::list<std::pair<std::string, const YAML::Node>> all_nodes;
    listAllMember("", root, all_nodes);
    for (auto& i : all_nodes) {
        std::string key = i.first;
        if (key.empty()) {
            continue;
        }
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        ConfigVarBase::ptr var = LookupBase(key);
        if (!var) {
            continue;
        }
        if (i.second.IsScalar()) {
            var->fromString(i.second.Scalar());
        } else {
            std::stringstream ss;
            ss << i.second;
            var->fromString(ss.str());
        }
    }
}

//加载path文件夹中所有配置文件
void Config::LoadFromConfigDir(const std::string& path, bool force) {
    std::vector<std::string> files;
    FSUtil::ListAllFiles(files, path, ".yml");
    for (auto& i : files) {
        try {
            YAML::Node node = YAML::LoadFile(i);
            LoadFromYaml(node);
            spdlog::get("system_logger")->info("\nLoad ConfigFile = {} OK.", i);
        }
        catch (...) {
            spdlog::get("system_logger")
                ->error("\nLoad ConfigFile = {} failed.", i);
        }
    }
}

//用cb遍历配置项所有参数
void Config::Visit(std::function<void(ConfigVarBase::ptr)>& cb) {
    std::shared_lock<std::shared_mutex> lock(GetMutex());

    ConfigVarMap& m = GetDatas();
    for (auto it = m.begin(); it != m.end(); ++it) {
        cb(it->second);
    }
}
}  // namespace YServer