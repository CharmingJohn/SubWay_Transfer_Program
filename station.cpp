//Copyright github.com/charmingjohn

#include <iostream>
#include <fstream>
#include <sstream>
#include <climits>
#include <string>


using namespace std;

#include "station.h"
#include "queue.h"
#include "stack.h"

station::station()
{
	memset(time_table, 0, sizeof(time_table));
	memset(station_head, 0, sizeof(station_head));
	memset(line_head, 0, sizeof(line_head));
	memset(size, 0, sizeof(size));
	memset(line_number, 0, sizeof(line_number));

}

station::~station()
{
	// ����� �޸� ����
	for (int i = 0; i < MAX_STATION; i++) {
		station_node* p = station_head[i];
		while (p != NULL) {
			station_node* q = p->next;
			delete p;
			p = q;
		}
	}

	// �뼱�� ��¿� �ʿ��� �� ���Ƽ� �غ������� �̱���(������)
	for (int i = 0; i < MAX_LINE; i++) {
		station_node* p = line_head[i];
		while (p != NULL) {
			station_node* q = p->next;
			delete p;
			p = q;
		}
	}

	// �� ���� �����ִ� �뼱�� �������� ����Ʈ�� ����
	for (int i = 0; i < MAX_STATION; i++) {
		station_node* p = line_number[i];
		while (p != NULL) {
			station_node* q = p->next;
			delete p;
			p = q;
		}
	}
}

void station::add_station(int line_no, int name1, int name2, int t)
{
	time_table[name1][name2] = t;
	time_table[name2][name1] = t;
	
	line_table[name1][name2] = line_no;
	line_table[name2][name1] = line_no;

	if (station_head[name1] == NULL){
		station_node *node = new station_node(line_no, name1);
		station_head[name1] = node;
	}
	station_node* node2 = new station_node(line_no, name2);
	station_node* p1 = station_head[name1];
	int inserted = 0;
	while (p1->next != NULL) {
		if (name2 < p1->next->name)
			//��������Ʈ�� �ֺ� ������ �̾���. �� �� ���ĺ� ������ �����ϰ� �����.
			//�ִܰŸ��� ���� ��� �����濡�� ���ĺ� ������ ���� ������ ���� ���� �ϱ� ����.
		{
			node2->next = p1->next;
			p1->next = node2;
			inserted = 1;
			break;
		}
		p1 = p1->next;
	}
	if(!inserted) // ������ �� ������ �ڿ� �߰�
		p1->next = node2;

	if (station_head[name2] == NULL) {
		station_node *node = new station_node(line_no, name2);
		station_head[name2] = node;
	}
	station_node* node1 = new station_node(line_no, name1);
	station_node* p2 = station_head[name2];
	inserted = 0;
	while (p2->next != NULL){
		if (name1 < p2->next->name)
		{
			node1->next = p2->next;
			p2->next = node1;
			inserted = 1;
			break;
		}
		p2 = p2->next;
	}
	if (!inserted)
		p2->next = node1;
}

void station::readfile(int line_no, char* line1)
{
	int last_time = 0;
	int last_station = 0;

	ifstream line_1;
	line_1.open(line1);
	string station;
	int time = 0;
	while (!line_1.eof())
	{
		getline(line_1, station);
		if (station.compare("") == 0)
			break;
		stringstream stream(station);
		stream >> station;
		stream >> time;
		int name = station[0];
		add_line_station(name, line_no);
		if (last_station != 0)
			add_station(line_no, last_station, name, last_time);

		station_node *nodel = new station_node(line_no, name);
		if (line_head[line_no] == NULL)
			line_head[line_no] = nodel;
		else { // ���� �ٿ��ش�.
			station_node *p = line_head[line_no];
			while (p->next != NULL)
				p = p->next;
			p->next = nodel;
		}
		size[name]++;//ȯ�¿����� �Ǵ� (size[]=2�� ��� ȯ�¿�)
		last_time = time;
		last_station = name;
	}
	line_1.close();


}

