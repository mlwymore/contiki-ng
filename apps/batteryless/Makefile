CONTIKI_PROJECT = broadcast-sender broadcast-receiver broadcast-both unicast-sender receive-counter-flash receiver-gpio flash-rw receiver-flash
all: $(CONTIKI_PROJECT)

CONTIKI = ../..

PLATFORMS_EXCLUDE = nrf52dk

CFLAGS += -DWITHOUT_LEDS=1

MODULES += os/storage/cfs

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC ?= MAKE_MAC_CONTIKIMAC
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include
