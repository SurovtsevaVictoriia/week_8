#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <vector>
#include <random>
#include <functional>
#include <mutex>

std::vector <char> nucleo{ 'A', 'T', 'G', 'C' };

std::string generate_dna(int N) {

	std::string chain;


	std::random_device rd;
	std::mt19937 mersenne(rd());
	std::uniform_int_distribution <> distrib(0, 3);

	for (int i = 0; i < N; ++i) {
		chain += nucleo[distrib(mersenne)];
	}

	return chain;

}

void find(int i, std::vector<std::string> sub_chains, std::string fragment, std::vector<int> &indexes) {

	std::mutex m_mutex;
	const std::lock_guard<std::mutex> lock(m_mutex);

	int t = 0;

	while (t <= (sub_chains[i].size()) ){

		if (sub_chains[i].find(fragment, t) != -1) {
			indexes.push_back(sub_chains[i].find(fragment, t) + (sub_chains[0].size() - fragment.size() + 1)* (i));
		}
		t++;
	}

}

std::vector <std::string> make_sub_chains(std::string chain, std::string fragment,int threads_num) {

	std::vector <std::string> sub;

	for (int i = 0; i < threads_num; i++) {
		sub.push_back( chain.substr(chain.size() / threads_num * i, chain.size() / threads_num + fragment.size() - 1));
	}
	return sub;
}



std::vector<int> parallel_search(std::string& chain, std::string& fragment) {

	int N = chain.size();
	int n = fragment.size();
	int threads_num = (std::thread::hardware_concurrency == 0) ? 4 : std::thread::hardware_concurrency();

	std::vector<std::string> sub_chains = make_sub_chains(chain, fragment, threads_num);
	

	std::vector<int> indexes;

	std::vector < std::thread > threads;

	for (std::size_t i = 1; i < threads_num; ++i)
	{
		threads.push_back(std::thread(find, std::ref(i), std::ref(sub_chains), std::ref(fragment), std::ref(indexes)));
	}

	std::for_each(threads.begin(), threads.end(),
		std::mem_fn(&std::thread::join));

	std::sort(indexes.begin(), indexes.end());
	indexes.erase(unique(indexes.begin(), indexes.end()), indexes.end());

	return indexes;
}

template <typename T>
void print(std::vector <T> vec) {
	std::cout << '{';
	for (auto i : vec) {
		std::cout << ' ' << i << ',';
	}
	std::cout << "};" << std::endl;
}

void test() {

	std::string chain = "ACGTAGCGAAAACGAAAAGTCTGAA";
	std::string fragment = "CGA";

	//std::cout << "chain: " << chain << std::endl;
	//std::cout << "fragment: " << fragment << std::endl;

	//std::cout << chain.find(fragment, 1);

	//std::vector<std::string> sub_chains = make_sub_chains(chain, fragment, 8);
	//print(sub_chains);


	//std::vector<int> indexes;
	//find(4, sub_chains, fragment, indexes);
	//print(indexes);

	print(parallel_search(chain, fragment));

}
int main() {

	/*int N = 100;
	int n = 5;
	std::string chain = generate_dna(N);
	std::string fragment = generate_dna(n);
	print(parallel_search(chain, fragment));*/

	test();

	return 0;
}
