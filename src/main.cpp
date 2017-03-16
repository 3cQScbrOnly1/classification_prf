#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

void getLabels(const char* path, vector<string>& labels);

int main(int argc, char* argv[])
{
	map<string, int> gold_label_table;
	vector<string> gold_labels;
	getLabels(argv[1], gold_labels);
	for(int i = 0;  i < gold_labels.size(); i++)
	{
		gold_label_table[gold_labels[i]]++;
	}
	for(map<string, int>::iterator it = gold_label_table.begin();
		it != gold_label_table.end(); it++)
	{
		cout << "gold label " << it->first << " num: " << it->second << endl;
	}
	string predict_line;
	vector<string> predict_labels;
	int g_table_size = gold_label_table.size();
	vector<int> predict_right(g_table_size);
	vector<int> predict_num(g_table_size);
	vector<double> precisions(g_table_size);
	vector<double> recalls(g_table_size);
	vector<double> fs(g_table_size);
	int pos;
	for(int i = 2; i < argc; i++)
	{
		predict_labels.clear();
		getLabels(argv[i], predict_labels);
		if(predict_labels.size() == gold_labels.size())
		{

			for(int j = 0; j < g_table_size; j++)
			{
				predict_right[j] = 0;
				predict_num[j] = 0;
				precisions[j] = 0;
				recalls[j] = 0;
				fs[j] = 0;
			}
			int gold_labels_size = gold_labels.size();
			for(int idx = 0; idx < gold_labels_size; idx++)						
			{
				// correct answer 
				if(predict_labels[idx] == gold_labels[idx])
				{
					pos = 0;
					for(map<string, int>::iterator it = gold_label_table.begin();
						it != gold_label_table.end(); it++)
					{
						if(predict_labels[idx] == it->first)
						{
							predict_right[pos]++;
							predict_num[pos]++;
							break;
						}
						pos++;
					} //for
				} //if 
				else 
				{	// error answer
					pos = 0;
					for(map<string, int>::iterator it = gold_label_table.begin();
						it != gold_label_table.end(); it++)
					{
						if(predict_labels[idx] == it->first)
						{
							predict_num[pos]++;
							break;
						}
						pos++;
					} //for
				}
			} //for
			// compute prf
			pos = 0;
			cout.precision(5);
			cout << endl;
			cout << "file: " << argv[i] << endl;
			cout << "type" << "\t" << "precision" << "\t" << "recalls" << "\t"  << "f score" << endl;
			double predict_right_sum = 0, predict_sum = 0, gold_sum = 0;
			for(map<string, int>::iterator it = gold_label_table.begin();
					it != gold_label_table.end(); it++)
			{
				predict_right_sum += predict_right[pos];
				predict_sum += predict_num[pos];
				gold_sum += it->second;
				precisions[pos] = (double) predict_right[pos] / predict_num[pos];
				recalls[pos] = (double) predict_right[pos] / it->second;
				fs[pos] = 2 * precisions[pos] * recalls[pos] / (precisions[pos] + recalls[pos]);
				cout << "label_" << it->first << "\t";
				cout << precisions[pos] << "\t";
				cout << recalls[pos] << "\t";
				cout << fs[pos] << endl;
				pos++;
			}
			double micro_precision = predict_right_sum / predict_sum;
			double micro_recall = predict_right_sum / gold_sum;
			double micro_f = 2 * micro_precision * micro_recall / (micro_precision + micro_recall);
			cout << "micro" << "\t" << micro_precision << "\t";
			cout << micro_recall << "\t";
			cout << micro_f << endl;
			double f_sum = 0, p_sum = 0, r_sum = 0;
			for(int idx = 0; idx < g_table_size; idx++)
			{
				p_sum += precisions[idx];
				r_sum += recalls[idx];
				f_sum += fs[idx];
			}
			cout << "macro" << "\t" <<  p_sum / g_table_size << "\t";
			cout << r_sum / g_table_size << "\t";
			cout << f_sum / g_table_size << endl;
			cout << endl;
		} //if
		else
			cout << "error file" << endl;
		//predict_file.close();
	}
	return 0;
}

void getLabels(const char* path, vector<string>& labels)
{
	labels.clear();
	ifstream file(path);
	string line;
	vector<string> fields;
	while(getline(file, line))
	{
		split(fields, line, is_any_of(" "));
		labels.push_back(fields[0]);	
	}
	file.close();
}
