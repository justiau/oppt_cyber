#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include <iostream>

namespace oppt
{
class CyberRewardPlugin: public RewardPlugin
{
public:
    CyberRewardPlugin():
        RewardPlugin() {
    }

    virtual ~CyberRewardPlugin() = default;

    virtual bool load(const std::string& optionsFile) override {
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        scenario = generalOptions->getScenario();
        // stepPenalty = generalOptions->stepPenalty;
        // exitReward = generalOptions->exitReward;
        // illegalMovePenalty = generalOptions->illegalMovePenalty;
        return true;
    }

    virtual FloatType getReward(const PropagationResultSharedPtr& propagationResult) const override {
        VectorFloat previousStateVector = propagationResult->previousState->as<VectorState>()->asVector();
        VectorFloat currentStateVector = propagationResult->nextState->as<VectorState>()->asVector();
        VectorFloat actionVec = propagationResult->action->as<VectorAction>()->asVector();
        scenario->setOpptState(currentStateVector);
        long actionVal = (unsigned int) actionVec[0] + 0.25;
        SAction action = scenario->getAction(actionVal);
        float reward = -1 * action.cost_;
        for (auto t : scenario->getTerminals()) {
            if (scenario->isAllAssignTrue(t.conditions_)) {
                reward += t.reward_;
            }
        }
        return reward;
    }

    virtual std::pair<FloatType, FloatType> getMinMaxReward() const override {
        // greedy min max
        std::vector<SAction> actions = scenario->getActions();
        std::vector<Terminal> terminals = scenario->getTerminals();
        float minActionReward = actions[0].cost_;
        float maxActionReward = actions[0].cost_;
        float minTerminalReward = terminals[0].reward_;
        float maxTerminalReward = terminals[0].reward_;
        // min and max action cost
        for (auto a : actions) {
            // convert cost to reward
            float reward = -1 * a.cost_;
            if (reward < minActionReward) minActionReward = reward;
            if (reward > maxActionReward) maxActionReward = reward;
        }
        // min and max terminal reward
        for (auto t : terminals) {
            if (t.reward_ < minTerminalReward) minTerminalReward = t.reward_;
            if (t.reward_ > maxTerminalReward) maxTerminalReward = t.reward_;
        }
        return std::make_pair(minActionReward + minTerminalReward, maxActionReward + maxTerminalReward);
    }

private:
    Scenario* scenario;
    FloatType stepPenalty;
    FloatType exitReward;
    FloatType illegalMovePenalty;
};

OPPT_REGISTER_REWARD_PLUGIN(CyberRewardPlugin)
}
