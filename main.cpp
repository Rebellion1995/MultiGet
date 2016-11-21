#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <curl/curl.h>
#define CH_NUM 4;
#define MiB 1048576;
using namespace std;

//chunk struct
struct MemoryStruct
{
	char* memory;
	size_t size;
};

//curl write_callback function
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct *mem = (struct MemoryStruct *)userp;

	mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
	if (mem->memory == NULL) {
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->memory[mem->size]), ptr, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}
//thread run function
static void get_chunk(void* url, string range, int num, MemoryStruct* chunks)
{
	cout << "Started thread" << num << "\n";
	CURL* curl;
	curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_URL, url);
	curl_easy_setopt(curl, CURLOPT_RANGE, (range.c_str()));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(chunks + num)); //write to the #j chunk
	curl_easy_perform(curl); 
	curl_easy_cleanup(curl);
}

int main(int argc, const char* argv[])
{
	if (argc < 2)
	{
		cout << "Usage: multiGet.exe [Options] url\n" << "\t-o string\n\t\tWrite output to <file> instead of default\n"
			<< "\t-scn number\n\t\tSet number of chunks to download (default is 4)\n"
			<< "\t-scs number\n\t\tSet chunk size (default is 1MiB)\n"
			<< "\t-parallel\n\t\tDownload chunks in parallel instead of sequentially\n";
		exit(1);
	}
	FILE* outputFile;
	size_t chunkNumber = CH_NUM;
	size_t chunkSize = MiB;
	int i = 1; //command line arguments iterator
	bool parallel = false;
	//File output option
	if (strcmp(argv[i], "-o") == 0)
	{
		//TODO: add test case when filename is missing
		outputFile = fopen(argv[++i],"wb");
		i++;
	}
	else 
		outputFile = fopen("Download4MiB.jar","wb"); //default file

	//Set chunks number option
	if (strcmp(argv[i], "-scn") == 0)
	{
		chunkNumber = atoi(argv[++i]);
		i++;
	}

	//Set chunk size option
	if (strcmp(argv[i], "-scs") == 0)
	{
		chunkSize = atoi(argv[++i]);
		i++;
	}

	//Parallel download option
	if (strcmp(argv[i], "-parallel") == 0)
	{
		parallel = true;
		i++;
	}
	if (i >= argc) //checks if an url was provided
	{
		cout << "Usage: multiGet.exe [Options] url\n" << "\t-o string\n\t\tWrite output to <file> instead of default\n";
		//<< "\t-parallel\n\t\tDownload chunks in parallel instead of sequentially\n";
		exit(1);
	}
	string url = argv[i];


	MemoryStruct* chunks = new MemoryStruct[chunkNumber];
	for (int j = 0; j < chunkNumber; j++) { chunks[j].memory = new char[1]; chunks[j].size = 0; }

	

	int lowerbound = 0;
	int upperbound = chunkSize;
	if (!parallel)
	{
		CURL *curl;
		CURLcode res;
		curl = curl_easy_init();
		for (int j = 0; j < chunkNumber; j++)
		{

			if (curl) {
				curl_easy_setopt(curl, CURLOPT_URL, url);
				std::stringstream range;
				range << lowerbound << "-" << upperbound - 1; //specify bytes range
				curl_easy_setopt(curl, CURLOPT_RANGE, (range.str()).c_str());
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)(chunks + j)); //write to the #j chunk
				res = curl_easy_perform(curl); //GET
			}
			lowerbound = upperbound;
			upperbound += chunkSize;
		}
		curl_easy_cleanup(curl);
	}
	else
	{

		std::thread* threads = new std::thread[chunkNumber];
		for (int j = 0; j < chunkNumber; j++) //create threads
		{
			std::stringstream range;
			range << lowerbound << "-" << upperbound - 1; //specify bytes range
			threads[j] = std::thread(get_chunk, (void *)url.c_str(), range.str(), j, chunks); //pass func and args
			lowerbound = upperbound;
			upperbound += chunkSize;
		}
		cout << "created all\n";
		for (int j = 0; i < chunkNumber; j++) //main waits for threads to finish
		{
			cout << "Main joining thread " << j << "\n";
			threads[j].join();
		}
		delete threads;
	}
	for (int j = 0; j < chunkNumber; j++) //write to file
	{
		fwrite(chunks[j].memory, sizeof(char), chunks[j].size, outputFile);
	}

	for (int j = 0; j < chunkNumber; j++)
		delete chunks[j].memory;
	delete chunks;
	fclose(outputFile);
	cout << "Happy end!\n";
	return 0;
}