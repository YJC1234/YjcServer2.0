#pragma once

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <exception>
#include <functional>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <boost/lexical_cast.hpp>
#include <boost/type_index.hpp>

namespace YServer {

/// @brief 配置变量基类
class ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVarBase>;

protected:
    std::string m_name;
    std::string m_description;

public:
    ConfigVarBase(const std::string& name, const std::string& description = "")
        : m_name(name), m_description(description) {
        std::transform(m_name.begin(), m_name.end(), m_name.begin(), ::tolower);
    }

    /// @brief 析构函数
    ~ConfigVarBase() {}

    /// @brief 返回配置参数名称
    const std::string& getName() const {
        return m_name;
    }

    /// @brief 返回配置参数
    const std::string& getDescription() const {
        return m_description;
    }
    /// @brief 转成字符串
    virtual std::string toString() = 0;

    /// @brief 从字符串初始化值
    virtual bool fromString(const std::string&) = 0;

    /// @brief 返回配置参数的类型名称
    virtual std::string getTypeName() = 0;
};

/*
 *------------------------------------------------------------
 *-------------------------类型转换----------------------------
 *------------------------------------------------------------
 */

// 类型转换
template <class From, class To>
class LexicalCast {
public:
    To operator()(const From& v) {
        return boost::lexical_cast<To>(v);
    }
};

//-------------根据模板匹配最具体，通过偏特化可以实现递归解析-----------

