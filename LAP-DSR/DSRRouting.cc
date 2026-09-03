// ============================================================================
// DSRRouting.cc
// LAP-DSR Implementation with Link Prediction, Dynamic Selfish Isolation
// (dropCount > 3), Metrics Collection, CSV Summary Export, and Traffic Generation
// ============================================================================

#include "DSRRouting.h"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

Define_Module(DSRRouting);

// ============================================================================
// DESTRUCTOR & HELPER METHODS
// ============================================================================

DSRRouting::~DSRRouting()
{
    routeDiscoveryTimersMap.clear();
    seenRREQs.clear();
    routeRequestTimers.clear();
    routeRequestRetries.clear();
    routeCache.clear();
    neighborTable.clear();
    linkPrediction.clear();
    packetTimings.clear();
    nodeModules.clear();
}

bool DSRRouting::getBoolPar(const char *name, bool defaultValue) const
{
    return hasPar(name) ? par(name).boolValue() : defaultValue;
}

int DSRRouting::getIntPar(const char *name, int defaultValue) const
{
    return hasPar(name) ? par(name).intValue() : defaultValue;
}

double DSRRouting::getDoublePar(const char *name, double defaultValue) const
{
    return hasPar(name) ? par(name).doubleValue() : defaultValue;
}

void DSRRouting::logMessage(const std::string& msg)
{
    if (enableLogging) {
        EV_INFO << "[Node " << nodeId << "] " << msg << "\n";
    }
}

// ============================================================================
// INITIALIZATION
// ============================================================================

void DSRRouting::initialize()
{
    cModule *parentModule = getParentModule();
    nodeId = (parentModule != nullptr) ? parentModule->getIndex() : 0;

    cModule *network = (parentModule != nullptr) ? parentModule->getParentModule() : nullptr;
    if (network != nullptr) {
        numNodes = network->par("numHosts").intValue();
        transmissionRange = network->par("transmissionRange").doubleValue();
    } else {
        numNodes = 50;
        transmissionRange = 250.0;
    }

    sequenceNumber = 0;
    packetIdCounter = 0;

    minSpeed = getDoublePar("minSpeed", 1.0);
    maxSpeed = getDoublePar("maxSpeed", 10.0);
    pauseTime = getDoublePar("pauseTime", 2.0);

    constraintMinX = getDoublePar("constraintAreaMinX", 0.0);
    constraintMinY = getDoublePar("constraintAreaMinY", 0.0);
    constraintMaxX = getDoublePar("constraintAreaMaxX", 500.0);
    constraintMaxY = getDoublePar("constraintAreaMaxY", 500.0);

    enableLogging = getBoolPar("enableLogging", true);
    isSelfish = getBoolPar("isSelfish", false);
    useLinkAvailabilityPrediction = getBoolPar("useLinkAvailabilityPrediction", true);
    linkAvailabilityThreshold = getDoublePar("linkAvailabilityThreshold", 0.70);
    helloInterval = getDoublePar("helloInterval", 1.0);

    routeCacheTimeout = SimTime(getDoublePar("routeCacheTimeout", 10.0));
    routeDiscoveryTimeout = SimTime(getDoublePar("routeDiscoveryTimeout", 2.0));
    maxRouteCacheSize = getIntPar("maxRouteCacheSize", 30);
    maxRouteRequestRetries = getIntPar("maxRouteRequestRetries", 3);

    // Dynamic assignment for specific malicious/uncooperative nodes
    if (nodeId != 0 && nodeId != (numNodes - 1)) {
        if (nodeId == 5 || nodeId == 12 || nodeId == 23 || nodeId == 34 || nodeId == 41) {
            isSelfish = true;
        }
    }

    nodePositions.resize(numNodes);
    nodeVelocities.resize(numNodes);
    targetPositions.resize(numNodes);
    currentSpeeds.resize(numNodes);
    pauseTimers.resize(numNodes, 0.0);

    for (int i = 0; i < numNodes; i++) {
        double x = uniform(constraintMinX, constraintMaxX);
        double y = uniform(constraintMinY, constraintMaxY);
        nodePositions[i] = Position2D(x, y);

        cModule *host = (network != nullptr) ? network->getSubmodule("host", i) : nullptr;
        if (host != nullptr) {
            nodeModules[i] = host;
            cDisplayString& ds = host->getDisplayString();
            std::string posStr = std::to_string((int)x) + "," + std::to_string((int)y);
            ds.setTagArg("p", 0, posStr.c_str());
        }

        targetPositions[i] = Position2D(
            uniform(constraintMinX, constraintMaxX),
            uniform(constraintMinY, constraintMaxY)
        );
        currentSpeeds[i] = uniform(minSpeed, maxSpeed);
    }

    simulationStartTime = simTime();
    updateNodeVisuals();

    helloTimer = new cMessage("helloTimer");
    scheduleAt(simTime() + helloInterval, helloTimer);

    mobilityTimer = new cMessage("mobilityTimer");
    scheduleAt(simTime() + 0.1, mobilityTimer);

    // Enable sendTimer on non-selfish nodes
    if (!isSelfish) {
        sendTimer = new cMessage("sendDataTimer");
        scheduleAt(simTime() + uniform(1.0, 5.0), sendTimer);
    }

    discoverNeighbors();
}

