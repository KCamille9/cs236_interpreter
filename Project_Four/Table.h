#ifndef TABLE_H
#define TABLE_H

#include <iterator>  
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <set>
#include "DatalogProgram.h"
#include "Schemes.h"
#include "Rules.h"

using namespace std;


//class Row : public vector<string> {};
//class Header : public vector<string> {};

class Table 
{
public:
	string name;
	vector<string> header;
	vector<vector<string>> rows;



	Table() {}

	void toPrint(QueryItem query) 
	{
		cout << query.toString();

		if (rows.size() != 0) 
		{
			cout << "Yes(" << to_string(rows.size()) << ")";

			if (header.size() != 0) 
			{
				cout << endl;
				for (unsigned int row = 0; row < rows.size(); row++) 
				{
					cout << " ";
					for (unsigned int col = 0; col < header.size() - 1; col++) 
					{
						cout << " " << header.at(col) << "=" << rows.at(row).at(col) << ",";
					}
					cout << " " << header.at(header.size() - 1) << "=" << rows.at(row).at(header.size() - 1) << endl;
				}
			}
			else 
			{
				// no params to print
				cout << endl;
			}
		}
		else 
		{
			// no matches
			cout << "No" << endl;
		}
	}

	string printRule(RuleItem rule) {
		string result = "";
		result += rule.headPredicate.toString("rule");
		result += " :- ";
		for (unsigned int i = 0; i < rule.predicates.size(); i++) {
			if (i > 0) {
				result += ",";
			}
			result += rule.predicates.at(i).toString("rule");

		}
		result += ".";
		return result;
	}

	string toStringHe(vector<string> header, vector<string> header2)
	{
		string result = "  ";

		for (unsigned int i = 0; i < header.size(); i++) 
		{
			if (i > 0) {
				result += ", ";
			}
			result += header2.at(i);
			result += "=";
			result += header.at(i);
		}
		return result;
	}


	void printRuleAdditions(vector<vector<string>> diff, RuleItem rule, vector<string> header) 
	{
		cout << printRule(rule) << endl;

		for (unsigned int i = 0; i < diff.size(); i++) 
		{
			cout << toStringHe(diff.at(i), header) << endl;
			//cout << diff.at(i).toStringHe(header) << endl;
		}
	}

	void addRow(vector<string> row) 
	{
		vector<string> currentRow;

		for (unsigned int j = 0; j < row.size(); j++) 
		{
			currentRow.push_back(row.at(j));
		}

		rows.push_back(currentRow);
	}

	void initializeRow(vector<DLString*>* row) 
	{
		vector<string> currentRow;

		for (unsigned int j = 0; j < row->size(); j++) 
		{
			currentRow.push_back(row->at(j)->value);
		}
		rows.push_back(currentRow);
	}

	// Gets header
	Table(Schemes* schemes, int i) 
	{
		// grab the headPredicate for the name of the table
		name = schemes->listOfSchemes->at(i)->id->value;

		// get the column names for the table
		header = schemes->getParameters(i);
	}

	// select is the where clause - keeps rows that have the condition
	// param to match - string is col1
	// make another function with a string and a colname
	// will call select on each one in the query list SK(A, B, c, D) go select(A,B) select(A,C), seleect(A,D)
	Table select(int col, string paramToMatch) 
	{
		Table newRelation;

		for (unsigned int row = 0; row < rows.size(); row++) 
		{
			if (rows.at(row).at(col) == paramToMatch) 
			{
				// add row to the new table
				newRelation.rows.push_back(rows.at(row));
			}
		}

		newRelation.header = header;
		newRelation.name = name;

		return newRelation;
	}

	Table select(int col1, int col2) 
	{
		Table newRelation;
		for (unsigned int row = 0; row < rows.size(); row++) 
		{
			if (rows.at(row).at(col1) == rows.at(row).at(col2)) 
			{
				// add row to the new table
				newRelation.rows.push_back(rows.at(row));
			}
		}

		newRelation.header = header;
		newRelation.name = name;
		return newRelation;
	}

