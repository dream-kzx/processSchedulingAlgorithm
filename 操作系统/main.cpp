#include<Windows.h>
#include<iostream>
#include<ctime>
#include<vector>
#include<string>
#include<algorithm>

using namespace std;


typedef struct job {
	string name;//作业名
	tm arriveTime;//到达时间
	int runTime;//估计运行时间
	int firstRun;//优先级别
	tm inTime;//进入内存时刻
	tm endTime;//运行结束时刻
	int minute;//作业周转时间
	int remainTime;//剩下运行时间
	int flag;//用来标志作业，0未进入内存，1进入内存
	tm justTime;//用于在抢占式中，记录每一次开始运行的时间
	//int n;//在进入内存时，记录之前的位置
}JOB;

typedef struct Course
{
	string name;
	int minute;
}COURSE;


//将string格式转为tm格式
tm StringToDateTime(string str);
//定义JOB结构体在vector中排序，sort为从大到小,优先数越小，优先级别越高,到达时间越早，就越大
bool LessSort1(JOB a, JOB b);
//定义JOB结构体在vector中排序，sort为从小到大,优先数越小，优先级别越高，对优先级别排序
bool LessSort2(JOB a, JOB b);
//先来先服务排序
bool LessSort3(JOB a, JOB b);
//短作业优先排序,加上时间排序
bool LessSort4(JOB a, JOB b);
//短作业优先排序
bool LessSort5(JOB a, JOB b);




//计算作业的运行结束时刻，以及作业周转时间
void takeJOB(JOB* a);
//用于计算周转时间
void takeJOBP(JOB* a);

void displayJOB(JOB a);
void displayJOBR(JOB a);
//用temp替换i位置的元素
void replaceJOB(vector<JOB>* ve, JOB temp, int i);

//递归计算上一次作业结束，和新一个作业进入，中间的时间，其他作业运行的情况
int recursionCompute(vector<JOB>* middle, vector<JOB> *result, vector<JOB> *process, tm arriveTime, tm endTime, time_t remainTime);

//先来先服务
void FCFS(vector<JOB> start, int n);

//短作业优先
void SJF(vector<JOB> start, int n);

//非抢占式优先级调度算法
void Priority(vector<JOB> start, int n);

//抢占式优先级调度算法
void PriorityP(vector<JOB> start, int n);

int main(void) {

	vector<JOB> start;
	JOB work;
	string time1;
	
	int n=0;//创建作业个数

	cout << "请输入创建作业的个数：" << endl;
	cin >> n;
	for (int i = 0; i < n; i++) {
		cout << "请输入作业" << i << "的name" << endl;
		cin >> work.name;
		cout << "请输入作业" << i << "的到达时间" << endl;
		cin >> time1;
		cout << "请输入作业" << i << "的估计运行时间" << endl;
		cin >> work.runTime;
		cout << "请输入作业" << i << "的优先级数" << endl;
		cin >> work.firstRun;

		work.arriveTime = StringToDateTime(time1);
		work.flag = 0;
		start.push_back(work);
	
	}

	//FCFS(start, n);
	//SJF(start, n);

	//Priority(start, n);
	PriorityP(start, n);
	
}


//先来先服务
void FCFS(vector<JOB> start, int n) {
	vector<JOB> result;
	sort(start.begin(), start.end(), LessSort3);
	JOB temp;

	for (int i = 0; i < n; i++) {
		temp = start.at(i);
		if (i == 0) {
			temp.inTime = temp.arriveTime;
			takeJOB(&temp);

			replaceJOB(&start, temp, i);

			result.push_back(temp);
		}
		else {
			time_t lastEndtime = mktime(&(result.at(i - 1).endTime));
			localtime_s(&(temp.inTime), &lastEndtime);
			takeJOB(&temp);

			result.push_back(temp);

		}
		
	}

	for (int i = 0; i < n; i++) {
		displayJOB(result.at(i));
	}
	
}

