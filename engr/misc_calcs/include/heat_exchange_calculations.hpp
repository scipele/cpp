#pragma once

class HeatExchangeCalculations {
public:
    void runHeatExchangeMenu();

private:
    void runAreaFromQDtU();
    void runQFromMdotCpDt();
    void runComparativeExchangerSize();
    void runTubeSurfaceAreaAndWeight();
    void runTubeFlowAreaAndVelocity();
    void runKelixFactor();
    void runBaffleCutArea();
    void runAverageMixDensity();
    void runExchangerNozzleRhoV2();
    void runTubeResistanceAtOD();
    void runShellPassCorrections();
    void runTubesheetThickness();
    void runMixingTemperature();
};
