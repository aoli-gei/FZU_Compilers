/*************************************************************************
	> File Name: LR.cpp
	  > Author: Netcan
	  > Blog: http://www.netcan666.com
	  > Mail: 1469709759@qq.com
	  > Created Time: 2016-10-17 一 21:03:17 CST

                                                    __----~~~~~~~~~~~------___
                                   .  .   ~~//====......          __--~ ~~
                   -.            \_|//     |||\\  ~~~~~~::::... /~
                ___-==_       _-~o~  \/    |||  \\            _/~~-
        __---~~~.==~||\=_    -_--~/_-~|-   |\\   \\        _/~
    _-~~     .=~    |  \\-_    '-~7  /-   /  ||    \      /
  .~       .~       |   \\ -_    /  /-   /   ||      \   /
 /  ____  /         |     \\ ~-_/  /|- _/   .||       \ /
 |~~    ~~|--~~~~--_ \     ~==-/   | \~--===~~        .\
          '         ~-|      /|    |-~\~~       __--~~
                      |-~~-_/ |    |   ~\_   _-~            /\
                           /  \     \__   \/~                \__
                       _--~ _/ | .-~~____--~-/                  ~~==.
                      ((->/~   '.|||' -_|    ~~-/ ,              . _||
                                 -_     ~\      ~~---l__i__i__i--~~_/
                                 _-~-__   ~)  \--______________--~~
                               //.-~~~-~_--~- |-------~~~~~~~~
                                      //.-~~~--\
						神兽保佑，代码无BUG!
 ************************************************************************/

#include "LR.h"
set<char>Item::Vn; // 全局静态变量
set<char>Item::Vt;
set<char>Item::Symbol;
const char*LR::actionStatStr[] = {
	"acc",
	"s",
	"r"
};

string Prod::displayStr() const{
	string p = string(1, noTerminal) + "->" + right.c_str();	//p放的整个产生式，c_str返回一个正规c字符串指针常量，内容与string相同
	int i = 0;
	for(const auto& c:additionalVt)	
		if(c != '#') p += string(1, i++==0?',':'|') + c; // 输出搜索符
	if(additionalVt.find('#') != additionalVt.end()) p += string(1, i++==0?',':'|') + "#";	//#（空）一直放在末尾
	return p;
}	//使用#表示文法输入完成

Prod::Prod(const string &in) {
	// printf("%s\n", in.c_str());
	noTerminal = in[0];	//非终结符就是字符串in的第一个字符
	right = cut(in, 3, in.length()); // 因为A->X1X2X3X4，所以产生式右部从3开始
}

void Item::add(const string &prod) {	//向项目集中添加产生式,prod是字符串，放着产生式
	if(prod.length() < 4) return;	//小于四，不是一个合理的产生式
	char noTerminal;
	if(Prod::cut(prod, 1, 3) == "->" && (isupper(prod[0]))) //如果中间是->,而且首字母大写，则非终结符为A A->...则noTerminal = A
		noTerminal = prod[0];
	else return;
	//上面这个判断产生式是否合法
	for(unsigned int i=0; i<prod.length(); ++i) { // 提取终结符、非终结符
		char c = prod[i];
		if(isupper(c)) {	//如果是非终结符（大写）
			Vn.insert(c);
			Symbol.insert(c);
		}
		else if(c!='|' && !(c=='-' && prod[i+1] == '>' && ++i)) {	//其余的都是终结符
			Vt.insert(c);
			Symbol.insert(c);
		}
	}
	//上面这个将产生式中所含有的非终结符和终结符提取出来
	for(unsigned int i=3; i<prod.length(); ++i) { // 提取候选式
		unsigned int j;
		for(j=i+1; j<prod.length() && prod[j] != '|'; ++j);	//j现在指到了产生式的最后，？为什么要!=|，是不是因为只看一个字符，后面的|都是另外一个函数弄进去的
		Prod p = Prod(string(1, noTerminal)+"->"+Prod::cut(prod, i, j));
		if(find(prods.begin(), prods.end(), p) == prods.end()) // 去重
			prods.push_back(p);
		i = j;
	}
}

void Item::display() const {	//打印项目集中的所有产生式
	for(const auto& prod: prods)
		cout << prod.displayStr() << endl;
}

void LR::add(const string &s) {	//向文法中添加产生式
	G.add(s);
}

void LR::loadStr(const string &in) {	//载入输入串
	inStr.push_back('#');	//模拟手画，#表示规约结束（$）放到栈底
	status.push_back(0);	//状态栈进0（从状态0开始）
	for(int i = in.length() - 1; i>=0; --i)	//把全部字符倒着输入
		inStr.push_back(in[i]);
}

