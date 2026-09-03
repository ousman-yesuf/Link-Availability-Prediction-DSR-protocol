// DsrPacket.msg
// LAP-DSR Packet Definition

message DsrPacket
{
    int sourceAddress = 0;
    int destinationAddress = 0;
    int packetType = 0;      // 0=RREQ, 1=RREP, 2=DATA, 3=RERR, 4=HELLO
    int sequenceNumber = 0;
    int brokenLink = -1;     // For RERR messages
    double linkAvailability = 0.0;  // For RREP and DATA
    double signalStrength = 0.0;    // For HELLO and all packets
    int packetId = 0;        // For tracking end-to-end delay
    int route[50];           // Fixed-size array for route
    int routeLength = 0;     // Number of hops in route
    string payload;          // For DATA packets
}

enum PacketType
{
    RREQ = 0;
    RREP = 1;
    DATA = 2;
    RERR = 3;
    HELLO = 4;
}

