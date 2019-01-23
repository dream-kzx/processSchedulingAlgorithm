#include<Windows.h>
#include<iostream>
#include<ctime>
#include<vector>
#include<string>
#include<algorithm>

using namespace std;


typedef struct job {
	string name;//��ҵ��
	tm arriveTime;//����ʱ��
	int runTime;//��������ʱ��
	int firstRun;//���ȼ���
	tm inTime;//�����ڴ�ʱ��
	tm endTime;//���н���ʱ��
	int minute;//��ҵ��תʱ��
	int remainTime;//ʣ������ʱ��
	int flag;//������־��ҵ��0δ�����ڴ棬1�����ڴ�
	tm justTime;//��������ռʽ�У���¼ÿһ�ο�ʼ���е�ʱ��
	//int n;//�ڽ����ڴ�ʱ����¼֮ǰ��λ��
}JOB;

typedef struct Course
{
	string name;
	int minute;
}COURSE;


//��string��ʽתΪtm��ʽ
tm StringToDateTime(string str);
//����JOB�ṹ����vector������sortΪ�Ӵ�С,������ԽС�����ȼ���Խ��,����ʱ��Խ�磬��Խ��
bool LessSort1(JOB a, JOB b);
//����JOB�ṹ����vector������sortΪ��С����,������ԽС�����ȼ���Խ�ߣ������ȼ�������
bool LessSort2(JOB a, JOB b);
//�����ȷ�������
bool LessSort3(JOB a, JOB b);
//����ҵ��������,����ʱ������
bool LessSort4(JOB a, JOB b);
//����ҵ��������
bool LessSort5(JOB a, JOB b);




//������ҵ�����н���ʱ�̣��Լ���ҵ��תʱ��
void takeJOB(JOB* a);
//���ڼ�����תʱ��
void takeJOBP(JOB* a);

void displayJOB(JOB a);
void displayJOBR(JOB a);
//��temp�滻iλ�õ�Ԫ��
void replaceJOB(vector<JOB>* ve, JOB temp, int i);

//�ݹ������һ����ҵ����������һ����ҵ���룬�м��ʱ�䣬������ҵ���е����
int recursionCompute(vector<JOB>* middle, vector<JOB> *result, vector<JOB> *process, tm arriveTime, tm endTime, time_t remainTime);

//�����ȷ���
void FCFS(vector<JOB> start, int n);

//����ҵ����
void SJF(vector<JOB> start, int n);

//����ռʽ���ȼ������㷨
void Priority(vector<JOB> start, int n);

//��ռʽ���ȼ������㷨
void PriorityP(vector<JOB> start, int n);

