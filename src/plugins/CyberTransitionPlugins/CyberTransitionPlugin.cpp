#include "oppt/plugin/Plugin.hpp"
#include "oppt/opptCore/Distribution.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include "../CyberUtils/Scenario.hpp"
#include "CyberActionSpaceDiscretizer.hpp"
#include <"
#include <tuple>
#include <random>
#include <chrono>

namespace oppt
{
class CyberTransitionPlugin: public TransitionPlugin
{
public:
    CyberTransitionPlugin():
        TransitionPlugin() {
    }

    virtual ~CyberTransitionPlugin() = default;

    virtual bool load(const std::string& optionsFile) override {
        // overwrite action space with custom action space discretizer
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        scenario = generalOptions->getScenario();

        // get action probabilities from cyber options
        // actionProbs_ = generalOptions->actionProbs;

        // get action probabilities from scenario file
        actionProbs_ = scenario->getActionProbs();


        // get action space and upper and lower bounds from robot cfg
        auto actionSpace = robotEnvironment_->getRobot()->getActionSapce();
        VectorFloat lowerControlLimits;
        VectorFloat upperControlLimits;
        oppt::ActionLimitsSharedPtr actionLimits = actionSpace->getActionLimits();
        actionLimits->getLimits()->as<VectorLimitsContainer>()->get(lowerControlLimits, upperControlLimits);
        FloatType lowerLimit = lowerControlLimits[0];
        FloatType upperLimit = upperControlLimits[0];

        // set action space discretizer to upper and lower bounds
        CyberActionSpaceDiscretizer cyberActionSpaceDiscretizer(actionSpace);
        cyberActionSpaceDiscretizer.setActionIndices(lowerLimit,upperLimit);

        // share pointer to action space discretizer
        std::shared_ptr<ActionSpaceDiscretizer> cyberActionSpaceDiscretizerShared =
            std::make_shared<CyberActionSpaceDiscretizer>(cyberActionSpaceDiscretizer);
        actionSpace->setActionSpaceDiscretizer(cyberActionSpaceDiscretizerShared);
        return true;
    }

    virtual PropagationResultSharedPtr propagateState(const PropagationRequest* propagationRequest) const override {
        PropagationResultSharedPtr propagationResult(new PropagationResult());
        VectorFloat actionVec = propagationRequest->action->as<VectorAction>()->asVector();
        VectorFloat resultingState(propagationRequest->currentState->as<VectorState>()->asVector());
        if (actionVec[0] < 2.25) resultingState[0] = 3.0;
        propagationResult->previousState = propagationRequest->currentState.get();
        propagationResult->nextState =
            std::make_shared<oppt::VectorState>(resultingState);
        return propagationResult;
    }
    
private:
    Scenario* scenario;
    VectorFloat actionProbs_;
};

OPPT_REGISTER_TRANSITION_PLUGIN(CyberTransitionPlugin)

}
