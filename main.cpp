#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <cstdio>
#include <cmath>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

typedef unsigned char uint8;

int dis(uint8 r, uint8 g, uint8 b, RGBQUAD* pal, int i);

int leafs = 0;

struct octNode
{
	long long cnt;//��������µ���������
	long long rSum, gSum, bSum;//��ɫ��������ɫ��������ɫ�����Ҷ�ֵ�ĺ�
	bool isLeaf;//�Ƿ���Ҷ�ӽ��
	int depth;//���ڵ�����
	int pos;
	octNode* child[8];//8���ӽ���ָ������
	octNode* fa, *bro_left, *bro_right;
	octNode() = default;
	octNode(int d) : cnt(1), rSum(0), gSum(0), bSum(0), isLeaf(false), depth(d), fa(NULL), bro_left(NULL), bro_right(NULL) { for (int i = 0; i < 8; i++) child[i] = NULL; }
	octNode(int d, int r, int g, int b) : cnt(1), rSum(r), gSum(g), bSum(b), isLeaf(true), depth(d), fa(NULL), bro_left(NULL), bro_right(NULL) { for (int i = 0; i < 8; i++) child[i] = NULL; }

};

class octTree
{
public:
	octTree() {};
	octTree(int maxColorNum) { maxColors = maxColorNum; root = new octNode(0); }
	~octTree();

	void insertColor(uint8 r, uint8 g, uint8 b);						//����һ����ɫ
	uint8 generatePalette(RGBQUAD* pal);						//���ɵ�ɫ��
private:
	octNode* root;														//�˲����ĸ�
	int colors;															//��ǰ����ɫ����
	int maxColors;														//�����ɫ��
	void reduceOctTree();
	void add2vec(vector<octNode*>& vec, octNode*);
	int getLeaves(octNode*);
};

//�ͷŰ˲������ڴ�ռ�
octTree::~octTree()
{
	//To do....
}

//���˲��������һ�����ص���ɫ
void octTree::insertColor(uint8 r, uint8 g, uint8 b)
{
	//....
	octNode* node = root;
	for (int i = 7; i >= 0; i--) {
		int index = ((r >> i) & 0x1) * 4 + ((g >> i) & 0x1) * 2 + ((b >> i) & 0x1);
		if (node->child[index] == NULL) {
			if (i != 0) {
				node->child[index] = new octNode(node->depth + 1);
			}
			else {
				node->child[index] = new octNode(node->depth + 1, r, g, b);
				this->colors++;
				leafs++;
			}

			node->child[index]->fa = node;
			node->child[index]->pos = index;
			for (int j = index - 1; j >= 0; j--)
				if (node->child[j] != NULL) {
					node->child[index]->bro_left = node->child[j];
					break;
				}
			for (int j = index + 1; j < 8; j++)
				if (node->child[j] != NULL) {
					node->child[index]->bro_right = node->child[j];
					break;
				}
		}
		else {
			node->child[index]->cnt++;
		}
		if (node->child[index]->isLeaf) {
			node->child[index]->rSum += r;
			node->child[index]->gSum += g;
			node->child[index]->bSum += b;
		}
		node = node->child[index];
	}
}

//�������еİ˲�����ѡ��256����ɫ��Ϊ���յĵ�ɫ���е���ɫ
uint8 octTree::generatePalette(RGBQUAD* pal)
{
	//....
	reduceOctTree();
	printf("Leaves: %d\n", getLeaves(root));
	int i = 0;
	queue<octNode*> q;
	q.push(root);
	while (!q.empty()) {
		auto node = q.front();
		q.pop();
		if (node->isLeaf) {
			pal[i].rgbRed = node->rSum / node->cnt;
			pal[i].rgbGreen = node->gSum / node->cnt;
			pal[i].rgbBlue = node->bSum / node->cnt;
			pal[i].rgbReserved = 0;
			i++;
		}
		else {
			for (int i = 0; i < 8; i++) {
				if (node->child[i] != NULL)
					q.push(node->child[i]);
			}
		}
	}
	return i;
}

int octTree::getLeaves(octNode* node) {
	if (node == NULL)
		return 0;
	if (node->isLeaf)
		return 1;
	int c = 0;
	for (int i = 0; i < 8; i++)
		c += getLeaves(node->child[i]);
	return c;
}

/*
ÿһ�ζ��ҵ������������������ٵĽڵ㣬�������ֵܺϲ����ҵ������ֵ����������ٵ�һ������ϲ������û���ֵܽڵ���븸�ڵ�ϲ�ͬʱ�����ڵ�����ΪҶ�ӽڵ㣬ͬʱ��ӵ�Ҷ�ӽڵ㼯����
*/

