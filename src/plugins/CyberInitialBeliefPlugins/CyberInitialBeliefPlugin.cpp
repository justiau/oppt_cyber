#ifndef _CYBER_INITIAL_BELIEF_PLUGIN_HPP_
#define _CYBER_INITIAL_BELIEF_PLUGIN_HPP_

#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include <stdlib.h>

namespace oppt
{
class CyberInitialBeliefPlugin: public InitialBeliefPlugin
{
public:
    CyberInitialBeliefPlugin():
        InitialBeliefPlugin(){
    }

    virtual ~CyberInitialBeliefPlugin() = default;

    virtual bool load(const std::string& optionsFile) override {
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        lowerBound = generalOptions->lowerBound;
        upperBound = generalOptions->upperBound;
        generalOptions->getScenario();
        exit(1);
        return true;
    }

    virtual RobotStateSharedPtr sampleAnInitState() override {
        std::uniform_int_distribution<unsigned int> dist(lowerBound[0],upperBound[0]);

        auto randomGenerator = robotEnvironment_->getRobot()->getRandomEngine();
        VectorFloat initStateVec = VectorFloat({dist(*(randomGenerator.get()))});
        unsigned int stateDimension = robotEnvironment_->getRobot()->getStateSpace()->getNumDimensions();
        if (initStateVec.size() != stateDimension)
            ERROR("Init state size doesnt fit");
        RobotStateSharedPtr initState(new VectorState(initStateVec));
        return initState;
    }

private:
    VectorFloat lowerBound;
    VectorFloat upperBound;
};

OPPT_REGISTER_INITIAL_BELIEF_PLUGIN(CyberInitialBeliefPlugin)

}

#endif
