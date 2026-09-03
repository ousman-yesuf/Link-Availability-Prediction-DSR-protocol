//
// Generated file, do not edit! Created by opp_msgtool 6.0 from DsrPacket.msg.
//

// Disable warnings about unused variables, empty switch stmts, etc:
#ifdef _MSC_VER
#  pragma warning(disable:4101)
#  pragma warning(disable:4065)
#endif

#if defined(__clang__)
#  pragma clang diagnostic ignored "-Wshadow"
#  pragma clang diagnostic ignored "-Wconversion"
#  pragma clang diagnostic ignored "-Wunused-parameter"
#  pragma clang diagnostic ignored "-Wc++98-compat"
#  pragma clang diagnostic ignored "-Wunreachable-code-break"
#  pragma clang diagnostic ignored "-Wold-style-cast"
#elif defined(__GNUC__)
#  pragma GCC diagnostic ignored "-Wshadow"
#  pragma GCC diagnostic ignored "-Wconversion"
#  pragma GCC diagnostic ignored "-Wunused-parameter"
#  pragma GCC diagnostic ignored "-Wold-style-cast"
#  pragma GCC diagnostic ignored "-Wsuggest-attribute=noreturn"
#  pragma GCC diagnostic ignored "-Wfloat-conversion"
#endif

#include <iostream>
#include <sstream>
#include <memory>
#include <type_traits>
#include "DsrPacket_m.h"

namespace omnetpp {

// Template pack/unpack rules. They are declared *after* a1l type-specific pack functions for multiple reasons.
// They are in the omnetpp namespace, to allow them to be found by argument-dependent lookup via the cCommBuffer argument

// Packing/unpacking an std::vector
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::vector<T,A>& v)
{
    int n = v.size();
    doParsimPacking(buffer, n);
    for (int i = 0; i < n; i++)
        doParsimPacking(buffer, v[i]);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::vector<T,A>& v)
{
    int n;
    doParsimUnpacking(buffer, n);
    v.resize(n);
    for (int i = 0; i < n; i++)
        doParsimUnpacking(buffer, v[i]);
}

// Packing/unpacking an std::list
template<typename T, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::list<T,A>& l)
{
    doParsimPacking(buffer, (int)l.size());
    for (typename std::list<T,A>::const_iterator it = l.begin(); it != l.end(); ++it)
        doParsimPacking(buffer, (T&)*it);
}

template<typename T, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::list<T,A>& l)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        l.push_back(T());
        doParsimUnpacking(buffer, l.back());
    }
}

// Packing/unpacking an std::set
template<typename T, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::set<T,Tr,A>& s)
{
    doParsimPacking(buffer, (int)s.size());
    for (typename std::set<T,Tr,A>::const_iterator it = s.begin(); it != s.end(); ++it)
        doParsimPacking(buffer, *it);
}

template<typename T, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::set<T,Tr,A>& s)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        T x;
        doParsimUnpacking(buffer, x);
        s.insert(x);
    }
}

// Packing/unpacking an std::map
template<typename K, typename V, typename Tr, typename A>
void doParsimPacking(omnetpp::cCommBuffer *buffer, const std::map<K,V,Tr,A>& m)
{
    doParsimPacking(buffer, (int)m.size());
    for (typename std::map<K,V,Tr,A>::const_iterator it = m.begin(); it != m.end(); ++it) {
        doParsimPacking(buffer, it->first);
        doParsimPacking(buffer, it->second);
    }
}

template<typename K, typename V, typename Tr, typename A>
void doParsimUnpacking(omnetpp::cCommBuffer *buffer, std::map<K,V,Tr,A>& m)
{
    int n;
    doParsimUnpacking(buffer, n);
    for (int i = 0; i < n; i++) {
        K k; V v;
        doParsimUnpacking(buffer, k);
        doParsimUnpacking(buffer, v);
        m[k] = v;
    }
}

// Default pack/unpack function for arrays
template<typename T>
void doParsimArrayPacking(omnetpp::cCommBuffer *b, const T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimPacking(b, t[i]);
}

template<typename T>
void doParsimArrayUnpacking(omnetpp::cCommBuffer *b, T *t, int n)
{
    for (int i = 0; i < n; i++)
        doParsimUnpacking(b, t[i]);
}

