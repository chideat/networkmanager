#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <NetworkManager/NetworkManager.h>

/*  NetworkManager Macro  */
#define  DBUS_NET_SERVICE NM_DBUS_SERVICE

#define  DBUS_NET_PATH NM_DBUS_PATH
#define  DBUS_NET_INTERFACE NM_DBUS_INTERFACE
#define  DBUS_NET_INTERFACE_DEVICE NM_DBUS_INTERFACE_DEVICE
#define  DBUS_NET_INTERFACE_WIRED NM_DBUS_INTERFACE_DEVICE_WIRED
#define  DBUS_NET_INTERFACE_WIRELESS NM_DBUS_INTERFACE_DEVICE_WIRELESS
#define  DBUS_NET_INTERFACE_ACCESS_POINT NM_DBUS_PATH_ACCESS_POINT
#define  DBUS_NET_INTERFACE_ACTIVE_CONNECTION  NM_DBUS_INTERFACE_ACTIVE_CONNECTION
#define  DBUS_NET_INTERFACE_IP4_CONFIG NM_DBUS_INTERFACE_IP4_CONFIG
#define  DBUS_NET_INTERFACE_DHCP4_CONFIG NM_DBUS_INTERFACE_DHCP4_CONFIG
#define  DBUS_NET_INTERFACE_IP6_CONFIG NM_DBUS_INTERFACE_IP6_CONFIG
#define  DBUS_NET_INTERFACE_DHCP6_CONFIG NM_DBUS_INTERFACE_DHCP6_CONFIG

#define  DBUS_NET_IFACE_SETTINGS NM_DBUS_IFACE_SETTINGS
#define  DBUS_NET_PATH_SETTINGS NM_DBUS_PATH_SETTINGS

#define  DBUS_NET_IFACE_SETTINGS_CONNECTION NM_DBUS_IFACE_SETTINGS_CONNECTION
#define  DBUS_NET_PATH_SETTINGS_CONNECTION NM_DBUS_PATH_SETTINGS_CONNECTION
#define  DBUS_NET_IFACE_SETTINGS_CONNECTION_SECRETS NM_DBUS_IFACE_SETTINGS_CONNECTION_SECRETS

#define PRO_CONNECTION "connection"
#define PRO_CONNECTION_AUTOCONNECT "autoconnect"
#define PRO_CONNECTION_ID "id"
#define PRO_CONNECTION_TYPE "type"
#define PRO_CONNECTION_UUID "uuid"

#define PRO_IPv4 "ipv4"
#define PRO_IPv6 "ipv6"
#define PRO_IP_ADDREDD "addresses"
#define PRO_IP_DNS "dns"
#define PRO_IP_METHOD "method"
#define PRO_IP_METHOD_AUTO "auto"
#define PRO_IP_ROUTES "routes"
#define PRO_802_3_ETHERNET "802-3-ethernet"
#define PRO_802_3_ETHERNET_DUPLEX "duplex"
#define PRO_802_3_ETHERNET_MAC_ADDRESS "mac-address"
#define PRO_802_3_ETHERNET_S390_OPTIONS "s390-options"

#define PRO_PPPoE "pppoe"

#define PRO_802_11_WIRELESS "802-11-wireless"


//interfaces' functions
//org.freedesktop.NetworkManager.Settings
#define DBUS_NET_IFACE_SETTINGS_AddConnection "AddConnection"
#define DBUS_NET_IFACE_SETTINGS_ListConnections "ListConnections"
#define DBUS_NET_IFACE_SETTINGS_GetConnectionByUuid "GetConnectionByUuid"
#define DBUS_NET_IFACE_SETTINGS_SIGNAL_NewConnection "NewConnection"

//org.freedesktop.NetworkManager.Settings.Connection
#define DBUS_NET_IFACE_SETTINGS_CONNECTION_GetSettings "GetSettings"
#define DBUS_NET_IFACE_SETTINGS_CONNECTION_GetSecrets "GetSecrets"
#define DBUS_NET_IFACE_SETTINGS_CONNECTION_Update "Update"
#define DBUS_NET_IFACE_SETTINGS_CONNECTION_Delete "Delete"
#define DBUS_NET_IFACE_SETTINGS_CONNECTION_SIGNAL_Removed "Removed"

//org.freedesktop.NetworkManager
#define DBUS_NET_INTERFACE_GetDevices "GetDevices"
#define DBUS_NET_INTERFACE_ActivateConnection "ActivateConnection"
#define DBUS_NET_INTERFACE_DeactivateConnection "DeactivateConnection"

#define CONNECTION_TYPE_802_3_ETHERNET PRO_802_3_ETHERNET
#define CONNECTION_TYPE_PPPoE PRO_PPPoE
#define CONNECTION_TYPE_802_11_WIRELESS PRO_802_11_WIRELESS

//properties
//org.freedesktop.DBus.Properties
#define DBUS_NET_PROPERTIES "org.freedesktop.DBus.Properties"
#define DBUS_NET_PROPERTIES_Get "Get"
#define DBUS_NET_PROPERTIES_GetAll "GetAll"
#define DBUS_NET_PROPERTIES_Set "Set"

#define DBUS_NET_INTERFACE_NetworkingEnabled "NetworkingEnabled"
#define DBUS_NET_INTERFACE_WirelessHardwareEnabled "WirelessHardwareEnabled"
#define DBUS_NET_INTERFACE_WirelessEnabled "WirelessEnabled"
#define DBUS_NET_INTERFACE_Version "Version"
#define DBUS_NET_INTERFACE_State "State"


#endif // NETWORKMANAGER_H