//短作业优先
void SJF(vector<JOB> start, int n) {
	vector<JOB> middle, result;
	sort(start.begin(), start.end(), LessSort4);
	JOB temp;

	for (int i = 0; i < n; i++) {
		if (i == 0) {
			temp = start.at(i);
			temp.inTime = temp.arriveTime;
			takeJOB(&temp);

			replaceJOB(&start, temp, i);

			result.push_back(temp);
			//displayJOB(temp);
		}
		else {
			//middle.clear();
			time_t lastEndtime = mktime(&(result.at(i - 1).endTime));

			//遍历vector,查找可以进入内存的作业
			for (int i = 1; i < n; i++) {
				if ((mktime(&(start.at(i).arriveTime)) <= lastEndtime) && start.at(i).flag == 0) {
					temp = start.at(i);
					temp.flag = 1;
					replaceJOB(&start, temp, i);
					middle.push_back(start.at(i));
				}
			}

			//对作业运行时间进行排序
			sort(middle.begin(), middle.end(), LessSort5);

			if (middle.empty()) {
				break;
			}
			temp = middle.at(0);
			localtime_s(&(temp.inTime), &lastEndtime);
			takeJOB(&temp);

			result.push_back(temp);
			//middle.pop_back();
			middle.erase(middle.begin());
		}

	}

	for (int i = 0; i < n; i++) {
		displayJOB(result.at(i));
	}
}

//非抢占式优先级调度算法
void Priority(vector<JOB> start,int n) {
	vector<JOB> middle, result;
	JOB temp;

	//对vector排序
	sort(start.begin(), start.end(), LessSort1);

	for (int i = 0; i < n; i++) {
		if (i == 0) {
			temp = start.at(i);
			temp.inTime = temp.arriveTime;
			takeJOB(&temp);

			replaceJOB(&start, temp, i);

			result.push_back(temp);
			//displayJOB(temp);
		}
		else {
			//middle.clear();
			time_t lastEndtime = mktime(&(result.at(i - 1).endTime));

			//遍历vector,查找可以进入内存的作业
			for (int i = 1; i < n; i++) {
				if ((mktime(&(start.at(i).arriveTime)) <= lastEndtime) && start.at(i).flag == 0) {
					temp = start.at(i);
					temp.flag = 1;
					replaceJOB(&start, temp, i);
					middle.push_back(start.at(i));
				}
			}

			//通过优先级对作业进行排序

			sort(middle.begin(), middle.end(), LessSort2);

			temp = middle.at(0);
			localtime_s(&(temp.inTime), &lastEndtime);
			takeJOB(&temp);

			result.push_back(temp);
			//middle.pop_back();
			middle.erase(middle.begin());
		}

	}

	for (int i = 0; i < n; i++) {
		displayJOB(result.at(i));
	}

}