// Default rule to prevent compiler from choosing base class' doParsimPacking() function
template<typename T>
void doParsimPacking(omnetpp::cCommBuffer *, const T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimPacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

template<typename T>
void doParsimUnpacking(omnetpp::cCommBuffer *, T& t)
{
    throw omnetpp::cRuntimeError("Parsim error: No doParsimUnpacking() function for type %s", omnetpp::opp_typename(typeid(t)));
}

}  // namespace omnetpp

Register_Class(DsrPacket)

DsrPacket::DsrPacket(const char *name, short kind) : ::omnetpp::cMessage(name, kind)
{
}

DsrPacket::DsrPacket(const DsrPacket& other) : ::omnetpp::cMessage(other)
{
    copy(other);
}

DsrPacket::~DsrPacket()
{
}

DsrPacket& DsrPacket::operator=(const DsrPacket& other)
{
    if (this == &other) return *this;
    ::omnetpp::cMessage::operator=(other);
    copy(other);
    return *this;
}

void DsrPacket::copy(const DsrPacket& other)
{
    this->sourceAddress = other.sourceAddress;
    this->destinationAddress = other.destinationAddress;
    this->packetType = other.packetType;
    this->sequenceNumber = other.sequenceNumber;
    this->brokenLink = other.brokenLink;
    this->linkAvailability = other.linkAvailability;
    this->signalStrength = other.signalStrength;
    this->packetId = other.packetId;
    for (size_t i = 0; i < 50; i++) {
        this->route[i] = other.route[i];
    }
    this->routeLength = other.routeLength;
    this->payload = other.payload;
}

void DsrPacket::parsimPack(omnetpp::cCommBuffer *b) const
{
    ::omnetpp::cMessage::parsimPack(b);
    doParsimPacking(b,this->sourceAddress);
    doParsimPacking(b,this->destinationAddress);
    doParsimPacking(b,this->packetType);
    doParsimPacking(b,this->sequenceNumber);
    doParsimPacking(b,this->brokenLink);
    doParsimPacking(b,this->linkAvailability);
    doParsimPacking(b,this->signalStrength);
    doParsimPacking(b,this->packetId);
    doParsimArrayPacking(b,this->route,50);
    doParsimPacking(b,this->routeLength);
    doParsimPacking(b,this->payload);
}

void DsrPacket::parsimUnpack(omnetpp::cCommBuffer *b)
{
    ::omnetpp::cMessage::parsimUnpack(b);
    doParsimUnpacking(b,this->sourceAddress);
    doParsimUnpacking(b,this->destinationAddress);
    doParsimUnpacking(b,this->packetType);
    doParsimUnpacking(b,this->sequenceNumber);
    doParsimUnpacking(b,this->brokenLink);
    doParsimUnpacking(b,this->linkAvailability);
    doParsimUnpacking(b,this->signalStrength);
    doParsimUnpacking(b,this->packetId);
    doParsimArrayUnpacking(b,this->route,50);
    doParsimUnpacking(b,this->routeLength);
    doParsimUnpacking(b,this->payload);
}

int DsrPacket::getSourceAddress() const
{
    return this->sourceAddress;
}

void DsrPacket::setSourceAddress(int sourceAddress)
{
    this->sourceAddress = sourceAddress;
}

int DsrPacket::getDestinationAddress() const
{
    return this->destinationAddress;
}

void DsrPacket::setDestinationAddress(int destinationAddress)
{
    this->destinationAddress = destinationAddress;
}

int DsrPacket::getPacketType() const
{
    return this->packetType;
}

void DsrPacket::setPacketType(int packetType)
{
    this->packetType = packetType;
}

int DsrPacket::getSequenceNumber() const
{
    return this->sequenceNumber;
}

void DsrPacket::setSequenceNumber(int sequenceNumber)
{
    this->sequenceNumber = sequenceNumber;
}

int DsrPacket::getBrokenLink() const
{
    return this->brokenLink;
}

void DsrPacket::setBrokenLink(int brokenLink)
{
    this->brokenLink = brokenLink;
}

double DsrPacket::getLinkAvailability() const
{
    return this->linkAvailability;
}

void DsrPacket::setLinkAvailability(double linkAvailability)
{
    this->linkAvailability = linkAvailability;
}

double DsrPacket::getSignalStrength() const
{
    return this->signalStrength;
}

void DsrPacket::setSignalStrength(double signalStrength)
{
    this->signalStrength = signalStrength;
}