// ============================================================================
// MOBILITY & GEOMETRY
// ============================================================================

Position2D DSRRouting::getNodePosition(int targetNodeId)
{
    if (targetNodeId >= 0 && targetNodeId < numNodes) {
        return nodePositions[targetNodeId];
    }
    return Position2D(0, 0);
}

double DSRRouting::calculateDistance(int nodeA, int nodeB)
{
    if (nodeA < 0 || nodeA >= numNodes || nodeB < 0 || nodeB >= numNodes) return 1e9;
    return nodePositions[nodeA].distance(nodePositions[nodeB]);
}

bool DSRRouting::isWithinRange(int source, int destination)
{
    return calculateDistance(source, destination) <= transmissionRange;
}

void DSRRouting::updateMobility()
{
    double deltaTime = 0.1;
    for (int i = 0; i < numNodes; i++) {
        if (pauseTimers[i] > 0.0) {
            pauseTimers[i] -= deltaTime;
            if (pauseTimers[i] <= 0.0) {
                targetPositions[i] = Position2D(
                    uniform(constraintMinX, constraintMaxX),
                    uniform(constraintMinY, constraintMaxY)
                );
                currentSpeeds[i] = uniform(minSpeed, maxSpeed);
            }
            continue;
        }

        double dx = targetPositions[i].x - nodePositions[i].x;
        double dy = targetPositions[i].y - nodePositions[i].y;
        double distance = sqrt(dx * dx + dy * dy);
        double step = currentSpeeds[i] * deltaTime;

        if (distance <= step) {
            nodePositions[i] = targetPositions[i];
            pauseTimers[i] = pauseTime;
            nodeVelocities[i] = Position2D(0, 0);
        } else {
            double vx = (dx / distance) * currentSpeeds[i];
            double vy = (dy / distance) * currentSpeeds[i];
            nodeVelocities[i] = Position2D(vx, vy);

            nodePositions[i].x += (dx / distance) * step;
            nodePositions[i].y += (dy / distance) * step;
        }

        if (getEnvir()->isGUI() && nodeModules.count(i) && nodeModules[i]) {
            cDisplayString& ds = nodeModules[i]->getDisplayString();
            std::string posStr = std::to_string((int)nodePositions[i].x) + "," +
                                 std::to_string((int)nodePositions[i].y);
            ds.setTagArg("p", 0, posStr.c_str());
        }
    }
}

