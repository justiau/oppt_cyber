#ifndef _CYBER_HEURISTIC_PLUGIN_HPP_
#define _CYBER_HEURISTIC_PLUGIN_HPP_
#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include "../CyberUtils/Scenario.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>

namespace oppt
{
class CyberES1HeuristicPlugin: public HeuristicPlugin
{
public:
    CyberES1HeuristicPlugin():
        HeuristicPlugin() {
    }

    virtual ~CyberES1HeuristicPlugin() {

    }

    virtual bool load(const std::string& optionsFile) override {        
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        maxReward = generalOptions->maxReward;
        scenario = generalOptions->getScenario();
        return true;
    }

    bool isStateSolveable() const {
        std::vector<SAction> actions = scenario->getActions();
        VectorFloat initialState = scenario->getOpptState();
        // for every action
        for (size_t i=0; i<actions.size(); ++i) {
            // reset state
            scenario->setOpptState(initialState);
            // apply action
            SAction action = actions[i];
            if (action.probSuccess_ > 0 && scenario->checkPreconditions(action)) {
                std::vector<Assignment> effects = action.onSuccess_.first;
                // assign each affect for action success
                for (auto e : effects) {
                    scenario->assignState(e);
                }
                // if max terminal is reachable then state is solveable
                for (auto t : scenario->getTerminals()) {
                    if (t.reward_ - action.cost_ == maxReward && scenario->isAllAssignTrue(t.conditions_)) {
                        return true;
                    }
                }
            }
        }
        scenario->setOpptState(initialState);
        return false;
    }

    virtual FloatType getHeuristicValue(const HeuristicInfo* heuristicInfo) const override {
        VectorFloat currentState = heuristicInfo->currentState->as<VectorState>()->asVector();
        scenario->setOpptState(currentState);
        return (isStateSolveable()) ? maxReward : 0;
    }   

private:
    Scenario* scenario;
    FloatType maxReward;

};

OPPT_REGISTER_HEURISTIC_PLUGIN(CyberES1HeuristicPlugin)

}

#endif