int DsrPacket::getPacketId() const
{
    return this->packetId;
}

void DsrPacket::setPacketId(int packetId)
{
    this->packetId = packetId;
}

size_t DsrPacket::getRouteArraySize() const
{
    return 50;
}

int DsrPacket::getRoute(size_t k) const
{
    if (k >= 50) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)50, (unsigned long)k);
    return this->route[k];
}

void DsrPacket::setRoute(size_t k, int route)
{
    if (k >= 50) throw omnetpp::cRuntimeError("Array of size %lu indexed by %lu", (unsigned long)50, (unsigned long)k);
    this->route[k] = route;
}

int DsrPacket::getRouteLength() const
{
    return this->routeLength;
}

void DsrPacket::setRouteLength(int routeLength)
{
    this->routeLength = routeLength;
}

const char * DsrPacket::getPayload() const
{
    return this->payload.c_str();
}

void DsrPacket::setPayload(const char * payload)
{
    this->payload = payload;
}

class DsrPacketDescriptor : public omnetpp::cClassDescriptor
{
  private:
    mutable const char **propertyNames;
    enum FieldConstants {
        FIELD_sourceAddress,
        FIELD_destinationAddress,
        FIELD_packetType,
        FIELD_sequenceNumber,
        FIELD_brokenLink,
        FIELD_linkAvailability,
        FIELD_signalStrength,
        FIELD_packetId,
        FIELD_route,
        FIELD_routeLength,
        FIELD_payload,
    };
  public:
    DsrPacketDescriptor();
    virtual ~DsrPacketDescriptor();

    virtual bool doesSupport(omnetpp::cObject *obj) const override;
    virtual const char **getPropertyNames() const override;
    virtual const char *getProperty(const char *propertyName) const override;
    virtual int getFieldCount() const override;
    virtual const char *getFieldName(int field) const override;
    virtual int findField(const char *fieldName) const override;
    virtual unsigned int getFieldTypeFlags(int field) const override;
    virtual const char *getFieldTypeString(int field) const override;
    virtual const char **getFieldPropertyNames(int field) const override;
    virtual const char *getFieldProperty(int field, const char *propertyName) const override;
    virtual int getFieldArraySize(omnetpp::any_ptr object, int field) const override;
    virtual void setFieldArraySize(omnetpp::any_ptr object, int field, int size) const override;

    virtual const char *getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const override;
    virtual std::string getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const override;
    virtual omnetpp::cValue getFieldValue(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const override;

    virtual const char *getFieldStructName(int field) const override;
    virtual omnetpp::any_ptr getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const override;
    virtual void setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const override;
};

Register_ClassDescriptor(DsrPacketDescriptor)

DsrPacketDescriptor::DsrPacketDescriptor() : omnetpp::cClassDescriptor(omnetpp::opp_typename(typeid(DsrPacket)), "omnetpp::cMessage")
{
    propertyNames = nullptr;
}

DsrPacketDescriptor::~DsrPacketDescriptor()
{
    delete[] propertyNames;
}

bool DsrPacketDescriptor::doesSupport(omnetpp::cObject *obj) const
{
    return dynamic_cast<DsrPacket *>(obj)!=nullptr;
}

const char **DsrPacketDescriptor::getPropertyNames() const
{
    if (!propertyNames) {
        static const char *names[] = {  nullptr };
        omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
        const char **baseNames = base ? base->getPropertyNames() : nullptr;
        propertyNames = mergeLists(baseNames, names);
    }
    return propertyNames;
}

const char *DsrPacketDescriptor::getProperty(const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? base->getProperty(propertyName) : nullptr;
}

int DsrPacketDescriptor::getFieldCount() const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    return base ? 11+base->getFieldCount() : 11;
}

unsigned int DsrPacketDescriptor::getFieldTypeFlags(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeFlags(field);
        field -= base->getFieldCount();
    }
    static unsigned int fieldTypeFlags[] = {
        FD_ISEDITABLE,    // FIELD_sourceAddress
        FD_ISEDITABLE,    // FIELD_destinationAddress
        FD_ISEDITABLE,    // FIELD_packetType
        FD_ISEDITABLE,    // FIELD_sequenceNumber
        FD_ISEDITABLE,    // FIELD_brokenLink
        FD_ISEDITABLE,    // FIELD_linkAvailability
        FD_ISEDITABLE,    // FIELD_signalStrength
        FD_ISEDITABLE,    // FIELD_packetId
        FD_ISARRAY | FD_ISEDITABLE,    // FIELD_route
        FD_ISEDITABLE,    // FIELD_routeLength
        FD_ISEDITABLE,    // FIELD_payload
    };
    return (field >= 0 && field < 11) ? fieldTypeFlags[field] : 0;
}