void LR::showStrStack() {	//把输入串倒着输出出来？（#在右边）
	for(vector<char>::reverse_iterator it = inStr.rbegin(); it != inStr.rend(); ++it)
		printf("%c", *it);
}


void LR::showStatusStack() {	//输出状态栈
	for(vector<int>::iterator it = status.begin(); it != status.end(); ++it) {
		if(*it < 10) printf(" %d ", *it);
		else printf(" <span class='underline'>%d</span> ", *it);//大于9，输出带下划线以免误解
	}
}

void LR::showParseStack() {		//输出分析栈（正序）
	for(vector<char>::iterator it = parse.begin(); it != parse.end(); ++it)
		printf("%c", *it);
}


void LR::parser() {
	printf("\"parser\": [");

	bool success = false;
	int step = 0;
	while(! success) {
		// printf("%d\t", step);
		printf("%s{", step == 0?"\n":"\n, ");
		printf("\"statusStack\": \"");
		showStatusStack();
		printf("\", \"parseStack\": \"");
		showParseStack();
		printf("\", \"inStrStack\": \"");
		showStrStack();
		printf("\", \"action\": ");

		int sTop = status.size() - 1; // 栈顶
		int iTop = inStr.size() - 1;
		pair<int, char> p = make_pair(status[sTop], inStr[iTop]);
		if(ACTION.find(p) == ACTION.end())  // 出错！
			break;
	 	pair<actionStat, int> act = ACTION[p];
		if(act.first == SHIFT) { // 移进
			printf("\"SHIFT\"}");

			status.push_back(act.second);
			parse.push_back(inStr[iTop]);
			inStr.pop_back();
		} else if(act.first == REDUCE){
			Prod p = G.prods[act.second];
			printf("\"REDUCE %c->%s\"}", p.noTerminal, p.right.c_str());

			if(p.right != "@") // 空串，无需出栈，直接规约
				for(unsigned i=0; i<p.right.size(); ++i) {
					status.pop_back();
					parse.pop_back();
				}
			parse.push_back(p.noTerminal);
			status.push_back(GOTO[make_pair(status[status.size() - 1], p.noTerminal)]);
		} else if(act.first == ACCEPT) {
			success = true;
			printf("\"ACCEPT\"}");
		}
		++step;
	}
	if(! success)
		printf("\"ERROR\"}\n");

	printf("]\n");
}

Item LR::closure(Item I) {	//求项目的闭包
	if(I.prods.size() == 0) return I;	//空的，直接返回
	// size_t size = 0;
	// while(size != I.prods.size()) { // 当没有项目加入的时候
		// size = I.prods.size();
		// for(auto &prod: I.prods) { // bug
	for(size_t i = 0; i < I.prods.size(); ++i) { // 枚举I的产生式
		// puts("======================");
		// printf("i=%ld, size:%ld\n", i, I.prods.size());
		// I.display();
		Prod prod = I.prods[i];
		unsigned long pointLoc = 0;	//放的是.的位置
		// printf("prod: %s\n", prod.displayStr().c_str());
		if((pointLoc = prod.right.find('.')) != string::npos && pointLoc != prod.right.length() - 1) { // 找到.，A->a.Bc,d，而且这个.不在最后
			char X = prod.right[pointLoc + 1];	//拿出.后面的第一个字符
			if(G.Vt.find(X) != G.Vt.end()) { 	//拿出的这个是终结符
				if(X == '@') // 这个终结符是@（空？）@特殊处理
					swap(I.prods[i].right[pointLoc], I.prods[i].right[pointLoc + 1]);//直接交换一下
				continue;//.后面是终结符，直接结束，去处理下一个产生式
			}	

			string f = Prod::cut(prod.right, pointLoc+2, prod.right.length());	//把.右边的字母的后面拉出来
			// prod.display();
			// printf("f: %s\n", f.c_str());
			// printf("====================");
			set<char> ff = {};
			for(const auto& c: prod.additionalVt) {//c里面放的搜索符
				set<char> fs = first(f + c);//c里面放的是搜索符
				ff.insert(fs.begin(), fs.end());
			}	//这个应该是把当前扩展的字母后面的first集合给求出来

			// if(ff == set<char>{'#'} && prod.noTerminal != EXTENSION_NOTERMINAL)  // 只含#，那么把Follow集加进来
			// ff.insert(FOLLOW[prod.noTerminal].begin(), FOLLOW[prod.noTerminal].end());


			for(vector<Prod>::iterator it = G.prods.begin(); it != G.prods.end(); ++it) {
				if(*it == X) { // 找到产生式，==经过了重载，所以可以直接用
					Prod p = *it;
					if(p.right[0] == '@') { // 特殊处理.@ => @.
						p.right = '.' + p.right;
						swap(p.right[0], p.right[1]);
					} else
						p.right = '.' + p.right;

					vector<Prod>::iterator Iit = find(I.prods.begin(), I.prods.end(), p); // 找I中是否存在产生式
					if(Iit != I.prods.end())  // 找到
						Iit->additionalVt.insert(ff.begin(), ff.end());
					else {
						p.additionalVt.insert(ff.begin(), ff.end());
						I.prods.push_back(p);	//没有这个产生式，那么就添加进去
					}
				}
			}
		}
	}
	// }
	return I;
}
//上面代码是求项目集闭包：遍历整个状态集，取出.后面是非终结符的，
//然后计算他的first集，搜索文法中的产生式，将.添加进去。
//然后再查看项目集闭包，有没有这个产生式，有的话就把first集加进去，没有就直接插入

