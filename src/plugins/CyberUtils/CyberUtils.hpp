// Author: Justin Au
#ifndef _CYBER_UTILS_HPP_
#define _CYBER_UTILS_HPP_
#include "oppt/plugin/PluginOptions.hpp"
#include "ScenarioParser.hpp"
#include "Scenario.hpp"

namespace oppt
{

class CyberOptions: public PluginOptions
{
public:
    CyberOptions() = default;

    virtual ~CyberOptions() = default;

    // path to the cyber scenario file
    std::string scenarioPath;

    float minReward;

    float maxReward;

    static std::unique_ptr<options::OptionParser> makeParser() {
        std::unique_ptr<options::OptionParser> parser =
            PluginOptions::makeParser();
        addCyberPluginOptions(parser.get());
        return std::move(parser);
    }

    static void addCyberPluginOptions(options::OptionParser* parser) {
        /*** Cyber scenario parser options ***/
        parser->addOption<std::string>("cyberScenarioOptions",
                                        "scenarioPath",
                                        &CyberOptions::scenarioPath);

        parser->addOption<float>("cyberScenarioOptions",
                                        "minReward",
                                        &CyberOptions::minReward);

        parser->addOption<float>("cyberScenarioOptions",
                                        "maxReward",
                                        &CyberOptions::maxReward);
    }

    Scenario* getScenario() {
        if (scenarioPath.empty())
            ERROR("getScenario called before path was set.");
        ScenarioParser scenarioParser;
        scenarioParser.parse(scenarioPath);
        return scenarioParser.get();
    }

};

}

#endif