#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
using namespace std;

typedef struct _node{
	int age;
	string name;
} Node;

void play();

int main()
{
	int b, c;
	int i = 0;
	pair<int,string> x(10,"james");
	cout<<x.first<<" "<<x.second<<endl;
	pair<int,string> y;
	cout<<y.first<<" "<<y.second<<endl;
	pair<int,string> z;
	z = make_pair(8,"lily");
	cout<<z.first<<" "<<z.second<<endl;
	cout<<z.second.size()<<endl;
	cout<<z.second.at(0)<<endl;
	cout<<z.second.find('i',0)<<endl;

	c = 1;
	b =(c==1)?2:1;
	cout<<b<<"*************\n";
	Node a;
	a.age = 10;
	a.name = "jake";
	cout<<a.age<<" "<<a.name<<endl;
	play();
	return 0;
}

void play()
{
	static int count = 0;	
	if(count>3)
		return;
	count++;
	cout<<"count: "<<count<<endl;
	play();
}