	Table project(QueryItem query) 
	{
		Table newRelation;
		//newRlation has same name as previous

		// init empty rows
		for (unsigned int a = 0; a < rows.size(); a++) 
		{
			vector<string> row;
			newRelation.rows.push_back(row);
		}

		map<string, int> seen;

		//for loop keep the ones in the order given;
		for (unsigned int i = 0; i < query.parameters.size(); i++) 
		{
			// if it is a constant erase it from the rows and header
			if (query.parameters.at(i).find("'") == string::npos && seen.find(query.parameters.at(i)) == seen.end()) 
			{
				seen[query.parameters.at(i)] = 1;
				newRelation.header.push_back(query.parameters.at(i));

				for (unsigned int j = 0; j < rows.size(); j++) 
				{
					newRelation.rows.at(j).push_back(rows.at(j).at(i));
				}
			}
		}

		// get only unique rows
		sort(newRelation.rows.begin(), newRelation.rows.end());
		newRelation.rows.erase(unique(newRelation.rows.begin(), newRelation.rows.end()), newRelation.rows.end());

		// assign the varaibles to the new table and return
		newRelation.name = name;
		return newRelation;
	}

	Table rename(QueryItem query) 
	{
		Table newRelation;

		for (unsigned int i = 0; i < header.size(); i++) 
		{
			header.at(i) = query.parameters.at(i);
		}
		return newRelation;
	}

	// gets vector pairs of column mappings
	vector<vector<int>> getJoinColumnsToKeep(vector<string> headerToJoin) 
	{
		vector<vector<int>> sameColumns;

		for (unsigned int col = 0; col < header.size(); col++) 
		{
			for (unsigned int colToJoin = 0; colToJoin < headerToJoin.size(); colToJoin++) 
			{
				if (header.at(col) == headerToJoin.at(colToJoin)) 
				{
					// add the pair of same column
					vector<int> pair;
					pair.push_back(col);
					pair.push_back(colToJoin);
					sameColumns.push_back(pair);
				}
			}
		}

		return sameColumns;
	}

	// Gets the header of a join operation
	vector<string> getJoinedHeader(vector<string> header, vector<string> headerToJoin, vector<int>& columnsToKeep)
	{
		for (unsigned int colToJoin = 0; colToJoin < headerToJoin.size(); colToJoin++) 
		{
			if (find(columnsToKeep.begin(), columnsToKeep.end(), colToJoin) != columnsToKeep.end()) 
			{
				header.push_back(headerToJoin.at(colToJoin));
			}
		}

		return header;
	}

	// utility function to return a vector full of indices (columns) in the 2nd table to keep
	vector<int> getJoinIndices(vector<string> row, vector<string> rowToJoin, vector<vector<int>>& columnsToKeep)
	{
		vector<int> colIndices;

		for (unsigned int column = 0; column < rowToJoin.size(); column++) 
		{
			bool joinedCol = false;

			for (unsigned int pair = 0; pair < columnsToKeep.size(); pair++) 
			{
				if (column == (unsigned)columnsToKeep.at(pair).at(1)) 
				{
					joinedCol = true;
				}
			}

			if (!joinedCol) 
			{
				colIndices.push_back(column);
			}
		}
		return colIndices;
	}

	bool satisfies(vector<string> row, vector<string> rowToJoin, vector<vector<int>>& columnsToKeep)
	{
		// for each matching pair of columns in the join
		for (unsigned int pair = 0; pair < columnsToKeep.size(); pair++) 
		{
			int col1 = columnsToKeep.at(pair).at(0);
			int col2 = columnsToKeep.at(pair).at(1);
			// if the two columns are NOT the same

			if (!(row.at(col1) == rowToJoin.at(col2))) 
			{
				return false;
			}
		}
		// all columns to match were matched
		return true;
	}

	// Gets the rows of a joined operation
	vector<string> joined(vector<string> row, vector<string> rowToJoin, vector<int>& columnsToKeep)
	{
		for (unsigned int colToJoin = 0; colToJoin < rowToJoin.size(); colToJoin++) 
		{
			if (find(columnsToKeep.begin(), columnsToKeep.end(), colToJoin) != columnsToKeep.end()) 
			{
				row.push_back(rowToJoin.at(colToJoin));
			}
		}
		return row;
	}