void station::shortest(int start, int destination, ofstream &out)
{
	station_node *p = line_number[start];
	station_node *p2 = line_number[destination];
	if (p == NULL || p2 == NULL) {
		if (p == NULL)
		{
			out << (char)start << " not exists in the graph" << endl;
		}
		if (p2 == NULL)
		{
			out << (char)destination << " not exists in the graph" << endl;
		}
		return;
	}
	//���ͽ�Ʈ�� �˰��� ���
	stack stack1(MAX_STATION);
	queue queue1;

	for (int i = 0; i < MAX_STATION; i++)
	{
		dist[i] = INT_MAX;
		previous[i] = 0;
	}

	dist[start] = 0;
	queue1.enqueue(start);
	int cur=0;

	int prev_line = 0;
	int current_line = 0;

	while (!queue1.isempty())
	{
		cur = queue1.dequeue();
		station_node* target = station_head[cur]->next;
	//	if (cur == destination)
		//	break;

		while (target != NULL)
		{
			int length = dist[cur] + time_table[cur][target->name];
			if (length < dist[target->name])
			{
				queue1.enqueue(target->name);
				previous[target->name] = cur;
				dist[target->name] = length;
			}
			target = target->next;
		}
	}

	int cur1 = destination;
	stack1.push(cur1);
	while (previous[cur1] != 0) {
		cur1 = previous[cur1];
		stack1.push(cur1);
	}

	out << "Short " << (char)start << " to " << (char)destination << endl;

	prev_line = 0; // ȯ�� ���θ� �˱� ���� ���� �������� �뼱 ��ȣ.
	int transfer_count = 0; // ȯ�� Ƚ��
	int prev_station = 0; // ��� ������ �̸����� ��ȣ.
	//int total_time = 0;

	out << (char)stack1._top(); // ��߿��� ǥ���Ѵ�.

	int time = 0;
	int prev_time = 0;

	while (!stack1.isempty())
	{
		int a = stack1.pop();
		if (prev_station != 0) { // ���� ���� �����ϸ�
			//total_time += time_table[prev_station][a]; // �ҿ�ð� ����.
			int current_line = line_table[prev_station][a]; // �뼱ǥ���� �뼱 ��ȣ�� �����´�.
			if (prev_line != 0 &&               // ���� �뼱 ��ȣ�� �����ϰ�, 
				current_line != prev_line )     // ���� �뼱�� ���� �뼱�� �ٸ��� ȯ���� �� ������ ��.
			{
				// �뼱�� ����Ǿ����� ������ �뼱 ��ȣ�� ȯ�¿��� ǥ���Ѵ�. ���� ���� ������ �ƴϸ� ǥ���� �ʿ�� ����.
				// ���� ���翪�� �����̶�� �� �ؿ��� �������� ǥ�õȴ�.
				time = dist[prev_station];
				out << "--" << (time - prev_time) % 200 << "--" << (char)prev_station; // �뼱��� ����
				prev_time = time;
				
				transfer_count++; // ȯ��Ƚ���� ����ص���. ���߿� ���� ���� �ִ�. ���� ������ ���߿� �����ص� �ȴ�.
			}
			prev_line = current_line; // ���� �뼱���� ���� �뼱�� �ٲ��ְ�
		}

		prev_station = a; // ���� ���� ���� ���� �ȴ�.
	}
	// ������ ���� �� �������� �뼱 ��ȣ�� ǥ���Ѵ�.
	// �� ������ �� ������ prev_station�� ������ �ִ�.
	out << "--" << (dist[prev_station]-prev_time)%200 << "--" << (char)prev_station << endl; // print destination
}

int station::calculate_expected_length(int cur2, int tname)
{//�ּ�ȯ�½� �������� �Ÿ��� ����ϱ� ����.
	stack s3(MAX_STATION);
	s3.push(tname);
	while (cur2 != 0) {
		s3.push(cur2);
		cur2 = previous[cur2];
	}
	
	int length = 0;
	int prev_line = 0;
	int prev = 0;
	while (!s3.isempty()) {
		int curr = s3.pop();
		if (prev != 0) {
			length += time_table[prev][curr];
			int curr_line = line_table[prev][curr];
			if (prev_line != 0 && prev_line != curr_line)
				length += 200; //ȯ���� �߻��ϸ� �ش� length(�� ������ �Ÿ�)�� 200 ����ġ�� �߰��Ͽ� ȯ�� ����ġ�� ��.
			prev_line = curr_line;
		}
		prev = curr;
	}

	return length;
}

