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
        auto observationSpace = robotEnvironment_->getRobot()->getObservationSpace();
        nObs = scenario->getObsSize();
        scenario->updateMaxObsVal();
        lowerBound_.resize(nObs, -1.0);
        for (auto var : scenario->getAllObs()) {
            upperBound_.push_back(var.getValueCount() - 1);
        }
        observationError_ = generalOptions->observationError;
        return true;
    }

    virtual ObservationResultSharedPtr getObservation(const ObservationRequest* observationRequest) const override {
        ObservationResultSharedPtr observationResult = std::make_shared<ObservationResult>();
        VectorFloat stateVec = observationRequest->currentState->as<VectorState>()->asVector();
        VectorFloat actionVec = observationRequest->action->as<VectorAction>()->asVector();
        // agent took action and landed in state - use to determine whether the action was succesful

        // -1.0 observation value represents a null observation
        VectorFloat observationVec(nObs, -1.0);

        scenario->setOpptState(stateVec);
        scenario->setOpptObs(observationVec);
        long binNumber = 0;

        long actionVal = (unsigned int) actionVec[0] + 0.25;
        SAction action = scenario->getAction(actionVal);
        bool actionSuccess = stateVec.back();
        std::vector<Assignment> effects = (actionSuccess) ? action.onSuccess_.second : action.onFail_.second;
        for (auto e : effects) {
            scenario->assignObs(e);
        }
        scenario->updateFoObs();
        binNumber = scenario->getBinNumber();
        // std::cout<<"Bin Number: "<<binNumber<<std::endl;
        observationVec = scenario->getOpptObs();
        ObservationSharedPtr observation = std::make_shared<DiscreteVectorObservation>(observationVec);
        observation->as<DiscreteVectorObservation>()->setBinNumber(binNumber);
        observationResult->observation = observation;
        observationResult->errorVector = observationRequest->errorVector;
        return observationResult;
    }

private:
    Scenario* scenario;
    int nObs;
    VectorFloat lowerBound_;
    VectorFloat upperBound_;
    FloatType observationError_;
};

OPPT_REGISTER_OBSERVATION_PLUGIN(CyberObservationPlugin)

}

#endif