#ifndef _CYBER_OBSERVATION_PLUGIN_HPP_
#define _CYBER_OBSERVATION_PLUGIN_HPP_

#include "oppt/plugin/Plugin.hpp"
#include "oppt/opptCore/Distribution.hpp"
#include "../CyberUtils/CyberUtils.hpp"

namespace oppt
{
class CyberObservationPlugin: public ObservationPlugin
{
public :
    CyberObservationPlugin():
        ObservationPlugin() {
    }

    virtual ~CyberObservationPlugin() = default;

    virtual bool load(const std::string& optionsFile) override {
        parseOptions_<CyberOptions>(optionsFile);
        CyberOptions* generalOptions = static_cast<CyberOptions*>(options_.get());
        scenario = generalOptions->getScenario();
        nObs = scenario->getObsSize();
        observationError_ = generalOptions->observationError;
        // exit(1);
        return true;
    }

    virtual ObservationResultSharedPtr getObservation(const ObservationRequest* observationRequest) const override {
        ObservationResultSharedPtr observationResult = std::make_shared<ObservationResult>();
        VectorFloat stateVec = observationRequest->currentState->as<VectorState>()->asVector();
        VectorFloat actionVec = observationRequest->action->as<VectorAction>()->asVector();
        // -1.0 observation value represents a null observation
        VectorFloat observationVec(nObs, -1.0);

        scenario->setOpptState(stateVec);
        scenario->setOpptObs(observationVec);
        long binNumber = 0;

        long actionVal = (unsigned int) actionVec[0] + 0.25;
        SAction action = scenario->getAction(actionVal);
        std::vector<Assignment> effects = (scenario->actionSuccess) ? action.onSuccess_.second : action.onFail_.second;
        for (auto e : effects) {
            scenario->assignObs(e);
        }
        // int obsId = 
        // int obsValue = 

        // only allows for one observation at a time
        // binNumber = 
        auto observationSpace = robotEnvironment_->getRobot()->getObservationSpace();
        ObservationSharedPtr observation = std::make_shared<DiscreteVectorObservation>(observationVec);
        observation->as<DiscreteVectorObservation>()->setBinNumber(binNumber);
        observationResult->observation = observation;
        observationResult->errorVector = observationRequest->errorVector;
        return observationResult;
    }

private:
    Scenario* scenario;
    int nObs;
    FloatType observationError_;
};

OPPT_REGISTER_OBSERVATION_PLUGIN(CyberObservationPlugin)

}

#endif