// ============================================================================
// DSRRouting.ned
// LAP-DSR Routing Module
// ============================================================================

simple DSRRouting
{
    parameters:
        int numHosts = default(50);

        // Mobility & Physics Parameters
        double transmissionRange @unit(m) = default(100m);
        double nodeSpeed @unit(mps) = default(10mps);
        
        // --- Declare the missing parameters here ---
        double minSpeed @unit(mps) = default(1mps);
        double maxSpeed @unit(mps) = default(10mps);
        double pauseTime @unit(s) = default(2s);

        double constraintAreaMinX @unit(m) = default(0m);
        double constraintAreaMinY @unit(m) = default(0m);
        double constraintAreaMaxX @unit(m) = default(500m);
        double constraintAreaMaxY @unit(m) = default(500m);

        // Protocol Flags & Settings
        bool enableLogging = default(true);
        bool isSelfish = default(false);
        bool useLinkAvailabilityPrediction = default(true);

        double linkAvailabilityThreshold = default(0.70);

        double helloInterval @unit(s) = default(1s);
        double routeCacheTimeout @unit(s) = default(600s);
        double routeDiscoveryTimeout @unit(s) = default(10s);

        int maxRouteCacheSize = default(50);
        int maxRouteRequestRetries = default(3);

        @display("i=block/router");

    gates:
        input in @directIn;
        input radioIn[numHosts];
        output radioOut[numHosts];
}