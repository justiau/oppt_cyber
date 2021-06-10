#include "oppt/plugin/Plugin.hpp"
#include "../CyberUtils/CyberUtils.hpp"

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
        // stepPenalty = generalOptions->stepPenalty;
        // exitReward = generalOptions->exitReward;
        // illegalMovePenalty = generalOptions->illegalMovePenalty;
        return true;
    }

    virtual FloatType getReward(const PropagationResultSharedPtr& propagationResult) const override {
        VectorFloat previousStateVector = propagationResult->previousState->as<VectorState>()->asVector();
        VectorFloat currentStateVector = propagationResult->nextState->as<VectorState>()->asVector();
        VectorFloat actionVec = propagationResult->action->as<VectorAction>()->asVector();
        if (actionVec[0] < 2.25) {
            if ((int) actionVec[0] + 0.25 == (int) previousStateVector[0] + 0.25) {
                // opened door with tiger
                return -100.0;
            } else {
                // opened different door
                return 10.0;
            }
        }
        // agent chose to listen
        return -1.0;
    }

    virtual std::pair<FloatType, FloatType> getMinMaxReward() const override {
        return std::make_pair(-100.0, 10.0);
    }

private:
    FloatType stepPenalty;
    FloatType exitReward;
    FloatType illegalMovePenalty;
};

OPPT_REGISTER_REWARD_PLUGIN(CyberRewardPlugin)
}
