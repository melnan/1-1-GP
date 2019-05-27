#include<stdio.h>
#include<vector>
#include<iostream>
#include<fstream>
//#include<curses.h>
#include<ctime>
#include<string>
#include<climits>

typedef std::vector< std::vector<int>> Vector_vect_int;


int init_num_variables = 3;	//initial number of variables (n in the paper)
int c = 2;				//majority parameter
int num_iter = 10;		// max number of iterations
int num_alg_runs = 10;


int majority = 1; // 0 = standart c-majority, 1 = 2/3-majority, 2/3- supermajority (part of the code with 2/3-supermajority realization is commented) 
bool bloat_control = 1;	//with/without bloat control			

int global_counter;

int norm_sign(int _sign)
{
	if (_sign == 0)
		return -1;

	return _sign;
}


Vector_vect_int initialisation(int _num_variables, int _init_tree_size)
{
	Vector_vect_int tree(_init_tree_size, std::vector<int>(2));

	for (int i = 0; i < tree.size(); ++i)
	{
		tree[i][0] = 0;
		tree[i][1] = 0;
	}

	for (int i = 0; i < _init_tree_size; ++i)
	{
		//random fill the tree
		tree[i][0] = rand() % _num_variables; //variable
		tree[i][1] = norm_sign(rand() % 2); //Sign of the variable (-1 or 1)
	}

	return tree;
}

void finalization(Vector_vect_int &_tree)
{
	for (int i = 0; i < _tree.size(); ++i)
	{
		//clean memory
		_tree[i].clear();

	}
	_tree.clear();
}


//mutation operator 
Vector_vect_int mutation(Vector_vect_int &_tree, int _num_variables)
{
	Vector_vect_int tree_new(_tree);

	//choose one of 3 possible operations u.a.r.
	int choice, choice_2;

	int random_case = rand() % 3;

	if ((tree_new.size() == 0) && (random_case != 1))
		return tree_new;//if tree is empty and insert does not happen, we do not change the tree

	switch (random_case)
	{
	case 0:	//substitute
		//std::cout << "substitution" << '\n';
		choice = rand() % tree_new.size(); //leaf to subst.
		tree_new[choice][0] = rand() % _num_variables;
		tree_new[choice][1] = norm_sign(rand() % 2);
		if (tree_new[choice][1] == 0) tree_new[choice][1] = -1;
		break;

	case 1:	//insert
		//std::cout << "insert" << '\n';
		if (tree_new.size() == 0)
			choice = 0;
		else
			choice = rand() % tree_new.size(); //leaf

		choice_2 = rand() % _num_variables; //variable for insert

		tree_new.insert(tree_new.begin() + choice, { choice_2, norm_sign(rand() % 2) });
		break;

	case 2: //delete
		//std::cout << "deletion" << '\n';
		choice = rand() % tree_new.size();
		tree_new.erase(tree_new.begin() + choice);
		break;

	}

	return tree_new;
}


//fitness
int f(Vector_vect_int &_tree, int _num_variables)
{
	Vector_vect_int	counter(_num_variables, { 0, 0 }); // second dimension: we need to exclude the case: c = 0, i.e., a variable is not presented in the tree but considered as expressed - wrong
	//Vector_vect_int tree(init_tree_size, std::vector<int>(2));

	for (int i = 0; i < _tree.size(); ++i)
	{
		counter[_tree[i][0]][0] += _tree[i][1];	//majority for each variable 
		if (_tree[i][1] > 0) counter[_tree[i][0]][1]++;
	}

	//total number of expressed variables
	int num_of_expressed_var = 0;

	for (int i = 0; i < _num_variables; ++i)
	{
		if ((counter[i][0] >= c && counter[i][1] != 0))
			num_of_expressed_var += 1;
	}

	counter.clear();

	return num_of_expressed_var;
}

