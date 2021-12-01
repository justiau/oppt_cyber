#include "ScenarioParser.hpp"
#include "Scenario.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

int main() {
    std::string scenarioPath = "/home/justin/dev/oppt_cyber/scenarios/es1_red_decay_ld_0100.yaml";
    ScenarioParser scenarioParser;
    scenarioParser.parse(scenarioPath);
    Scenario *scenario = scenarioParser.get();
    // scenario->show();
    SAction a = scenario->getAction("port_22_scan");
    std::vector<double> initState = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    std::vector<double> initObs = {-1, 1};
    scenario->setOpptState(initState);
    scenario->setOpptObs(initObs);

    bool valid = scenario->checkPreconditions(a);

    std::cout << "initial state and obs" << std::endl;
    print_vector(scenario->getOpptState());
    print_vector(scenario->getOpptObs());
    scenario->applyAction(a,valid);
    std::cout << "after state and obs" << std::endl;
    print_vector(scenario->getOpptState());
    print_vector(scenario->getOpptObs());
}