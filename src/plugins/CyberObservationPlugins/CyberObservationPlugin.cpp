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
        bool actionSuccess = stateVec.back();
        std::vector<Assignment> effects = (actionSuccess) ? sAction.onSuccess_.second : sAction.onFail_.second;
        for (auto e : effects) {
            scenario->assignObs(e);
        }
        observationVecRes = scenario->getOpptObs();
        if (observationVecReq == observationVecRes) {
            return 1;
        }
        // std::cout << "action: " << sAction.name_ << std::endl;
        // std::cout << "state: " << std::endl;
        // for (ssize_t i=0; i < stateVec.size() - 1; ++i) {
        //     SVar sVar = scenario->getStateVar(i);
        //     std::cout << sVar.name_ << " : " << sVar.getValue(stateVec[i]) << ", ";
        // }
        // std::string actionSuccessStr = (stateVec.back()) ? "True" : "False";
        // std::cout << "action_success : " << actionSuccessStr;
        // std::cout << std::endl;
        // std::cout << "observation: " << std::endl;
        // for (ssize_t i=0; i < observationVecReq.size(); ++i) {
        //     SVar sVar = scenario->getObsVar(i);
        //     std::string obsVal = (observationVecReq[i] > -1) ? sVar.getValue(observationVecReq[i]) : "null";
        //         std::cout << sVar.name_ << " : " << obsVal << ", ";
        // }
        // std::cout << std::endl;
        // getchar();
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