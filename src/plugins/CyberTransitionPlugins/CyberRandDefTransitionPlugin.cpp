#include "oppt/plugin/Plugin.hpp"
#include "oppt/opptCore/Distribution.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include "../CyberUtils/Scenario.hpp"
#include "../CyberUtils/SAction.hpp"
#include "CyberActionSpaceDiscretizer.hpp"
#include <tuple>
#include <iostream>

namespace oppt
{
class CyberRandDefTransitionPlugin: public TransitionPlugin
{
public:
    CyberRandDefTransitionPlugin():
        TransitionPlugin() {
    }

    virtual ~CyberRandDefTransitionPlugin() = default;

    virtual bool load(const std::string& optionsFile) override {
        // overwrite action space with custom action space discretizer
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        scenario = generalOptions->getScenario();
        defender = generalOptions->getDefender();
        auto actionSpace = robotEnvironment_->getRobot()->getActionSpace();

        // upper and lower bounds from scenario values
        FloatType lowerLimit = 0;
        FloatType upperLimit = scenario->getActionsSize() - 1;

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
        VectorFloat currentState = propagationRequest->currentState->as<VectorState>()->asVector();
        std::uniform_real_distribution<double> successDist(0,1);
        auto randomGenerator = robotEnvironment_->getRobot()->getRandomEngine();
        // attacker action
        // load oppt state into scenario
        scenario->setOpptState(currentState);
        
        int actionVal = (unsigned int) actionVec[0] + 0.25;
        SAction action = scenario->getAction(actionVal);
        FloatType p = action.probSuccess_;
        FloatType success = (FloatType) successDist(*(randomGenerator.get()));

        // action preconditions depend on currentState
        bool preconTrue = scenario->checkPreconditions(action);
        bool actionSuccess = preconTrue && (success < p);
        std::vector<Assignment> effects = (actionSuccess) ? action.onSuccess_.first : action.onFail_.first;
        for (auto e : effects) {
            scenario->assignState(e);
        }

        // defender action
        // get random defender action
        int defActionSize = defender->getActionsSize();
        std::uniform_int_distribution<> defActionDist(0,defActionSize - 1);
        int defActionVal = defActionDist(*(randomGenerator.get()));
        SAction defAction = defender->getAction(defActionVal);
        FloatType defActionProb = defAction.probSuccess_;
        FloatType defSuccess = (FloatType) successDist(*(randomGenerator.get()));
        bool defPreconTrue = scenario->checkPreconditions(defAction);
        bool defActionSuccess = defPreconTrue && (defSuccess < defActionProb);
        std::vector<Assignment> defEffects = (defActionSuccess) ? defAction.onSuccess_.first : defAction.onFail_.first;
        for (auto e : defEffects) {
            scenario->assignState(e);
        }
        VectorFloat resultingState = scenario->getOpptState();
        // set action success state to 1 or 0  depending on result to be used by reward and observation
        resultingState.back() = (actionSuccess) ? 1.0 : 0.0;
        propagationResult->previousState = propagationRequest->currentState.get();
        propagationResult->nextState = std::make_shared<oppt::VectorState>(resultingState);
        return propagationResult;
    }
    
private:
    Scenario* scenario;
    Scenario* defender;
};

OPPT_REGISTER_TRANSITION_PLUGIN(CyberRandDefTransitionPlugin)

}
