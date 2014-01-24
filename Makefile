all : example

example : grubaryba_example.cc mojagrubaryba.h mojagrubaryba.cc grubaryba.h
	g++ -std=c++11 -Wall -ggdb grubaryba_example.cc mojagrubaryba.h mojagrubaryba.cc grubaryba.h -o example