void DSRRouting::discoverNeighbors()
{
    for (int i = 0; i < numNodes; ++i) {
        if (i == nodeId) continue;
        double dist = calculateDistance(nodeId, i);
        bool inRange = (dist <= transmissionRange);

        if (inRange) {
            NeighborInfo info;
            info.nodeId = i;
            info.lastSeen = simTime();
            info.signalStrength = 1.0 - (dist / transmissionRange);
            info.isActive = true;
            neighborTable[i] = info;

            updateLinkPrediction(i, info.signalStrength);
        } else if (neighborTable.count(i)) {
            neighborTable[i].isActive = false;
        }
    }
}

void DSRRouting::updateNodeVisuals()
{
    cModule *host = getParentModule();
    if (host != nullptr && getEnvir()->isGUI()) {
        cDisplayString& ds = host->getDisplayString();
        if (isSelfish) {
            ds.setTagArg("i", 0, "device/cellphone");
            ds.setTagArg("i", 1, "red");
            ds.setTagArg("t", 0, "RED NODE (Selfish/Isolated)");
            ds.setTagArg("t", 1, "r");
        } else {
            char buf[64];
            snprintf(buf, sizeof(buf), "Node %d", nodeId);
            ds.setTagArg("t", 0, buf);
            ds.setTagArg("i", 1, "");
        }
    }
}

// ============================================================================
// LINK PREDICTION & AVAILABILITY (LAP)
// ============================================================================

void DSRRouting::updateLinkPrediction(int neighborId, double signalStrength)
{
    LinkPrediction& lp = linkPrediction[neighborId];
    lp.neighborId = neighborId;
    lp.signalStrength = signalStrength;
    lp.lastUpdate = simTime();

    if (lp.totalPackets == 0) {
        lp.movingAverage = signalStrength;
        lp.linkStartTime = simTime();
    } else {
        lp.movingAverage = (0.7 * lp.movingAverage) + (0.3 * signalStrength);
    }

    lp.totalPackets++;
    lp.availability = calculateLinkAvailability(neighborId);
}

double DSRRouting::calculateLinkAvailability(int neighborId)
{
    if (neighborId < 0 || neighborId >= numNodes) return 0.0;

    Position2D posA = nodePositions[nodeId];
    Position2D posB = nodePositions[neighborId];
    Position2D vA = nodeVelocities[nodeId];
    Position2D vB = nodeVelocities[neighborId];

    double dx = posA.x - posB.x;
    double dy = posA.y - posB.y;
    double dvx = vA.x - vB.x;
    double dvy = vA.y - vB.y;

    double a = dvx * dvx + dvy * dvy;
    double dist = calculateDistance(nodeId, neighborId);
    double distFactor = std::max(0.0, 1.0 - (dist / transmissionRange));

    if (a == 0.0) return distFactor;

    double b = 2 * (dx * dvx + dy * dvy);
    double c = dx * dx + dy * dy - transmissionRange * transmissionRange;

    double disc = b * b - 4 * a * c;
    if (disc < 0) return 0.0;

    double let = (-b + std::sqrt(disc)) / (2 * a);
    if (let <= 0) return 0.0;

    double letScore = std::min(let / 10.0, 1.0);
    return 0.6 * letScore + 0.4 * distFactor;
}

void DSRRouting::updateLinkLifetime(int neighborId, bool isActive)
{
    if (linkPrediction.count(neighborId)) {
        if (!isActive && linkPrediction[neighborId].linkEndTime == SIMTIME_ZERO) {
            linkPrediction[neighborId].linkEndTime = simTime();
            double lifetime = (linkPrediction[neighborId].linkEndTime - linkPrediction[neighborId].linkStartTime).dbl();
            totalLinkLifetime += lifetime;
            totalLinks++;
        }
    }
}

void DSRRouting::checkLinkPredictionAccuracy(int neighborId, bool predicted, bool actual)
{
    predictionsTotal++;
    if (predicted == actual) {
        predictionsCorrect++;
    } else if (predicted && !actual) {
        falsePositives++;
    } else {
        falseNegatives++;
    }
}