int f_2_3(Vector_vect_int &_tree, int _num_variables)
{
	Vector_vect_int	counter(_num_variables, { 0, 0 }); //count number of negative and positive variables at the same time

	for (int i = 0; i < _tree.size(); ++i)
	{
		if (_tree[i][1] == 1)
			counter[_tree[i][0]][0] ++;	//number of positive variables 
		else
			counter[_tree[i][0]][1] ++;  //number of negative variables
	}

	//total number of expressed variables
	int num_of_expressed_var = 0;

	for (int i = 0; i < _num_variables; ++i)
	{
		if (counter[i][0] >= 2 * counter[i][1] && counter[i][0] > 0)
			num_of_expressed_var += 1;
	}

	counter.clear();

	return num_of_expressed_var;
}

//long double f_sup_maj(Vector_vect_int &_tree)
//{
//	long double result = 0;
//
//	Vector_vect_int	counter(num_variables, { 0, 0 }); //count number of negative and positive variables at the same time
//
//	for (int i = 0; i < _tree.size(); ++i)
//	{
//		if (_tree[i][1] == 1)
//			counter[_tree[i][0]][0] ++;	//number of positive variables 
//		else
//			counter[_tree[i][0]][1] ++;  //number of negative variables
//	}
//
//	for (int i = 0; i < num_variables; ++i)
//	{
//		if ((counter[i][0] != 0 || counter[i][1] != 0) && (counter[i][0]>=2*counter[i][1]))
//		{
//			result += 2 - powl(2, counter[i][1] - counter[i][0]);
//		}
//	}
//
//	counter.clear();
//
//	return result;
//}



int GA(Vector_vect_int &_tree,
	int _num_variables,
	int _k, //number of mutations (operations)
	int _majority, // 0 = standart c-majority, 1 = 2/3-majority, 2-supermajoity
	bool bloat_control	//with/without bloat control
	)
{
	Vector_vect_int tree_new;

	int f_best;
	int size_best = _tree.size();

	switch (_majority)
	{
	case 0:
		f_best = f(_tree, _num_variables);
		break;
	case 1:
		f_best = f_2_3(_tree, _num_variables);
		break;
		//case 2:
		//	f_best = f_sup_maj(_tree);
		//	break;
	}

	for (int i = 1; i <= _k; i++)
	{
		tree_new = mutation(_tree, _num_variables);

		long double f_new;

		switch (_majority)
		{
		case 0:
			f_new = f(tree_new, _num_variables);
			break;
		case 1:
			f_new = f_2_3(tree_new, _num_variables);
			break;
			//case 2:
			//	f_new = f_sup_maj(tree_new);
			//	break;
		}

		if (((f_new >= f_best) && (!bloat_control)) ||	//no bloat control
			(f_new > f_best) ||						//both cases
			((f_new == f_best) && (tree_new.size() <= _tree.size()) && (bloat_control))	//with bloat
			)
		{
			_tree.clear();
			_tree = tree_new;
			tree_new.clear();
			f_best = f_new;
			//size_best = _tree.size();

			//screen_output(tree);
			//std::cout << "f_best= " << f_best <<"size_best = "<< tree.size()<< '\n';

		}
		else
		{
			//std::cout << "was not exsepted f=" << f_new << "size = " << tree_new.size() << '\n';
			//screen_output(tree_new);
			tree_new.clear();
		}
	}

	return (int)(f_best);
}




