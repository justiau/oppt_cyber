// Author: Justin Au
#ifndef _CYBER_UTILS_HPP_
#define _CYBER_UTILS_HPP_
#include "oppt/plugin/PluginOptions.hpp"

namespace oppt
{

class CyberOptions: public PluginOptions
{
public:
    CyberOptions() = default;

    virtual ~CyberOptions() = default;

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
        addIDPluginOptions(parser.get());
        return std::move(parser);
    }

    static void addIDPluginOptions(options::OptionParser* parser) {
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
};

}

#endif