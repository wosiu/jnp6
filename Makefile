all : example

example : grubaryba_example.cc mojagrubaryba.h mojagrubaryba.cc grubaryba.h
	g++ -std=c++11 -Wall grubaryba_example.cc mojagrubaryba.h mojagrubaryba.cc grubaryba.h -o example

