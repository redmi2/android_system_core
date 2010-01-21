.PHONY : all
all:
	cd libmincrypt && $(MAKE)
	cd mkbootimg && $(MAKE)
