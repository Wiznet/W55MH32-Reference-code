
/********************************************************************************************
 * SNMP : User Customization Part
 *  - OID Registration
 *  - User defined functions for OID related
 *  	+ Integer value, String
 *  	+ I/O control / Chip registers
 *  	+ Network Informations
 *  	+ Etc.
 *
 *********************************************************************************************/
#include "snmp_custom.h"

/* control user led callback function */
getUserLED_handle getUserLED_cb = NULL;
setUserLED_handle setUserLED_cb = NULL;

dataEntryType snmpData[] =
    {
        // System MIB
        // SysDescr Entry
        {   8,                                  {0x2b, 6, 1, 2, 1, 1, 1, 0}, SNMPDTYPE_OCTET_STRING,   30,               {"WIZnet Embedded SNMP Agent"},                  NULL,                  NULL},

        // SysObjectID Entry
        {   8,                                  {0x2b, 6, 1, 2, 1, 1, 2, 0},       SNMPDTYPE_OBJ_ID,    8,         {"\x2b\x06\x01\x02\x01\x01\x02\x00"},                  NULL,                  NULL},

        // SysUptime Entry
        {   8,                                  {0x2b, 6, 1, 2, 1, 1, 3, 0},   SNMPDTYPE_TIME_TICKS,    0,                                         {""},         currentUptime,                  NULL},

        // sysContact Entry
        {   8,                                  {0x2b, 6, 1, 2, 1, 1, 4, 0}, SNMPDTYPE_OCTET_STRING,   30,             {"http://www.wizwiki.net/forum"},                  NULL,                  NULL},

        // sysName Entry
        {   8,                                  {0x2b, 6, 1, 2, 1, 1, 5, 0}, SNMPDTYPE_OCTET_STRING,   30,                  {"http://www.wiznet.co.kr"},                  NULL,                  NULL},

        // Location Entry
        {   8,                                  {0x2b, 6, 1, 2, 1, 1, 6, 0}, SNMPDTYPE_OCTET_STRING,   30,                         {"4F Humax Village"},                  NULL,                  NULL},

        // SysServices
        {   8,                                  {0x2b, 6, 1, 2, 1, 1, 7, 0},      SNMPDTYPE_INTEGER,    4,                                         {""},                  NULL,                  NULL},

        {   8,                                  {0x2b, 6, 1, 2, 1, 12, 2, 0}, SNMPDTYPE_OCTET_STRING,   30,                                         {""}, get_LEDStatus_UserLED,                  NULL},

        {   8,                                  {0x2b, 6, 1, 2, 1, 12, 1, 0},      SNMPDTYPE_INTEGER,    4,                                         {""},                  NULL, set_LEDStatus_UserLED},

        // OID Test #1 (long-length OID example, 19865)
        {0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0x9b, 0x19, 0x01, 0x00}, SNMPDTYPE_OCTET_STRING,   30,                  {"long-length OID Test #1"},                  NULL,                  NULL},

        // OID Test #2 (long-length OID example, 22210)
        {0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xad, 0x42, 0x01, 0x00}, SNMPDTYPE_OCTET_STRING,   35,                  {"long-length OID Test #2"},                  NULL,                  NULL},

        // OID Test #2: SysObjectID Entry
        {0x0a, {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0xad, 0x42, 0x02, 0x00},       SNMPDTYPE_OBJ_ID, 0x0a, {"\x2b\x06\x01\x04\x01\x81\xad\x42\x02\x00"},                  NULL,                  NULL},
};

const int32_t maxData = (sizeof(snmpData) / sizeof(dataEntryType));

/* Registration function */
void user_led_control_init(getUserLED_handle get_fun, setUserLED_handle set_fun)
{
    if (get_fun != NULL && set_fun != NULL)
    {
        getUserLED_cb = get_fun;
        setUserLED_cb = set_fun;
    }
}

void initTable(void)
{
    // Example integer value for [OID 1.3.6.1.2.1.1.7.0]
    snmpData[6].u.intval = -5;
}


static void get_LEDStatus_UserLED(void *ptr, uint8_t *len)
{
    (*getUserLED_cb)(ptr, len);
}
static void set_LEDStatus_UserLED(int32_t val)
{
    (*setUserLED_cb)(val);
}


void initial_Trap(uint8_t *managerIP, uint8_t *agentIP)
{
    // SNMP Trap: WarmStart(1) Trap
    {
        dataEntryType enterprise_oid = {
            0x0a,
            {0x2b, 0x06, 0x01, 0x04, 0x01, 0x81, 0x9b, 0x19, 0x01, 0x00},
            SNMPDTYPE_OBJ_ID,
            0x0a,
            {"\x2b\x06\x01\x04\x01\x81\x9b\x19\x10\x00"},
            NULL,
            NULL
        };
        // Generic Trap: warmStart COMMUNITY
        snmp_sendTrap(managerIP, agentIP, (void *)COMMUNITY, enterprise_oid, SNMPTRAP_WARMSTART, 0, 0);
    }
}
