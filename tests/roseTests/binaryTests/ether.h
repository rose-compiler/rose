#ifndef __ETHER_H__
#define __ETHER_H__

int ether_terminate(int xc_iface, int dom);
int ether_readguest(int xc_iface, int dom, unsigned long va, unsigned char *buffer, int length);
int ether_ss(int xc_iface, int dom, int on_or_off); 
int ether_ss_notify(int xc_iface, int dom, int on_or_off);
int ether_memwrite_notify(int xc_iface, int dom, int on_or_off);
int ether_unpack_notify(int xc_iface, int dom, int on_or_off);
int ether_name(int xc_iface, int dom, uint32_t request, char* name);
int ether_set_guest_sysenter(int xc_iface, int dom, unsigned long new_eip);
int ether_initialize_communication(int xc_iface, int dom, struct ether_communication *comm);
int ether_add_cr3_filter(int xc_iface, int dom, unsigned long cr3);

#endif 