// ============================================================================
// ROUTE DISCOVERY & ROUTING LOGIC
// ============================================================================

bool DSRRouting::isRouteValid(int destination)
{
    if (!routeCache.count(destination)) return false;
    RouteEntry& entry = routeCache[destination];
    if (simTime() > entry.expiryTime) return false;

    for (size_t i = 0; i < entry.path.size() - 1; i++) {
        int u = entry.path[i];
        int v = entry.path[i + 1];
        if (useLinkAvailabilityPrediction) {
            double linkScore = (u == nodeId) ? calculateLinkAvailability(v) : 0.8;
            if (linkScore < linkAvailabilityThreshold) return false;
        }
    }
    return true;
}

bool DSRRouting::isDuplicateRREQ(int seqNum, int source)
{
    auto reqKey = std::make_pair(source, seqNum);
    if (seenRREQs.count(reqKey)) {
        duplicateRREQs++;
        return true;
    }
    seenRREQs.insert(reqKey);
    return false;
}

void DSRRouting::sendRREQ(int destination)
{
    rreqSent++;
    totalRoutingPackets++;
    sequenceNumber++;

    cPacket *rreq = new cPacket("DSR_RREQ");
    rreq->addPar("src") = (long)nodeId;
    rreq->addPar("dest") = (long)destination;
    rreq->addPar("seqNum") = (long)sequenceNumber;
    rreq->addPar("path") = std::to_string(nodeId).c_str();

    logMessage("Broadcasting RREQ for target Node " + std::to_string(destination));

    for (auto& pair : neighborTable) {
        if (pair.second.isActive) {
            cPacket *copy = rreq->dup();
            sendToNode(copy, pair.first);
        }
    }
    delete rreq;
}

void DSRRouting::processRREQ(cMessage *msg)
{
    cPacket *pkt = check_and_cast<cPacket*>(msg);
    int src = (int)pkt->par("src").longValue();
    int dest = (int)pkt->par("dest").longValue();
    int seqNum = (int)pkt->par("seqNum").longValue();
    std::string pathStr = pkt->par("path").stringValue();

    if (isDuplicateRREQ(seqNum, src)) {
        delete msg;
        return;
    }

    std::vector<int> path;
    std::stringstream ss(pathStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        path.push_back(std::stoi(token));
    }
    path.push_back(nodeId);

    if (nodeId == dest) {
        sendRREP(src, path);
        delete msg;
        return;
    }

    if (!isSelfish) {
        std::string updatedPath = "";
        for (size_t i = 0; i < path.size(); i++) {
            updatedPath += std::to_string(path[i]) + (i == path.size() - 1 ? "" : ",");
        }
        pkt->par("path") = updatedPath.c_str();

        for (auto& pair : neighborTable) {
            if (pair.second.isActive && std::find(path.begin(), path.end(), pair.first) == path.end()) {
                cPacket *copy = pkt->dup();
                sendToNode(copy, pair.first);
            }
        }
    } else {
        selfishDrops++;
        if (selfishDrops > 3) {
            logMessage("Selfish drop threshold exceeded (dropCount > 3). Node isolated.");
            if (sendTimer && sendTimer->isScheduled()) {
                cancelAndDelete(sendTimer);
                sendTimer = nullptr;
            }
            updateNodeVisuals();
        }
    }

    delete msg;
}

void DSRRouting::sendRREP(int destination, std::vector<int> reversePath)
{
    rrepSent++;
    totalRoutingPackets++;

    cPacket *rrep = new cPacket("DSR_RREP");
    rrep->addPar("src") = (long)nodeId;
    rrep->addPar("dest") = (long)destination;

    std::string pathStr = "";
    for (size_t i = 0; i < reversePath.size(); i++) {
        pathStr += std::to_string(reversePath[i]) + (i == reversePath.size() - 1 ? "" : ",");
    }
    rrep->addPar("path") = pathStr.c_str();
    rrep->addPar("hopIndex") = (long)(reversePath.size() - 2);

    int nextHop = reversePath[reversePath.size() - 2];
    sendToNode(rrep, nextHop);
}