const char *DsrPacketDescriptor::getFieldName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldName(field);
        field -= base->getFieldCount();
    }
    static const char *fieldNames[] = {
        "sourceAddress",
        "destinationAddress",
        "packetType",
        "sequenceNumber",
        "brokenLink",
        "linkAvailability",
        "signalStrength",
        "packetId",
        "route",
        "routeLength",
        "payload",
    };
    return (field >= 0 && field < 11) ? fieldNames[field] : nullptr;
}

int DsrPacketDescriptor::findField(const char *fieldName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    int baseIndex = base ? base->getFieldCount() : 0;
    if (strcmp(fieldName, "sourceAddress") == 0) return baseIndex + 0;
    if (strcmp(fieldName, "destinationAddress") == 0) return baseIndex + 1;
    if (strcmp(fieldName, "packetType") == 0) return baseIndex + 2;
    if (strcmp(fieldName, "sequenceNumber") == 0) return baseIndex + 3;
    if (strcmp(fieldName, "brokenLink") == 0) return baseIndex + 4;
    if (strcmp(fieldName, "linkAvailability") == 0) return baseIndex + 5;
    if (strcmp(fieldName, "signalStrength") == 0) return baseIndex + 6;
    if (strcmp(fieldName, "packetId") == 0) return baseIndex + 7;
    if (strcmp(fieldName, "route") == 0) return baseIndex + 8;
    if (strcmp(fieldName, "routeLength") == 0) return baseIndex + 9;
    if (strcmp(fieldName, "payload") == 0) return baseIndex + 10;
    return base ? base->findField(fieldName) : -1;
}

const char *DsrPacketDescriptor::getFieldTypeString(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldTypeString(field);
        field -= base->getFieldCount();
    }
    static const char *fieldTypeStrings[] = {
        "int",    // FIELD_sourceAddress
        "int",    // FIELD_destinationAddress
        "int",    // FIELD_packetType
        "int",    // FIELD_sequenceNumber
        "int",    // FIELD_brokenLink
        "double",    // FIELD_linkAvailability
        "double",    // FIELD_signalStrength
        "int",    // FIELD_packetId
        "int",    // FIELD_route
        "int",    // FIELD_routeLength
        "string",    // FIELD_payload
    };
    return (field >= 0 && field < 11) ? fieldTypeStrings[field] : nullptr;
}

const char **DsrPacketDescriptor::getFieldPropertyNames(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldPropertyNames(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

const char *DsrPacketDescriptor::getFieldProperty(int field, const char *propertyName) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldProperty(field, propertyName);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    }
}

int DsrPacketDescriptor::getFieldArraySize(omnetpp::any_ptr object, int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldArraySize(object, field);
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_route: return 50;
        default: return 0;
    }
}

void DsrPacketDescriptor::setFieldArraySize(omnetpp::any_ptr object, int field, int size) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldArraySize(object, field, size);
            return;
        }
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set array size of field %d of class 'DsrPacket'", field);
    }
}

const char *DsrPacketDescriptor::getFieldDynamicTypeString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldDynamicTypeString(object,field,i);
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        default: return nullptr;
    }
}

std::string DsrPacketDescriptor::getFieldValueAsString(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValueAsString(object,field,i);
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: return long2string(pp->getSourceAddress());
        case FIELD_destinationAddress: return long2string(pp->getDestinationAddress());
        case FIELD_packetType: return long2string(pp->getPacketType());
        case FIELD_sequenceNumber: return long2string(pp->getSequenceNumber());
        case FIELD_brokenLink: return long2string(pp->getBrokenLink());
        case FIELD_linkAvailability: return double2string(pp->getLinkAvailability());
        case FIELD_signalStrength: return double2string(pp->getSignalStrength());
        case FIELD_packetId: return long2string(pp->getPacketId());
        case FIELD_route: return long2string(pp->getRoute(i));
        case FIELD_routeLength: return long2string(pp->getRouteLength());
        case FIELD_payload: return oppstring2string(pp->getPayload());
        default: return "";
    }
}

