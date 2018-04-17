// from the kernel; please note the double-meaning of port_id; I don't
// know if this is a gcc-ism or not
typedef unsigned short __u16;
typedef __u16 port_id;
struct net_bridge_port
{
  port_id port_id;
  port_id designated_port;
};