void station::least(int start, int destination, ofstream &out)
{

	station_node *p = line_number[start];
	station_node *p2 = line_number[destination];
	if (p == NULL || p2 == NULL) {
		if (p == NULL)
		{
			out << (char)start << " not exists in the graph" << endl;
		}
		if (p2 == NULL)
		{
			out << (char)destination << " not exists in the graph" << endl;
		}
		return;
	}
	

		stack stack2(MAX_STATION);
		queue queue2;

		for (int i = 0; i < MAX_STATION; i++)
		{
			dist[i] = INT_MAX;
			previous[i] = 0;
		}

		dist[start] = 0;
		queue2.enqueue(start);
		int cur2;
		int prev_line = 0;
		while (!queue2.isempty())
		{

			cur2 = queue2.dequeue();
			station_node* target2 = station_head[cur2]->next;


			while (target2 != NULL)
			{
				int current_line = line_table[cur2][target2->name];
				
				int length = calculate_expected_length(cur2, target2->name);
				//����ġ 200��ŭ ������ length�� ����Ѵ�.
				//��, ȯ���� �� �� �Ҷ����� 200��ŭ ����� �����ϹǷ� 

				prev_line = current_line;
				if (length < dist[target2->name])
				{
					queue2.enqueue(target2->name);
					previous[target2->name] = cur2;
					dist[target2->name] = length;
				}
				target2 = target2->next;
			}
		}

		int cur1 = destination;
		stack2.push(cur1);
		while (previous[cur1] != 0) {
			cur1 = previous[cur1];
			stack2.push(cur1);
		}

		out << "Least " << (char)start << " to " << (char)destination << endl;

		prev_line = 0; // ȯ�� ���θ� �˱� ���� ���� �������� �뼱 ��ȣ.
		int transfer_count = 0; // ȯ�� Ƚ��
		int prev_station = 0; // ��� ������ �̸����� ��ȣ.
		//int total_time = 0; ������ ����

		out << (char)stack2._top(); // ��߿��� ǥ���Ѵ�.

		int time = 0;
		int prev_time = 0;

		while (!stack2.isempty())
		{
			int a = stack2.pop();
			if (prev_station != 0) { // ���� ���� �����ϸ�
				//total_time += time_table[prev_station][a]; // �ҿ�ð� ����.
				int current_line = line_table[prev_station][a]; // �뼱ǥ���� �뼱 ��ȣ�� �����´�.
				if (prev_line != 0 &&               // ���� �뼱 ��ȣ�� �����ϰ�, 
					current_line != prev_line)     // ���� �뼱�� ���� �뼱�� �ٸ��� ȯ���� �� ���̴�.
				{
					// �뼱�� ����Ǿ����� ������ �뼱 ��ȣ�� ȯ�¿��� ǥ���Ѵ�. ���� ���� ������ �ƴϸ� ǥ���� �ʿ�� ����.
					// ���� ���翪�� �����̶�� �� �ؿ��� �������� ǥ�õȴ�.
					time = dist[prev_station] % 200; //200�� ������� ������ 200�� �������� ���.
					out << "--" << time << "--" << (char)prev_station; // �뼱��� �����̴�.
					prev_time = time;

					transfer_count++; // ȯ��Ƚ���� ����ص���. ���߿� ���� ���� �ִ�. ���� ������ ���߿� �����ص� �ȴ�.
				}
				prev_line = current_line; // ���� �뼱���� ���� �뼱�� �ٲ��ְ�
			}

			prev_station = a; // ���� ���� ���� ���� �ȴ�.
		}
		// ������ ���� �� �������� �뼱 ��ȣ�� ǥ���Ѵ�.
		// �� ������ �� ������ prev_station�� ������ �ִ�.
		out << "--" << (dist[prev_station]%200) - prev_time << "--" << (char)prev_station << endl; // print destination
}

void station::read_command(int sta, ofstream &out)
{
	// �뼱 ��ȣ�� �����´�
	if (sta < 0 || sta >= MAX_STATION) {
		out << (char)sta << " not exist in the graph" << endl;
		return;
	}

	station_node *p = line_number[sta];
	if (p == NULL) {
		out << (char)sta << " not exist in the graph" << endl;
		return;
	}

	// �뼱�� ���󰣴�
	out << "Read " << (char)sta<<endl;
	while (p != NULL) {
		out << "Line number: " << p->line << endl;
		follow_line(p->line, sta, out);
		p = p->next;
	}
}

void station::follow_line(int line, int sta, ofstream &out)
{
	station_node *p = line_head[line];
	int prev_station = 0;
	while (p != NULL) {
		if (p->name == sta) { // ���� ã�Ҵ�.
			if (prev_station != 0)
				out << (char)prev_station << "--" << time_table[prev_station][sta] << "--";
			out << (char)sta;
			if (p->next != NULL) {
				station_node* q = p->next;
				out << "--" << time_table[sta][q->name] << "--" << (char)q->name;
			}
			out << endl;
			break;
		}
		prev_station = p->name;
		p = p->next;
	}
}

void station::add_line_station(int sta, int line)
{
	// �� ���� �뼱 ��ȣ ����
	station_node *node = new station_node(line, sta);
	if (line_number[sta] == NULL) {
		line_number[sta] = node;
	} else {
		// �뼱 ��ȣ ���� �ͺ��� �����ϱ�, �׻� �������� �ٿ��ָ� �ȴ�.
		station_node *p = line_number[sta];
		while (p->next != NULL)
			p = p->next;
		p->next = node;
	}
}
