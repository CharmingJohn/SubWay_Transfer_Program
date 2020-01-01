//Copyright github.com/charmingjohn

#include "stack.h"
#include <iostream>

using namespace std;

stack::stack(int s)
{
	top = 0;
	stack_n = new int[s];
}

stack::~stack()
{
	delete stack_n;
}

void stack::push(int data)
{
	stack_n[top] = data;
	top++;
}

int stack::pop()
{
	if (isempty())
		cout << "���� ����" << endl;
	else
		return(stack_n[--top]);
}

int stack::_top()
{
	if (isempty())
		cout << "���� ����" << endl;
	else
		return(stack_n[top-1]);
}

bool stack::isempty()
{
	if (top == 0)
		return true;
	else
		return false;
}