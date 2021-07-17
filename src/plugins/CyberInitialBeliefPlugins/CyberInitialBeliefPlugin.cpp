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
        return true;
    }

    virtual RobotStateSharedPtr sampleAnInitState() override {
        VectorFloat initStateVec;
        auto randomGenerator = robotEnvironment_->getRobot()->getRandomEngine();
        for(int i=0; i < nStates - 1; i++) {
            SVar var = scenario->getStateVar(i);
            if (var.initValue == "uniform") {
                std::uniform_int_distribution<> d(0, var.getValueCount() - 1);
                auto randomGenerator = robotEnvironment_->getRobot()->getRandomEngine();
                initStateVec.push_back(d(*(randomGenerator.get())));
            } else {
                initStateVec.push_back(var.getIndex(var.initValue));
            }
        }
        // last state indicates action success
        initStateVec.push_back(0);
        if (initStateVec.size() != nStates)
            ERROR("Init state size doesnt fit");
        RobotStateSharedPtr initState(new VectorState(initStateVec));
        return initState;
    }

private:
    Scenario* scenario;
    int nStates;
};

OPPT_REGISTER_INITIAL_BELIEF_PLUGIN(CyberInitialBeliefPlugin)

}

#endif
