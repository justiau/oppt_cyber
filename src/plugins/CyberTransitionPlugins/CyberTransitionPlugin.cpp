#include "oppt/plugin/Plugin.hpp"
#include "oppt/opptCore/Distribution.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include "../CyberUtils/Scenario.hpp"
#include "CyberActionSpaceDiscretizer.hpp"
#include "../CyberUtils/SAction.hpp"
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
        VectorFloat resultingState(currentState);

        unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
        std::default_random_engine generator(seed1);
        std::uniform_real_distribution<double> distribution(0,1);

        int actionVal = (unsigned int) actionVec[0] + 0.25;
        SAction action = scenario->getAction(actionVal);
        FloatType p = action.probSuccess_;
        FloatType success = (FloatType) distribution(generator);
        bool preconTrue = scenario->isPreconditionsTrue(action.preconditions_);
        if (preconTrue) {
            // on preconditions true
            if (success < p) {
                // on success
                for (auto a : action.onSuccess_.first) {
                    scenario->makeAssignment(a);
                }
            } else {
                // on fail
                for (auto a : action.onFail_.first) {
                    scenario->makeAssignment(a);
                }
            }
        }

        propagationResult->previousState = propagationRequest->currentState.get();
        propagationResult->nextState = std::make_shared<oppt::VectorState>(resultingState);
        return propagationResult;
    }
    
private:
    Scenario* scenario;
};

OPPT_REGISTER_TRANSITION_PLUGIN(CyberTransitionPlugin)

}