//抢占式优先级调度算法
void PriorityP(vector<JOB> start, int n) {
	vector<JOB> middle, result, process;
	JOB temp, justRun;
	time_t lastEndtime;
	int race=0;//用来记录一个作业不被抢占的次数
	int recursionResult = 0;//用来记录递归结果

	//对vector排序
	sort(start.begin(), start.end(), LessSort1);


	for (int i = 0; i < n; i++) {
		temp = start.at(i);
		temp.inTime = temp.arriveTime;
		temp.flag = 1;
		temp.remainTime = temp.runTime;
		temp.justTime = temp.inTime;
		if (i == 0) {
			//takeJOB(&temp);
			replaceJOB(&start, temp, i);
			middle.push_back(temp);
			//process.push_back(temp);
			//displayJOB(temp);
		}
		else {
			justRun = middle.at(0);
			if (race < 1) {
				process.push_back(justRun);
			}
			
			time_t endTime = mktime(&justRun.justTime) + justRun.remainTime * 60;
			if (endTime < mktime(&temp.arriveTime)) {//如果到达时间小于上一次在运行作业结束的时间
				//计算上一个进程并结束
				localtime_s(&justRun.endTime, &endTime);
				takeJOBP(&justRun);

				justRun.justTime = justRun.endTime;
				justRun.remainTime = 0;
				process.push_back(justRun);
				//作业结束的进入result的vector
				result.push_back(justRun);
				//移除middle中完成的作业
				middle.erase(middle.begin());
				
				
				//计算上一个作业完成后，计算中间时间的运行
				time_t remainTime = mktime(&temp.arriveTime) - endTime;
				if (!middle.empty()) {
					if (recursionCompute(&middle, &result, &process, temp.arriveTime, justRun.endTime, remainTime)) {
						middle.push_back(temp);
						if (!middle.empty()) {
							justRun = middle.at(0);
							justRun.justTime = result.back().endTime;
							replaceJOB(&middle, justRun, 0);

							process.push_back(middle.at(0));
							race = 1;
						}
						else {
							race = 1;
						}
						
					}
					else
					{
						middle.push_back(temp);
					}
				}
				else {
					middle.push_back(temp);
				}
				
				
			}
			else {//否则判断优先级，进行抢占
				if (temp.firstRun < justRun.firstRun) {//如果新进入的作业优先级比正在运行的优先级高
					race = 0;
				}
				else {
					race++;
				}
				justRun.remainTime = (justRun.remainTime * 60 - (mktime(&temp.arriveTime) - mktime(&justRun.justTime))) / 60;
				if (justRun.remainTime == 0) {
					justRun.endTime = temp.arriveTime;
					//justRun.justTime = temp.arriveTime;
					takeJOBP(&justRun);

					process.push_back(justRun);
					result.push_back(justRun);
					middle.erase(middle.begin());
					justRun = middle.at(0);
				}
				justRun.justTime = temp.arriveTime;
				process.push_back(justRun);
				replaceJOB(&middle, justRun, 0);
				middle.push_back(temp);
				sort(middle.begin(), middle.end(), LessSort2);
				if ((i == start.size() - 1)&&(race==0)) {
					temp.justTime = temp.inTime;
					temp.remainTime = temp.runTime;
					process.push_back(temp);
				}
			}

			/*if (mktime(&justRun.endTime) <= mktime(&temp.arriveTime)) {
				
				takeJOB(&justRun);
				result.push_back(justRun);
				temp.inTime = temp.arriveTime;
				takeJOB(&temp);


			}*/
			
		}

	}

	if (!middle.empty()) {
		sort(middle.begin(), middle.end(), LessSort2);
	}

	tm endTime;
	
	time_t backTime;
	int i = 0;
	while (!middle.empty()) {
		
		
		justRun = middle.at(0);
		if (i != 0) {
			justRun.justTime = endTime;
			process.push_back(justRun);
		}
		//cout << justRun.remainTime << endl;
		
		backTime = mktime(&justRun.justTime) + justRun.remainTime * 60;
		localtime_s(&justRun.endTime, &backTime);
		
		takeJOBP(&justRun);
		

		endTime = justRun.endTime;
		
		justRun.justTime = justRun.endTime;
		justRun.remainTime = 0;
		process.push_back(justRun);
		result.push_back(justRun);
		middle.erase(middle.begin());
		i++;
	}

	//for (int i = 0; i < result.size(); i++) {
	//	displayJOB(result.at(i));
	//}

	for (int i = 0; i < process.size(); i++) {
		displayJOBR(process.at(i));
	}

	vector<COURSE> courses;
	COURSE course;
	JOB lastNode,nowNode;
	for (int i = 0; i < process.size(); i++) {
		if (i == 0) {
			lastNode = process.at(i);
			continue;
		}
		nowNode = process.at(i);
		if (nowNode.name == lastNode.name) {
			course.name = nowNode.name;
			course.minute = lastNode.remainTime - nowNode.remainTime;
			courses.push_back(course);
			lastNode = nowNode;
			cout << course.name << endl;
			cout << course.minute <<"分钟"<< endl;
		}
		else {
			lastNode = process.at(i);
		}

	}



}

tm StringToDateTime(string str) {
	char* cha = (char*)str.data();
	tm datetime;
	int hour;
	int minute;
	sscanf_s(cha, "%d:%d", &hour, &minute);
	
	datetime.tm_year = 2018 - 1900;
	datetime.tm_mon = 12;
	datetime.tm_mday = 1;
	datetime.tm_hour = hour;
	datetime.tm_min = minute;
	datetime.tm_sec = 0;
	datetime.tm_isdst = 0;

	return datetime;

}



//定义JOB结构体在vector中排序，sort为从大到小,优先数越小，优先级别越高,到达时间越早，就越大
bool LessSort1(JOB a, JOB b) {
	

	if (mktime(&(a.arriveTime)) < mktime(&(b.arriveTime))) {
		return true;
	}
	else if (mktime(&(a.arriveTime)) > mktime(&(b.arriveTime))) {
		return false;
	}
	else {
		if (a.firstRun < b.firstRun) {
			return true;
		}
		else {
			return false;
		}
	}
}

//定义JOB结构体在vector中排序，sort为从大到小,优先数越小，优先级别越高，对优先级别排序
bool LessSort2(JOB a, JOB b) {
	if (a.firstRun < b.firstRun) {
		return true;
	}
	else {
		/*if (a.firstRun == b.firstRun) {
			if (a.remainTime < b.remainTime) {
				return true;
			}
		}*/
	
		return false;

		
	}
	
}

