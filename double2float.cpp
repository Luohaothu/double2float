#include <string>
#include <iostream>
#include <fstream>
#include <array>
#include <chrono>

constexpr auto nsize = (1 << 18);

int main(int argc, char* argv[]) {
	using namespace std;

	string filename;
	if (argc > 1)
		filename = string(argv[1]);

	int infilesize = ifstream(filename, ifstream::ate | ifstream::binary).tellg() / 8;

	int infilesize0 = infilesize;

	if (infilesize <= 0) return 0;

	auto infile = ifstream(filename, ifstream::binary);

	auto outfile = ofstream(filename + ".float", ofstream::binary);

	shared_ptr<array<double, nsize>> dvar = make_shared<array<double, nsize>>();
	shared_ptr<array<float, nsize>>  fvar = make_shared<array<float, nsize>>();

	auto t0 = chrono::high_resolution_clock::now();

	while (infilesize - nsize > 0) {

		infile.read(reinterpret_cast<char*>(dvar->data()), nsize * sizeof(double));

		for (int i = 0; i < nsize; i++)
		{
			(*fvar)[i] = (*dvar)[i];
		}
		
		outfile.write(reinterpret_cast<char*>(fvar->data()), nsize * sizeof(float));

		infilesize -= nsize;

		float progress = (float)(infilesize0 - infilesize) / infilesize0;

		if ((int)(progress * 1000) % 10 == 0)
			cout << "Current progress: " << progress * 100 << "%" << endl;
	}

	auto remain = infilesize;

	infile.read(reinterpret_cast<char*>(dvar->data()), remain * sizeof(double));

	for (int i = 0; i < remain; i++)
	{
		(*fvar)[i] = (*dvar)[i];
	}

	outfile.write(reinterpret_cast<char*>(fvar->data()), remain * sizeof(float));

	auto t1 = chrono::high_resolution_clock::now();

	cout << "Used time: " << chrono::duration_cast<chrono::seconds>(t1 - t0).count() << "s\n";

	return 0;
}