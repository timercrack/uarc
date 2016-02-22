/*
 * TableFile.h
 *
 *  Created on: 2014-9-24
 *      Author: root
 */

#ifndef UARC_PROTOCOL_IEC104_TABLEFILE_H_
#define UARC_PROTOCOL_IEC104_TABLEFILE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

namespace Uarc {

namespace Protocol {

namespace IEC104 {

class TableFile {
public:
	TableFile(const std::string &name, char delimiter = ',', char hidden = '#');
	virtual ~TableFile();
	void setDelimiter(const char &delimiter);
	char getDelimiter();
	void setHidden(const char &hidden);
	char getHidden();

	bool load();
	bool save();

	void creatRef();
	bool find(const std::string &data, std::map<int, int> &map,
			int row = -1, int column = -1);

	std::string value(int row, int column);
	int getRowCount();
	int getColumnCount(int row = 0);
	bool put(int row, int column, const std::string &data);
	bool get(int row, int column, std::string &data);
	bool insert(int row, int column, const std::string &data);
	bool remove(int row, int column);
	bool insertRow(int row);
	bool removeRow(int row);
	void show();

	static void trimLeft(std::string &data);
	static void trimRight(std::string &data);
	static void trim(std::string &data);

private:
	bool open();
private:
	std::fstream m_file;

	bool m_open;
	std::string m_name;
	char m_delimiter;
	char m_hidden;

	std::vector<std::vector<std::string> > m_buffer;

	std::multimap<std::string, int> m_rowMap;
	std::multimap<std::string, int> m_colMap;

};

}//namespace IEC104

}//namespace Protocol

}//namespace Uarc

#endif /* UARC_PROTOCOL_IEC104_TABLEFILE_H_ */