Item LR::Goto(const Item& I, char X) {	//求I经过X到达的项目集
	Item J;
	if(I.prods.size() == 0 || X == '@') return J; // 项目集为空或者@则返回空项目

	for(const auto& p: I.prods)  {// I中的每个项目
		string right = p.right;
		unsigned long pointLoc = right.find('.');
		if(right[pointLoc + 1] == X) {	//这个产生式可以通过X到达
			swap(right[pointLoc], right[pointLoc + 1]);	//.和X交换位置（匹配成功）
			J.prods.push_back(Prod(p.noTerminal, right, p.additionalVt));	//把p的产生式，非终结符，搜索符丢进新的项目集J中
		}
	}
	return closure(J);//对J求闭包，返回一个新的项目集
}
//这个函数是来求一个项目集I，遇到X，可以到达的下一个项目集（状态），所以返回的是一个闭包closure


void LR::items() // 求项目集状态机DFA！!
{
	Item initial;
	initial.prods.push_back(Prod(EXTENSION_NOTERMINAL, '.' + string(1, G.prods[0].noTerminal), {'#'})); // 初值，^->.S,#
	C.push_back(closure(initial)); // 置C初值
	size_tXQ size = 0;
	while(size != C.size()) { // 当没有项目集加入C中
		size = C.size();
		// for(auto &I: C) { // C的每个项目集，这样写有坑！！
		for(unsigned int i=0; i<C.size(); ++i) { // C的每个项目集
			Item I = C[i];
			for(const auto &X: G.Symbol) { // 每个文法Vt符号X
				Item next = Goto(I, X);		//对每一个符号X，遍历C中包含的状态集合，跑一下goto函数，跑一次goto函数都会弄一次闭包~
											//求闭包的时候就会看是不是这个闭包以前有，所以不会出现重复的相同闭包
				if(next.prods.size() != 0) { // 不为空
					auto it = find(C.begin(), C.end(), next);	//看看C里面有没有这个goto生成的状态，返回这个状态的坐标？
					if(it != C.end()) { // 找到
						GOTO[make_pair(i, X)] = it - C.begin();	//这个路径记录下来
					} else {
						C.push_back(next);		//这个是一个新的状态，就插入到末尾
						GOTO[make_pair(i, X)] = C.size()-1;		//记录下坐标
					}
				}
			}
		}
	}
	// printf("size: %ld\n", C.size());
	// for(const auto& Item: C) {
		// printf("===%ld===\n", &Item - &C[0]);
		// Item.display();
	// }
}

