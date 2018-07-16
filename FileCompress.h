#pragma once
#pragma warning(disable:4996)

#include <string>
#include <fstream>

#include "HuffmanTree.h"
using namespace std;

struct CharInfo {
	char _ch;
	size_t _count;
	string _code;

	CharInfo operator+(const CharInfo& info)
	{
		CharInfo ret;
		ret._count = _count + info._count;
		return ret;
	}
	bool operator>(const CharInfo& info)
	{
		return _count > info._count;
	}

	bool operator!=(const CharInfo& info)
	{
		return this->_count != info._count;
	}
};

class FileCompress {
	typedef HuffmanTreeNode<CharInfo> Node;
public:
	FileCompress()
	{
		for (int i = 0; i < 256; i++)
		{
			_hashInfos[i]._ch = i;
			_hashInfos[i]._count = 0;
		}
	}
	void Compress(const char* file)
	{
		//1.统计文件中字符的个数
		ifstream ifs(file);
		char ch;
		while ( ifs.get(ch) )
			_hashInfos[ch]._count++;

		ifs.close();
		
		//构建huffman树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);
		GenerateHuffmanCode(tree.GetRoot());
		Decompression(file);
	}
	void GenerateHuffmanCode(Node* pRoot)
	{
		//构建huffman编码
		if (pRoot == NULL)
			return;
		Node* parent = pRoot->_parent;
		if (parent)
		{
			if (pRoot == parent->_left)
			{
				pRoot->_val._code = parent->_val._code + "1";
			}
			else pRoot->_val._code += parent->_val._code + "0";
		}
		if(pRoot->_left == NULL && pRoot->_right == NULL)
			_hashInfos[pRoot->_val._ch]._code = pRoot->_val._code;
		else
		{
			GenerateHuffmanCode(pRoot->_left);
			GenerateHuffmanCode(pRoot->_right);
		}
	}

//压缩
	void Decompression(const char* file) 
	{
		string name = file;
		name += ".Huffman";
		//为解压输入统计结果
		FILE* fp = fopen(name.c_str(), "wb");
		struct Tem {
			char _ch;
			size_t _count;
		}tem;
		for (int index = 0; index < 255; index++)
		{
			if (_hashInfos[index]._count > 0)
			{
				tem._ch = _hashInfos[index]._ch;
				tem._count = _hashInfos[index]._count;
				fwrite(&tem, sizeof(struct Tem), 1, fp);
			}
		}
		tem._count = 0;
		fwrite(&tem, sizeof(struct Tem), 1, fp);
		//fclose(fp);

		//压缩
		ifstream ifs(file);
		int i = 0;
		char buf = 0;
		char ch = 0;
		while (ifs.get(ch))
		{
			string code = _hashInfos[ch]._code;
			for (size_t j = 0; j < code.size(); j++)
			{
				if (code[j] == '1')
					buf |= (1 << i++);
				else
					buf &= ~(1 << i++);
				if (i >= 8)
				{
					fputc(buf, fp);
					i = 0;
					buf = 0;
				}
			}
		}
		
		if(i > 0 && i < 8)
			fputc(buf, fp);
		ifs.close();
		fclose(fp);
	}

//解压
	void UnDecompression(const char* file)
	{
		//构建huffman树
		struct Tem {
			char _ch;
			size_t _count;
		}tem;

		FILE* fp = fopen(file, "rb");
		while (1)
		{
			fread(&tem, sizeof(struct Tem), 1, fp);
			if (tem._count == 0)
				break;
			_hashInfos[tem._ch]._count = tem._count;
		}


		//构建huffman树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);
		

		//构建解压文件
		string name = file;
		for (int i = 0; i<8; ++i)
			name.pop_back();
		name += ".un";
		ofstream ofs(name);

		//开始解压
		int count = tree.GetRoot()->_val._count;
		char buf;
		Node* root = tree.GetRoot();
		while (char ch = fgetc(fp))
		{
			for (int i = 0; i < 8; i++)
			{
				if ( (ch & (1 << i)) == 0)
				{
					root = root->_right;
					if (root->_left == NULL && root->_right == NULL)
					{
						ofs.put(root->_val._ch);
						--count;
						if (count <= 0)
							return;
						root = tree.GetRoot();
					}
				}
				else
				{
					root = root->_left;
					if (root && root->_left == NULL && root->_right == NULL)
					{
						ofs.put(root->_val._ch);
						--count;
						if (count <= 0)
							return;
						root = tree.GetRoot();
					}
				}
			}
			if (count <= 0)
				break;
		}
		ofs.close();
		fclose(fp);
	}


private:
	CharInfo _hashInfos[256];
};

void TestFileCompress()
{
	FileCompress fc;
	fc.Compress("input.txt");
}

void UnDecompression()
{
	FileCompress fc;
	fc.UnDecompression("input.txt.Huffman");
}