void octTree::reduceOctTree() {
	vector<octNode*> vec;
	add2vec(vec, this->root);
	while (colors > maxColors) {
		/*vector<octNode*>::iterator it = vec.begin();
		for (auto i = vec.begin(); i != vec.end(); i++) {
			if ((*i)->depth < (*it)->depth || ((*i)->depth == (*it)->depth && (*i)->cnt < (*it)->cnt)) {
				it = i;
			}
		}*/
		int ind = 0;
		for (int i = 0; i < vec.size(); i++)
			if (vec[i]->depth < vec[ind]->depth || (vec[i]->depth == vec[ind]->depth && vec[i]->cnt < vec[ind]->cnt))
				ind = i;
		if (vec[ind]->bro_left != NULL && vec[ind]->bro_right != NULL) {
			if (vec[ind]->bro_left->cnt <= vec[ind]->bro_right->cnt) {
				vec[ind]->isLeaf = false;
				vec[ind]->bro_left->bro_right = vec[ind]->bro_right;
				vec[ind]->bro_right->bro_left = vec[ind]->bro_left;
				vec[ind]->bro_left->cnt += vec[ind]->cnt;
				vec[ind]->bro_left->rSum += vec[ind]->rSum;
				vec[ind]->bro_left->gSum += vec[ind]->gSum;
				vec[ind]->bro_left->bSum += vec[ind]->bSum;
				delete vec[ind];
				vec[ind] = NULL;
				vec.erase(vec.begin() + ind);
				colors--;
			}
			else {
				vec[ind]->isLeaf = false;
				vec[ind]->bro_left->bro_right = vec[ind]->bro_right;
				vec[ind]->bro_right->bro_left = vec[ind]->bro_left;
				vec[ind]->bro_right->cnt += vec[ind]->cnt;
				vec[ind]->bro_right->rSum += vec[ind]->rSum;
				vec[ind]->bro_right->gSum += vec[ind]->gSum;
				vec[ind]->bro_right->bSum += vec[ind]->bSum;
				delete vec[ind];
				vec[ind] = NULL;
				vec.erase(vec.begin() + ind);
				colors--;
			}
		}
		else if (vec[ind]->bro_left == NULL && vec[ind]->bro_right != NULL) {
			vec[ind]->isLeaf = false;
			vec[ind]->bro_right->bro_left = NULL;
			vec[ind]->bro_right->cnt += vec[ind]->cnt;
			vec[ind]->bro_right->rSum += vec[ind]->rSum;
			vec[ind]->bro_right->gSum += vec[ind]->gSum;
			vec[ind]->bro_right->bSum += vec[ind]->bSum;
			delete vec[ind];
			vec[ind] = NULL;
			vec.erase(vec.begin() + ind);
			colors--;
		}
		else if (vec[ind]->bro_left != NULL && vec[ind]->bro_right == NULL) {
			vec[ind]->isLeaf = false;
			vec[ind]->bro_left->bro_right = NULL;
			vec[ind]->bro_left->cnt += vec[ind]->cnt;
			vec[ind]->bro_left->rSum += vec[ind]->rSum;
			vec[ind]->bro_left->gSum += vec[ind]->gSum;
			vec[ind]->bro_left->bSum += vec[ind]->bSum;
			delete vec[ind];
			vec[ind] = NULL;
			vec.erase(vec.begin() + ind);
			colors--;
		}
		else {
			vec[ind]->isLeaf = false;
			vec[ind]->fa->isLeaf = true;
			vec[ind]->fa->cnt += vec[ind]->cnt;
			vec[ind]->fa->rSum += vec[ind]->rSum;
			vec[ind]->fa->gSum += vec[ind]->gSum;
			vec[ind]->fa->bSum += vec[ind]->bSum;
			vec.push_back(vec[ind]->fa);
			delete vec[ind];
			vec[ind] = NULL;
			vec.erase(vec.begin() + ind);
		}
	}
}

void octTree::add2vec(vector<octNode*>& vec, octNode* node) {
	if (node == NULL)
		return;
	if (node->isLeaf)
		vec.emplace_back(node);
	else {
		for (int i = 0; i < 8; i++)
			add2vec(vec, node->child[i]);
	}
}

//�ӵ�ɫ����ѡ���������ɫ��ӽ�����ɫ
uint8 selectClosestColor(uint8 r, uint8 g, uint8 b, RGBQUAD* pal)
{
	int index = 0;
	int temp = dis(r, g, b, pal, index);
	for (int i = 0; i < 256; i++) {
		int t = dis(r, g, b, pal, i);
		if (t < temp) {
			temp = t;
			index = i;
		}
	}
	return (uint8_t)index;
}

int dis(uint8 r, uint8 g, uint8 b, RGBQUAD* pal, int i) {
	int R = (int)(r - pal[i].rgbRed) * (r - pal[i].rgbRed);
	int G = (int)(g - pal[i].rgbGreen) * (g - pal[i].rgbGreen);
	int B = (int)(b - pal[i].rgbBlue) * (b - pal[i].rgbBlue);
	return R + G + B;
}

