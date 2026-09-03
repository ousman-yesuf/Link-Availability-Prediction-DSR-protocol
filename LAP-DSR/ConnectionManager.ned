// ConnectionManager.ned
simple ConnectionManager
{
    parameters:
        int numHosts;
        double transmissionRange @unit(m);
        @display("i=block/connect");
    gates:
        output radioOut[numHosts];
        input radioIn[numHosts];
}