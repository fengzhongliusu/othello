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
	int get_eval(string s);   //算出给定局面的估价值
	int value(int x,int y);   //获取每一个点的估价值
	int eval_node(int depth,int color,int cmp_val);  //返回倒退值
	vector<pair<int, int> > m_order(vector<pair<int, int> > ans,int depth);
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
	vector<pair<int,int> > order_ans;
	
	ori_string = o.tostring(); //保存初始局面
	b_index = 0;
	b_value = -0x4000;
   	ans = o.allmove(o.mycolor);
	ans_size = ans.size();
	depth = 0;

	order_ans = m_order(ans,depth);

	init_color = (o.mycolor==1)?2:1;

	for(int i=0; i<ans_size; i++)
		if((ans[i].first == 0 && ans[i].second == 0)
			|| (ans[i].first == 0 && ans[i].second == 15)
			|| (ans[i].first == 15 && ans[i].second == 0)
			|| (ans[i].first == 15 && ans[i].second == 15)
			)
		{
			std::cerr<<"get corner:<,"<<ans[i].first<<","<<ans[i].second<<">\n";
			return ans[i];
		}			


	std::cerr<<"\nroot--->move size: "<<ans_size<<endl;		
	for(int i=0; i<ans_size; i++){

		o.play(o.mycolor,order_ans[i].first,order_ans[i].second);
		temp_value = eval_node(depth+1,init_color,b_value);
		std::cerr<<"move["<<i<<"] value:"<<temp_value<<endl;
		o.init(o.mycolor,ori_string);

		if(temp_value == 0x40000 || temp_value == -0x40000)
			break;

		if(b_value < temp_value){
			b_value = temp_value;
			b_index = i;
		}
	}	
	std::cerr<<"root--->choose move["<<b_index<<"]\n\n";
	return order_ans[b_index];
}



int othello_ai::eval_node(int depth,int color,int cmp_val){
	if(get_time()>1990){ //超时
		std::cerr<<"timeout..\n";
		return (depth%2==0)?0x40000:-0x40000;
	}
	if(depth == 4){  // 达到指定的搜索深度				
		return get_eval(o.tostring());		
	}

	int best_value,temp_value;
	int op_col;
	int end_op;
	vector<pair<int,int> >  ans; 
	vector<pair<int,int> >  order_ans; 
	int chd_size;
	string ori_string; //保持起初的局面

	op_col = (color==1)?2:1;
	best_value = (depth%2==0)?-0x40000:0x40000;
	ori_string = o.tostring();

	if(o.canmove(color) == false){  //color颜色方无法走
		return (depth%2==0)?-0x50000:0x50000;	
	}

	ans = o.allmove(color);  //获取所有可下位置列表
	chd_size = ans.size();
	order_ans = m_order(ans,depth);

	for(int i=0;i<chd_size;i++){
		o.play(color,order_ans[i].first,order_ans[i].second);
		temp_value = eval_node(depth+1,op_col,best_value);
		o.init(o.mycolor,ori_string);

		if(temp_value == 0x40000 || temp_value == -0x40000){ //child node timeout
			std::cerr<<"test2\n";
			return 0x40000;
		}
		
		if(depth % 2 == 0 && cmp_val <= temp_value){   //减枝			
			return temp_value;
		}
		if(depth % 2 != 0 && cmp_val >= temp_value){   //减枝			
			return temp_value;
		}

		if(depth % 2 ==0 && best_value < temp_value)
			best_value = temp_value;
		else if(depth % 2 != 0 && best_value > temp_value)
			best_value = temp_value;
	}
	return best_value;
}



