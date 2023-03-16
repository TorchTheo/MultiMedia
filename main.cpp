#define _CRT_SECURE_NO_DEPRECATE

#include <windows.h>
#include <cstdio>
#include <cmath>

typedef unsigned char uint8;

struct octNode
{
	long long cnt;//本结点以下的像素总数
	long long rSum, gSum, bSum;//红色分量、绿色分量、蓝色分量灰度值的和
	bool isLeaf;//是否是叶子结点
	int depth;//本节点的深度
	octNode* child[8];//8个子结点的指针数组
	octNode() = default;
	octNode(int d) : cnt(1), rSum(0), gSum(0), bSum(0), isLeaf(false), depth(d) {}
};

class octTree
{
public:
	octTree(){};
	octTree(int maxColorNum){maxColors=maxColorNum;}
	~octTree();

	void insertColor(uint8 r, uint8 g, uint8 b);						//插入一个颜色
	uint8 generatePalette(RGBQUAD *pal);						//生成调色板
private:
	octNode *root;														//八叉树的根
	int colors;															//当前的颜色总数
	int maxColors;														//最大颜色数
};

//释放八叉树的内存空间
octTree::~octTree()
{
	//To do....
	
}

//往八叉树中添加一个像素的颜色
void octTree::insertColor(uint8 r, uint8 g, uint8 b)
{
	//....
	this->colors ++;
	octNode* node = root;
	for (int i = 7; i >= 0; i--) {
		int index = ((r >> i) & 0x1) * 4 + ((g >> i) & 0x1) * 2 + ((b >> i) & 0x1);
		if (node->child[index]->isLeaf) {
			node->child[index]->cnt++;
			node->child[index]->rSum += r;
			node->child[index]->gSum += g;
			node->child[index]->bSum += b;
		}
		else if (node->child[index] == NULL) {
			node->child[index] = new octNode(node->depth + 1);
		}
	}
}

//根据现有的八叉树，选择256个颜色作为最终的调色板中的颜色
uint8 octTree::generatePalette(RGBQUAD *pal)
{
	//....
	return 0;
}


//从调色板中选出与给定颜色最接近的颜色
uint8 selectClosestColor(uint8 r, uint8 g, uint8 b, RGBQUAD *pal)
{
	return (uint8)0;//给定某颜色，返回其在调色板中最近似颜色的索引值；
}

int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("using: exe[0], input file[1], output file[2]\n");
		return -1;
	}
	BITMAPFILEHEADER bf, *pbf;//输入、输出文件的文件头
	BITMAPINFOHEADER bi, *pbi;//输入、输出文件的信息头
	RGBQUAD *pRGBQuad;//待生成的调色板指针
	uint8 *pImage;//转换后的图象数据
	DWORD bfSize;//文件大小
	LONG biWidth, biHeight;//图象宽度、高度
	DWORD biSizeImage;//图象的大小，以字节为单位，每行字节数必须是4的整数倍
	unsigned long biFullWidth;//每行字节数必须是4的整数倍

	//打开输入文件
	char *inputName, *outputName;
	FILE *fpIn, *fpOut;
	inputName = argv[1];
	outputName = argv[2];
	printf("Opening %s ... ", inputName);
	if (!(fpIn = fopen(inputName, "rb")))
	{
		printf("\nCan't open %s!\n", inputName);
		return -1;
	}
	printf("Success!\n");

	//创建输出文件
	printf("Creating %s ... ", outputName);
	if (!(fpOut = fopen(outputName, "wb")))
	{
		printf("\nCan't create %s!\n", outputName);
		return -1;
	}
	printf("Success!\n");

	//读取输入文件的文件头、信息头
	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fpIn);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fpIn);

	//读取文件信息
	biWidth = bi.biWidth;
	biHeight = bi.biHeight;
	biFullWidth = ceil(biWidth / 4.) * 4;//bmp文件每一行的字节数必须是4的整数倍
	biSizeImage = biFullWidth*biHeight;
	bfSize = biFullWidth*biHeight + 54 + 256 * 4;//图象文件的大小，包含文件头、信息头

	//设置输出文件的BITMAPFILEHEADER
	pbf = new BITMAPFILEHEADER;
	pbf->bfType = 19778;
	pbf->bfSize = bfSize;
	pbf->bfReserved1 = 0;
	pbf->bfReserved2 = 0;
	pbf->bfOffBits = 54 + 256 * 4;
	//写出BITMAPFILEHEADER
	if (fwrite(pbf, sizeof(BITMAPFILEHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap file header!\n");
		fclose(fpOut);
		return -1;
	}

	//设置输出文件的BITMAPINFOHEADER
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
	//写出BITMAPFILEHEADER
	if (fwrite(pbi, sizeof(BITMAPINFOHEADER), 1, fpOut) != 1)
	{
		printf("\nCan't write bitmap info header!\n");
		fclose(fpOut);
		return -1;
	}

	//构建颜色八叉树
	printf("Building Color OctTree ...  ");
	octTree *tree;
	tree = new octTree(256);
	uint8 RGB[3];
	//读取图像中每个像素的颜色，并将其插入颜色八叉树
	for (int i = 0; i < bi.biHeight; i++)
	{
		fseek(fpIn, bf.bfOffBits + i*ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//读取一个像素的颜色，并将其插入颜色八叉树
			fread(&RGB, 3, 1, fpIn);
			tree->insertColor(RGB[2], RGB[1], RGB[0]);
		}
	}
	printf("Success!\n");

	//生成并填充调色板
	printf("Generating palette ... ");
	pRGBQuad = new RGBQUAD[256];
	tree->generatePalette(pRGBQuad);

	//输出256色调色板
	if (fwrite(pRGBQuad, 256 * sizeof(RGBQUAD), 1, fpOut) != 1)
	{
		printf("\nCan't write palette!\n");
		fclose(fpOut);
		return -1;
	}
	printf("Success!\n");

	//填充图像数据
	printf("Generating the output image ... ");
	pImage = new uint8[biSizeImage];
	memset(pImage, 0, biSizeImage);
	for (int i = 0; i < bi.biHeight; i++)
	{		
		fseek(fpIn, bf.bfOffBits + i*ceil(biWidth * 3 / 4.) * 4, 0);
		for (int j = 0; j < bi.biWidth; j++)
		{
			//读取一个像素的颜色，并将其转换位颜色索引值
			fread(&RGB, 3, 1, fpIn);
			pImage[i*biFullWidth + j] = selectClosestColor(RGB[2], RGB[1], RGB[0], pRGBQuad);
		}
	}
	//输出图象数据
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