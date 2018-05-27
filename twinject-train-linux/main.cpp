#
#include <vector>
#include <iostream>
#include <fstream>
     
#include "tiny_dnn/tiny_dnn.h"
using namespace tiny_dnn;

#include "external.h"

vec_t sample_to_imat(const sample &i)
{
	vec_t inputMat;
	for (int j = 0; j < SAMPLE_SIZE; ++j)
	{
		if (j < i.sampleCount)
		{
			inputMat.push_back(i.pos[j].x);
			inputMat.push_back(i.pos[j].y);
			inputMat.push_back(i.vel[j].x);
			inputMat.push_back(i.vel[j].y);
		}
		else
		{
			inputMat.push_back(0);
			inputMat.push_back(0);
			inputMat.push_back(0);
			inputMat.push_back(0);
		}
	}
	return inputMat;
}


int main()
{
	const std::string filename = "nn_response.samp";
	std::cout << "loading samples from " << filename << std::endl;

	std::vector<sample> samples = load_samples(filename);
	if(samples.empty())
	{
		std::cout << "samples are empty" << std::endl;
		return 1;
	}
	unsigned int inputNeurons = SAMPLE_SIZE * 2 * 2;
	unsigned int outputNeurons = 5;
	std::cout << "using " << inputNeurons << " input neurons, " << outputNeurons << " output neurons" << std::endl;

	std::vector<vec_t> input;
	std::vector<vec_t> output;

	std::cout << "converting sample data into matrix form" << std::endl;

	for (auto i : samples)
	{
		vec_t inputMat = sample_to_imat(i);
		input.push_back(inputMat);
		vec_t outputMat = {
			i.keys.up ? 1.f : 0.f,
			i.keys.down ? 1.f : 0.f,
			i.keys.left ? 1.f : 0.f,
			i.keys.right ? 1.f : 0.f,
			i.keys.slow ? 1.f : 0.f
		};
		output.push_back(outputMat);
	}

	std::cout << "training..." << std::endl;
	auto nn = make_mlp<activation::tanh>({ inputNeurons, 100, outputNeurons });

	timer t;
	int epoch = 0;
	adagrad optimizer;

	nn.fit<mse>(optimizer, input, output, 24, 64,
		// called for each mini-batch
		[&]() {

	},
		// called for each epoch
		[&]() {
		std::cout << "\tcompleted epoch " << epoch << " after " << t.elapsed() << std::endl;
		t.restart();
		++epoch;
	});

	std::stringstream nnfile;
	nnfile << "nn_network_" << SAMPLE_SIZE << ".nn";
	std::cout << "writing network state to " << nnfile.str() << std::endl;
	nn.save(nnfile.str());
}

