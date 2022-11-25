#include <iostream>

#include <cpu.hpp>
#include <io.hpp>

int main(int argc, char *argv[])
{
	system("cls");
	IOHandler io = IOHandler();
	io.read_card();
} 