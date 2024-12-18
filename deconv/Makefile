include ../../mitool/Makefile.in
include ../../mitool/milib/Makefile.in
include ../../mitool/miblaslib/Makefile.in
include ../../mitool/mifitslib/Makefile.in
include ../../mitool/mirootlib/Makefile.in
# include ../Makefile.in

INCLUDE = $(MIROOTLIB_INC) $(MIFITSLIB_INC) $(MIBLASLIB_INC) \
	$(MILIB_INC) $(CFITSIO_INC) $(ROOT_INC) $(MT_INC)
LDLIBS  = $(MIROOTLIB_LIB) $(MIFITSLIB_LIB) $(MIBLASLIB_LIB) \
	$(MILIB_LIB) $(CFITSIO_LIB) \
        $(ROOT_ALLLIBS) $(LAPACK_LIB) $(BLAS_LIB) $(MT_LIB) $(LIB) -lgfortran

TARGET = deconv

all: $(TARGET) 

deconv: deconv.o  arg_deconv.o rl.o  rl_statval.o
	$(CXXN) $@.o arg_$@.o rl.o rl_statval.o  ${LDLIBS} -o $@

#richlucy_openblas: richlucy.o  arg_richlucy.o
#	$(CXXN) richlucy.o arg_richlucy.o ${LDLIBS_OPENBLAS} -o $@


.SUFFIXES:
.SUFFIXES: .o .c .f .F .cc

.c.o:
	$(CCN) -c $<

.F.o : 
	$(FCN) -c $<

.f.o:
	$(FCN) -c $<

.cc.o:
	${CXXN} -c $<

install: all
	if [ ! -d $(INST_BIN_DIR) ]; then mkdir $(INST_BIN_DIR); fi
	install ${TARGET} ${INST_BIN_DIR}

clean:
	-rm -f *.o

cleaner: clean
	-rm -f ${TARGET} *% *~ core
