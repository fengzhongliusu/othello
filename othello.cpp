#include "othello16.h"
#include <stdlib.h>
#include <iostream>
#include <time.h>
#include "gamti.h"

class othello_ai{
	othello16 o;//瀹炰緥鍖杘thello16绫�		
	int pari;
	public:
	void init(int color, string s);
	void move(int color, int x, int y);
	pair<int, int> get();
	int get_eval(string s,int g);   //绠楀嚭缁欏畾灞€闈㈢殑浼颁环鍊�
	int value(int x,int y);   //鑾峰彇姣忎竴涓偣鐨勪及浠峰€�
	int eval_node(int depth,int color,int cmp_val);  //杩斿洖鍊掗€€鍊�
	int jdg_side(int x,int y);
	vector<pair<int, int> > m_order(vector<pair<int, int> > ans,int depth);
};

void othello_ai::init(int color, string s){
	std::cerr<<"my color: "<<color<<endl;
	o.init(color, s);//璁﹕dk鍒濆鍖栧眬闈�
}

void othello_ai::move(int color, int x, int y){
	std::cerr<<"move: <"<<color<<","<<x<<","<<y<<">\n";
	o.play(color, x, y);//鍛婅瘔sdk钀藉瓙鎯呭喌,鏀瑰彉灞€闈�
}

pair<int, int> othello_ai::get(){
	int init_color, depth, ans_size;
	int b_index, b_value, temp_value;
	int cmp_val;
	string ori_string;
	vector<pair<int,int> > ans;
	vector<pair<int,int> > order_ans;
	
	pari = (o.count(0)%2==0)?1:0;	
	ori_string = o.tostring(); //淇濆瓨鍒濆灞€闈�
	b_index = 0;
	b_value = -0x4000000;
   	ans = o.allmove(o.mycolor);
	ans_size = ans.size();
	depth = 0;

	std::cerr<<"----left:"<<o.count(0)<<endl;
	order_ans = m_order(ans,depth);
	std::cerr<<"****left:"<<o.count(0)<<endl;

	init_color = (o.mycolor==1)?2:1;

	for(int i=0; i<ans_size; i++)
		if((ans[i].first == 0 && ans[i].second == 0)
			|| (ans[i].first == 0 && ans[i].second == 15)
			|| (ans[i].first == 15 && ans[i].second == 0)
			|| (ans[i].first == 15 && ans[i].second == 15)
			)
		{
			std::cerr<<"get corner:<"<<ans[i].first<<","<<ans[i].second<<">\n";
			return ans[i];
		}			


	std::cerr<<"\nroot--->move size: "<<ans_size<<endl;		
	for(int i=0; i<ans_size; i++){

		o.play(o.mycolor,order_ans[i].first,order_ans[i].second);
		temp_value = eval_node(depth+1,init_color,b_value);
		std::cerr<<"move["<<i<<"] value:"<<temp_value<<endl;
		o.init(o.mycolor,ori_string);

		if(temp_value == 0x4000000)
			break;

		if(temp_value == 0x888888)	//win		
			return order_ans[i];				

		if(temp_value == -0x888888 || temp_value == -0x999999)  //lose or even to odd
			continue;

		if(temp_value == 0x999999){  //odd to even
			std::cerr<<"pari:<<"<<order_ans[i].first<<","<<order_ans[i].second<<">>\n";			
			if(jdg_side(order_ans[i].first,order_ans[i].second)==1)
				continue;
			else{
				std::cerr<<"turn over pari\n";
				return order_ans[i];
			}
		}  

		if(b_value < temp_value){
			b_value = temp_value;
			b_index = i;
		}
	}	
	std::cerr<<"root--->choose move["<<b_index<<"]\n\n";
	return order_ans[b_index];
}



