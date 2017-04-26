#include "stdafx.h"

#include "Akinator.h"
#include <time.h>
#include <iostream>
#include <string>



int main(void)
{
	srand(time(NULL));
	CTwentyAnswers qa;
	int itemNum;
	int questionNum;
	std::vector<std::string> itemName;
	std::vector<int> itemPlayCount;
	std::vector<std::string> question;
	std::vector<std::vector<int> > questionItemAnswer;
	FILE* fp;
	fp = fopen("data/item.txt", "r");
	fscanf(fp, "%d\n", &itemNum);
	for (int i = 0; i < itemNum; i++)
	{
		char s[512];
		fgets(s, 510, fp);
		s[strlen(s) - 1] = 0;
		itemName.push_back(s);
	}
	fclose(fp);

	fp = fopen("data/itemplaycount.txt", "r");
	for (int i = 0; i < itemNum; i++)
	{
		int n;
		fscanf(fp, "%d,", &n);
		itemPlayCount.push_back(n);
	}
	fclose(fp);

	fp = fopen("data/question.txt", "r");
	fscanf(fp, "%d\n", &questionNum);
	for (int i = 0; i < questionNum; i++)
	{
		char s[512];
		fgets(s, 510, fp);
		s[strlen(s) - 1] = 0;
		question.push_back(s);
	}
	fclose(fp);

	fp = fopen("data/item_question.txt", "r");
	for (int i = 0; i < questionNum; i++)
	{
		std::vector<int> temp;
		for (int j = 0; j < itemNum; j++)
		{
			int y, n;
			fscanf(fp, "%d,%d,", &n, &y);
			temp.push_back(n);
			temp.push_back(y);
		}
		questionItemAnswer.push_back(temp);
	}
	fclose(fp);
	qa.init(itemNum, questionNum, itemPlayCount, questionItemAnswer);

	for (int i = 0; i < 50; i++)
	{
		int q = qa.GetQuestion(i);
		int it = qa.GetMaxProbItem();

		if (q == -1)
		{
			std::cout << "It's ["  << it << " ";
			std::cout << itemName[it] << "]? " << qa.GetMaxProb() * 100 << "%" << std::endl;
		}
		else
		{
			std::cout << question[q] << "    ";
			std::cout << it << " ";
			std::cout << itemName[it] << " " << qa.GetMaxProb() * 100 << "%" << std::endl;
		}
		int a;
		scanf("%d", &a);
		qa.SendAnswer(a);
		if (q == -1 && a == 1)
		{
			// Game clear!
			itemPlayCount[it]++;
			fp = fopen("data/itemplaycount.txt", "w");
			for (auto i : itemPlayCount)
			{
				fprintf(fp, "%d,", i);
			}
			fclose(fp);
			for (int i = 0;; i++)
			{
				int q = qa.GetQuestion(i);
				int a = qa.GetAnswer(i);
				if (q == -2)
				{
					break;
				}
				if (q >= 0)
				{
					questionItemAnswer[q][it * 2 + a]++;
				}
			}
			fp = fopen("data/item_question.txt", "w");
			for (auto i : questionItemAnswer)
			{
				for (auto j : i)
				{
					fprintf(fp, "%d,", j);
				}
				fprintf(fp, "\n");
			}
			fclose(fp);
			break;
		}
	}
	return 0;
}
