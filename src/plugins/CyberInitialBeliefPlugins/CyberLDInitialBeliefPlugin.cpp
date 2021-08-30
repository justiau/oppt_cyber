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
        // second last state will respresent decay
        nStates += 1;
        // last state will represent action success
        nStates += 1;
        decayStep = scenario->getDecayStep();
        return true;
    }

    virtual RobotStateSharedPtr sampleAnInitState() override {
        VectorFloat initStateVec;
        auto randomGenerator = robotEnvironment_->getRobot()->getRandomEngine();
        for(int i=0; i < nStates - 2; i++) {
            SVar var = scenario->getStateVar(i);
            if (var.initValue == "uniform") {
                std::uniform_int_distribution<> d(0, var.getValueCount() - 1);
                initStateVec.push_back(d(*(randomGenerator.get())));
            } else {
                initStateVec.push_back(var.getIndex(var.initValue));
            }
        }
        // set decay value to uniform value over discretized resolution
        if (decayStep > 0) {
            int maxDecayEnum = static_cast<int>(1 / decayStep);
            std::uniform_int_distribution<> d(0, maxDecayEnum);
            initStateVec.push_back(d(*(randomGenerator.get())));
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
    float decayStep;
};

OPPT_REGISTER_INITIAL_BELIEF_PLUGIN(CyberInitialBeliefPlugin)

}

#endif
