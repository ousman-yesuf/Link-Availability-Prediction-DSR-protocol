import inet.mobility.contract.IMobility;
import inet.mobility.single.RandomWaypointMobility;

module LAPDSRHost
{
    parameters:
        int numHosts = default(50);
        double transmissionRange @unit(m) = default(250m);
        
        // --- Random Waypoint Mobility Parameters ---
        string mobilityType = default("RandomWaypointMobility");
        double minSpeed @unit(mps) = default(1mps);
        double maxSpeed @unit(mps) = default(10mps);
        double pauseTime @unit(s) = default(2s);

        double constraintAreaMinX @unit(m) = default(0m);
        double constraintAreaMinY @unit(m) = default(0m);
        double constraintAreaMinZ @unit(m) = default(0m);
        double constraintAreaMaxX @unit(m) = default(500m);
        double constraintAreaMaxY @unit(m) = default(500m);
        double constraintAreaMaxZ @unit(m) = default(0m);

        bool isSelfish = default(false);

        @display("i=device/cellphone");

    gates:
        input in @directIn;
        input radioIn[];
        output radioOut[];

    submodules:
        // INET Mobility Module without hardcoded NED parameters
        mobility: <mobilityType> like IMobility {
            @display("p=100,50");
        }

        // Custom LAP-DSR Routing Engine
        routing: DSRRouting {
            parameters:
                numHosts = parent.numHosts;
                transmissionRange = parent.transmissionRange;
                isSelfish = parent.isSelfish;
                @display("p=100,150");
        }

    connections allowunconnected:
        
        for i=0..sizeof(radioIn)-1 {
            radioIn[i] --> routing.radioIn++;
        }
        for i=0..sizeof(radioOut)-1 {
            routing.radioOut++ --> radioOut[i];
        }
}