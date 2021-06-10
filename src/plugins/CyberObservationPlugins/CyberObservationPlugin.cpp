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
        return true;
    }

    virtual ObservationResultSharedPtr getObservation(const ObservationRequest* observationRequest) const override {
        ObservationResultSharedPtr observationResult = std::make_shared<ObservationResult>();

        // Get the observation
        VectorFloat stateVec = observationRequest->currentState->as<VectorState>()->asVector();
        VectorFloat actionVec = observationRequest->action->as<VectorAction>()->asVector();
        VectorFloat observationVec(robotEnvironment_->getRobot()->getObservationSpace()->getNumDimensions(), 0.0);
        long binNumber = 0;
        if (actionVec[0] < 2.25) {
            observationVec[0] = 0.0;
        } else {
            FloatType probability = 1.0 - observationError_;
            bool obsMatches =
                std::bernoulli_distribution(
                    probability)(*(robotEnvironment_->getRobot()->getRandomEngine().get()));
            int stateInt = (int) stateVec[0] + 0.25;
            if (obsMatches) {
                observationVec[0] = stateInt;
            } else {
                // xor obs vec with 3 will convert 1 -> 2 and 2 - > 1
                observationVec[0] = stateInt^3;
            }
            binNumber = (int) observationVec[0] + 0.25;
        }
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