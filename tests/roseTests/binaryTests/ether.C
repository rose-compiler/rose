#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern "C" {
#  include <xenctrl.h> 
#  include <xen/domctl.h>
#  include <xen/hvm/ether.h>
}

#include "ether.h"

/* the following ether_* functions are 
 * just used to call a domctl into the hypervisor.
 *
 */
static int ether_domctl(int xc_iface, int dom, uint32_t request)
{

    struct xen_domctl hvm_op;
    int result;

    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = request;

    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1) {
	    perror("Could not run xc_domctl");
    }

    return result;

}

int ether_terminate(int xc_iface, int dom)
{
	return ether_domctl(xc_iface, dom, XEN_DOMCTL_ETHER_TERMINATE);
}

int ether_readguest(int xc_iface, int dom, 
		unsigned long va,
		unsigned char *buffer, int length)
{

    struct xen_domctl hvm_op;
    int result;

    /* this is actually necessary to get this memory
     * paged in, so the hypervisor can write to it
     */
    memset(buffer, 0, length);

    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_READ_GUEST;
    hvm_op.u.ether.guest_va = va;
    hvm_op.u.ether.guest_buffer = buffer;
    hvm_op.u.ether.data_length = length;
    

    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1)
    {
	    perror("Could not run xc_domctl");
    }
    return result;

}

int ether_ss(int xc_iface, int dom, 
		int on_or_off)
{

    struct xen_domctl hvm_op;
    int result;

    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_SINGLE_STEP;
    hvm_op.u.ether.on_or_off = on_or_off;
    
    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1)
    {
	    perror("Could not run xc_domctl");
    }
    return result;

}

int ether_ss_notify(int xc_iface, int dom, 
		int on_or_off)
{

    struct xen_domctl hvm_op;
    int result;

    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_SS_DETECT;
    hvm_op.u.ether.on_or_off = on_or_off;
    
    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1)
    {
	    perror("Could not run xc_domctl");
    }
    return result;

}

int ether_memwrite_notify(int xc_iface, int dom, 
		int on_or_off)
{

    struct xen_domctl hvm_op;
    int result;

    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_MEMWRITE;
    hvm_op.u.ether.on_or_off = on_or_off;
    
    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1)
    {
	    perror("Could not run xc_domctl");
    }
    return result;

}

int ether_unpack_notify(int xc_iface, int dom, 
		int on_or_off)
{

    struct xen_domctl hvm_op;
    int result;

    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_UNPACK;
    hvm_op.u.ether.on_or_off = on_or_off;
    
    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1)
    {
	    perror("Could not run xc_domctl");
    }
    return result;

}

int ether_name(int xc_iface, int dom, uint32_t request,
		char* name)
{

    struct xen_domctl hvm_op;
    int result;


    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = request;
    hvm_op.u.ether.data_length = strlen(name)+1;
    hvm_op.u.ether.guest_buffer = (unsigned char*)(name);

    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1)
    {
	    perror("Could not run xc_domctl");
    }


    return result;

}

int ether_set_guest_sysenter(int xc_iface, int dom, unsigned long new_eip)
{
	int result = -1;
	struct xen_domctl hvm_op;

	if(new_eip != 0)
		printf("Trying to set new SYSENTER EIP to: 0x%lx\n", new_eip);
	else
		printf("Restoring original SYSENTER EIP\n");

	hvm_op.domain = (domid_t)dom;
	hvm_op.cmd = XEN_DOMCTL_ether;
	hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_SET_SYSENTER;
	hvm_op.u.ether.sysenter_cs = 0;
	hvm_op.u.ether.sysenter_eip = new_eip;

	result = xc_domctl(xc_iface, &hvm_op);

	if(result == -1)
	{
		perror("Could not run xc_domctl");
	}

	return result;
}

int ether_initialize_communication(int xc_iface, int dom, 
                                   struct ether_communication *comm/*out*/)
{
	int result = -1;
	struct xen_domctl hvm_op;

	memset(comm, 0, sizeof(struct ether_communication));

	hvm_op.domain = (domid_t)dom;
	hvm_op.cmd = XEN_DOMCTL_ether;
	hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_INIT;

	memcpy(&(hvm_op.u.ether.comm), comm, 
		sizeof(struct ether_communication));

	comm->domid_source = 0;
	comm->event_channel_port = 0;
	result = xc_domctl(xc_iface, &hvm_op);

	if(result == -1)
	{
	    perror("Could not run xc_domctl");
	    return result;
	}

	memcpy(comm, &(hvm_op.u.ether.comm), 
		    sizeof(struct ether_communication));

	return result;
} 

int ether_add_cr3_filter(int xc_iface, int dom, unsigned long cr3)
{

    struct xen_domctl hvm_op;
    int result;

    hvm_op.domain = (domid_t)dom;
    hvm_op.cmd = XEN_DOMCTL_ether;
    hvm_op.u.ether.command_code = XEN_DOMCTL_ETHER_ADD_CR3;
    hvm_op.u.ether.cr3_value = cr3;
    
    result = xc_domctl(xc_iface, &hvm_op);

    if(result == -1)
    {
	    perror("Could not run xc_domctl");
    }
    return result;

}
