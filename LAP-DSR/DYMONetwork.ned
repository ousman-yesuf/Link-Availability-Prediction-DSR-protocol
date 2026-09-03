import inet.visualizer.contract.IIntegratedVisualizer;
import inet.common.scenario.ScenarioManager;

network DYMONetwork
{
    parameters:
        int numHosts = default(50);
        double constraintAreaMinX @unit(m) = default(0m);
        double constraintAreaMinY @unit(m) = default(0m);
        double constraintAreaMinZ @unit(m) = default(0m);
        double constraintAreaMaxX @unit(m) = default(500m);
        double constraintAreaMaxY @unit(m) = default(500m);
        double constraintAreaMaxZ @unit(m) = default(0m);
        
        // --- Global Mobility Defaults ---
        double minSpeed @unit(mps) = default(1mps);
        double maxSpeed @unit(mps) = default(15mps);
        double pauseTime @unit(s) = default(2s);
        double transmissionRange @unit(m) = default(250m);

        @display("bgb=800,600");

    submodules:
        // Option A: Instantiates the visualizer directly
        visualizer: <default("IntegratedCanvasVisualizer")> like IIntegratedVisualizer {
            @display("p=50,50");
        }

        scenarioManager: ScenarioManager {
            @display("p=50,120");
        }

        host[numHosts]: LAPDSRHost {
            parameters:
                numHosts = parent.numHosts;
                transmissionRange = parent.transmissionRange;
                minSpeed = parent.minSpeed;
                maxSpeed = parent.maxSpeed;
                pauseTime = parent.pauseTime;
                constraintAreaMinX = parent.constraintAreaMinX;
                constraintAreaMinY = parent.constraintAreaMinY;
                constraintAreaMinZ = parent.constraintAreaMinZ;
                constraintAreaMaxX = parent.constraintAreaMaxX;
                constraintAreaMaxY = parent.constraintAreaMaxY;
                constraintAreaMaxZ = parent.constraintAreaMaxZ;
                @display("i=device/cellphone");
        }

    connections allowunconnected:
        
}