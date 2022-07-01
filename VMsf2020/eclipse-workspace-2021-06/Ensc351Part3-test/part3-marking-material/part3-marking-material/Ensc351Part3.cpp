/* Ensc351Part3-test.cpp -- October -- Copyright 2019 Craig Scratchley */
#include <stdlib.h> // EXIT_SUCCESS
#include <sys/socket.h>
#include <pthread.h>
#include <thread>
#include <chrono>         // std::chrono::
#include "myIO.h"
#include "SenderY.h"
#include "ReceiverY.h"
#include "Medium.h"
#include "VNPE.h"
#include "AtomicCOUT.h"
#include "posixThread.hpp"

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE ESCN351Part3
#include <vector>
#include <sstream>
#include <fstream>
#include <boost/test/included/unit_test.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/algorithm/string/join.hpp>
using namespace boost::algorithm;

#include <boost/date_time.hpp>
#include <boost/thread/thread.hpp>

using namespace boost::algorithm;

using namespace std;


/* This program can be used to test your changes to myIO.cpp
 *
 * Put this project in the same workspace as your Ensc351Part2SolnLib and Ensc351 library projects,
 * and build it.
 *
 * With two created threads for a total of 3 threads, the output that I get is:

 *
 */
 
#include <stdio.h>
#include <string.h>
#include <iostream>

static int daSktPr[2];	  // Descriptor Array for Socket Pair
cpu_set_t cpuSet;
int myCpu=0;

float Mark=0;
ofstream outfile;	

void threadT42Func(void)
{
    PE(sched_setaffinity(0, sizeof(cpuSet), &cpuSet)); // set processor affinity for current thread

	try{
		PE_NOT(myWrite(daSktPr[1], "ijkl", 5), 5);
		int RetVal = PE(myTcdrain(daSktPr[1])); // will block until myClose
		
		cout << "T42. RetVal: " << RetVal << endl;
		Mark = RetVal == 0 ? 5 : 0;  outfile << "PE_NOT(myWrite(daSktPr[1], ijkl, 5), 5)-" << Mark << "\n";
		//cout << "Mark: " << Mark << endl;		
	}
	catch(int e) {	}
	
}

void threadT32Func(void)
{
    PE(sched_setaffinity(0, sizeof(cpuSet), &cpuSet)); // set processor affinity for current thread

	try{
		PE_NOT(myWrite(daSktPr[0], "1234", 4), 4);
		PE(myTcdrain(daSktPr[0])); // will block until 1st myReadcond

		PE_NOT(myWrite(daSktPr[0], "abcd56789", 10), 10); // don't forget nul termination character
		this_thread::sleep_for (chrono::milliseconds(10));

		PE(myWrite(daSktPr[0], "tsu", 4));
		int RetVal = PE(myClose(daSktPr[0]));
		this_thread::sleep_for (chrono::milliseconds(10));

		cout << "T32. RetVal: " << RetVal << endl;
		//if(RetVal==0) Mark=5; 
		Mark = RetVal == 0 ? 5 : 0;
		outfile << "PE(myWrite(daSktPr[0], tsu, 4))-" << Mark << "\n";
	}
	catch(int e) {	}
	
	//cout << "Mark: " << Mark << endl;	
}

