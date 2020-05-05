CONTIKI_PROJECT = do-nothing-die cca-die broadcast-die
all: $(CONTIKI_PROJECT)

CONTIKI = ../../../

PLATFORMS_EXCLUDE = nrf52dk

CFLAGS += -DWITHOUT_LEDS=1

MODULES += os/storage/cfs

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC ?= MAKE_MAC_NULLMAC
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include
