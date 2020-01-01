//Copyright github.com/charmingjohn

#include <iostream>

using namespace std;

#define	HEAP_SIZE	100

typedef struct {
	char linebuf[256];
	char command[32];
	int key; // �켱����
	int source; // �����, �Ǵ� ������ ��
	int dest; // ������
} heap_entry;

class Heap
{
private:
	heap_entry *heap;
	int capacity;
	int last;
public:
	Heap(int capacity);
	~Heap();
	int isEmpty();
	void insert(string linebuf, string cmd, int key, string src, string dst);
	void extractmin();
	void swap(int a, int b);
	void print();
	heap_entry output();
};