int main(void) {

	vector<JOB> start;
	JOB work;
	string time1;
	
	int n=0;//������ҵ����

	cout << "�����봴����ҵ�ĸ�����" << endl;
	cin >> n;
	for (int i = 0; i < n; i++) {
		cout << "��������ҵ" << i << "��name" << endl;
		cin >> work.name;
		cout << "��������ҵ" << i << "�ĵ���ʱ��" << endl;
		cin >> time1;
		cout << "��������ҵ" << i << "�Ĺ�������ʱ��" << endl;
		cin >> work.runTime;
		cout << "��������ҵ" << i << "�����ȼ���" << endl;
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


//�����ȷ���
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

//����ҵ����
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

			//����vector,���ҿ��Խ����ڴ����ҵ
			for (int i = 1; i < n; i++) {
				if ((mktime(&(start.at(i).arriveTime)) <= lastEndtime) && start.at(i).flag == 0) {
					temp = start.at(i);
					temp.flag = 1;
					replaceJOB(&start, temp, i);
					middle.push_back(start.at(i));
				}
			}

			//����ҵ����ʱ���������
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

//����ռʽ���ȼ������㷨
void Priority(vector<JOB> start,int n) {
	vector<JOB> middle, result;
	JOB temp;

	//��vector����
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

			//����vector,���ҿ��Խ����ڴ����ҵ
			for (int i = 1; i < n; i++) {
				if ((mktime(&(start.at(i).arriveTime)) <= lastEndtime) && start.at(i).flag == 0) {
					temp = start.at(i);
					temp.flag = 1;
					replaceJOB(&start, temp, i);
					middle.push_back(start.at(i));
				}
			}

			//ͨ�����ȼ�����ҵ��������

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

//��ռʽ���ȼ������㷨
void PriorityP(vector<JOB> start, int n) {
	vector<JOB> middle, result, process;
	JOB temp, justRun;
	time_t lastEndtime;
	int race=0;//������¼һ����ҵ������ռ�Ĵ���
	int recursionResult = 0;//������¼�ݹ���

	//��vector����
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
			if (endTime < mktime(&temp.arriveTime)) {//�������ʱ��С����һ����������ҵ������ʱ��
				//������һ�����̲�����
				localtime_s(&justRun.endTime, &endTime);
				takeJOBP(&justRun);

				justRun.justTime = justRun.endTime;
				justRun.remainTime = 0;
				process.push_back(justRun);
				//��ҵ�����Ľ���result��vector
				result.push_back(justRun);
				//�Ƴ�middle����ɵ���ҵ
				middle.erase(middle.begin());
				
				
				//������һ����ҵ��ɺ󣬼����м�ʱ�������
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
			else {//�����ж����ȼ���������ռ
				if (temp.firstRun < justRun.firstRun) {//����½������ҵ���ȼ����������е����ȼ���
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
			cout << course.minute <<"����"<< endl;
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



//����JOB�ṹ����vector������sortΪ�Ӵ�С,������ԽС�����ȼ���Խ��,����ʱ��Խ�磬��Խ��
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

//����JOB�ṹ����vector������sortΪ�Ӵ�С,������ԽС�����ȼ���Խ�ߣ������ȼ�������
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

//�����ȷ�������
bool LessSort3(JOB a, JOB b) {
	return mktime(&(a.arriveTime)) < mktime(&(b.arriveTime)); 
}

//����ҵ��������,����ʱ������
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

//����ҵ��������
bool LessSort5(JOB a, JOB b) {
	return a.runTime < b.runTime;
}
//������ҵ�����н���ʱ�̣��Լ���ҵ��תʱ��
void takeJOB(JOB* a) {
	time_t num;
	tm* temp;
	
	//�������н���ʱ��ʱ��
	num = mktime(&(a->inTime));
	num += (a->runTime * 60);
	localtime_s(&(a->endTime),&num);

	//������תʱ��
	a->minute = difftime(mktime(&(a->endTime)), mktime(&(a->arriveTime))) / 60;
	a->flag = 1;
}

//���ڼ�����תʱ��
void takeJOBP(JOB* a) {
	//������תʱ��
	a->minute = difftime(mktime(&(a->endTime)), mktime(&(a->arriveTime))) / 60;
	a->flag = 1;
}

//��temp�滻iλ�õ�Ԫ��
void replaceJOB(vector<JOB>* ve, JOB temp, int i) {
	ve->erase(ve->begin() + i);//ɾ��ָ��λ��i��Ԫ��
	ve->insert((ve->begin() + i), temp);//��ָ��λ��i������һ��Ԫ��
}

void displayJOB(JOB a) {
	
	cout << "��ҵ����" << a.name << endl;
	cout << "����ʱ��Ϊ:" << a.arriveTime.tm_hour << ":" << a.arriveTime.tm_min << endl;
	cout << "��������ʱ��Ϊ:" << a.runTime << "����" << endl;
	cout << "�����ڴ�ʱ��Ϊ:" << a.inTime.tm_hour << ":" << a.inTime.tm_min << endl;
	cout << "�������н���ʱ��Ϊ:" << a.endTime.tm_hour << ":" << a.endTime.tm_min << endl;
	cout << "��ҵ��תʱ��Ϊ:" << a.minute << "����" << endl;
	
}

void displayJOBR(JOB a) {

	cout << "��ҵ����" << a.name << endl;
	cout << "��������ʱ��Ϊ:" << a.runTime << "����" << endl;
	cout << "���п�ʼʱ��Ϊ:" << a.justTime.tm_hour << ":" << a.justTime.tm_min << endl;
	cout << "ʣ������ʱ��Ϊ:" << a.remainTime << "����" << endl;

	
	

}

//�ݹ������һ����ҵ����������һ����ҵ���룬�м��ʱ�䣬������ҵ���е����
int recursionCompute(vector<JOB>* middle, vector<JOB> *result , vector<JOB> *process,tm arriveTime, tm endTime, time_t remainTime) {
	JOB justRun;
	
	sort(middle->begin(), middle->end(), LessSort2);
	justRun = middle->at(0);
	
	if ((justRun.remainTime * 60 - remainTime) > 0) {//���ʣ��ʱ�䲻�������������ҵ
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

