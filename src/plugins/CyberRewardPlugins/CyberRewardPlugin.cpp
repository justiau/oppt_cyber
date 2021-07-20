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
        minReward = generalOptions->minReward;
        maxReward = generalOptions->maxReward;
        scenario = generalOptions->getScenario();
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
        return std::make_pair(minReward, maxReward);
    }

private:
    Scenario* scenario;
    FloatType minReward;
    FloatType maxReward;
};

OPPT_REGISTER_REWARD_PLUGIN(CyberRewardPlugin)
}
