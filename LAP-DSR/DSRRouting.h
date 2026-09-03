#ifndef __DSRROUTING_H_
#define __DSRROUTING_H_

#include <omnetpp.h>
#include <map>
#include <set>
#include <vector>
#include <string>
#include <cmath>

using namespace omnetpp;

// ----------------------------------------------------------------------------
// STRUCT DEFINITIONS
// ----------------------------------------------------------------------------

struct Position2D {
    double x, y;
    Position2D(double _x = 0, double _y = 0) : x(_x), y(_y) {}
    double distance(const Position2D& other) const {
        return std::sqrt((x - other.x) * (x - other.x) + (y - other.y) * (y - other.y));
    }
};

struct RouteEntry {
    int destination;
    std::vector<int> path;
    size_t hopCount;
    double linkAvailability;
    simtime_t creationTime;
    simtime_t expiryTime;
};

struct NeighborInfo {
    int nodeId;
    simtime_t lastSeen;
    double signalStrength;
    bool isActive;
};

struct LinkPrediction {
    int neighborId;
    double availability;
    double signalStrength;
    simtime_t lastUpdate;
    double movingAverage;
    double trend;
    int packetLossCount = 0;
    int totalPackets = 0;
    simtime_t linkStartTime = SIMTIME_ZERO;
    simtime_t linkEndTime = SIMTIME_ZERO;
    double totalLifetime = 0.0;
    int linkBreakCount = 0;
};

struct PacketTiming {
    simtime_t sentTime;
    simtime_t receivedTime;
    int packetId;
    int sourceAddress;
    int destinationAddress;
};

struct PerformanceMetrics {
    double lapPacketLoss = 0.0;
    double lapThroughput = 0.0;
    double lapDelay = 0.0;
    double lapOverhead = 0.0;
    double lapPDR = 0.0;

    double dsrPacketLoss = 0.0;
    double dsrThroughput = 0.0;
    double dsrDelay = 0.0;
    double dsrOverhead = 0.0;
    double dsrPDR = 0.0;

    double dsrpmPacketLoss = 0.0;
    double dsrpmThroughput = 0.0;
    double dsrpmDelay = 0.0;
    double dsrpmOverhead = 0.0;
    double dsrpmPDR = 0.0;
};

struct Statistics {
    double totalPacketsSent = 0.0;
    double totalPacketsReceived = 0.0;
    double totalPacketsLost = 0.0;
    double totalBytesTransmitted = 0.0;
    double totalBytesReceived = 0.0;
    double totalRoutingPackets = 0.0;
    double totalDataPackets = 0.0;
    double totalDelay = 0.0;
    double totalDelaySamples = 0.0;
    double totalRouteDiscoveries = 0.0;
    double totalRouteDiscoverySuccess = 0.0;
    double totalRouteBreaks = 0.0;
    double totalOverhead = 0.0;
    double simulationTime = 0.0;
};

struct ResearchResults {
    double speed;
    double lapPacketLoss;
    double lapThroughput;
    double lapDelay;
    double lapOverhead;
    double dsrPacketLoss;
    double dsrThroughput;
    double dsrDelay;
    double dsrOverhead;
    double dsrpmPacketLoss;
    double dsrpmThroughput;
    double dsrpmDelay;
    double dsrpmOverhead;
};

// ----------------------------------------------------------------------------
// MODULE CLASS DECLARATION
// ----------------------------------------------------------------------------

class DSRRouting : public cSimpleModule
{
  private:
    // Node & Network Info
    int nodeId;
    int numNodes;
    double transmissionRange;
    double nodeSpeed;
    int sequenceNumber;
    int packetIdCounter;

    // Mobility & Bounds Parameters
    double minSpeed;
    double maxSpeed;
    double pauseTime;
    double constraintMinX;
    double constraintMinY;
    double constraintMaxX;
    double constraintMaxY;

    // Parameters
    bool enableLogging;
    bool isSelfish;
    bool useLinkAvailabilityPrediction;
    double linkAvailabilityThreshold;
    double helloInterval;
    SimTime routeCacheTimeout;
    SimTime routeDiscoveryTimeout;
    int maxRouteCacheSize;
    int maxRouteRequestRetries;

    // Dynamic Timers & Tracking Containers
    cMessage *helloTimer = nullptr;
    cMessage *mobilityTimer = nullptr;
    cMessage *sendTimer = nullptr;
    std::map<int, cMessage*> routeDiscoveryTimersMap;
    std::set<std::pair<int, int>> seenRREQs;
    std::map<int, simtime_t> routeRequestTimers;
    std::map<int, int> routeRequestRetries;

