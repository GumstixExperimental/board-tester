# board-tester
A C library and collection of binaries for interfacing with, and batch testing GPIOs, communication buses, etc.

**Author:** Keith Lee

**Date:** July 5, 2016

## Purpose

Facilitating and automating the testing of Gumstix Geppetto boards requires interfacing with a variety buses and IO.  This library attempts to automate this process.

The idea is that a tester could attach diagnostic equipment to I/O and external devices to communication ports, run the software herein and determine  board functionality.

The library will also provide a starting point for user software development.
