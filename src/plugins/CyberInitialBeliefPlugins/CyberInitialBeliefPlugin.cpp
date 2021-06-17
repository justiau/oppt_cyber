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
        scenario = generalOptions->getScenario();
        // lowerBound = generalOptions->lowerBound;
        // upperBound = generalOptions->upperBoun
        exit(1);
        return true;
    }

    virtual RobotStateSharedPtr sampleAnInitState() override {
        VectorFloat initStateVec;
        auto randomGenerator = robotEnvironment_->getRobot()->getRandomEngine();
        for(int i=0; i < nStates; i++) {
            std::uniform_int_distribution<unsigned int> dist(lowerBound.at(i),upperBound.at(i));
            initStateVec.push_back(dist(*(randomGenerator.get())));
        }
        // unsigned int stateDimension = robotEnvironment_->getRobot()->getStateSpace()->getNumDimensions();
        // if (initStateVec.size() != stateDimension)
        //     ERROR("Init state size doesnt fit");
        RobotStateSharedPtr initState(new VectorState(initStateVec));
        return initState;
    }

private:
    Scenario* scenario;
    int nStates;
    VectorFloat lowerBound;
    VectorFloat upperBound;
};

OPPT_REGISTER_INITIAL_BELIEF_PLUGIN(CyberInitialBeliefPlugin)

}

#endif