int othello_ai::eval_node(int depth,int color,int cmp_val){
	if(get_time()>1800){ //瓒呮椂
		std::cerr<<"timeout..\n";
		return 0x4000000;
	}	

	int best_value,temp_value;
	int op_col;
	int end_op;
	vector<pair<int,int> >  ans; 
	vector<pair<int,int> >  order_ans; 
	int chd_size;
	string ori_string; //淇濇寔璧峰垵鐨勫眬闈�

	op_col = (color==1)?2:1;
	best_value = (depth%2==0)?-0x4000000:0x4000000;
	ori_string = o.tostring();

	if(!o.canmove(color)){  //color棰滆壊鏂规棤娉曡蛋		
		if(!o.canmove(op_col)){
			if(color == o.mycolor){
				if(o.count(o.mycolor) >= o.count(op_col)){
					std::cerr<<"depth:"<<depth<<"get win"<<endl;
					return 0x888888;
				}
				else{
					std::cerr<<"depth:"<<depth<<"get lose"<<endl;
					return -0x888888;
				}
			} else {				
				if(o.count(o.mycolor) >= o.count(color)){
					std::cerr<<"depth:"<<depth<<"get win"<<endl;
					return 0x888888;
				}
				else{
					std::cerr<<"depth:"<<depth<<"get lose"<<endl;
					return -0x888888;
				}
			}
		}
		else{				
			if(pari == 1){				
				return 0x999999;
			}
			else
				return -0x999999;
		}		
	}

	if(depth == 4){  // 杈惧埌鎸囧畾鐨勬悳绱㈡繁搴�				
		return get_eval(o.tostring(),0);		
	}

	ans = o.allmove(color);  //鑾峰彇鎵€鏈夊彲涓嬩綅缃垪琛�
	chd_size = ans.size();	

	for(int i=0;i<chd_size;i++){
		o.play(color,ans[i].first,ans[i].second);
		temp_value = eval_node(depth+1,op_col,best_value);
		o.init(o.mycolor,ori_string);

		if(temp_value == 0x4000000){ //child node timeout			
			return 0x4000000;
		}

		if(temp_value == 0x888888 || temp_value == -0x888888 || temp_value == 0x999999 || temp_value ==-0x999999)
			return temp_value;
		
		if(depth % 2 == 0 && cmp_val <= temp_value){   //beta鍑忔灊			        	
			return temp_value;
		}
		if(depth % 2 != 0 && cmp_val >= temp_value){   //alpha鍑忔灊			        	
			return temp_value;
		}

		if(depth % 2 ==0 && best_value < temp_value)
			best_value = temp_value;
		else if(depth % 2 != 0 && best_value > temp_value)
			best_value = temp_value;
	}
	return best_value;
}