	// Joins two columns together based on shared column names
	Table join(Table toJoin, int querySize = 2) 
	{
		Table joinResult;
		vector<vector<int>> columnsToKeep = getJoinColumnsToKeep(toJoin.header); //vector string?
		vector<int> columnJoinIndices = getJoinIndices(header, toJoin.header, columnsToKeep);

		joinResult.header = getJoinedHeader(header, toJoin.header, columnJoinIndices);
		joinResult.name = name;

		for (vector<string> row : rows) 
		{
			for (vector<string> rowToJoin : toJoin.rows) 
			{
				if (satisfies(row, rowToJoin, columnsToKeep)) 
				{
					joinResult.rows.push_back(joined(row, rowToJoin, columnJoinIndices));
				}
			}
		}

		if (rows.size() == 0 && querySize == 1) 
		{
			joinResult.rows = toJoin.rows;
		}
		else if (toJoin.rows.size() == 0 && querySize == 1) 
		{
			joinResult.rows = rows;
		}

		return joinResult;
	}

	// function for head predicate column
	vector<vector<int>> getJoinColumnsToKeep(vector<string> header, vector<string> headerToJoin) 
	{
		vector<vector<int>> sameColumns;
		for (unsigned int col = 0; col < header.size(); col++) 
		{
			for (unsigned int colToJoin = 0; colToJoin < headerToJoin.size(); colToJoin++) 
			{
				if (header.at(col) == headerToJoin.at(colToJoin)) 
				{
					// add the pair of same column
					vector<int> pair;
					pair.push_back(col);
					pair.push_back(colToJoin);
					sameColumns.push_back(pair);
				}
			}
		}
		return sameColumns;
	}

	// keep only the columns needed for the head predicate
	Table filter(QueryItem head, Table joined) 
	{
		Table newRelation;
		newRelation.name = head.table;

		// keep the schema for the headPredicate
		for (string param : head.parameters) 
		{
			newRelation.header.push_back(param);
		}
		// Get the mapping of the columns we need
		vector<vector<int>> columnsToKeep = getJoinColumnsToKeep(head.parameters, header);

		for (vector<string> row : joined.rows) 
		{
			vector<string> filtered;

			for (unsigned int i = 0; i < columnsToKeep.size(); i++) 
			{
				// this should push the columns back in the right order
				filtered.push_back(row.at(columnsToKeep.at(i).at(1)));
			}
			newRelation.rows.push_back(filtered);
		}
		return newRelation;
	}

	bool unionOp(Table ruleTable, RuleItem rule) 
	{
		// TODO: do I need to print "Rule Evaluation" and "Query Evaluation"???
		// remove this if we go with the wiki's format (also this is slow) at least 0(nlogn)
		// Get diff between vectors and print

		std::sort(ruleTable.rows.begin(), ruleTable.rows.end());
		set<vector<string>> sRule(ruleTable.rows.begin(), ruleTable.rows.end());
		ruleTable.rows.assign(sRule.begin(), sRule.end());

		

		std::sort(rows.begin(), rows.end());
		std::vector<vector<string>> diff;
		std::set_difference(ruleTable.rows.begin(), ruleTable.rows.end(), rows.begin(), rows.end(),
		std::inserter(diff, diff.begin()));

		printRuleAdditions(diff, rule, header);
		// end rule printouts

		int sizeBefore = rows.size();
		for (vector<string> row : ruleTable.rows) 
		{
			rows.push_back(row);
		}
		// sort the database and delete non-unique additions
		// sort the database and delete non-unique additions
		set<vector<string>> s(rows.begin(), rows.end());
		rows.assign(s.begin(), s.end());


		/*set<Row> s;
		unsigned size = rows.size();
		for (unsigned i = 0; i < size; ++i) s.insert(rows[i]);
		rows.assign(s.begin(), s.end());*/

		/*	sort(rows.begin(), rows.end());
			rows.erase(unique(rows.begin(), rows.end()), rows.end());*/

		int sizeAfter = rows.size();

		// if there were rows added return true
		return (sizeAfter - sizeBefore);
	}

};

// Databases are just a collection of tables
class Database 
{
public:
	map<string, Table> tables;

	int runs;
	bool newAdd;

	// initialize the database with the schemes and facts
	Database(DatalogProgram* program) 
	{
		vector<Scheme*>* schList = program->schemes->listOfSchemes;

		//vector<Fact*>* faList = program->facts->factList;

		// create table names
		for (unsigned int i = 0; i < schList->size(); i++)
		{
			// create table columns and names
			tables[schList->at(i)->id->value] = Table(program->schemes, i);
		}

		for (unsigned int i = 0; i < program->facts->factList->size(); i++) 
		{
			string tableName = program->facts->factList->at(i)->id->value;
			tables[tableName].initializeRow(program->facts->factList->at(i)->listOfStrings);
		}

	}


