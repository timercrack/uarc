/*
 * TableFile.cpp
 *
 *  Created on: 2014-9-24
 *      Author: root
 */

#include "TableFile.h"

namespace Uarc {

namespace Protocol {

namespace IEC104 {

TableFile::TableFile(const std::string &name, char delimiter, char hidden)
			:m_open(false), m_name(name), m_delimiter(delimiter), m_hidden(hidden){
	// TODO Auto-generated constructor stub
	if (open())
	{
		m_open = true;
	}else {
		m_name = "";
	}
}

TableFile::~TableFile() {
	// TODO Auto-generated destructor stub
	if (m_open)
	{
		m_file.close();
		m_open = false;
	}
}

void TableFile::trimLeft(std::string &data)
{
	data.erase(data.begin(),
			std::find_if(data.begin(), data.end(), not1(std::ptr_fun(::isspace))));
}

void TableFile::trimRight(std::string &data)
{
	data.erase(std::find_if(data.rbegin(), data.rend(), not1(std::ptr_fun(::isspace))).base(),
			data.end());
}

void TableFile::trim(std::string &data)
{
	trimLeft(data);
	trimRight(data);
}

bool TableFile::open()
{
	m_file.open(m_name.c_str(), std::ios::in|std::ios::out|std::ios::app);
	if (!m_file)
	{
		return false;
	}

	return load();
}

bool TableFile::load()
{
	std::string buffer;

	m_file.seekg(0, std::ios::beg);
	for (int i = 0; ; ++i)
	{
		buffer.clear();
		if (!getline(m_file, buffer))
		{
			break;
		}

		trim(buffer);
		if (!buffer.size() || buffer[0] == m_hidden)
		{
			--i;
			continue;
		}

		m_buffer.push_back(std::vector<std::string>());
		int pos = 0;

		for (int j = 0; ; ++j)
		{
			if (!buffer.size())
			{
				break;
			}

			if (j != 0)
			{
				buffer.erase(0, 1);//删除分割符
			}
			int pos2 = buffer.find_first_of(m_delimiter);
			if (pos2 == -1)
			{
				pos2 = buffer.size();
			}
			std::string item = buffer.substr(pos, pos2);
			trim(item);
			buffer.erase(pos, pos2);

			m_buffer[i].push_back(item);
		}
	}

	return true;
}

bool TableFile::save()
{
	m_file.close();
	m_file.open(m_name.c_str(), std::ios::out|std::ios::trunc);
	m_file.seekg(0, std::ios::beg);
	for (int i = 0; i < (int)m_buffer.size(); ++i)
	{
		for (int j = 0; j < (int)m_buffer[i].size(); ++j)
		{
			m_file<<m_buffer[i][j];
			if (j+1 != (int)m_buffer[i].size())
				m_file<<m_delimiter;
		}
		if (i+1 != (int)m_buffer.size())
			m_file<<std::endl;
	}
	m_file.close();

	return open();
}

void TableFile::creatRef()
{
	m_rowMap.clear();
	m_colMap.clear();
	for (int i = 0; i < (int)m_buffer.size(); ++i)
	{
		for (int j = 0; j < (int)m_buffer[i].size(); ++j)
		{
			m_rowMap.insert(std::pair<std::string, int>(m_buffer[i][j], i));
			m_colMap.insert(std::pair<std::string, int>(m_buffer[i][j], j));
		}
	}
}

bool TableFile::find(const std::string &data, std::map<int, int> &map,
			int row, int column)
{
	bool res = false;

	std::multimap<std::string, int>::iterator itRow;
	std::multimap<std::string, int>::iterator itColumn;

	int countRow = m_rowMap.count(data);
	int countColumn = m_colMap.count(data);

	itRow = m_rowMap.find(data);
	itColumn = m_colMap.find(data);

	if (row >= 0 && row < getRowCount())//给定行号
	{
		int columnCount = getColumnCount(row);
		if (column >= 0 && column <columnCount)//给定行号&给定列号
		{
			if (m_buffer[row][column] == data)
			{
				map.insert(std::pair<int, int>(row, column));
				return true;
			}else {
				return false;
			}
		}
		for (int i = 0; i < countColumn; ++i, ++itColumn)
		{
			if (m_buffer[row][itColumn->second] == data)
			{
				map.insert(std::pair<int, int>(row, itColumn->second));
				res = true;
			}
		}
		return res;
	}

	if (column != -1)//给定列号
	{
		for (int i = 0; i < countRow; ++i, ++itRow)
		{
			int columnCount = getColumnCount(itRow->second);
			if (column >= 0 && column < columnCount)
			{
				if (m_buffer[itRow->second][column] == data)
				{
					map.insert(std::pair<int, int>(itRow->second, column));
					res = true;
				}
			}
		}
		return res;
	}

	for (int i = 0; i < countRow; ++i, ++itRow)
	{
		for (int j = 0; j < countColumn; ++j, ++itColumn)
		{
			if (m_buffer[itRow->second][itColumn->second] == data)
			{
				map.insert(std::pair<int, int>(itRow->second, itColumn->second));
				res = true;
			}
		}
	}

	return res;
}

void TableFile::setDelimiter(const char &delimiter)
{
	m_delimiter = delimiter;
}

char TableFile::getDelimiter()
{
	return m_delimiter;
}

void TableFile::setHidden(const char &hidden)
{
	m_hidden = hidden;
}

char TableFile::getHidden()
{
	return m_hidden;
}

int TableFile::getRowCount()
{
	return m_buffer.size();
}

int TableFile::getColumnCount(int row)
{
	return m_buffer[row].size();
}

bool TableFile::put(int row, int column, const std::string &data)
{
	if (row > (int)m_buffer.size()-1)
	{
		return false;
	}
	if (column > (int)m_buffer[row].size()-1)
	{
		return false;
	}
	m_buffer[row][column] = data;
	return true;
}

bool TableFile::get(int row, int column, std::string &data)
{
	if (row > (int)m_buffer.size()-1)
	{
		return false;
	}
	if (column > (int)m_buffer[row].size()-1)
	{
		return false;
	}
	data = m_buffer[row][column];
	return true;
}

std::string TableFile::value(int row, int column)
{
	if (row > (int)m_buffer.size()-1)
	{
		return "";
	}
	if (column > (int)m_buffer[row].size()-1)
	{
		return "";
	}

	return m_buffer[row][column];
}

bool TableFile::insertRow(int row)
{
	if (row > (int)m_buffer.size())
	{
		return false;
	}else if (row == (int)m_buffer.size())
	{
		m_buffer.push_back(std::vector<std::string>());
		return true;
	}
	m_buffer.insert(m_buffer.begin()+row, std::vector<std::string>());

	return true;
}

bool TableFile::removeRow(int row)
{
	if (row > (int)m_buffer.size()-1)
	{
		return false;
	}
	m_buffer.erase(m_buffer.begin()+row);

	return true;
}

bool TableFile::insert(int row, int column, const std::string &data)
{
	if (row > (int)m_buffer.size()-1)
	{
		return false;
	}
	if (column > (int)m_buffer[row].size())
	{
		return false;
	}else if (column == (int)m_buffer[row].size())
	{
		m_buffer[row].push_back(data);
		return true;
	}

	m_buffer[row].insert(m_buffer[row].begin()+column, data);
	return true;
}

bool TableFile::remove(int row, int column)
{
	if (row > (int)m_buffer.size()-1)
	{
		return false;
	}
	if (column > (int)m_buffer[row].size()-1)
	{
		return false;
	}
	m_buffer[row].erase(m_buffer[row].begin()+column);
	return true;
}

void TableFile::show()
{
	std::cout<<"=================="<<m_name<<"=================="<<std::endl;
	for (int i = 0; i < (int)m_buffer.size(); ++i)
	{
		for (int j = 0; j < (int)m_buffer[i].size(); ++j)
		{
			std::cout<<m_buffer[i][j];
			if (j+1 != (int)m_buffer[i].size())
				std::cout<<m_delimiter;
		}
		std::cout<<std::endl;
	}
	std::cout<<"================================================"<<std::endl;
}

}//namespace IEC104

}//namespace Protocol

}//namespace Uarc
