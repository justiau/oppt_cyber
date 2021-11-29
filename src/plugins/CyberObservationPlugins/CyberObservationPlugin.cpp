#ifndef _CYBER_OBSERVATION_PLUGIN_HPP_
#define _CYBER_OBSERVATION_PLUGIN_HPP_

#include "oppt/plugin/Plugin.hpp"
#include "oppt/opptCore/Distribution.hpp"
#include "../CyberUtils/CyberUtils.hpp"
#include <iostream>
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
        scenario->updateMaxObsSize();
        lowerBound_.resize(nObs, -1.0);
        for (auto var : scenario->getAllObs()) {
            upperBound_.push_back(var.getValueCount() - 1);
        }
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
        binNumber = scenario->getBinNumber();
        observationVec = scenario->getOpptObs();
        ObservationSharedPtr observation = std::make_shared<DiscreteVectorObservation>(observationVec);
        observation->as<DiscreteVectorObservation>()->setBinNumber(binNumber);
        observationResult->observation = observation;
        observationResult->errorVector = observationRequest->errorVector;
        return observationResult;
    }

    virtual FloatType calcLikelihood(const RobotStateSharedPtr& state,
                                     const Action *action,
                                     const Observation *observation) const override {
        VectorFloat stateVec = state->as<VectorState>()->asVector();
        VectorFloat actionVec = action->as<VectorAction>()->asVector();
        // observation vector for which we request to calculate likelihood given state and action
        VectorFloat observationVecReq = observation->as<DiscreteVectorObservation>()->asVector();
        // observation vector result given state and action
        VectorFloat observationVecRes(nObs, -1.0);
        // assume observation is not noisy
        // calculate the likelihood of getting observation given state and action
        scenario->setOpptState(stateVec);
        scenario->setOpptObs(observationVecRes);
        long actionVal = (unsigned int) actionVec[0] + 0.25;
        SAction sAction = scenario->getAction(actionVal);

        scenario->applyAction(sAction, true);
        VectorFloat observationVecResSucc = scenario->getOpptObs();
        // scenario->setOpptState(stateVec);
        scenario->setOpptObs(observationVecRes);
        // get obs vec for false
        scenario->applyAction(sAction, false);
        VectorFloat observationVecResFail = scenario->getOpptObs();

        bool preconTrue = scenario->checkPreconditions(sAction);
        if (!preconTrue) {
            if (observationVecReq == observationVecResFail) return 1;
        } else {
            if (observationVecReq == observationVecResSucc) {
                // apply success effects and return action prob success if match succ req
                return sAction.probSuccess_;
            } else if (observationVecReq == observationVecResFail) {
                // apply fail effects and return 1 - action prob success if match fail req
                return 1 - sAction.probSuccess_;
            }
        }
        return 0;
    }

private:
    Scenario* scenario;
    int nObs;
    VectorFloat lowerBound_;
    VectorFloat upperBound_;
};

OPPT_REGISTER_OBSERVATION_PLUGIN(CyberObservationPlugin)

}

#endif