void DSRRouting::processRREP(cMessage *msg)
{
    cPacket *pkt = check_and_cast<cPacket*>(msg);
    int dest = (int)pkt->par("dest").longValue();
    std::string pathStr = pkt->par("path").stringValue();
    int hopIndex = (int)pkt->par("hopIndex").longValue();

    std::vector<int> path;
    std::stringstream ss(pathStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        path.push_back(std::stoi(token));
    }

    if (nodeId == dest) {
        RouteEntry entry;
        entry.destination = path.back();
        entry.path = path;
        entry.hopCount = path.size() - 1;
        entry.linkAvailability = 0.9;
        entry.creationTime = simTime();
        entry.expiryTime = simTime() + routeCacheTimeout;

        routeCache[entry.destination] = entry;
        routeDiscoverySuccess++;
        recordRouteDiscovery(true, simTime() - routeRequestTimers[entry.destination]);

        logMessage("Route discovered to Node " + std::to_string(entry.destination));
        delete msg;
        return;
    }

    if (hopIndex >= 0 && hopIndex < (int)path.size()) {
        pkt->par("hopIndex") = (long)(hopIndex - 1);
        int nextHop = path[hopIndex];
        sendToNode(pkt, nextHop);
    } else {
        delete msg;
    }
}

// ============================================================================
// DATA PACKET HANDLING
// ============================================================================

void DSRRouting::sendDataPacket(int destination, const std::string& payload)
{
    packetsSent++;
    dataPacketsSent++;
    totalDataPackets++;
    packetIdCounter++;

    cPacket *dataPkt = new cPacket("DSR_DATA");
    dataPkt->setByteLength(512);
    dataPkt->addPar("packetId") = (long)packetIdCounter;
    dataPkt->addPar("src") = (long)nodeId;
    dataPkt->addPar("dest") = (long)destination;
    dataPkt->addPar("creationTime") = simTime().dbl();

    recordPacketSent(packetIdCounter, destination);

    if (isRouteValid(destination)) {
        RouteEntry& entry = routeCache[destination];
        std::string pathStr = "";
        for (size_t i = 0; i < entry.path.size(); i++) {
            pathStr += std::to_string(entry.path[i]) + (i == entry.path.size() - 1 ? "" : ",");
        }
        dataPkt->addPar("path") = pathStr.c_str();
        dataPkt->addPar("hopIndex") = 1L;

        int nextHop = entry.path[1];
        sendToNode(dataPkt, nextHop);
    } else {
        logMessage("No valid route to " + std::to_string(destination) + ". Initiating RREQ.");
        routeRequestTimers[destination] = simTime();
        sendRREQ(destination);
        delete dataPkt;
    }
}

void DSRRouting::processData(cMessage *msg)
{
    cPacket *pkt = check_and_cast<cPacket*>(msg);
    int dest = (int)pkt->par("dest").longValue();
    int src = (int)pkt->par("src").longValue();
    int pktId = (int)pkt->par("packetId").longValue();

    if (isSelfish && nodeId != dest) {
        selfishDrops++;
        packetsDropped++;

        if (selfishDrops > 3) {
            isSelfish = true;
            logMessage("Drop count exceeded 3! Node marked as dynamic Selfish Node.");
            if (sendTimer && sendTimer->isScheduled()) {
                cancelAndDelete(sendTimer);
                sendTimer = nullptr;
            }
            updateNodeVisuals();
        }

        delete msg;
        return;
    }

    if (nodeId == dest) {
        packetsDelivered++;
        dataPacketsReceived++;
        totalBytesReceived += pkt->getByteLength();

        double creationTime = pkt->par("creationTime").doubleValue();
        double delay = simTime().dbl() - creationTime;
        totalDelay += delay;
        delayCount++;
        delaySamples.push_back(delay);

        recordPacketReceived(pktId);
        logMessage("DATA Packet #" + std::to_string(pktId) + " delivered successfully.");
        delete msg;
        return;
    }

    packetForwarded++;
    int hopIndex = (int)pkt->par("hopIndex").longValue();
    std::string pathStr = pkt->par("path").stringValue();

    std::vector<int> path;
    std::stringstream ss(pathStr);
    std::string token;
    while (std::getline(ss, token, ',')) {
        path.push_back(std::stoi(token));
    }

    if (hopIndex < (int)path.size()) {
        int nextHop = path[hopIndex];
        pkt->par("hopIndex") = (long)(hopIndex + 1);

        if (isWithinRange(nodeId, nextHop) && (!useLinkAvailabilityPrediction || calculateLinkAvailability(nextHop) >= linkAvailabilityThreshold)) {
            sendToNode(pkt, nextHop);
        } else {
            recordRouteBreak(dest);
            sendRERR(src, nextHop);
            packetsDropped++;
            delete msg;
        }
    } else {
        packetsDropped++;
        delete msg;
    }
}