void start() {
	//outfile.open("Marks.txt", std::ios_base::app); // append instead of overwrite
	outfile.open("priority-test-score.txt", std::ios_base::trunc); //overwrite existing 
    CPU_SET(myCpu, &cpuSet);
    PE(sched_setaffinity(0, sizeof(cpuSet), &cpuSet)); // set processor affinity for current thread

	int RetVal=-2;
	char Ba[200];
	try{
		PE(mySocketpair(AF_LOCAL, SOCK_STREAM, 0, daSktPr));
		thread threadT32(threadT32Func);
		this_thread::sleep_for (chrono::milliseconds(10));

		thread threadT42(threadT42Func);  // you can try without this thread too.
		//sched_yield();
		this_thread::sleep_for (chrono::milliseconds(10));

		RetVal = PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0));  // will block until myWrite of 10 characters
		cout << "1. RetVal: " << RetVal << " Ba: " << Ba << endl;	
		//if(RetVal==14) Mark=5;
		Mark = RetVal == 14 ? 5 : 0;
		outfile << "PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0))_1-" << Mark << "\n";
		//if(strcmp(Ba,"1234abcd56789")==0) Mark=5; 
		Mark = strcmp(Ba,"1234abcd56789")==0 ? 5 : 0; 
		outfile << "strcmp(Ba,1234abcd56789)==0-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;

		// this_thread::sleep_for (chrono::milliseconds(30));  // added to test Piazza post
		RetVal = PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0)); // will block until myClose in T32
		threadT32.join();
		threadT42.join(); // only needed if you created the thread above.

		cout << "2. RetVal: " << RetVal << " Ba: " << Ba << endl;
		//if(RetVal==4) Mark=5;  
		Mark = RetVal == 4 ? 5 : 0;
		outfile << "PE(myReadcond(daSktPr[1], Ba, 200, 12, 0, 0))_2-" << Mark << "\n";
		//if(strcmp(Ba,"tsu")==0) Mark=5; 
		Mark = (strcmp(Ba, "tsu") == 0) ? 5 : 0;
		outfile << "strcmp(Ba,tsu)==0-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}

	try{
		PE(myClose(daSktPr[1]));
		RetVal = myWrite(daSktPr[1], Ba, 200);
		cout << "3. RetVal: " << RetVal << " errno: " << strerror(errno) << endl;
		//cout << " " << strerror(errno) << endl;
		//if(RetVal==-1) Mark=1;  
		Mark = RetVal == -1 ? 1 : 0; 
		outfile << "myWrite(daSktPr[1], Ba, 200)-" << Mark << "\n";
		//if(strcmp(strerror(errno),"Bad file descriptor")==0) Mark=1; 
		Mark = (strcmp(strerror(errno),"Bad file descriptor")==0) ? 1 : 0;
		outfile << "(strcmp(strerror(errno),Bad file descriptor)==0)_1-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}
	try{
		RetVal = myRead(daSktPr[1], Ba, 200);
		cout << "4. RetVal: " << RetVal << endl;
		//if(RetVal==0) Mark=1;  
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myRead(daSktPr[1], Ba, 200)-" << Mark << "\n";	
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}

	try{
		RetVal = myClose(daSktPr[1]);
		cout << "5. RetVal: " << RetVal << endl;
		//if(RetVal==0) Mark=1; 
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myClose(daSktPr[1])_1-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}

	try{
		RetVal = myClose(daSktPr[1]);
		cout << "6. RetVal: " << RetVal << " errno: " << strerror(errno) << endl;
		//cout << " " << strerror(errno) << endl;
		//if(RetVal==-1) Mark=1;
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myClose(daSktPr[1])_2-" << Mark << "\n";
		//if(strcmp(strerror(errno),"Bad file descriptor")==0) Mark=1;
		Mark = (strcmp(strerror(errno),"Bad file descriptor")==0) ? 1 : 0;
		outfile << "(strcmp(strerror(errno),Bad file descriptor)==0)_2-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
		}
	catch(int e) {	}

	try{
		RetVal = myClose(5000);
		cout << "7. RetVal: " << RetVal << " errno: " << strerror(errno) << endl;
		//if(RetVal==-1) Mark=1; 
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myClose(5000)-" << Mark << "\n";
		//if(strcmp(strerror(errno),"Bad file descriptor")==0) Mark=1;
		Mark = (strcmp(strerror(errno),"Bad file descriptor")==0) ? 1 : 0;
		outfile << "(strcmp(strerror(errno),Bad file descriptor)==0)_3-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}

	try{
		RetVal = myRead(5000, Ba, 200);
		cout << "8. RetVal: " << RetVal << " errno: " << strerror(errno) << endl;
		//if(RetVal==-1) Mark=1;  
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myRead(5000, Ba, 200)-" << Mark << "\n";
		//if(strcmp(strerror(errno),"Bad file descriptor")==0) Mark=1; 
		Mark = (strcmp(strerror(errno),"Bad file descriptor")==0) ? 1 : 0; 
		outfile << "(strcmp(strerror(errno),Bad file descriptor)==0)_4-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}

	try{
		RetVal = myWrite(5000, Ba, 200);
		cout << "9. RetVal: " << RetVal << " errno: " << strerror(errno) << endl;	
		//if(RetVal==-1) Mark=1;  
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myWrite(5000, Ba, 200)-" << Mark << "\n";
		//if(strcmp(strerror(errno),"Bad file descriptor")==0) Mark=1;  
		Mark = (strcmp(strerror(errno),"Bad file descriptor")==0) ? 1 : 0;
		outfile << "(strcmp(strerror(errno),Bad file descriptor)==0)_5-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}

	try{
		RetVal = myTcdrain(5000);
		cout << "10. RetVal: " << RetVal << " errno: " << strerror(errno) << endl;		
		//if(RetVal==-1) Mark=1;  
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myTcdrain(5000)-" << Mark << "\n";
		//if(strcmp(strerror(errno),"Bad file descriptor")==0) Mark=1;
		Mark = (strcmp(strerror(errno),"Bad file descriptor")==0) ? 1:0; 
		outfile << "(strcmp(strerror(errno),Bad file descriptor)==0)_6-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
	}
	catch(int e) {	}

	try{
		RetVal = myReadcond(5000, Ba, 200, 12, 0, 0);
		cout << "11. RetVal: " << RetVal << " errno: " << strerror(errno) << endl;			
		//if(RetVal==-1) Mark=1; 
		Mark = RetVal == -1 ? 1 : 0;
		outfile << "myReadcond(5000, Ba, 200, 12, 0, 0)-" << Mark << "\n";
		//if(strcmp(strerror(errno),"Bad file descriptor")==0) Mark=1; 
		Mark = (strcmp(strerror(errno),"Bad file descriptor")==0) ? 1 : 0;
		outfile << "(strcmp(strerror(errno),Bad file descriptor)==0)_7-" << Mark << "\n";
		cout << "Mark: " << Mark << endl;
		}
	catch(int e) {	}

	//outfile << Mark << "\n";
	//saveMark(outFile);
	cout << "DONE.";
	outfile.close();
}

boost::posix_time::time_duration timeout = boost::posix_time::milliseconds(10000);
BOOST_AUTO_TEST_CASE( priority_test )
{
	boost::thread thrd(start);
	thrd.timed_join(timeout);
}

///////////////////////////////////////////////////////////////////

enum  {Term1, Term2};
enum  {TermSkt, MediumSkt};

//static int daSktPr[2];	  //Socket Pair between term1 and term2
static int daSktPrT1M[2];	  //Socket Pair between term1 and medium
static int daSktPrMT2[2];	  //Socket Pair between medium and term2

std::string inputTextFile="mailcap2";
std::string sendingMode="CRC";

std::vector<char> ReadAllBytes(char const* filename)
{
	ifstream ifs(filename, ios::binary|ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	std::vector<char>  result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read(&result[0], pos);

	return result;
}

	
struct Stats{
public:
	std::vector<int> ACKs;
	std::vector<int> NAKs;
	int ACKnum=0, NAKnum=0;
	int lines=0;
};

int check(std::vector<int> expected, std::vector<int> user)
{
	int NotMatched=0;
	int matched=0;
	for(int i=0;i<expected.size() && i<user.size();i++)
	{
		if(expected[i] == user[i])
			matched++;
		else
			NotMatched++;
	}
	if(NotMatched>0)
		BOOST_ERROR("Error: "+ std::to_string(NotMatched) + " Acks/Naks not matched.");
	if(	expected.size()!=user.size())
		BOOST_ERROR("Error: Number of Acks/Naks not matched.");
	return matched;
}

Stats getStats(const std::string& outFile)
{
	Stats stat;
	std::vector<char> allBytes = ReadAllBytes(outFile.c_str());
	for(int i=0;i<allBytes.size();i++)
	{
		if(allBytes[i]==6){ //cout << "ACK: " << i << endl;
			stat.ACKnum++;
			stat.ACKs.push_back(i);
		}
		if(allBytes[i]==21){ //cout << "NAK: " << i << endl;
			stat.NAKnum++;
			stat.NAKs.push_back(i);
		}
	}

	//////
	std::ifstream expectedFile(outFile);
	std::string strExpected;
	while (std::getline(expectedFile, strExpected))
		stat.lines++;

	return stat;
}

int compareFiles2(const std::string& expected, const std::string& user)
{
	std::ifstream expectedFile(expected);
	std::ifstream userFile(user);
	std::string strExpected, strUser;
	int linesNotMatched=0; int linesMatched=0;
	while (std::getline(expectedFile, strExpected) && std::getline(userFile, strUser))
	{
		if(strExpected==strUser)
			linesMatched++;
		else
			linesNotMatched++;
	}
	if(linesNotMatched>0)
		BOOST_ERROR("Error: "+ std::to_string(linesNotMatched) + " lines not matched in '"+user+"'");
	if(!expectedFile.eof() || !userFile.eof())
		BOOST_ERROR("Error: "+ expected + " and "+user+" output files not matched.");
	return linesMatched;
}

bool compareFiles(const std::string& expectedFile, const std::string& userFile)
{
    std::ifstream file1(expectedFile.c_str());
    std::ifstream file2(userFile.c_str());
    std::istreambuf_iterator<char> first1(file1);
    std::istreambuf_iterator<char> first2(file2);
	std::istreambuf_iterator<char> end;
    while(first1 != end && first2 != end)
    {
        if(*first1 != *first2) return false;
        ++first1;
        ++first2;
    }
    return (first1 == end) && (first2 == end);
}

void saveMark()
{
	///// report the mark (one test case each time)!
	std::string fileName("Marks2.txt");
	ifstream infile;
	infile.open (fileName.c_str());
	float preMark=0;
	infile >> preMark;
	infile.close();
	ofstream outFile;
	outFile.open(fileName.c_str());
	 outfile << Mark+preMark;
	outFile.close();

	cout << " Marks2: " << Mark << endl;
	//Mark=0;
}

//bool check(const std::string& inputFile, const std::string& mode, const std::string& expectedLogFile, const std::string& expectedTransferredFile, float mark=0)
//{
	//// delete the output files to make sure the previous runs do not leak here
	//remove( "xmodemData" );
	//remove( "transferredFile" );

	////outfile.open("Marks_.txt", std::ios_base::app); // append instead of overwrite

	//std::cout <<  "\n\n*** " << boost::unit_test::framework::current_test_case().p_name << " ***\n\n";

	////cout << "before" << endl;
	//boost::thread thrd(start2, inputFile, mode);	
	//thrd.timed_join(timeout);
	//cout << "after" << endl;

	//Stats userStat = getStats("xmodemData");
	//Stats expectedStat = getStats(expectedLogFile);

	//int ACKsmatched = check(userStat.ACKs, expectedStat.ACKs);
	////cout << ACKsmatched << endl;
	//int NAKsmatched = check(userStat.NAKs, expectedStat.NAKs);
	//Mark=(((float)ACKsmatched/expectedStat.ACKnum)*(mark/4)+((float)NAKsmatched/expectedStat.NAKnum)*(mark/4));

	//int linesMatched1 = compareFiles2(expectedLogFile,"xmodemData");
	//Mark=((float)linesMatched1/expectedStat.lines)*(mark/4);

	//Stats expectedStat2 = getStats(expectedTransferredFile);
	//int linesMatched2 = compareFiles2(expectedTransferredFile, "transferredFile");
	//Mark=((float)linesMatched2/expectedStat2.lines)*(mark);

	//cout << " Marks2: " << Mark << endl;
	//outfile << Mark;
	////saveMark();
//}

// smaller input file
//BOOST_AUTO_TEST_CASE( test1_CRC ){
	//check("test1", "CRC", "expectedXmodemData_CRC1", "expectedTransferredFile1", 10);
//}

// smaller input file
//BOOST_AUTO_TEST_CASE( test1_CS ){
	//check("test1", "CS", "expectedXmodemData_CS", "expectedTransferredFile1", 10);
//}

// 17.5 pts
//BOOST_AUTO_TEST_CASE( test2_CRC ){
	//boost::thread thrd(start2, "test2", "checksum", mode);
	//thrd.timed_join(timeout);

	//check("test2", "CRC", "expectedXmodemData_CRC2", "expectedTransferredFile2", 10);
//}

// 17.5 pts (no difference with CRC)
//BOOST_AUTO_TEST_CASE( test2_CS ){
	///// mark previous
	//check("test2", "checksum", "expectedXmodemData_CS2", "expectedTransferredFile2", 10);

	///// next
	//boost::thread thrd(start2, "test2", "CRC", true);
	//thrd.timed_join(timeout);
//}
