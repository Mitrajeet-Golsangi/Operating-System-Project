#include <iostream>

#include <cpu.hpp>
#include <io.hpp>

int main()
{
	system("cls");
	IOHandler io = IOHandler();
	io.read_card();
} 