void DsrPacketDescriptor::setFieldValueAsString(omnetpp::any_ptr object, int field, int i, const char *value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValueAsString(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: pp->setSourceAddress(string2long(value)); break;
        case FIELD_destinationAddress: pp->setDestinationAddress(string2long(value)); break;
        case FIELD_packetType: pp->setPacketType(string2long(value)); break;
        case FIELD_sequenceNumber: pp->setSequenceNumber(string2long(value)); break;
        case FIELD_brokenLink: pp->setBrokenLink(string2long(value)); break;
        case FIELD_linkAvailability: pp->setLinkAvailability(string2double(value)); break;
        case FIELD_signalStrength: pp->setSignalStrength(string2double(value)); break;
        case FIELD_packetId: pp->setPacketId(string2long(value)); break;
        case FIELD_route: pp->setRoute(i,string2long(value)); break;
        case FIELD_routeLength: pp->setRouteLength(string2long(value)); break;
        case FIELD_payload: pp->setPayload((value)); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'DsrPacket'", field);
    }
}

omnetpp::cValue DsrPacketDescriptor::getFieldValue(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldValue(object,field,i);
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: return pp->getSourceAddress();
        case FIELD_destinationAddress: return pp->getDestinationAddress();
        case FIELD_packetType: return pp->getPacketType();
        case FIELD_sequenceNumber: return pp->getSequenceNumber();
        case FIELD_brokenLink: return pp->getBrokenLink();
        case FIELD_linkAvailability: return pp->getLinkAvailability();
        case FIELD_signalStrength: return pp->getSignalStrength();
        case FIELD_packetId: return pp->getPacketId();
        case FIELD_route: return pp->getRoute(i);
        case FIELD_routeLength: return pp->getRouteLength();
        case FIELD_payload: return pp->getPayload();
        default: throw omnetpp::cRuntimeError("Cannot return field %d of class 'DsrPacket' as cValue -- field index out of range?", field);
    }
}

void DsrPacketDescriptor::setFieldValue(omnetpp::any_ptr object, int field, int i, const omnetpp::cValue& value) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldValue(object, field, i, value);
            return;
        }
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        case FIELD_sourceAddress: pp->setSourceAddress(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_destinationAddress: pp->setDestinationAddress(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_packetType: pp->setPacketType(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_sequenceNumber: pp->setSequenceNumber(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_brokenLink: pp->setBrokenLink(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_linkAvailability: pp->setLinkAvailability(value.doubleValue()); break;
        case FIELD_signalStrength: pp->setSignalStrength(value.doubleValue()); break;
        case FIELD_packetId: pp->setPacketId(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_route: pp->setRoute(i,omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_routeLength: pp->setRouteLength(omnetpp::checked_int_cast<int>(value.intValue())); break;
        case FIELD_payload: pp->setPayload(value.stringValue()); break;
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'DsrPacket'", field);
    }
}

const char *DsrPacketDescriptor::getFieldStructName(int field) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructName(field);
        field -= base->getFieldCount();
    }
    switch (field) {
        default: return nullptr;
    };
}

omnetpp::any_ptr DsrPacketDescriptor::getFieldStructValuePointer(omnetpp::any_ptr object, int field, int i) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount())
            return base->getFieldStructValuePointer(object, field, i);
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        default: return omnetpp::any_ptr(nullptr);
    }
}

void DsrPacketDescriptor::setFieldStructValuePointer(omnetpp::any_ptr object, int field, int i, omnetpp::any_ptr ptr) const
{
    omnetpp::cClassDescriptor *base = getBaseClassDescriptor();
    if (base) {
        if (field < base->getFieldCount()){
            base->setFieldStructValuePointer(object, field, i, ptr);
            return;
        }
        field -= base->getFieldCount();
    }
    DsrPacket *pp = omnetpp::fromAnyPtr<DsrPacket>(object); (void)pp;
    switch (field) {
        default: throw omnetpp::cRuntimeError("Cannot set field %d of class 'DsrPacket'", field);
    }
}

Register_Enum(PacketType, (PacketType::RREQ, PacketType::RREP, PacketType::DATA, PacketType::RERR, PacketType::HELLO));

namespace omnetpp {

}  // namespace omnetpp