void DSRRouting::sendRERR(int destination, int unreachableNode)
{
    rerrSent++;
    totalRoutingPackets++;

    cPacket *rerr = new cPacket("DSR_RERR");
    rerr->addPar("src") = (long)nodeId;
    rerr->addPar("dest") = (long)destination;
    rerr->addPar("unreachable") = (long)unreachableNode;

    sendToNode(rerr, destination);
}

void DSRRouting::processRERR(cMessage *msg)
{
    cPacket *pkt = check_and_cast<cPacket*>(msg);
    int unreachable = (int)pkt->par("unreachable").longValue();

    for (auto it = routeCache.begin(); it != routeCache.end();) {
        std::vector<int>& path = it->second.path;
        if (std::find(path.begin(), path.end(), unreachable) != path.end()) {
            it = routeCache.erase(it);
        } else {
            ++it;
        }
    }
    delete msg;
}

// ============================================================================
// HELLO MESSAGES
// ============================================================================

void DSRRouting::sendHello()
{
    cPacket *hello = new cPacket("DSR_HELLO");
    hello->addPar("src") = (long)nodeId;

    for (auto& pair : neighborTable) {
        if (pair.second.isActive) {
            cPacket *copy = hello->dup();
            sendToNode(copy, pair.first);
        }
    }
    delete hello;
}

void DSRRouting::processHello(cMessage *msg)
{
    cPacket *pkt = check_and_cast<cPacket*>(msg);
    int src = (int)pkt->par("src").longValue();

    neighborTable[src].nodeId = src;
    neighborTable[src].lastSeen = simTime();
    neighborTable[src].isActive = true;

    updateLinkPrediction(src, 0.95);
    delete msg;
}

// ============================================================================
// DIRECT TRANSMISSION & MESSAGE HANDLER
// ============================================================================

void DSRRouting::sendToNode(cMessage *msg, int targetNodeId)
{
    if (nodeModules.count(targetNodeId) && nodeModules[targetNodeId]) {
        cModule *targetRouting = nodeModules[targetNodeId]->getSubmodule("routing");
        if (targetRouting) {
            if (targetRouting->hasGate("in")) {
                sendDirect(msg, targetRouting, "in");
                return;
            } else if (targetRouting->hasGate("radioIn")) {
                sendDirect(msg, targetRouting, "radioIn", 0);
                return;
            } else {
                for (int i = 0; i < targetRouting->gateCount(); i++) {
                    cGate *g = targetRouting->gate(i);
                    if (g && g->getType() == cGate::INPUT) {
                        sendDirect(msg, targetRouting, g->getName(), g->isVector() ? 0 : -1);
                        return;
                    }
                }
            }
        }
    }
    delete msg;
}