void LR::build() { // 构造Action、GOTO表
	// follow();
	items();//求DFA
	for(unsigned int i=0; i<C.size(); ++i) { // 逐个项目集
		const Item & item = C[i];
		for(const auto& prod: item.prods) { // 逐个项目
			unsigned long pointLoc = prod.right.find('.');
			if(pointLoc < prod.right.length() - 1) { // 不是最后一个
				char X = prod.right[pointLoc + 1];
				if(G.Vt.find(X) != G.Vt.end() && GOTO.find(make_pair(i, X)) != GOTO.end()) { // 终结符
					int j = GOTO[make_pair(i, X)];
					ACTION[make_pair(i, X)] = make_pair(SHIFT, j);
				}
			} else {
				if(prod == Prod(EXTENSION_NOTERMINAL, string(1, G.prods[0].noTerminal)+'.', {}) && prod.additionalVt == set<char>({'#'}))  // S'->S.,# acction[i, #] = acc
					ACTION[make_pair(i, '#')] = make_pair(ACCEPT, 0);
				else if(prod.noTerminal != EXTENSION_NOTERMINAL) {
					string right = prod.right;
					right.erase(pointLoc, 1); // 删除.
					for(const auto& X: prod.additionalVt) { // A->a.,b，枚举b
						vector<Prod>::iterator it = find(G.prods.begin(), G.prods.end(), Prod(prod.noTerminal, right, set<char>{}));
						if(it != G.prods.end())  // 找到了
							ACTION[make_pair(i, X)] = make_pair(REDUCE, it - G.prods.begin());
					}
				}
			}
		}
	}
	if(G.Vt.find('@') != G.Vt.end()) { // 删除@，移进#
		G.Vt.erase(G.Vt.find('@'));
		G.Symbol.erase(G.Symbol.find('@'));
	}
	G.Vt.insert('#');
	G.Symbol.insert('#');
}

void LR::showTable() {
	printf("\"parseTable\": {\n");
	printf("\"Vt\": [");

	for(const auto & X: G.Vt) {
		if(X != '#')
			printf("\"%c\", ", X);
	}
	printf("\"%c\"", '#'); // #放到最后一列显示，美观
	printf("],\n\"Vn\": [");

	int firstComma = 0; // 处理第一个逗号
	for(const auto & X: G.Vn)
		printf("%s\"%c\"", firstComma++ == 0?" ":", ",X);
	printf("],\n\"Body\": [\n");

	for(unsigned int i=0; i<C.size(); ++i) {
		printf("%s[", i == 0?" ":", ");
		int firstComma = 0;
		for(const auto & X: G.Vt) {
			if(X != '#') {
				pair<int, char> p = make_pair(i, X);
				if(ACTION.find(p) != ACTION.end()) {
					pair<actionStat, int> res = ACTION[p];
					printf("%s\"%s%d\"", firstComma++ == 0?" ":", ", actionStatStr[res.first], res.second);
				}
				else printf("%s\"\"", firstComma++ == 0?" ":", ");
			}
		}

		pair<int, char> p = make_pair(i, '#');
		if(ACTION.find(p) != ACTION.end()) {
			pair<actionStat, int> res = ACTION[p];
			printf("%s\"%s", firstComma++ == 0?" ":", ", actionStatStr[res.first]);
			if(res.first != ACCEPT)
				printf("%d\"", res.second);
			else printf("\"");
		}
		else printf("%s\"\"", firstComma++ == 0?" ":", ");

		for(const auto & X: G.Vn) {
			pair<int, char> p = make_pair(i, X);
			if(GOTO.find(p) != GOTO.end())
				printf("%s%d", firstComma++ == 0?" ":", ", GOTO[make_pair(i, X)]);
			else printf("%s\"\"", firstComma++ == 0?" ":", ");
		}
		puts("]");
	}

	printf("]}\n");
}

set<char> LR::first(const string &s) { // s不为产生式！
	if(s.length() == 0)
		return set<char>({'@'});	//S是空的，说明后面没东西，所以直接返回空（@）
	else if(s.length() == 1) {	//S只有一个字符
		if(G.Vt.find(s[0]) != G.Vt.end() || s[0] == '#') // 终结符
			return set<char>({s[0]});	//s只有一个字符，而且是终结符或者是#（结束符还是空？忘记了iai）直接返回当前字符
		else
			if(FIRST[s[0]].size() != 0) return FIRST[s[0]];	//FIRST是一个map（map<char, set<char> > FIRST），如果这个非终结符的FIRST集不为空，直接返回
			else {
				for(vector<Prod>::iterator it = G.prods.begin(); it != G.prods.end(); ++it)	//遍历文法G的所有产生式
					if(*it == s[0]) {	//有找到it作为产生式左部的产生式，这里产生式重载了
						// 防止直接左递归
						size_t xPos = it->right.find(it->noTerminal);	
						// printf("prod: %s right: %s\n", it->displayStr().c_str(), it->right.c_str());
						if(xPos != string::npos) { // 找到X->aXb
							if(xPos == 0) continue; // X->Xb如果X在第一个，跳过
							else { // X->aXb
								string a = Prod::cut(it->right, 0, xPos);	
								if(first(a) == set<char>{'@'}) continue;	//把X左边的东西拿出来，在对他们求first集，如果成空了（@）跳出
							}
						}
						set<char> f = first(it->right);		//新建一个集合，插入first集
						FIRST[s[0]].insert(f.begin(), f.end());		//插入当前非终结符的first集合
					}
				// printf("first(%s) = ", s.c_str());
				// for(auto c: FIRST[s[0]])
					// printf("%c, ", c);
				// puts("");

				return FIRST[s[0]];
			}
	} else { // first(X1X2X3X4)...
		set<char> ret;
		for(unsigned int i=0; i<s.length(); ++i) {
			set<char> f = first(string(1, s[i])); // 逐个符号求first(Xi)集
			if(f.find('@') != f.end() && s.length() - 1 != i) { // 发现@
				f.erase(f.find('@')); // 减去@
				ret.insert(f.begin(), f.end()); // 放入first集合
			} else { // 无@或者最后一个Xi，则不需要求下去了
				ret.insert(f.begin(), f.end());
				break;
			}
		}
		return ret;
	}
}


