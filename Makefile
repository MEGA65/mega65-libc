
SUBDIRS= \
		 cc65

.PHONY: all clean cleanall

all:
	@for path in $(SUBDIRS); do \
		make -C $$path all; \
	done

clean:
	@for path in $(SUBDIRS); do \
		make -C $$path clean; \
	done

cleanall:
	@for path in $(SUBDIRS); do \
		make -C $$path cleanall; \
	done