//行动力与棋子分值各占50%
int othello_ai::get_eval(string s)
{
	int my_pown=0,op_pown=0;
	int p_val = 0;
	int my_corner = 0, op_corner = 0;
	int my_c_s = 0, op_c_s = 0;
	int my_front_pown=0, op_front_pown=0;
	int round_x[] = {-1, -1, 0, 1, 1, 1, 0, -1};
	int round_y[] = {0, 1, 1, 1, 0, -1, -1, -1};
	int op_color = (o.mycolor==1)?2:1;
	int x,y;	
	double score;
	double pown_val, mv_val, front_val, corner_val, side_val;


	if(o.count(0) <= 6) {   //endgame		
		for(int i=0; i<16; i++)
			for(int j=0; j<16; j++)
				if(o.is(o.mycolor,i,j))
					p_val++;
				else if(o.is(op_color,i,j))
					p_val--;
		return p_val;
	}
	
	for(int i=0; i<16; i++)
		for(int j=0; j<16; j++){
			if(o.is(o.mycolor,i,j)){
				p_val += value(i,j);
				my_pown++;
			}
			else if(o.is(op_color,i,j)){
				p_val -= value(i,j);
				op_pown++;
			}
			if(!o.is(0,i,j)){			
				for(int k = 0; k<8; k++){
					x = i + round_x[k];
					y = j + round_y[k];
					if(x>=0 && x<16 && y>=0 && y<16 && o.is(0,x,y)){						
						if(o.is(o.mycolor,i,j))				
							my_front_pown++;
						else 
							op_front_pown++;
						break;
					}
				}				
			}
		}
	//num of pown
	if(my_pown > op_pown)
		pown_val = 100.0*my_pown /(my_pown + op_pown);
	else if(my_pown < op_pown)
		pown_val = -100.0*op_pown/ (my_pown + op_pown);
	else	
		pown_val = 0;

	// num of pown outside
	if(my_front_pown > op_front_pown)
		front_val = -100.0*my_front_pown/(my_front_pown + op_front_pown);
	else if(my_front_pown < op_front_pown)
		front_val = 100.0 * op_front_pown/ (my_front_pown + op_front_pown);
	else 
		front_val = 0;

	//corner 
	if(o.is(o.mycolor,0,0)) my_corner++;
	else if(o.is(op_color,0,0)) op_corner++;
	if(o.is(o.mycolor,0,15)) my_corner++;
	else if(o.is(op_color,0,15)) op_corner++;
	if(o.is(o.mycolor,15,0)) my_corner++;
	else if(o.is(op_color,15,0)) op_corner++;
	if(o.is(o.mycolor,15,15)) my_corner++;
	else if(o.is(op_color,15,15)) op_corner++;
	corner_val = 25 * (my_corner - op_corner);

	//corner side
	if(o.is(0,0,0)){
		if(o.is(o.mycolor,0,1)) my_c_s++;
		else if(o.is(op_color,0,1)) op_c_s++;
		if(o.is(o.mycolor,1,1)) my_c_s++;
		else if(o.is(op_color,1,1)) op_c_s++;
		if(o.is(o.mycolor,1,0)) my_c_s++;
		else if(o.is(op_color,1,0)) op_c_s++;
	}
	if(o.is(0,0,15)){
		if(o.is(o.mycolor,0,14)) my_c_s++;
		else if(o.is(op_color,0,14)) op_c_s++;
		if(o.is(o.mycolor,1,14)) my_c_s++;
		else if(o.is(op_color,1,14)) op_c_s++;
		if(o.is(o.mycolor,1,15)) my_c_s++;
		else if(o.is(op_color,1,15)) op_c_s++;
	}
	if(o.is(0,15,0)){
		if(o.is(o.mycolor,15,1)) my_c_s++;
		else if(o.is(op_color,15,1)) op_c_s++;
		if(o.is(o.mycolor,14,1)) my_c_s++;
		else if(o.is(op_color,14,1)) op_c_s++;
		if(o.is(o.mycolor,14,0)) my_c_s++;
		else if(o.is(op_color,14,0)) op_c_s++;
	}
	if(o.is(0,15,15)){
		if(o.is(o.mycolor,15,14)) my_c_s++;
		else if(o.is(op_color,15,14)) op_c_s++;
		if(o.is(o.mycolor,14,14)) my_c_s++;
		else if(o.is(op_color,14,14)) op_c_s++;
		if(o.is(o.mycolor,14,15)) my_c_s++;
		else if(o.is(op_color,14,15)) op_c_s++;
	}
	side_val = -12.5 * (my_c_s - op_c_s);

	//mobility
	int size1=0, size2=0;
	if(o.canmove(o.mycolor))
		size1 = o.allmove(o.mycolor).size();
	if(o.canmove(op_color))
		size2 = o.allmove(op_color).size();
	if(size1 > size2)
		mv_val = 100.0 * size1 / (size1 + size2);
	else if(size1 < size2)
		mv_val = -100.0 * size2 / (size1 + size2);
	else 
		mv_val = 0;

	score = 10*pown_val + 800 * corner_val + 400 * side_val + 80 * mv_val + 90*front_val + 11 * p_val;		
	return score;
}

int othello_ai::value(int x,int y)
{
	int i,j;	
	int val[16][16] = {
		{40,0,10,10,10,8,8,8},
		{0,0,2,2,2,2,2,2},
		{10,2,6,6,6,6,6,6},
		{10,2,6,3,3,3,3,3},
		{10,2,6,3,4,4,4,4},
		{8,2,6,3,4,2,2,2},
		{8,2,6,3,4,2,1,1},
		{8,2,6,3,4,2,1,0}		
	};	

	if(x<8 && y<8)
		return val[x][y];	
	if(x<8 && y>8)
		return val[x][15-y];
	if(x>8 && y<8)
		return val[15-x][y];
	if(x>8 && y>8)
		return val[15-x][15-y];
}

vector<pair<int, int> > othello_ai::m_order(vector<pair<int, int> > ans,int depth)
{	
	vector<pair<int, int> > new_ans;		
	vector<pair<int,int> > c_value;   //index and value
	pair<int,int> temp;
	string s;	
	int index;
	int op_color;
	int i_color;

	op_color = (o.mycolor == 1)?2:1;
	i_color = (depth%2==0)?o.mycolor:op_color;

	
	s = o.tostring();

	for(int i=0; i<ans.size(); i++) 	
	{
		o.play(i_color,ans[i].first,ans[i].second);
		if(depth%2==0){
			temp = make_pair(i,get_eval(o.tostring()));
			c_value.push_back(temp);
		}
		else{
			temp = make_pair(i,-1*get_eval(o.tostring()));
			c_value.push_back(temp);
		}
		o.init(o.mycolor,s);
	}
	
	for(int i=0; i<c_value.size(); i++)	
	{		
		for(int j=i+1; j<c_value.size(); j++){
			if(c_value[j].second > c_value[i].second){
				temp = c_value[i];
				c_value[i] = c_value[j];
				c_value[j] = temp;
			}
		}
		index = c_value[i].first;
		new_ans.push_back(ans[index]);
	}

	if(depth ==1){
		std::cerr<<"\nori_order------------------\n";
		for(int i=0; i<ans.size(); i++){
			std::cerr<<"<"<<ans[i].first<<","<<ans[i].second<<">\n";		
		}
		std::cerr<<"\nori_order------------------\n";
		std::cerr<<"\nm_order---------------------\n";
		for(int i=0; i<ans.size(); i++){
			std::cerr<<"<"<<new_ans[i].first<<","<<new_ans[i].second<<">\n";		
		}
		std::cerr<<"\nm_order---------------------\n";
	}

	return new_ans;
}

