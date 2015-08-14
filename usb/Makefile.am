usbdir = /usr/bin/usb
compdir = /usr/bin/usb/compositions
initddir = /etc/init.d

usb_SCRIPTS = target
comp_SCRIPTS = compositions/9025 compositions/empty compositions/hsic_next \
	compositions/hsusb_next
initd_SCRIPTS = start_usb

install-data-hook:
	test -L $(DESTDIR)$(usbdir)/boot_hsusb_composition || $(LN_S) $(compdir)/9025 $(DESTDIR)$(usbdir)/boot_hsusb_composition
	test -L $(DESTDIR)$(usbdir)/boot_hsic_composition || $(LN_S) $(compdir)/empty $(DESTDIR)$(usbdir)/boot_hsic_composition
