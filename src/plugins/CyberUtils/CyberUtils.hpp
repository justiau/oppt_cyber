// Author: Justin Au
#ifndef _CYBER_UTILS_HPP_
#define _CYBER_UTILS_HPP_
#include "oppt/plugin/PluginOptions.hpp"
#include "ScenarioParser.hpp"

namespace oppt
{

class CyberOptions: public PluginOptions
{
public:
    CyberOptions() = default;

    virtual ~CyberOptions() = default;

    // path to the cyber scenario file
    std::string scenarioPath;

    // upperbound and lowerbound for initial belief values
    VectorFloat upperBound;
    VectorFloat lowerBound;

    // observation error
    FloatType observationError;

    // reward options
    FloatType stepPenalty;
    FloatType exitReward;
    FloatType illegalMovePenalty;

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

        /*** Initial belief options ***/

        // Lower starting bound
        parser->addOption<VectorFloat>("initialBeliefOptions",
                                       "lowerBound",
                                       &CyberOptions::lowerBound);
        // Upper starting bound
	    parser->addOption<VectorFloat>("initialBeliefOptions",
                                       "upperBound",
                                       &CyberOptions::upperBound);

        /*** Observation options ***/
        parser->addOption<FloatType>("observationPluginOptions",
                                     "observationError",
                                     &CyberOptions::observationError);

        /*** Reward Plugin options ***/
        parser->addOption<FloatType>("rewardPluginOptions",
                                         "stepPenalty",
                                         &CyberOptions::stepPenalty);

        parser->addOption<FloatType>("rewardPluginOptions",
                                         "exitReward",
                                         &CyberOptions::exitReward);

        parser->addOption<FloatType>("rewardPluginOptions",
                                         "illegalMovePenalty",
                                         &CyberOptions::illegalMovePenalty);

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