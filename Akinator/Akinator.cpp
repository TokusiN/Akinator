// Akinator.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"

#pragma warning (disable:4996)

//#include <stdlib.h>
//#include <cmath>
//#include <iostream>
//#include <string>

#include "Akinator.h"

void QuestionAnswer::Init(int y, int n)
{
	yes = y + 1;
	no = n + 1;
	yes_possibility = (yes + 0.0) / (yes + no);
	no_possibility = (no + 0.0) / (yes + no);
}


CTwentyAnswers::CTwentyAnswers()
{
	m_itemNum = 0;
	m_questionNum = 0;
	m_questionHistoryNum = 0;

	m_stockQuestion.clear();
	m_itemPossibility.clear();
	m_qa.clear();
	m_questionHistory.clear();
	m_answerHistory.clear();
}

CTwentyAnswers::~CTwentyAnswers()
{

}

int CTwentyAnswers::GetQuestion(int idx)
{
	if (idx < m_questionHistoryNum)
		return this->m_questionHistory[idx];
	else
		return -2;
}

int CTwentyAnswers::GetAnswer(int idx)
{
	if(idx < m_answerHistory.size())
	{
		return m_answerHistory[idx];
	}
	else
	{
		return -1;
	}
}

void CTwentyAnswers::init(int itemNum, int questionNum, std::vector<int> itemPlayCount, std::vector<std::vector<int> > qa)
{
	m_itemNum = itemNum;
	m_questionNum = questionNum;
	m_itemPossibility.clear();
	for (int times : itemPlayCount)
	{
		m_itemPossibility.push_back(1.f + times);
	}
	m_stockQuestion.resize(m_questionNum);
	for (int i = 0; i < questionNum; i++)
	{
		m_stockQuestion[i] = i;
	}
	m_qa.clear();
	for (const auto& q : qa)
	{
		std::vector<QuestionAnswer> question;
		question.resize(m_itemNum);
		for (int i = 0; i < m_itemNum; i++)
		{
			question[i].Init(q[i * 2 + 1], q[i * 2]);
		}
		m_qa.push_back(question);
	}
	CalcMaxProb();
	MakeQuestion();
}

// 質問に対する答えを登録する
bool CTwentyAnswers::SendAnswer(int y)
{
	m_answerHistory.push_back(y);
	int q = m_questionHistory[m_questionHistoryNum - 1];
	if (q >= 0)
	{
		m_itemPossibility = GetNextPossibirity(m_questionHistory[m_questionHistoryNum - 1], y); // -1???
	}
	else
	{
		if (y)
		{
			// 正解！
		}
		else
		{
			// ハズレ
			m_itemPossibility[m_maxProbItem] = 0;
		}
	}
	m_entoropy = CalcEntoropy(m_itemPossibility);
	CalcMaxProb();
	return MakeQuestion();
}

double CTwentyAnswers::GetCurrentEntoropy()
{
	return m_entoropy;
}

/*
int m_itemNum;
int m_questionNum;
int m_questionHistoryNum;
std::vector<int> m_stockQuestion;
std::vector<double> m_itemPossibility;
std::vector<std::vector<QuestionAnswer> > m_qa;
std::vector<int> m_questionHistory;

*/
// 次の質問を作成する
bool CTwentyAnswers::MakeQuestion()
{
	if (m_stockQuestion.size() == 0)
	{
		// 質問のストックが無くなった！
		m_questionHistory.push_back(-1);
		m_questionHistoryNum++;
		return true;
	}
	if (m_maxProb > 90)
	{
		// 9割以上の確率で正解できそう
		m_questionHistory.push_back(-1);
		m_questionHistoryNum++;
		return true;
	}
	// 既に次の質問は作成済み
	if ((m_questionHistory.size()) > m_answerHistory.size())
		abort();

	const int MAX_SELECT = 5;
	double usefulness[MAX_SELECT];
	std::vector<int> selectedQuestion;
	int selectnum = 0;
	while (selectnum < MAX_SELECT && m_stockQuestion.size() > 0)
	{
		int r;
		r = rand() % m_stockQuestion.size();
		selectedQuestion.push_back(m_stockQuestion[r]);
		m_stockQuestion[r] = m_stockQuestion[m_stockQuestion.size() - 1];
		m_stockQuestion.pop_back();
		selectnum++;
	}
	int i, j;
	double minEntoropy = 1e100;
	int minQuestion = -1;
	i = 0;
	for (int quest : selectedQuestion)
	{
		double yesPoss = 0;
		double noPoss = 0;
		std::vector<double> tempPossibility;
		// calc yes_usefulness
		tempPossibility = GetNextPossibirity(quest, 1);
		double yesEntoropy = CalcEntoropy(tempPossibility);
		for (double p : tempPossibility)
		{
			yesPoss += p;
		}
		// calc no_usefulness
		tempPossibility = GetNextPossibirity(quest, 0);
		double noEntoropy = CalcEntoropy(tempPossibility);
		for (double p : tempPossibility)
		{
			noPoss += p;
		}
		double entoropy = yesEntoropy * yesPoss / (yesPoss + noPoss)
			+ noEntoropy  * noPoss / (yesPoss + noPoss);
		if (minEntoropy > entoropy)
		{
			minEntoropy = entoropy;
			minQuestion = quest;
		}
		i++;
	}
	for (int quest : selectedQuestion)
	{
		if (quest != minQuestion)
		{
			m_stockQuestion.push_back(quest);
		}
	}
	m_questionHistory.push_back(minQuestion);
	m_questionHistoryNum++;
	return true;
}

// 確率リストからエントロピーを計算する
double CTwentyAnswers::CalcEntoropy(std::vector<double> possibility)
{
	double total = 0;
	double entoropy = 0;
//	std::vector<double> entoropy;
		for (auto poss : possibility)
		{
			total += poss;
		}
	for (auto poss : possibility)
	{
		double prob = poss / total;
		entoropy += prob * std::log2(prob);
	}
	return entoropy;
}

// 質問に答えた時の確率テーブルを作成する
std::vector<double> CTwentyAnswers::GetNextPossibirity(int question, int answer)
{
	std::vector<double> possibility;
	if (answer == 1)
	{
		for (int j = 0; j < m_itemNum; j++)
		{
			double y = m_qa[question][j].yes_possibility;
			double poss = y * m_itemPossibility[j];
			possibility.push_back(poss);
		}
	}
	else
	{
		for (int j = 0; j < m_itemNum; j++)
		{
			double n = m_qa[question][j].no_possibility;
			double poss = n * m_itemPossibility[j];
			possibility.push_back(poss);
		}
	}
	return possibility;
}

void CTwentyAnswers::CalcMaxProb()
{
	int i = 0;
	int max = -1;
	double maxp = 0;
	double totalp = 0;
	for (i = 0; i < m_itemPossibility.size(); i++)
	{
		totalp += m_itemPossibility[i];
		if (m_itemPossibility[i] > maxp)
		{
			max = i;
			maxp = m_itemPossibility[i];
		}
	}
	m_maxProbItem = max;
	m_maxProb = maxp / totalp;
	m_maxProbItemHistory.push_back(max);

}