int othello_ai::get_eval(string s,int g)
{	
	int my_pown=0,op_pown=0;
	int pown_3 = 0;
	int p_val = 0;
	int my_corner = 0, op_corner = 0;
	int my_c_s = 0, op_c_s = 0;
	int my_s_3 = 0, op_s_3 = 0;
	int my_front_pown=0, op_front_pown=0;
	int round_x[] = {-1, -1, 0, 1, 1, 1, 0, -1};
	int round_y[] = {0, 1, 1, 1, 0, -1, -1, -1};
	int op_color = (o.mycolor==1)?2:1;
	int x,y;	
	double score;
	double pown_val, mv_val, front_val, corner_val, side_val;


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

	side_val = -12.5 * my_c_s;   //op get all the bottom line

	//endgame		
	if(o.count(0) <= 12) {   
		for(int i=0; i<16; i++)
			for(int j=0; j<16; j++)
				if(o.is(o.mycolor,i,j))
					p_val++;
				else if(o.is(op_color,i,j))
					p_val--;
		score = 10000*corner_val + 6000*side_val + 200 * p_val;
		return score;		
	}
	

	//count pown
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

	//corner side side
	if(o.is(0,0,0) && o.is(0,0,1) && o.is(0,1,0) && o.is(0,1,1)){
		if(o.is(o.mycolor,0,2)) my_s_3++;
		else if(o.is(op_color,0,2)) op_s_3++;
		if(o.is(o.mycolor,1,2)) my_s_3++;
		else if(o.is(op_color,1,2)) op_s_3++;
		if(o.is(o.mycolor,2,2)) my_s_3++;
		else if(o.is(op_color,2,2)) op_s_3++;	
		if(o.is(o.mycolor,2,0)) my_s_3++;
		else if(o.is(op_color,2,0)) op_s_3++;	
		if(o.is(o.mycolor,2,1)) my_s_3++;
		else if(o.is(op_color,2,1)) op_s_3++;	
	}
	if(o.is(0,0,15) && o.is(0,0,14) && o.is(0,1,14) && o.is(0,1,15)){
		if(o.is(o.mycolor,0,13)) my_s_3++;
		else if(o.is(op_color,0,13)) op_s_3++;
		if(o.is(o.mycolor,1,13)) my_s_3++;
		else if(o.is(op_color,1,13)) op_s_3++;
		if(o.is(o.mycolor,2,13)) my_s_3++;
		else if(o.is(op_color,2,13)) op_s_3++;
		if(o.is(o.mycolor,2,14)) my_s_3++;
		else if(o.is(op_color,2,14)) op_s_3++;
		if(o.is(o.mycolor,2,15)) my_s_3++;
		else if(o.is(op_color,2,15)) op_s_3++;
	}
	if(o.is(0,15,0) && o.is(0,15,1) && o.is(0,14,0) && o.is(0,14,1)){
		if(o.is(o.mycolor,13,0)) my_s_3++;
		else if(o.is(op_color,13,0)) op_s_3++;
		if(o.is(o.mycolor,13,1)) my_s_3++;
		else if(o.is(op_color,13,1)) op_s_3++;
		if(o.is(o.mycolor,13,2)) my_s_3++;
		else if(o.is(op_color,13,2)) op_s_3++;	
		if(o.is(o.mycolor,14,2)) my_s_3++;
		else if(o.is(op_color,14,2)) op_s_3++;	
		if(o.is(o.mycolor,15,2)) my_s_3++;
		else if(o.is(op_color,15,2)) op_s_3++;	
	}
	if(o.is(0,14,14) && o.is(0,14,15) && o.is(0,15,14) && o.is(0,15,15)){
		if(o.is(o.mycolor,15,13)) my_s_3++;
		else if(o.is(op_color,15,13)) op_s_3++;
		if(o.is(o.mycolor,14,13)) my_s_3++;
		else if(o.is(op_color,14,13)) op_s_3++;
		if(o.is(o.mycolor,13,13)) my_s_3++;
		else if(o.is(op_color,13,13)) op_s_3++;
		if(o.is(o.mycolor,13,14)) my_s_3++;
		else if(o.is(op_color,13,14)) op_s_3++;
		if(o.is(o.mycolor,13,15)) my_s_3++;
		else if(o.is(op_color,13,15)) op_s_3++;
	}
	pown_3 = 10 * (my_s_3 - op_s_3);


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

   if(g == 0){
   		if(o.count(0) > 226)   //begin game
			score = 10000 * corner_val + 6000 * side_val + 80 * mv_val;	
		else      //mid game
			score = 10*pown_val + 10000 * corner_val + 6000 * side_val + 110 * mv_val + 90*front_val + 10 * p_val;			
   }
   else	
   		score = 8*pown_val + 10000 * corner_val + 6000 * side_val + 300 * mv_val + 400*front_val + 8 * p_val;	
	return score;
}

int othello_ai::value(int x,int y)
{
	int i,j;	
	int val[16][16] = {
		{40,0,20,20,20,13,12,12},
		{0,0,2,2,2,2,2,2},
		{20,2,6,6,6,6,6,6},
		{20,2,6,3,3,3,3,3},
		{20,2,6,3,4,4,4,4},
		{13,2,6,3,4,2,2,2},
		{12,2,6,3,4,2,1,1},
		{12,2,6,3,4,2,1,0}		
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
	if(get_time() > 1800)
		return ans;
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
			temp = make_pair(i,get_eval(o.tostring(),1));
			c_value.push_back(temp);
		}
		else{
			temp = make_pair(i,-1*get_eval(o.tostring(),1));
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
	return new_ans;
}

int othello_ai::jdg_side(int x,int y)
{
	if((x==0 && y==1) || (x==1 && y==1) || (x==1 && y==0)){
		if(o.is(0,0,0))
			return 1;
	}
	if((x==0 && y==14) || (x==1 && y==14) || (x==1 && y==15)){
		if(o.is(0,0,15))
			return 1;
	}
	if((x==15 && y==1) || (x==14 && y==1) || (x==14 && y==0)){
		if(o.is(0,15,0))
			return 1;
	}
	if((x==15 && y==14) || (x==14 && y==14) || (x==14 && y==15)){
		if(o.is(0,15,15))
			return 1;
	}
	return 0;
}