void LR::follow() {
	FOLLOW[G.prods[0].noTerminal].insert('#'); // 开始符号放'#'
	for(auto pp: G.prods) { // 直到follow(X)不在增大
		unsigned int size = 0;
		while(size != FOLLOW[pp.noTerminal].size()) {
			size = FOLLOW[pp.noTerminal].size();
			for(auto prod: G.prods) { // 求出所有非终结符的follow集合
				char X = prod.noTerminal;
				for(auto p: G.prods) {// 求出X的follow集合
					string s = p.right;
					unsigned long loc = 0;
					if((loc = s.find(X)) != string::npos) { // 找到非终结符X
						set<char> f = first(string(s.begin() + loc + 1, s.end())); // 求first(b)
						FOLLOW[X].insert(f.begin(), f.end()); // 加入到follow(X)中
						if(f.find('@') != f.end()) {// 找到@
							FOLLOW[X].erase(FOLLOW[X].find('@')); // 删除@
							set<char> fw = FOLLOW[p.noTerminal]; // 把follow(A)放入follow(X)
							FOLLOW[X].insert(fw.begin(), fw.end());
						}
					}
				}
			}
		}
	}
}

void LR::debug() {
	string in;
	while(cin >> in && in != "#")
		add(in);

	puts("=====Proj:======");
	for(auto pro: G.prods)
		printf("%s\n", (string(1, pro.noTerminal) + "->" + pro.right).c_str());
	puts("=====Vt:======");
	for(auto vt: G.Vt)
		printf("%c, ", vt);
	puts("\n=====Vn:======");
	for(auto vn: G.Vn)
		printf("%c, ", vn);
	// puts("\n=====FIRST:======");
	// set<char> f = first("S");
	// for(auto c: f)
		// printf("%c, ", c);
	puts("");
	// puts("\n=====Item:======");
	// Item I;
	// I.prods.push_back(Prod('B', "b.B", {'b', 'a'}));
	// I = closure(I);
	// for(auto prod: I.prods)
		// prod.display();
	build();
	showGrammar();

	in = "";
	cin >> in;
	if(in == "#" || in.size() == 0) return;
	loadStr(in);
	parser();
	puts("");
}

void LR::showGrammar() {
	printf("\"Grammar\": [\n");
	for(const auto & p: G.prods)
		printf("%s\"%c->%s\"", (&p - &G.prods[0]) == 0?" ":", ", p.noTerminal, p.right.c_str());
	printf("\n]\n");
}

string Prod::replaceAll(const string &in, const string from, const string to) {
	size_t replacePos = in.find(from);
	string res = in;
	if(replacePos != string::npos)
		res.replace(replacePos, from.length(), to);
	return res;
}