void DSRRouting::handleMessage(cMessage *msg)
{
    if (msg == mobilityTimer) {
        updateMobility();
        discoverNeighbors();
        scheduleAt(simTime() + 0.1, mobilityTimer);
        return;
    }

    if (msg == helloTimer) {
        sendHello();
        scheduleAt(simTime() + helloInterval, helloTimer);
        return;
    }

    if (msg == sendTimer) {
        int destinationNode = intuniform(0, numNodes - 1);
        while (destinationNode == nodeId) {
            destinationNode = intuniform(0, numNodes - 1);
        }

        sendDataPacket(destinationNode, "LAP-DSR Payload");
        scheduleAt(simTime() + uniform(1.5, 3.5), sendTimer);
        return;
    }

    std::string name = msg->getName();
    if (name == "DSR_HELLO") processHello(msg);
    else if (name == "DSR_RREQ") processRREQ(msg);
    else if (name == "DSR_RREP") processRREP(msg);
    else if (name == "DSR_DATA") processData(msg);
    else if (name == "DSR_RERR") processRERR(msg);
    else delete msg;
}

// ============================================================================
// METRICS LOGGING & METRIC CALCULATION
// ============================================================================

void DSRRouting::recordPacketSent(int packetId, int destination)
{
    PacketTiming pt;
    pt.packetId = packetId;
    pt.sentTime = simTime();
    pt.sourceAddress = nodeId;
    pt.destinationAddress = destination;
    packetTimings[packetId] = pt;
}

void DSRRouting::recordPacketReceived(int packetId)
{
    if (packetTimings.count(packetId)) {
        packetTimings[packetId].receivedTime = simTime();
    }
}

void DSRRouting::recordRouteBreak(int destination)
{
    routeBreaks++;
}

void DSRRouting::recordRouteDiscovery(bool success, simtime_t discoveryTime)
{
    routeDiscoveries++;
    if (success) {
        totalDiscoveryTime += discoveryTime.dbl();
        discoveryCount++;
    }
}

void DSRRouting::collectMetrics()
{
    calculatePacketLoss();
    calculateThroughput();
    calculateEndToEndDelay();
    calculateRoutingOverhead();
}

void DSRRouting::calculatePacketLoss()
{
    long totalAttempted = packetsSent + packetForwarded;
    metrics.lapPacketLoss = (totalAttempted > 0) ? ((double)packetsDropped / (double)totalAttempted) * 100.0 : 0.0;
}

void DSRRouting::calculateThroughput()
{
    double duration = (simTime() - simulationStartTime).dbl();
    metrics.lapThroughput = (duration > 0) ? ((totalBytesReceived * 8.0) / (duration * 1000.0)) : 0.0;
}

void DSRRouting::calculateEndToEndDelay()
{
    metrics.lapDelay = (delayCount > 0) ? (totalDelay / delayCount) : 0.0;
}

void DSRRouting::calculateRoutingOverhead()
{
    long ctrl = rreqSent + rrepSent + rerrSent;
    metrics.lapOverhead = (dataPacketsReceived > 0) ? ((double)ctrl / (double)dataPacketsReceived) : 0.0;
}

// ============================================================================
// CSV EXPORT WITH LABELS AND SUMMARY
// ============================================================================

