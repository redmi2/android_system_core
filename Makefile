.PHONY : all
all:
	cd libmincrypt && $(MAKE)
	cd mkbootimg && $(MAKE)

.PHONY : clean
clean:
	cd libmincrypt && $(MAKE) clean
	cd mkbootimg && $(MAKE) clean