int main()
{
	srand(time(0));
	int time_total = 0;
	int time_max = 0;
	int time_min = INT_MAX;
	int f_best = 0;
	int f_best_total = 0;

	std::string f_type;

	switch (majority)
	{
	case 0:
		f_type = "c_" + std::to_string(c) + "_";
		break;
	case 1:
		f_type = "2_3_";
		break;
	case 2:
		f_type = "sup_maj_";
		break;
	default:
		break;
	}

	switch (bloat_control)
	{
	case 0:
		f_type = f_type + "no_bloat_control_";
		break;
	case 1:
		f_type = f_type + "with_bloat_control_";
		break;
	default:
		break;
	}

	f_type = f_type + "_10n_Tinit_" + std::to_string(num_alg_runs) + "alg_runs.txt";

	std::string file_time_average = "time_average_" + f_type;
	std::string file_time_max = "time_max_" + f_type;
	std::string file_time_min = "time_min_" + f_type;
	std::string file_f_best_average = "final_f_average_" + f_type;
	std::string file_f_best_max = "final_f_max_" + f_type;
	std::string file_f_best_min = "final_f_min_" + f_type;
	std::string file_tree_size_aver = "average_tree_size_" + f_type;
	std::string file_time = "time_" + f_type;

	std::ofstream(file_time_average, std::ofstream::trunc);
	std::ofstream(file_time_max, std::ofstream::trunc);
	std::ofstream(file_time_min, std::ofstream::trunc);
	std::ofstream(file_f_best_average, std::ofstream::trunc);
	std::ofstream(file_f_best_max, std::ofstream::trunc);
	std::ofstream(file_f_best_min, std::ofstream::trunc);
	std::ofstream(file_tree_size_aver, std::ofstream::trunc);
	std::ofstream(file_time, std::ofstream::trunc);

	std::ofstream output_file_time_aver(file_time_average, std::ios::app);
	std::ofstream output_file_time_max(file_time_max, std::ios::app);
	std::ofstream output_file_time_min(file_time_min, std::ios::app);
	std::ofstream output_file_f_aver(file_f_best_average, std::ios::app);
	std::ofstream output_file_f_best_max(file_f_best_max, std::ios::app);
	std::ofstream output_file_f_best_min(file_f_best_min, std::ios::app);
	std::ofstream output_file_tree_size_aver(file_tree_size_aver, std::ios::app);
	std::ofstream output_file_time(file_time, std::ios::app);



	//screen_output(tree);
	//std::cout << "f_best_initial= " << f(tree) << '\n';

	for (int num_variables = init_num_variables; num_variables <= 10; num_variables += 1)
	{
		int init_tree_size = num_variables * 10;
		time_total = 0;
		time_min = INT_MAX;
		time_max = 0;
		f_best = 0;
		f_best_total = 0;
		int f_best_max = 0;
		int f_best_min = INT_MAX;
		int size_average = 0;
		Vector_vect_int tree(init_tree_size, std::vector<int>(2));

		for (int j = 0; j < num_alg_runs; j++)
		{
			global_counter = 0;
			f_best = 0;

			tree = initialisation(num_variables, init_tree_size);
			//std::cout << "f_tree_init=" << f(tree) << ", size=" << tree.size() << '\n';

			while ((global_counter <= num_iter) && (f_best < num_variables))
			{

				//std::cout << "new iteration" << '\n';
				f_best = GA(tree, num_variables, 1, majority, bloat_control);
				global_counter++;
			}

			//update all
			time_total += global_counter;
			f_best_total += f_best;
			size_average += tree.size();

			std::cout << j << ".) " << "f_best = " << f_best << "; time = " << global_counter << '\n';
			output_file_time << global_counter << '\n';

			if (global_counter > time_max) time_max = global_counter;
			if ((global_counter < time_min) && (global_counter>0)) time_min = global_counter;

			if (f_best > f_best_max) f_best_max = f_best;
			if (f_best < f_best_min) f_best_min = f_best;

			//clear_tree();
			if (num_variables > tree.size())
			{
				while (tree.size() != num_variables)
					tree.push_back({ 0, 0 });
			}
			else
				tree.erase(tree.begin() + num_variables, tree.end());
		}
		//print out an average time for current n
		output_file_time_aver << time_total / num_alg_runs << '\n';
		output_file_time_max << time_max << '\n';
		output_file_time_min << time_min << '\n';
		output_file_f_aver << f_best_total / num_alg_runs << '\n';
		output_file_f_best_max << f_best_max << '\n';
		output_file_f_best_min << f_best_min << '\n';
		output_file_tree_size_aver << size_average / num_alg_runs << '\n';
		output_file_time << '\n';

		std::cout << '\n';
		std::cout << num_variables << ".) " << "f_aver = " << f_best_total / num_alg_runs << "; time_aver = " << time_total / num_alg_runs << '\n';

		finalization(tree);
		//output time values
	}

	output_file_time_aver.close();
	output_file_time_max.close();
	output_file_time_min.close();
	output_file_f_aver.close();
	output_file_f_best_max.close();
	output_file_f_best_min.close();
	output_file_tree_size_aver.close();
	output_file_time.close();

	std::cout << "done" << '\n';
	return 0;
}