void DSRRouting::saveMetricsToFile()
{
    std::string filename = "lap_dsr_results.csv";

    // Check if the file exists to write column headers on creation
    std::ifstream checkFile(filename);
    bool fileExists = checkFile.good();
    checkFile.close();

    std::ofstream file(filename, std::ios::app);
    if (file.is_open()) {
        // Write Label Headers if file is newly created
        if (!fileExists) {
            file << "# ============================================================================\n";
            file << "# LAP-DSR SIMULATION EXECUTION METRICS REPORT\n";
            file << "# Protocol: Link Availability Prediction Dynamic Source Routing (LAP-DSR)\n";
            file << "# ============================================================================\n";
            file << "Node_ID,Node_Role,Packets_Sent,Packets_Delivered,Packets_Forwarded,"
                 << "Packets_Dropped,Selfish_Drops,Packet_Loss_Rate_Percent,"
                 << "Throughput_Kbps,Avg_EndToEnd_Delay_Sec,Routing_Overhead_Ratio\n";
        }

        // Export Per-Node Metric Rows
        std::string roleLabel = isSelfish ? "Selfish/Isolated" : "Normal";
        file << nodeId << ","
             << roleLabel << ","
             << packetsSent << ","
             << packetsDelivered << ","
             << packetForwarded << ","
             << packetsDropped << ","
             << selfishDrops << ","
             << std::fixed << std::setprecision(2) << metrics.lapPacketLoss << ","
             << std::fixed << std::setprecision(2) << metrics.lapThroughput << ","
             << std::fixed << std::setprecision(4) << metrics.lapDelay << ","
             << std::fixed << std::setprecision(2) << metrics.lapOverhead << "\n";

        // Append explicit Node Summary Section at Node 0 finish call
        if (nodeId == 0) {
            file << "\n# --- SIMULATION CONFIGURATION SUMMARY ---\n";
            file << "Metric_Label,Metric_Value\n";
            file << "Total_Nodes," << numNodes << "\n";
            file << "Transmission_Range_Meters," << transmissionRange << "\n";
            file << "Link_Availability_Threshold," << linkAvailabilityThreshold << "\n";
            file << "Route_Cache_Timeout_Sec," << routeCacheTimeout.dbl() << "\n";
            file << "# --- END OF SUMMARY ---\n\n";
        }

        file.close();
    }
}

void DSRRouting::compareProtocols() {}
void DSRRouting::printComparisonResults() {}
void DSRRouting::printExpectedResults() {}

// ============================================================================
// FINISH ROUTINE
// ============================================================================

void DSRRouting::finish()
{
    if (helloTimer) { cancelAndDelete(helloTimer); helloTimer = nullptr; }
    if (mobilityTimer) { cancelAndDelete(mobilityTimer); mobilityTimer = nullptr; }
    if (sendTimer) { cancelAndDelete(sendTimer); sendTimer = nullptr; }

    for (auto& pair : routeDiscoveryTimersMap) {
        if (pair.second) cancelAndDelete(pair.second);
    }
    routeDiscoveryTimersMap.clear();

    collectMetrics();
    saveMetricsToFile();

    recordScalar("Packets Sent", packetsSent);
    recordScalar("Packets Delivered", packetsDelivered);
    recordScalar("Packets Forwarded", packetForwarded);
    recordScalar("Packets Dropped", packetsDropped);
    recordScalar("Selfish Drops", selfishDrops);
    recordScalar("Packet Loss Rate (%)", metrics.lapPacketLoss);
    recordScalar("Throughput (kbps)", metrics.lapThroughput);
    recordScalar("Avg Delay (s)", metrics.lapDelay);

    if (enableLogging) {
        EV_INFO << "\n============================================================\n";
        EV_INFO << "         LAP-DSR METRICS SUMMARY (Node " << nodeId << ")\n";
        EV_INFO << "============================================================\n";
        EV_INFO << "Is Red/Selfish Node    : " << (isSelfish ? "YES" : "NO") << "\n";
        EV_INFO << "Packets Sent           : " << packetsSent << "\n";
        EV_INFO << "Packets Delivered      : " << packetsDelivered << "\n";
        EV_INFO << "Packets Forwarded      : " << packetForwarded << "\n";
        EV_INFO << "Packets Dropped        : " << packetsDropped << " (Selfish: " << selfishDrops << ")\n";
        EV_INFO << "Packet Loss Rate       : " << std::fixed << std::setprecision(2) << metrics.lapPacketLoss << " %\n";
        EV_INFO << "Throughput             : " << std::fixed << std::setprecision(2) << metrics.lapThroughput << " kbps\n";
        EV_INFO << "Avg End-to-End Delay   : " << std::fixed << std::setprecision(4) << metrics.lapDelay << " s\n";
        EV_INFO << "============================================================\n";
    }
}
