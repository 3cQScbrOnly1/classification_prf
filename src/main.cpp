#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <boost/algorithm/string.hpp>

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
	ifstream gold_file(argv[1]), predict_file;
	map<string, int> gold_label_table;
	string gold_line;
	vector<string> fields;
	vector<string> gold_labels;
	while(getline(gold_file, gold_line))
	{
		split(fields, gold_line, is_any_of("\t"));
		gold_label_table[fields[0]]++;
		gold_labels.push_back(fields[0]);
	}
	for(map<string, int>::iterator it = gold_label_table.begin();
		it != gold_label_table.end(); it++)
	{
		cout << "gold label " << it->first << " num: " << it->second << endl;
	}
	gold_file.close();
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
		predict_file.open(argv[i]);
		predict_labels.clear();
		while(getline(predict_file, predict_line))
		{
			split(fields, predict_line, is_any_of("\t"));
			predict_labels.push_back(fields[0]);
		}
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
			cout << "================================================================================" << endl;
			cout << "file: " << argv[i] << endl;
			for(map<string, int>::iterator it = gold_label_table.begin();
					it != gold_label_table.end(); it++)
			{
				precisions[pos] = (double) predict_right[pos] / predict_num[pos];
				recalls[pos] = (double) predict_right[pos] / it->second;
				fs[pos] = 2 * precisions[pos] * recalls[pos] / (precisions[pos] + recalls[pos]);
				cout << "label: " << it->first << "\t";
				cout << "precision: " << precisions[pos] << "\t";
				cout << "recalls: " << recalls[pos] << "\t";
				cout << "f score: " << fs[pos] << endl;
				cout << "-------------------------------------------------------------------" << endl;
				pos++;
			}
			double f_avg = 0;
			for(int idx = 0; idx < g_table_size; idx++)
			{
				f_avg += fs[idx];
			}
			cout << "f avg: " << f_avg / g_table_size << endl;
			cout << "================================================================================" << endl;
		} //if
		else
			cout << "error file" << endl;
		predict_file.close();
	}
	return 0;
}
