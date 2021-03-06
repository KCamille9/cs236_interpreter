#include <algorithm>
#include "Lexer.h"
#include "DatalogProgram.h"
#include "Table.h"
using namespace std;


int main(int argc, char* argv[]) 
{
	Lexer* parse = new Lexer(argv[1]);
	DatalogProgram* program;

	try 
	{
		//program = new DatalogProgram(*parse);
		////program->outputResults();
		//Database db(program);
		//db.executeQueries(program);
		//delete program;

		program = new DatalogProgram(*parse);
		//program->outputResults();

		Database db(program);
		cout << "Rule Evaluation" << endl;
		db.processRules(program);
		cout << endl << "Schemes populated after " << db.runs << " passes through the Rules." << endl << endl;
		cout << "Query Evaluation" << endl;
		db.executeQueries(program);
		delete program;
	}

	catch (const std::runtime_error&) 
	{
		// Error occured.  Failure will be output where it threw the error
	}
	delete parse;

	return 0;


}