    // State & Cache Structures
    std::map<int, RouteEntry> routeCache;
    std::map<int, NeighborInfo> neighborTable;
    std::map<int, LinkPrediction> linkPrediction;
    std::map<int, PacketTiming> packetTimings;
    std::map<int, cModule*> nodeModules;

    // Vector & Mobility States
    std::vector<Position2D> nodePositions;
    std::vector<Position2D> nodeVelocities;
    std::vector<Position2D> targetPositions;
    std::vector<double> currentSpeeds;
    std::vector<double> pauseTimers;
    std::map<int, double> linkLifetimes;

    enum ProtocolType {
        LAP_DSR,
        STANDARD_DSR,
        DSR_PM
    } currentProtocol = LAP_DSR;

    PerformanceMetrics metrics;
    Statistics stats;

    simtime_t pauseStartTime = SIMTIME_ZERO;
    bool isPaused = false;

    // Statistics Counter Variables
    long packetsSent = 0;
    long packetsDelivered = 0;
    long packetsDropped = 0;
    double totalDelay = 0.0;
    long delayCount = 0;
    double maxDelay = 0.0;
    double minDelay = 0.0;
    std::vector<double> delaySamples;
    double totalBytesSent = 0.0;
    double totalBytesReceived = 0.0;
    simtime_t simulationStartTime = SIMTIME_ZERO;
    long rreqSent = 0;
    long rrepSent = 0;
    long rerrSent = 0;
    long dataPacketsSent = 0;
    long dataPacketsReceived = 0;
    long routeBreaks = 0;
    simtime_t totalRouteLifetime = SIMTIME_ZERO;
    long routeCount = 0;
    long routeDiscoveries = 0;
    long routeDiscoverySuccess = 0;
    long routeDiscoveryFailures = 0;
    double totalDiscoveryTime = 0.0;
    long discoveryCount = 0;
    long predictionsCorrect = 0;
    long predictionsTotal = 0;
    long falsePositives = 0;
    long falseNegatives = 0;
    double totalLinkLifetime = 0.0;
    long totalLinks = 0;
    long duplicateRREQs = 0;
    long packetForwarded = 0;
    long selfishDrops = 0;
    long letWarnings = 0;
    simtime_t lastHelloTime = SIMTIME_ZERO;

    // Additional Global Packet Counters
    long totalRoutingPackets = 0;
    long totalDataPackets = 0;

    void sendToNode(cMessage *msg, int targetNodeId);

  protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void finish() override;
    virtual ~DSRRouting();

    bool getBoolPar(const char *name, bool defaultValue) const;
    int getIntPar(const char *name, int defaultValue) const;
    double getDoublePar(const char *name, double defaultValue) const;
    void logMessage(const std::string& msg);
    void updateNodeVisuals();
    void discoverNeighbors();
    double calculateDistance(int nodeA, int nodeB);
    Position2D getNodePosition(int targetNodeId);
    void updateMobility();
    bool isWithinRange(int source, int destination);
    void recordPacketSent(int packetId, int destination);
    void recordPacketReceived(int packetId);
    void updateLinkLifetime(int neighborId, bool isActive);
    void checkLinkPredictionAccuracy(int neighborId, bool predicted, bool actual);
    void recordRouteBreak(int destination);
    void recordRouteDiscovery(bool success, simtime_t discoveryTime);
    void sendHello();
    void processHello(cMessage *msg);
    void updateLinkPrediction(int neighborId, double signalStrength);
    double calculateLinkAvailability(int neighborId);
    bool isRouteValid(int destination);
    bool isDuplicateRREQ(int seqNum, int source);
    void sendRREQ(int destination);
    void processRREQ(cMessage *msg);
    void sendRREP(int destination, std::vector<int> reversePath);
    void processRREP(cMessage *msg);
    void sendDataPacket(int destination, const std::string& payload);
    void processData(cMessage *msg);
    void sendRERR(int destination, int unreachableNode);
    void processRERR(cMessage *msg);

    void collectMetrics();
    void saveMetricsToFile();
    void compareProtocols();
    void calculatePacketLoss();
    void calculateThroughput();
    void calculateEndToEndDelay();
    void calculateRoutingOverhead();
    void printComparisonResults();
    void printExpectedResults();
};

#endif