//先来先服务排序
bool LessSort3(JOB a, JOB b) {
	return mktime(&(a.arriveTime)) < mktime(&(b.arriveTime)); 
}

//短作业优先排序,加上时间排序
bool LessSort4(JOB a, JOB b) {
	if (mktime(&(a.arriveTime)) < mktime(&(b.arriveTime))) {
		return true;
	}
	else if (mktime(&(a.arriveTime)) > mktime(&(b.arriveTime))) {
		return false;
	}
	else {
		if (a.runTime < b.runTime) {
			return true;
		}
		else {
			return false;
		}
	}
}

//短作业优先排序
bool LessSort5(JOB a, JOB b) {
	return a.runTime < b.runTime;
}
//计算作业的运行结束时刻，以及作业周转时间
void takeJOB(JOB* a) {
	time_t num;
	tm* temp;
	
	//计算运行结束时刻时间
	num = mktime(&(a->inTime));
	num += (a->runTime * 60);
	localtime_s(&(a->endTime),&num);

	//计算周转时间
	a->minute = difftime(mktime(&(a->endTime)), mktime(&(a->arriveTime))) / 60;
	a->flag = 1;
}

//用于计算周转时间
void takeJOBP(JOB* a) {
	//计算周转时间
	a->minute = difftime(mktime(&(a->endTime)), mktime(&(a->arriveTime))) / 60;
	a->flag = 1;
}

//用temp替换i位置的元素
void replaceJOB(vector<JOB>* ve, JOB temp, int i) {
	ve->erase(ve->begin() + i);//删除指定位置i的元素
	ve->insert((ve->begin() + i), temp);//向指定位置i处增加一个元素
}

void displayJOB(JOB a) {
	
	cout << "作业名：" << a.name << endl;
	cout << "到达时间为:" << a.arriveTime.tm_hour << ":" << a.arriveTime.tm_min << endl;
	cout << "估计运行时间为:" << a.runTime << "分钟" << endl;
	cout << "进入内存时刻为:" << a.inTime.tm_hour << ":" << a.inTime.tm_min << endl;
	cout << "进入运行结束时刻为:" << a.endTime.tm_hour << ":" << a.endTime.tm_min << endl;
	cout << "作业周转时间为:" << a.minute << "分钟" << endl;
	
}

void displayJOBR(JOB a) {

	cout << "作业名：" << a.name << endl;
	cout << "估计运行时间为:" << a.runTime << "分钟" << endl;
	cout << "运行开始时间为:" << a.justTime.tm_hour << ":" << a.justTime.tm_min << endl;
	cout << "剩余运行时间为:" << a.remainTime << "分钟" << endl;

	
	

}

//递归计算上一次作业结束，和新一个作业进入，中间的时间，其他作业运行的情况
int recursionCompute(vector<JOB>* middle, vector<JOB> *result , vector<JOB> *process,tm arriveTime, tm endTime, time_t remainTime) {
	JOB justRun;
	
	sort(middle->begin(), middle->end(), LessSort2);
	justRun = middle->at(0);
	
	if ((justRun.remainTime * 60 - remainTime) > 0) {//如果剩余时间不过运行完这个作业
		justRun.justTime = arriveTime;
		justRun.remainTime = (justRun.remainTime * 60 - remainTime) / 60;
		process->push_back(justRun);
		replaceJOB(middle, justRun, 0);
		return 0;
	}
	else {
		justRun.justTime = endTime;
		process->push_back(justRun);
		time_t backTime = mktime(&justRun.justTime) + justRun.remainTime * 60;
		localtime_s(&justRun.endTime, &backTime);
		takeJOBP(&justRun);
		remainTime = remainTime - justRun.remainTime * 60;

		justRun.justTime = justRun.endTime;
		justRun.remainTime = 0;
		process->push_back(justRun);
		result->push_back(justRun);
		middle->erase(middle->begin());
		if (remainTime <= 0) {
			return 1;
		}
		else {
			if (middle->empty()) {
				return 1;
			}
			else {
				return(recursionCompute(middle, result, process, arriveTime, justRun.endTime,remainTime));
			}
		}
		
	}
}

/*
6
B
10:20
30
3
A
10:00
40
5
C
10:30
60
4
D
10:50
20
6
E
11:00
20
4
F
11:10
10
4

*/

/*
4
A
10:00
20
3
B
10:05
20
2
C
10:10
20
3
D
11:20
20
4
E
10:15
50
3
*/

