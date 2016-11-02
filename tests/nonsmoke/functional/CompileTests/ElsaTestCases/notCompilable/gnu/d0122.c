const char __module_parm_desc_aic7xxx[] __attribute__((section(".modinfo"))) =
  "parm_desc_" "aic7xxx" "=" "period delimited, options string.
	verbose			Enable verbose/diagnostic logging
	no_probe		Disable EISA/VLB controller probing
	no_reset		Supress initial bus resets
	extended		Enable extended geometry on all controllers
	periodic_otag		Send an ordered tagged transaction periodically
				to prevent tag starvation.  This may be
				required by some older disk drives/RAID arrays. 
	reverse_scan		Sort PCI devices highest Bus/Slot to lowest
	tag_info:<tag_str>	Set per-target tag depth
	seltime:<int>		Selection Timeout(0/256ms,1/128ms,2/64ms,3/32ms)

	Sample /etc/modules.conf line:
		Enable verbose logging
		Disable EISA/VLB probing
		Set tag depth on Controller 2/Target 2 to 10 tags
		Shorten the selection timeout to 128ms from its default of 256

	options aic7xxx='\"verbose.no_probe.tag_info:{{}.{}.{..10}}.seltime:1\"'
"
  ;
