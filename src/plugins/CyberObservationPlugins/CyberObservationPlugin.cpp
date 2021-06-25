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
        // The observationError parameter acts as the efficiency distance here
        observationError_ = generalOptions->observationError;
        exit(1);
        return true;
    }

    virtual ObservationResultSharedPtr getObservation(const ObservationRequest* observationRequest) const override {
        ObservationResultSharedPtr observationResult = std::make_shared<ObservationResult>();

        // Get the observation
        VectorFloat stateVec = observationRequest->currentState->as<VectorState>()->asVector();
        VectorFloat actionVec = observationRequest->action->as<VectorAction>()->asVector();
        VectorFloat observationVec(robotEnvironment_->getRobot()->getObservationSpace()->getNumDimensions(), 0.0);
        
        int actionVal = (unsigned int) actionVec[0] + 0.25;
        

        auto observationSpace = robotEnvironment_->getRobot()->getObservationSpace();
        ObservationSharedPtr observation = std::make_shared<DiscreteVectorObservation>(observationVec);
        observation->as<DiscreteVectorObservation>()->setBinNumber(binNumber);
        observationResult->observation = observation;
        observationResult->errorVector = observationRequest->errorVector;
        return observationResult;
    }

private:
    FloatType observationError_;
};

OPPT_REGISTER_OBSERVATION_PLUGIN(CyberObservationPlugin)

}

#endif