	//void executeQueries(DatalogProgram* program) {
	//	vector<Table> results;
	//	Table newRelation;
	//	vector<QueryItem> queriesToExecute = program->queries->getQueries();

	//	for (QueryItem query : queriesToExecute) {
	//		newRelation = tables[query.table];
	//		// for each parameter
	//		for (unsigned int i = 0; i < query.parameters.size(); i++) {
	//			// if there is a constant, perform a select
	//			if (query.parameters.at(i).find("'") != string::npos) {
	//				newRelation = newRelation.select(i, query.parameters.at(i));
	//			}
	//			for (unsigned int j = i + 1; j < query.parameters.size(); j++) {
	//				// the two parameters are both the same variable.  Perform a select
	//				if (query.parameters.at(i) == query.parameters.at(j)) {
	//					newRelation = newRelation.select(i, j);
	//				}
	//			}
	//		}
	//		newRelation = newRelation.project(query);
	//		newRelation.toPrint(query);
	//		results.push_back(newRelation);
	//	}
	//}


	Table executeQueries(DatalogProgram* program, vector<QueryItem> queriesToDo = vector<QueryItem>(), bool toPrint = true) 
	{
		Table newRelation;
		vector<QueryItem> queriesToExecute;

		// size of optional param is 0
		if (!queriesToDo.size()) 
		{
			queriesToExecute = program->queries->getQueries();
		}
		else 
		{
			queriesToExecute = queriesToDo;
		}

		for (QueryItem query : queriesToExecute) 
		{
			newRelation = tables[query.table];
			// for each parameter
			for (unsigned int i = 0; i < query.parameters.size(); i++) {
				// if there is a constant, perform a select
				if (query.parameters.at(i).find("'") != string::npos) {
					newRelation = newRelation.select(i, query.parameters.at(i));
				}
				for (unsigned int j = i + 1; j < query.parameters.size(); j++) {
					// the two parameters are both the same variable.  Perform a select
					if (query.parameters.at(i) == query.parameters.at(j)) {
						newRelation = newRelation.select(i, j);
					}
				}
			}
			newRelation = newRelation.project(query);
			if (toPrint) {
				newRelation.toPrint(query);
			}
		}
		// need to put this where the new rule gets added
		return newRelation;
	}

	Table process(QueryItem rule, bool& ruleAdded, DatalogProgram*& program) 
	{
		Table newRelation;
		vector<QueryItem> ruleToDo;
		ruleToDo.push_back(rule);
		newRelation = executeQueries(program, ruleToDo, false);

		if (newAdd == true) 
		{
			ruleAdded = true;
		}

		return newRelation;
	}

	void processRules(DatalogProgram* program) 
	{
		vector<RuleItem> rulesToProcess = program->rules->getRules();
		bool redo = true;
		vector<Table> interimTables;
		Table joined;


		// TODO: Determine whether it is fixed point for each rule or all sets of rules
		bool ruleAdded = true;
		while (redo) 
		{
			redo = false;
			runs++;

			// for each rule
			for (unsigned int i = 0; i < rulesToProcess.size(); i++) 
			{
				interimTables.clear();
				// get the tables from each rule query
				for (unsigned int j = 0; j < rulesToProcess.at(i).predicates.size(); j++) 
				{
					// process the j-th predicate query
					interimTables.push_back(process(rulesToProcess.at(i).predicates.at(j), ruleAdded, program));
				}

				// join the tables from each query
				joined = interimTables.at(0);
				for (unsigned int k = 1; k < interimTables.size(); k++) 
				{
					joined = joined.join(interimTables.at(k), rulesToProcess.at(i).predicates.size());
				}

				// slice out the columns we don't need
				Table renamed = joined.filter(rulesToProcess.at(i).headPredicate, joined);
				// Union the results with the main table and see if a new row was added
				ruleAdded = tables[renamed.name].unionOp(renamed, rulesToProcess.at(i));
				if (ruleAdded) 
				{
					redo = true;
				}
			}
		}
	}




	~Database() {};
};
#endif // !TABLE_H
