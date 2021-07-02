#ifndef _CYBER_INITIAL_BELIEF_PLUGIN_HPP_
#define _CYBER_INITIAL_BELIEF_PLUGIN_HPP_

#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include "../CyberUtils/Scenario.hpp"

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
        nStates = scenario->getStateSize();
        // last state will represent action success
        nStates += 1;
        // lower bound all start at 0
        lowerBound.resize(nStates, 0);
        // upper bound is num of values zero indexed
        std::vector<SVar> stateVars = scenario->getStateVars();
        for (auto it = stateVars.begin(); it != stateVars.end(); ++it) {
            upperBound.push_back(it->getValueCount() - 1);
        }
        // 0 for action failure 1 for action success
        upperBound.push_back(1);
        return true;
    }

    virtual RobotStateSharedPtr sampleAnInitState() override {
        VectorFloat initStateVec;
        auto randomGenerator = robotEnvironment_->getRobot()->getRandomEngine();
        for(int i=0; i < nStates; i++) {
            std::uniform_int_distribution<unsigned int> dist(lowerBound.at(i),upperBound.at(i));
            initStateVec.push_back(dist(*(randomGenerator.get())));
        }
        if (initStateVec.size() != nStates)
            ERROR("Init state size doesnt fit");
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