template <class T>
class LexicalCast<std::string, std::vector<T>> {
public:
    std::vector<T> operator()(const std::string& v) {
        YAML::Node        node = YAML::Load(v);
        std::vector<T>    res;
        std::stringstream ss;
        for (int i = 0; i < node.size(); i++) {
            ss.str("");
            ss << node[i];
            res.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return res;
    }
};

template <class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator()(const std::vector<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (int i = 0; i < v.size(); i++) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(v[i])));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <class T>
class LexicalCast<std::string, std::list<T>> {
public:
    std::list<T> operator()(const std::string& v) {
        YAML::Node        node = YAML::Load(v);
        std::list<T>      vec;
        std::stringstream ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.push_back(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::list<T>, std::string> {
public:
    std::string operator()(const std::list<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <class T>
class LexicalCast<std::string, std::set<T>> {
public:
    std::set<T> operator()(const std::string& v) {
        YAML::Node           node = YAML::Load(v);
        typename std::set<T> vec;
        std::stringstream    ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator()(const std::set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <class T>
class LexicalCast<std::string, std::unordered_set<T>> {
public:
    std::unordered_set<T> operator()(const std::string& v) {
        YAML::Node                     node = YAML::Load(v);
        typename std::unordered_set<T> vec;
        std::stringstream              ss;
        for (size_t i = 0; i < node.size(); ++i) {
            ss.str("");
            ss << node[i];
            vec.insert(LexicalCast<std::string, T>()(ss.str()));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::unordered_set<T>, std::string> {
public:
    std::string operator()(const std::unordered_set<T>& v) {
        YAML::Node node(YAML::NodeType::Sequence);
        for (auto& i : v) {
            node.push_back(YAML::Load(LexicalCast<T, std::string>()(i)));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <class T>
class LexicalCast<std::string, std::map<std::string, T>> {
public:
    std::map<std::string, T> operator()(const std::string& v) {
        YAML::Node                        node = YAML::Load(v);
        typename std::map<std::string, T> vec;
        std::stringstream                 ss;
        for (auto it = node.begin(); it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(it->first.Scalar(),
                                      LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <class T>
class LexicalCast<std::string, std::unordered_map<std::string, T>> {
public:
    std::unordered_map<std::string, T> operator()(const std::string& v) {
        YAML::Node                                  node = YAML::Load(v);
        typename std::unordered_map<std::string, T> vec;
        std::stringstream                           ss;
        for (auto it = node.begin(); it != node.end(); ++it) {
            ss.str("");
            ss << it->second;
            vec.insert(std::make_pair(
                it->first
                    .Scalar(),  //这里是因为第一个肯定是string，可以用scalar
                LexicalCast<std::string, T>()(ss.str())));
        }
        return vec;
    }
};

template <class T>
class LexicalCast<std::unordered_map<std::string, T>, std::string> {
public:
    std::string operator()(const std::unordered_map<std::string, T>& v) {
        YAML::Node node(YAML::NodeType::Map);
        for (auto& i : v) {
            node[i.first] = YAML::Load(LexicalCast<T, std::string>()(i.second));
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
/*
 *------------------------------------------------------------
 *------------------------------------------------------------
 *-----------------------------------------------------------
 */

template <class T, class FromStr = LexicalCast<std::string, T>,
          class ToStr = LexicalCast<T, std::string>>
class ConfigVar : public ConfigVarBase {
public:
    using ptr = std::shared_ptr<ConfigVar<T>>;

    using on_change_cb =
        std::function<void(const T& old_value, const T& new_value)>;

private:
    std::shared_mutex                m_mutex;
    T                                m_value;
    std::map<uint64_t, on_change_cb> m_cbs;

public:
    ConfigVar(const std::string& name, const std::string& description,
              const T& default_value)
        : ConfigVarBase(name, description), m_value(default_value) {}

    //参数转化为YAML string,失败则抛出异常
    std::string toString() override {
        try {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            return ToStr()(m_value);
        }
        catch (std::exception& e) {
            spdlog::get("system_logger")
                ->error("Configure var::toString exception : {}, convert "
                        "{} to string name {}",
                        e.what(), typeid(T).name(), m_name);
        }
        return "";
    }

    ///从yaml string初始化参数
    virtual bool fromString(const std::string& val) override {
        try {
            setValue(FromStr()(val));
            return true;
        }
        catch (std::exception& e) {
            spdlog::get("system_logger")
                ->error("Configure var::fromString exception : {}\n, convert "
                        "{} to string name {}",
                        e.what(), typeid(T).name(), m_name);
        }
        return false;
    }

    T getValue() {
        std::shared_lock<std::shared_mutex> lock(m_mutex);
        return m_value;
    }

    //设置当前参数的值，如果值发生变化，通知对应的回调函数
    void setValue(const T& v) {
        {
            std::shared_lock<std::shared_mutex> lock(m_mutex);
            if (v == m_value) {
                return;
            }
            for (auto& i : m_cbs) {
                i.second(m_value, v);
            }
        }
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_value = v;
    }

    std::string getTypeName() {
        return boost::typeindex::type_id<T>().pretty_name();
    }

    /// @brief 添加变化回调函数
    /// @return 返回该回调函数对应的唯一id，用于删除回调
    uint64_t addListener(on_change_cb cb) {
        static uint64_t                     s_fun_id = 0;
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        ++s_fun_id;
        m_cbs[s_fun_id] = std::move(cb);
        return s_fun_id;
    }

    /// @brief 删除变化回调函数
    /// @param key 被删除函数的唯一id
    void delListener(uint64_t key) {
        std::unique_lock<std::shared_mutex> lock(m_mutex);
        m_cbs.erase(key);
    }
};

// ConfigVar的管理类
class Config {
public:
    using ConfigVarMap = std::map<std::string, ConfigVarBase::ptr>;

private:
    /// @brief 返回所有配置项(单例模式)
    static ConfigVarMap& GetDatas() {
        static ConfigVarMap s_datas;
        return s_datas;
    }

    /// @brief 返回配置项的rwMutex
    static std::shared_mutex& GetMutex() {
        static std::shared_mutex s_mutex;
        return s_mutex;
    }

public:
    //查找(name->T)。存在则直接返回，否则用default_value创建.
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name,
                                             const std::string& description,
                                             const T&           default_value) {
        std::unique_lock<std::shared_mutex> lock(GetMutex());

        auto it = GetDatas().find(name);
        if (it != GetDatas().end()) {
            auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
            if (tmp) {
                spdlog::get("system_logger")
                    ->info("Lookup name = {} exists.", name);
                return tmp;
            } else {
                spdlog::get("system_logger")
                    ->error("Lookup name = {} exists but type not {} "
                            "realtype = {}, real is {}",
                            name, typeid(T).name(), it->second->getTypeName(),
                            it->second->toString());
                return nullptr;
            }
        }

        if (name.find_first_not_of("abcdefghikjlmnopqrstuvwxyz._0123456789") !=
            std::string::npos) {
            spdlog::error("Lookup name:{} is not valid.", name);
            throw std::invalid_argument(name);
        }

        typename std::shared_ptr<ConfigVar<T>> v(
            new ConfigVar<T>(name, description, default_value));
        GetDatas()[name] = v;
        return v;
    }

    //查找(name->T)，查找失败返回nullptr
    template <class T>
    static typename ConfigVar<T>::ptr Lookup(const std::string& name) {
        std::shared_lock<std::shared_mutex> lock(GetMutex());

        auto it = GetDatas().find(name);
        if (it == GetDatas().end()) {
            return nullptr;
        }
        auto tmp = std::dynamic_pointer_cast<ConfigVar<T>>(it->second);
        if (tmp) {
            spdlog::get("system_logger")
                ->info("Lookup name = {} exists.", name);
            return tmp;
        }
        spdlog::get("system_logger")
            ->error("Lookup name = {} exists but type not {} "
                    "realtype = {}, real is {}",
                    name, typeid(T).name(), it->second->getTypeName(),
                    it->second->toString());
        return nullptr;
    }

    //查找配置参数，返回配置参数基类
    static ConfigVarBase::ptr LookupBase(std::string& name);

    //从Node初始化配置模块
    static void LoadFromYaml(YAML::Node& root);

    //加载path文件夹中所有配置文件
    static void LoadFromConfigDir(const std::string& path, bool force = false);

    //用cb遍历配置项所有参数
    static void Visit(std::function<void(ConfigVarBase::ptr)>& cb);
};

}  // namespace YServer
