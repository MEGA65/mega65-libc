
SUBDIRS= \
		 cc65

.PHONY: all clean cleanall cc65

all: $(SUBDIRS)

cc65:
	make -C cc65 all

clean:
	@for path in $(SUBDIRS); do \
		make -C $$path clean; \
	done

cleanall:
	@for path in $(SUBDIRS); do \
		make -C $$path cleanall; \
	done

