#pragma once
#pragma warning(disable:4996)

#define MP3_NAME "1.png"
#define MP3_NAME_UN  "1.png.Huffman"
#define FILE_NAME "input.txt"
#define FILE_NAME_UN  "input.txt.Huffman"

#include <string>
#include <fstream>

#include "HuffmanTree.h"
using namespace std;

struct CharInfo {
	unsigned char _ch;
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
		//io流
		ifstream ifs(file);
		unsigned char ch;
		while ( ifs.get((char&)ch) )
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
			unsigned char _ch;
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
		unsigned char ch;
		while (ifs.get((char&)ch))
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
			unsigned char _ch;
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
		Node* root = tree.GetRoot();

		while ( count>0 )  //用总量来控制循环，防止没有解压完，就退出  while( unsigned char ch = fgetc(fp) )
		{
			unsigned char ch = fgetc(fp);
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
							break;
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
							break;
						root = tree.GetRoot();
					}
				}
			}
		}
		ofs.close();
		fclose(fp);
	}

	void CompressMP3(const char* file)
	{

		string name = file;
		name += ".un";
		ofstream ofs(name);

		//1.统计文件中字符的个数
		//io流
		//ifstream ifs(file);
		//unsigned char ch;
		//while (ifs.get((char&)ch))
		//	_hashInfos[ch]._count++;
		//ifs.close();

		FILE * fp = fopen(file, "rb");
		unsigned int ch;
		ch = fgetc(fp);
		int num = 0;
		while ((int)ch != -1)
		{
			++num;
			_hashInfos[ch]._count++;
			ofs.put((char)ch);
			ch = fgetc(fp);
		}
		fclose(fp);




		//构建huffman树
		CharInfo invalid;
		invalid._count = 0;
		HuffmanTree<CharInfo> tree(_hashInfos, 256, invalid);
		GenerateHuffmanCode(tree.GetRoot());
		Decompression(file);
	}


private:
	CharInfo _hashInfos[256];
};

void MP3()
{
	FileCompress fc;
	fc.CompressMP3(MP3_NAME);
}

void TestFileCompress()
{
	FileCompress fc;
	fc.Compress(FILE_NAME);
}

void UnDecompression()
{
	FileCompress fc;
	fc.UnDecompression(MP3_NAME_UN);
}