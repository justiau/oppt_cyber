#include "oppt/plugin/Plugin.hpp"
#include "oppt/opptCore/Distribution.hpp"
#include "../CyberUtils/CyberUtils.hpp"

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
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        // load action space discretizer here
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

};

OPPT_REGISTER_TRANSITION_PLUGIN(CyberTransitionPlugin)

}
