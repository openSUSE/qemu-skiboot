# -*-Makefile-*-
SUBDIRS += hw
HW_OBJS  = xscom.o chiptod.o lpc.o lpc-uart.o psi.o sbe.o
HW_OBJS += homer.o slw.o occ.o fsi-master.o centaur.o imc.o
HW_OBJS += nx.o nx-rng.o nx-crypto.o nx-compress.o nx-842.o nx-gzip.o
HW_OBJS += sfc-ctrl.o fake-rtc.o bt.o p8-i2c.o prd.o
HW_OBJS += dts.o lpc-rtc.o xive.o phb4.o
HW_OBJS += fake-nvram.o lpc-mbox.o npu2.o npu2-hw-procedures.o
HW_OBJS += npu2-common.o npu2-opencapi.o phys-map.o sbe-p9.o capp.o
HW_OBJS += occ-sensor.o vas.o dio-p9.o lpc-port80h.o cache-p9.o
HW_OBJS += npu-opal.o ocmb.o xive2.o pau.o pau-hw-procedures.o
ifeq ($(CONFIG_P8),1)
HW_OBJS += phb3.o sbe-p8.o slw-p8.o
HW_OBJS += npu.o npu-hw-procedures.o
endif
HW=hw/built-in.a

include $(SRC)/hw/fsp/Makefile.inc
include $(SRC)/hw/ast-bmc/Makefile.inc
include $(SRC)/hw/ipmi/Makefile.inc

$(HW): $(HW_OBJS:%=hw/%) $(FSP) $(EC) $(AST_BMC) $(IPMI)