int main(int argc, char* argv[])
{
	if (argc < 3)
	{
		printf("using: exe[0], input file[1], output file[2]\n");
		return -1;
	}
	BITMAPFILEHEADER bf, * pbf;//���롢����ļ����ļ�ͷ
	BITMAPINFOHEADER bi, * pbi;//���롢����ļ�����Ϣͷ
	RGBQUAD* pRGBQuad;//�����ɵĵ�ɫ��ָ��
	uint8* pImage;//ת�����ͼ������
	DWORD bfSize;//�ļ���С
	LONG biWidth, biHeight;//ͼ���ȡ��߶�
	DWORD biSizeImage;//ͼ��Ĵ�С�����ֽ�Ϊ��λ��ÿ���ֽ���������4��������
	unsigned long biFullWidth;//ÿ���ֽ���������4��������

	//�������ļ�
	char* inputName, * outputName;
	FILE* fpIn, * fpOut;
	inputName = argv[1];
	outputName = argv[2];
	/*const char* inputName = "test_in.bmp";
	const char* outputName = "out.bmp";*/
	printf("Opening %s ... ", inputName);
	if (!(fpIn = fopen(inputName, "rb")))
	{
		printf("\nCan't open %s!\n", inputName);
		return -1;
	}
	printf("Success!\n");

	//��������ļ�
	printf("Creating %s ... ", outputName);
	if (!(fpOut = fopen(outputName, "wb")))
	{
		printf("\nCan't create %s!\n", outputName);
		return -1;
	}
	printf("Success!\n");

	//��ȡ�����ļ����ļ�ͷ����Ϣͷ
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fpIn);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fpIn);

	//��ȡ�ļ���Ϣ
	biWidth = bi.biWidth;
	biHeight = bi.biHeight;
	biFullWidth = ceil(biWidth / 4.) * 4;//bmp�ļ�ÿһ�е��ֽ���������4��������
	biSizeImage = biFullWidth * biHeight;
	bfSize = biFullWidth * biHeight + 54 + 256 * 4;//ͼ���ļ��Ĵ�С�������ļ�ͷ����Ϣͷ

	//��������ļ���BITMAPFILEHEADER
	pbf = new BITMAPFILEHEADER;
	pbf->bfType = 19778;
	pbf->bfSize = bfSize;
	pbf->bfReserved1 = 0;
	pbf->bfReserved2 = 0;
	pbf->bfOffBits = 54 + 256 * 4;
	//д��BITMAPFILEHEADER
	if (fwrite(pbf, sizeof(BITMAPFILEHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap file header!\n");
		fclose(fpOut);
		return -1;
	}

	//��������ļ���BITMAPINFOHEADER
	pbi = new BITMAPINFOHEADER;
	pbi->biSize = 40;
	pbi->biWidth = biWidth;
	pbi->biHeight = biHeight;
	pbi->biPlanes = 1;
	pbi->biBitCount = 8;
	pbi->biCompression = 0;
	pbi->biSizeImage = biSizeImage;
	pbi->biXPelsPerMeter = 0;
	pbi->biYPelsPerMeter = 0;
	pbi->biClrUsed = 0;
	pbi->biClrImportant = 0;
	//д��BITMAPFILEHEADER
	if (fwrite(pbi, sizeof(BITMAPINFOHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap info header!\n");
		fclose(fpOut);
		return -1;
	}

	//������ɫ�˲���
	printf("Building Color OctTree ...  ");
	octTree* tree;
	tree = new octTree(256);
	uint8 RGB[3];
	//��ȡͼ����ÿ�����ص���ɫ�������������ɫ�˲���
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i * ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//��ȡһ�����ص���ɫ�������������ɫ�˲���
			fread(&RGB, 3, 1, fpIn);
			tree->insertColor(RGB[2], RGB[1], RGB[0]);
		}
	}
	printf("Success!\n");
	printf("Leafs: %d\n", leafs);

	//���ɲ�����ɫ��
	printf("Generating palette ... ");
	pRGBQuad = new RGBQUAD[256];
	tree->generatePalette(pRGBQuad);

	//���256ɫ��ɫ��
	if (fwrite(pRGBQuad, 256 * sizeof(RGBQUAD), 1, fpOut) != 1)
	{
		printf("\nCan't write palette!\n");
		fclose(fpOut);
		return -1;
	}
	printf("Success!\n");

	//���ͼ������
	printf("Generating the output image ... ");
	pImage = new uint8[biSizeImage];
	memset(pImage, 0, biSizeImage);
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i * ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//��ȡһ�����ص���ɫ��������ת��λ��ɫ����ֵ
			fread(&RGB, 3, 1, fpIn);
			pImage[i * biFullWidth + j] = selectClosestColor(RGB[2], RGB[1], RGB[0], pRGBQuad);
		}
	}
	//���ͼ������
	if (fwrite(pImage, biSizeImage, 1, fpOut) != 1)
	{
		printf("\nCan't write image data!\n");
		fclose(fpOut);

		return -1;
	}
	printf("Success!\n");


	delete tree;
	delete pbf;
	delete pbi;
	delete[] pRGBQuad;
	delete[] pImage;
	fclose(fpIn);
	fclose(fpOut);
	printf("All done!\n");
	return 0;
}