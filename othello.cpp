#include "othello16.h"
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "gamti.h"

class othello_ai{
	othello16 o;//实例化othello16类
	public:
	void init(int color, string s);
	void move(int color, int x, int y);
	pair<int, int> get();
	int get_eval(string s,int color);   //算出给定局面的估价值
	int value(int x,int y,int col);   //获取每一个点的估价值
	int eval_node(int depth,int color,int cmp_val);  //返回倒退值
};

void othello_ai::init(int color, string s){
	o.init(color, s);//让sdk初始化局面
}

void othello_ai::move(int color, int x, int y){
	o.play(color, x, y);//告诉sdk落子情况,改变局面
}

pair<int, int> othello_ai::get(){
	int init_color, depth, ans_size;
	int b_index, b_value, temp_value;
	int cmp_val;
	string ori_string;
	vector<pair<int,int> > ans;

	ori_string = o.tostring(); //保存初始局面
	b_index = 0;
	b_value = -0x4000;
   	ans = o.allmove(o.mycolor);
	ans_size = ans.size();
	depth = 0;

	init_color = (o.mycolor==1)?2:1;

	std::cerr<<"root-->allmove size is "<<ans_size<<endl;		
	for(int i=0; i<ans_size; i++){
		o.play(o.mycolor,ans[i].first,ans[i].second);
		temp_value = eval_node(depth+1,init_color,b_value);
		std::cerr<<"root--->move["<<i<<"] value is "<<temp_value<<endl;
		o.init(o.mycolor,ori_string);
		if(b_value < temp_value){
			b_value = temp_value;
			b_index = i;
		}
	}	
	std::cerr<<"root--->choose move["<<b_index<<"]\n";
	return ans[b_index];
}



int othello_ai::eval_node(int depth,int color,int cmp_val){
	if(get_time()>1995){ //超时
		std::cerr<<"timeout..\n";
		return get_eval(o.tostring(),color);
	}
	if(depth == 4)  // 达到指定的搜索深度
		return get_eval(o.tostring(),color);

	int best_value,temp_value;
	int op_col;
	vector<pair<int,int> >  ans; 
	int chd_size;
	string ori_string; //保持起初的局面

	op_col = (color==1)?2:1;
	best_value = (depth%2==0)?-0x40000:0x40000;
	ori_string = o.tostring();

	if(o.canmove(color) == false){  //color颜色方无法走
		return (depth%2==0)?-0x40000:0x40000;
	}

	ans = o.allmove(color);  //获取所有可下位置列表
	chd_size = ans.size();

	for(int i=0;i<chd_size;i++){
		o.play(color,ans[i].first,ans[i].second);
		temp_value = eval_node(depth+1,op_col,best_value);
		
		if(depth % 2 == 0 && cmp_val <= temp_value){   //减枝			
			return temp_value;
		}
		if(depth % 2 != 0 && cmp_val >= temp_value){   //减枝			
			return temp_value;
		}

		//std::cerr<<depth<<"--ans["<<i<<"] value is "<<temp_value<<endl;
		o.init(o.mycolor,ori_string);
		if(depth % 2 ==0 && best_value < temp_value)
			best_value = temp_value;
		else if(depth % 2 != 0 && best_value > temp_value)
			best_value = temp_value;
	}
	//std::cerr<<depth<<"--node value is "<<best_value<<endl;
	return best_value;
}



//行动力与棋子分值各占50%
int othello_ai::get_eval(string s,int color){
	int sum;
	int x,y;
	int op_col;
	int eval;

	eval = 0;
	sum = 0;
	op_col = (o.mycolor==1)?2:1;

	for(int i=0; i<s.size();i++){
		x = i/16;
		y = i%16;
		if(s.at(i)-0x30 == o.mycolor)
			sum += value(x,y,o.mycolor);
		else if(s.at(i)-0x30 == op_col)
			sum -= value(x,y,op_col);
	}

	// eval += 0.5*sum;

	/*op_col = (color==1)?2:1;   //next person to put
	if(o.canmove(color)){
		if(color== o.mycolor)  //下一步我方走
			eval += 0.5*o.allmove(color).size();
		else 
			eval -= 0.5*o.allmove(color).size();
	}
	else if(o.canmove(op_col)){
		if(op_col== o.mycolor)
			eval += 0.5*o.allmove(op_col).size();
		else 
			eval -= 0.5*o.allmove(op_col).size();
	}
	else{
		//std::cerr<<"game end\n";
	}*/

	return sum;
}



int othello_ai::value(int x,int y,int col){
	if((x==0 && y==0) || (x==0 && y==15) || (x==15 && y==0) || (x==15 && y==15)) //corner
		return 100;

	if((x==0 && y==1) || (x==1 && y==0) || (x==1 && y==1))		
			return -100;

	if((x==0 && y==14) || (x==1 && y==14) || (x==1 && y==15))
			return -100;

	if((x==14 && y==0) || (x==14 && y==1) || (x==15 && y==1))
			return -100;

	if((x==14 && y==14) || (x==14 && y==15) || (x==15 && y==15))
			return -100;

	if(x == 0 || x == 15 || y == 0 || y == 15)   //border
		return 32;

	return 1;
}



