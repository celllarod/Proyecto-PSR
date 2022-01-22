#include <ostream>
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/stats-module.h"
#include "ns3/stats-module.h"
#include "etiquetaTiempo.h"


using namespace ns3;


NS_LOG_COMPONENT_DEFINE ("EtiquetaTiempo");


TypeId 
TimestampTag::GetTypeId (void)
{
    NS_LOG_FUNCTION_NOARGS();
    static TypeId tid = TypeId ("TimestampTag")
        .SetParent<Tag> ()
        .AddConstructor<TimestampTag> ()
        .AddAttribute ("Timestamp",
                    "Some momentous point in time!",
                    EmptyAttributeValue (),
                    MakeTimeAccessor (&TimestampTag::GetTimestamp),
                    MakeTimeChecker ())
    ;
    return tid;
}


TypeId 
TimestampTag::GetInstanceTypeId (void) const
{
    NS_LOG_FUNCTION_NOARGS();
    return GetTypeId ();
}


uint32_t 
TimestampTag::GetSerializedSize (void) const
{
    NS_LOG_FUNCTION_NOARGS();
    return 8;
}


void 
TimestampTag::Serialize (TagBuffer i) const
{
    NS_LOG_FUNCTION_NOARGS();
    int64_t t = m_timestamp.GetNanoSeconds ();
    i.Write ((const uint8_t *)&t, 8);
}


void 
TimestampTag::Deserialize (TagBuffer i)
{
    NS_LOG_FUNCTION_NOARGS();
    int64_t t;
    i.Read ((uint8_t *)&t, 8);
    m_timestamp = NanoSeconds (t);
}


void
TimestampTag::SetTimestamp (Time time)
{
    NS_LOG_FUNCTION(time);
    m_timestamp = time;
}


Time
TimestampTag::GetTimestamp (void) const
{
    NS_LOG_FUNCTION_NOARGS();
    return m_timestamp;
}


void 
TimestampTag::Print (std::ostream &os) const
{
    NS_LOG_FUNCTION_NOARGS();
    os << "t=" << m_timestamp;
}