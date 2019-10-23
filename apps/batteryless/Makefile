CONTIKI_PROJECT = broadcast-sender broadcast-receiver broadcast-both receiver-gpio
all: $(CONTIKI_PROJECT)

CONTIKI = ../..

PLATFORMS_EXCLUDE = nrf52dk

CFLAGS += -DWITHOUT_LEDS=1

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC ?= MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include
