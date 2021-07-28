#include "oppt/plugin/Plugin.hpp"
#include "oppt/opptCore/Distribution.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include "../CyberUtils/Scenario.hpp"
#include "../CyberUtils/SAction.hpp"
#include "CyberActionSpaceDiscretizer.hpp"
#include <tuple>
#include <unordered_set>

namespace oppt
{
class CyberTransitionPlanningPlugin: public TransitionPlugin
{
public:
    CyberTransitionPlanningPlugin():
        TransitionPlugin() {
    }

    virtual ~CyberTransitionPlanningPlugin() = default;

    virtual bool load(const std::string& optionsFile) override {
        // overwrite action space with custom action space discretizer
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        scenario = generalOptions->getScenario();

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

        // load oppt state into scenario
        scenario->setOpptState(currentState);

        int actionVal = (unsigned int) actionVec[0] + 0.25;
        SAction action = scenario->getAction(actionVal);
        FloatType p = action.probSuccess_;
        FloatType success = (FloatType) successDist(*(randomGenerator.get()));

        // action preconditions depend on currentState
        bool preconTrue = scenario->isAllAssignTrue(action.preconditions_);
        if (preconTrue) {
            // on preconditions true
            std::vector<Assignment> effects = (success < p) ? action.onSuccess_.first : action.onFail_.first;
            for (auto e : effects) {
                scenario->assignState(e);
            }
        }
        
        // simulate defender behaviour with information decay
        FloatType decaySuccess;
        std::unordered_set<std::string> affectedSet = action.getAffectedSet();
        std::vector<SVar> stateVars = scenario->getStateVars();
        for (size_t i=0; i<stateVars.size(); ++i) {
            SVar var = stateVars[i];
            if (var.decay > 0) {
                if (affectedSet.find(var.name_) == affectedSet.end()) {
                    decaySuccess = (FloatType) successDist(*(randomGenerator.get()));
                    if (decaySuccess < var.decay) {
                        int opptVal = scenario->getOpptVal(i);
                        std::vector<std::string> values = var.getValues();
                        // erase existing value
                        values.erase(values.begin() + opptVal);
                        std::uniform_int_distribution<> decayStateDist(0,values.size() - 1);
                        // make decay assignment
                        int decayStateVal = decayStateDist(*(randomGenerator.get()));
                        Assignment decayChange = var.createAssignment(values[decayStateVal]);
                        scenario->assignState(decayChange);
                    }
                }
            }
        }

        VectorFloat resultingState = scenario->getOpptState();
        // set action success state to 1 or 0  depending on result to be used by reward and observation
        resultingState.back() = (success < p) ? 1.0 : 0.0;
        propagationResult->previousState = propagationRequest->currentState.get();
        propagationResult->nextState = std::make_shared<oppt::VectorState>(resultingState);
        return propagationResult;
    }
    
private:
    Scenario* scenario;
};

OPPT_REGISTER_TRANSITION_PLUGIN(CyberTransitionPlanningPlugin)

}
