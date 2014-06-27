#include "othello16.h"
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "gamti.h"

class othello_ai{
	othello16 o;//实例化othello16类
	int glb_depth;
	public:
	void init(int color, string s);
	void move(int color, int x, int y);
	pair<int, int> get();
	int get_eval(string s,int color);   //算出给定局面的估价值
	int value(int x,int y);   //获取每一个点的估价值
	int begin_value(int x,int y);
	double mid_score();
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

	glb_depth = 4;
	ori_string = o.tostring(); //保存初始局面
	b_index = 0;
	b_value = -0x4000;
   	ans = o.allmove(o.mycolor);
	ans_size = ans.size();
	depth = 0;

	init_color = (o.mycolor==1)?2:1;

	//position list contain corner,return directly
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
		o.play(o.mycolor,ans[i].first,ans[i].second);
		temp_value = eval_node(depth+1,init_color,b_value);
		std::cerr<<"move["<<i<<"] value:"<<temp_value<<endl;
		o.init(o.mycolor,ori_string);

		if(temp_value == 0x40000 || temp_value == -0x40000)
			break;

		if(temp_value == 0x50000){ //op can't move
			std::cerr<<"move["<<i<<"],op can't move in serch end...\n";
			return ans[i];
		}

		if(b_value < temp_value){
			b_value = temp_value;
			b_index = i;
		}
	}	
	std::cerr<<"root--->choose move["<<b_index<<"]\n\n";
	return ans[b_index];
}



int othello_ai::eval_node(int depth,int color,int cmp_val){
	if(get_time()>1990){ //超时
		std::cerr<<"timeout..\n";
		return (depth%2==0)?0x40000:-0x40000;
	}
	if(depth == glb_depth){  // 达到指定的搜索深度
		return get_eval(o.tostring(),color);
	}

	int best_value,temp_value;
	int op_col;
	int end_op;
	vector<pair<int,int> >  ans; 
	int chd_size;
	string ori_string; //保持起初的局面

	op_col = (color==1)?2:1;
	best_value = (depth%2==0)?-0x40000:0x40000;
	ori_string = o.tostring();

	if(!o.canmove(color)){  //color颜色方无法走
		if(!o.canmove(op_col)){  //end								
			if(o.count(o.mycolor) >= 128){
				std::cerr<<"get end,max win\n";
				return 0x50000;     //root choose num				
			}
			else{
				std::cerr<<"get end,min win\n";
				if(depth == 1)
					return -0x80000;
				else
					return (depth%2==0)?-0x80000:0x80000;
			}								
		} else {  //not end
			return get_eval(o.tostring(),color);
		}
	}

	ans = o.allmove(color);  //获取所有可下位置列表
	chd_size = ans.size();

	for(int i=0;i<chd_size;i++){
		o.play(color,ans[i].first,ans[i].second);
		temp_value = eval_node(depth+1,op_col,best_value);
		o.init(o.mycolor,ori_string);

		if(temp_value == 0x40000 || temp_value == -0x40000){ //child node timeout
			std::cerr<<"test2\n";
			return 0x40000;
		}

		if(temp_value == 0x50000)  //op can't move
			return 0x50000;	
		
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
int othello_ai::get_eval(string s,int color)
{
	int be_value = 0; 		
	double score;
	int p_val;	
	int x,y;
	int op_color = (o.mycolor==1)?2:1;

	//计算开局时的估价函数
	if(o.count(0) > 212){		
		// p_val = 0;
		// for(int i=0; i<16; i++)
		// 	for(int j=0; j<16; j++)
		// 		if(o.is(o.mycolor,i,j))
		// 			p_val--;
		// 		else if(o.is(op_color,i,j))
		// 			p_val++;
		// score = p_val;
		int size1=0, size2=0;
		int mv_val = 0;
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
		score = mv_val;
	}
	//中间阶段的估价函数
	else if(o.count(0) > 16){
		score = mid_score();
	}
	//最后阶段
	else{
		p_val = 0;
		for(int i=0; i<16; i++)
			for(int j=0; j<16; j++)
				if(o.is(o.mycolor,i,j))
					p_val++;
				else if(o.is(op_color,i,j))
					p_val--;
		score = p_val;
	}		

	return score;
}

int othello_ai::value(int x,int y)
{
	int i,j;	
	int val[16][16] = {
		{40,0,10,10,10,8,8,8},
		{0,0,-2,-2,-2,-2,-2,-2},
		{10,-2,6,6,6,6,6,6},
		{10,-2,6,3,3,3,3,3},
		{10,-2,6,3,4,4,4,4},
		{8,-2,6,3,4,2,2,2},
		{8,-2,6,3,4,2,-2,-2},
		{8,-2,6,3,4,2,-2,-4}
		// {40,-12,28,24,20,16,12,8},
		// {-12,-16,-12,-8,-6,-2,1,6},
		// {28,-12,20,16,12,10,8,4},
		// {24,-8,16,12,10,8,6,3},
		// {20,-6,12,10,8,6,4,2},
		// {16,-2,10,8,6,4,2,1},
		// {12,1,8,6,4,2,1,0},
		// {8,6,4,3,2,1,0,-1}
	};	

	for(i=0;i<8;i++)
		for(j=8; j<16; j++)
			val[i][j] = val[i][15-j];

	for(i=8; i<16; i++)
		for(j=0; j<8; j++)
			val[i][j] = val[15-i][j];

	for(i=8; i<16; i++)
		for(j=8; j<16; j++)
			val[i][j] = val[15-i][15-j];
	
		return val[x][y];	
}

int othello_ai::begin_value(int x,int y){
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

double othello_ai::mid_score()
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

	int count = 0;
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
			count = 0;
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

	// pown_val = 100.0*(my_pown-op_pown)/ (my_pown + op_pown);

	// num of pown outside
	if(my_front_pown > op_front_pown)
		front_val = -100.0*my_front_pown/(my_front_pown + op_front_pown);
	else if(my_front_pown < op_front_pown)
		front_val = 100.0 * op_front_pown/ (my_front_pown + op_front_pown);
	else 
		front_val = 0;
	// front_val = 100.0 * (my_front_pown - op_front_pown)/(my_front_pown + op_front_pown);

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
	// mv_val = 100.0 * (size1 - size2) / (size1 + size2);
	score = 10*pown_val + 800 * corner_val + 400 * side_val + 79 * mv_val + 80*front_val + 11 * p_val;
	return score;
}