void changeBackto(string S)
{
	char replace_str[] = {'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'a', 'z', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'p', 'j', '<', '>', '#'};
	string replace_to[] = {"id_lists", "compound_stmt", "optional_stmts", "stmts", "stmt", "expr", "bool", "term", "factor", "id", ".", "begin", "end", ":=", "if", "then", "else", "while", "do", "program", "num", "&lt;", "&gt;","$"};
	for(const auto &ch: S)
	{
		int j=0,find=0;
		for(auto i:replace_str)
		{
			if(ch==i)
			{
				cout<<replace_to[j];
				find=1;
			}
				
			j++;
		}	
		if(!find)
			printf("%c",ch);
		
	}
}

void LR::drawGraph() {
	printf("\"Graph\": {");
	// 图的全部信息
	printf("\"All\": \"");
	printf("digraph all{"
			"node [shape=box style=filled];"
		  );
	// 文法信息
	printf("Grammar[style=rounded label=\\\"Grammar\\n");
	for(const auto &prod: G.prods)
	{
		changeBackto(prod.displayStr());
		printf("\\n");
		// printf("%s\\n", prod.displayStr().c_str());
	}
		
	printf("\\\" ];\\n");

	// 画节点
	for(const auto &I: C) { // 遍历项目集
		int i = &I - &C[0];
		printf("I%d[label=\\\"I%d\\n", i, i);
		for(const auto &p: I.prods) { // 列出项目
			string res = p.displayStr();
			if(res.find('^') != string::npos)
				res = Prod::replaceAll(res, "^", string(1, G.prods[0].noTerminal)+"'");
			changeBackto(res);
			printf("\\n");
			// printf("%s\\n", res.c_str());
		}
		printf("\\\" ];\\n");
	}

	// 画边
	for(const auto &link: GOTO) {
		int i = link.first.first;
		string X = string(1, link.first.second);
		int j = link.second;
		printf("I%d -> I%d[label=\\\"",i,j);
		changeBackto(X);
		printf("\\\"];");
		// printf("I%d -> I%d[label=\\\"%s\\\"];", i, j, X.c_str());
	}
	printf("Grammar -> I0[style=invis];");
	printf("}\"\n, ");
	// 图的简要信息
	printf("\"Simple\": \"");
	printf("digraph simple {"
			"node [shape = circle style=filled];"
		  );
	// 文法信息
	printf("Grammar[shape=box style=rounded label=\\\"Grammar\\n");
	for(const auto &prod: G.prods)
	{
		changeBackto(prod.displayStr());
		printf("\\n");
		// printf("%s\\n", prod.displayStr().c_str());
	}
		
	printf("\\\" ];\\n");

	// 画节点
	for(const auto &I: C) { // 遍历项目集
		int i = &I - &C[0];
		printf("I%d[tooltip=\\\"I%d\\n", i, i);
		for(const auto &p: I.prods) { // 列出项目
			string res = p.displayStr();
			if(res.find('^') != string::npos)
				res = Prod::replaceAll(res, "^", string(1, G.prods[0].noTerminal)+"'");
			// printf("%s\\n", res.c_str());
			changeBackto(res);
			printf("\\n");
		}
		printf("\\\" ];\\n");
	}
	// 画边
	for(const auto &link: GOTO) {
		int i = link.first.first;
		string X = string(1, link.first.second);
		int j = link.second;
		// printf("I%d -> I%d[label=\\\"%s\\\"];", i, j, X.c_str());
		printf("I%d -> I%d[label=\\\"",i,j);
		changeBackto(X);
		printf("\\\"];");
	}
	printf("Grammar -> I0[style=invis];");

	printf("}\"");
	printf("}");

}

void LR::generateDot() {
	printf("digraph all{\n"
			"node [shape=box style=filled];\n"
		  );
	// 文法信息
	printf("Grammar[style=rounded label=\"Grammar\n");
	for(const auto &prod: G.prods)
		printf("%s\n", prod.displayStr().c_str());
	printf("\" ];\n");

	// 画节点
	for(const auto &I: C) { // 遍历项目集
		int i = &I - &C[0];
		printf("I%d[label=\"I%d\n", i, i);
		for(const auto &p: I.prods) { // 列出项目
			string res = p.displayStr();
			if(res.find('^') != string::npos)
				res = Prod::replaceAll(res, "^", string(1, G.prods[0].noTerminal)+"'");
			printf("%s\n", res.c_str());
		}
		printf("\" ];\n");
	}

	// 画边
	for(const auto &link: GOTO) {
		int i = link.first.first;
		string X = string(1, link.first.second);
		int j = link.second;
		printf("I%d -> I%d[label=\"%s\"];\n", i, j, X.c_str());
	}
	printf("Grammar -> I0[style=invis];");
	printf("\n}\n");
}

void LR::run() {
	string in;
	while(cin >> in && in != "#")
		add(in);
	in = "";

	printf("{");
	showGrammar();
	printf(",");

	build();
	showTable();
	printf(",");

	drawGraph();
	printf(",\n");

	cin >> in;
	if(in == "#" || in.size() == 0) return;
	loadStr(in);
	parser();
	printf("}\n");
}

#ifndef _GENERATE_DOT_
int main() {
	LR lr;
	lr.run();

	return 0;
}
#endif


